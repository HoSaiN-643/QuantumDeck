#include "client.h"
#include "connect.h"
#include "login.h"
#include "signup.h"
#include "choose_mode.h"
#include "mainmenu.h"
#include "gamewindow.h"
#include "history.h"
#include "change_profile.h"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

Client::Client(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this)),
    player(new Player()),
    loginWindow(nullptr),
    m_connect(nullptr),
    m_signup(nullptr),
    m_chooseMode(nullptr),
    cf(nullptr),
    menu(nullptr),
    gw(nullptr),
    history(nullptr)
{
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &Client::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::OnReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
}

Client::~Client()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        DisconnectFromServer();
    }
    delete gw;
    delete menu;
    delete loginWindow;
    delete history;
    delete cf;
    delete m_signup;
    delete m_connect;
    delete m_chooseMode;
    delete player;
    delete m_socket;
}

void Client::ConnectToServer(const QHostAddress &host, quint16 port)
{
    m_socket->connectToHost(host, port);
    QTimer::singleShot(5000, this, [this]() {
        if (m_socket->state() == QAbstractSocket::ConnectedState) {
            qDebug() << "Connection is stable";
        } else {
            qDebug() << "Connection failed or was lost";
            if (m_connect) {
                m_connect->OnErrorOccurred("Connection to server is unstable");
            }
        }
    });
}

void Client::DisconnectFromServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected(1000);
    }
}

Player& Client::GetPlayer()
{
    return *player;
}

void Client::WriteToServer(const QString &data)
{
    qDebug() << "Sending data to server:" << data.trimmed();
    m_socket->write(data.toUtf8());
    m_socket->flush();
}

QStringList Client::extractFields(const QString &payload) const
{
    QStringList fields;
    int pos = 0;
    while (true) {
        int start = payload.indexOf('[', pos);
        int end = payload.indexOf(']', start + 1);
        if (start < 0 || end < 0) break;
        fields << payload.mid(start + 1, end - start - 1);
        pos = end + 1;
    }
    return fields;
}

void Client::sendCardSelection(const QString &card)
{
    QString data = QString("G[CC][%1]\n").arg(card);
    WriteToServer(data);
}

void Client::sendChatMessage(const QString &message)
{
    QString data = message + "\n";
    WriteToServer(data);
}

void Client::onConnected()
{
    if (m_connect) {
        m_connect->OnConnected();
    }
    qDebug() << "Successfully connected to server";
}

void Client::onDisconnected()
{
    qDebug() << "Disconnected from server";
    if (m_connect) {
        m_connect->OnErrorOccurred("Connection to server was unexpectedly lost. Please try again.");
    }
}

void Client::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socketError << m_socket->errorString();
    if (m_connect) {
        m_connect->OnErrorOccurred(m_socket->errorString());
    }
}

void Client::OnReadyRead()
{
    while (m_socket->bytesAvailable()) {
        QString message = m_socket->readLine().trimmed();
        qDebug() << "Received from server:" << message;

        if (message.isEmpty()) {
            qDebug() << "Empty message received";
            continue;
        }

        QChar cmd = message.at(0);
        QString payload = message.mid(1);
        QStringList fields = extractFields(payload);
        qDebug() << "Message type:" << cmd << ", Fields:" << fields;

        if (fields.isEmpty()) {
            qDebug() << "Empty fields received";
            continue;
        }

        switch (cmd.unicode()) {
        case 'L': handleLogin(fields); break;
        case 'S': handleSignup(fields); break;
        case 'R': handleRecover(fields); break;
        case 'P': handlePreGame(fields); break;
        case 'C': handleUpdateProfile(fields); break;
        case 'G': handleGame(fields); break;
        default:
            qDebug() << "Unknown command received:" << cmd << fields;
            handleErrorCmd(fields);
            break;
        }
    }
}

void Client::handleLogin(const QStringList &fields)
{
    if (fields[0] == "OK") {
        if (fields.size() < 8) {
            QMessageBox::information(nullptr, tr("Login"), tr("Invalid response format"));
            return;
        }
        QMessageBox::information(nullptr, tr("Welcome!"), tr("Successfully logged in"));
        player->SetInfo(fields[2], fields[3], fields[5], fields[4], fields[6], fields[7]);
        qDebug() << "Player info:" << player->firstName() << player->lastName() << player->phone()
                 << player->email() << player->username() << player->password();

        QStringList gameHistory;
        for (int i = 8; i < fields.size(); ++i) {
            if (fields[i] == "HIST") continue;
            gameHistory << fields[i];
        }
        if (!gameHistory.isEmpty() && history) {
            history->displayHistory(gameHistory);
        }

        if (loginWindow) {
            loginWindow->Open_menu();
        } else {
            qDebug() << "loginWindow is nullptr";
        }
    } else if (fields[0] == "ERROR") {
        QString errorMsg = fields.size() > 1 ? fields[1] : "Unknown error";
        QMessageBox::information(nullptr, tr("Login failed"), errorMsg);
    } else {
        QMessageBox::information(nullptr, tr("Login failed"), tr("Unknown response"));
    }
}

void Client::handleSignup(const QStringList &fields)
{
    if (fields[0] == "OK") {
        QMessageBox::information(m_signup, tr("Signup"), tr("Successfully signed up"));
        if (m_signup) m_signup->OnSuccesfullSignUp();
    } else if (fields[0] == "ERROR") {
        QString errorMsg = fields.size() > 1 ? fields[1] : "Unknown error";
        QMessageBox::information(m_signup, tr("Signup failed"), errorMsg);
    }
}

void Client::handleRecover(const QStringList &fields)
{
    if (fields[0] == "OK") {
        if (fields.size() >= 3 && fields[1] == "Password found") {
            QMessageBox::information(nullptr, tr("Recover"), tr("Password recovered: %1").arg(fields[2]));
        }
    } else if (fields[0] == "ERROR") {
        QString errorMsg = fields.size() > 1 ? fields[1] : "Unknown error";
        QMessageBox::information(nullptr, tr("Recover failed"), errorMsg);
    }

    if (!loginWindow) {
        loginWindow = new Login(*player, this);
    }
    loginWindow->show();
}

void Client::handleErrorCmd(const QStringList &fields)
{
    if (fields[0] == "ERROR") {
        QString errorMsg = fields.size() > 1 ? fields[1] : "Unknown error";
        QMessageBox::information(nullptr, tr("Error"), errorMsg);
    }
}

void Client::handlePreGame(const QStringList &fields)
{
    if (fields.size() < 2) {
        qDebug() << "Invalid PreGame response format";
        return;
    }

    int mode = fields[0].toInt();
    if (mode == 2) {
        if (fields[1] == "1" || fields[1] == "Searching for a match") {
            if (m_chooseMode) m_chooseMode->onPreGameSearching(fields[2].toInt(), fields[2].toInt());
        } else if (fields[1] == "Found") {
            QStringList opponents;
            for (int i = 2; i < fields.size(); ++i) {
                opponents << fields[i];
            }
            if (m_chooseMode) m_chooseMode->onPreGameFound(opponents);
        } else if (fields[1] == "FULL") {
            if (m_chooseMode) QMessageBox::information(m_chooseMode, tr("Error"), tr("Server is full"));
        }
    }
}

void Client::handleGame(const QStringList &fields)
{
    if (fields.isEmpty()) {
        qDebug() << "Empty game message";
        return;
    }

    if (gw) {
        if (fields[0] == "ST") {
            if (fields.size() >= 4) {
                qDebug() << "Handling game message with fields:" << fields;
                gw->displayDiamondCards(fields[2], fields[4]);
            }
        } else if (fields[0] == "CC") {
            if (fields[1] == "YOU") {
                QStringList cards = fields.mid(2);
                gw->displayCommunityCards(cards);
            } else if (fields[1] == "WAIT") {
                gw->displayWaitingMessage("Waiting for opponent to choose...");
            }
        } else if (fields[0] == "RF") {
            if (fields.size() >= 2) {
                gw->displayGameStatus(QString("Round winner: %1").arg(fields[1]));
                gw->clearCards();
            }
        } else if (fields[0] == "END") {
            if (fields.size() >= 2) {
                gw->displayGameStatus(QString("Game ended. Winner: %1").arg(fields[1]));
                gw->clearCards();
                gw->close();
            }
        } else if (fields[0] == "CHAT") {
            if (fields.size() >= 3) {
                gw->displayChatMessage(QString("%1: %2").arg(fields[1], fields[2]));
            }
        }
    }
}

void Client::handleUpdateProfile(const QStringList &fields)
{
    if (fields[0] == "CF") {
        if (fields[1] == "OK") {
            if (fields.size() >= 8) {
                player->SetInfo(fields[3], fields[4], fields[6], fields[5], fields[7], fields[8]);
                if (cf) {
                    QMessageBox::information(cf, tr("Profile"), tr("Profile updated successfully"));
                    cf->close();
                }
            }
            if (!menu) {
                menu = new MainMenu(*player, this);
            }
            menu->show();
        } else if (fields[1] == "ERROR") {
            if (cf) QMessageBox::information(cf, tr("Profile"), tr("Update failed"));
            if (!menu) {
                menu = new MainMenu(*player, this);
            }
            menu->show();
        }
    }
}
