// client.cpp
#include "client.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QMessageBox>
#include "login.h"
#include "player.h"
#include <connect.h>
#include <signup.h>
#include <choose_mode.h>
#include <mainmenu.h>

Client::Client(QObject *parent)
    : QObject(parent),
      m_socket(new QTcpSocket(this)),
      loginWindow(new Login(player, this)),
      m_connect(nullptr),
      m_signup(nullptr),
      m_chooseMode(nullptr),
      cf(nullptr),
      menu(new MainMenu(player, this))
{
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &Client::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::OnReadyRead);
}

Client::~Client()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        DisconnectFromServer();
}

void Client::ConnectToServer(const QHostAddress &host, quint16 port)
{
    m_socket->connectToHost(host,port);
}

void Client::DisconnectFromServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

void Client::onConnected()
{
    qDebug() << "Connected";
    if (m_connect) {
        m_connect->OnConnected();
    }
}

void Client::onDisconnected()
{
  qDebug() << "Disconnected";
}

void Client::onError(QAbstractSocket::SocketError)
{
    QString err = m_socket->errorString();
    qDebug() << "Error:" << err;
    if (m_connect) {
        m_connect->OnErrorOccurred(err);
    }
}

void Client::OnReadyRead()
{
    // جمع‌آوری داده‌ها در بافر
    readBuffer.append(m_socket->readAll());

    // پردازش خطوط کامل (تا \n)
    while (readBuffer.contains('\n')) {
        int end = readBuffer.indexOf('\n');
        QByteArray line = readBuffer.mid(0, end);
        readBuffer.remove(0, end + 1);

        if (line.isEmpty())
            continue;

        // تبدیل خط به QString
        QString message = QString::fromUtf8(line).trimmed();
        qDebug() << "Received full message:" << message;

        // بررسی نوع پیام
        QChar cmd = message.at(0); // تغییر از char به QChar
        QString payload = message.mid(1); // از بعد از کاراکتر اول
        QStringList fields = extractFields(payload);
        qDebug() << "Type received:" << cmd;

        if (fields.isEmpty()) {
            qDebug() << "Empty fields in message";
            continue;
        }

        // پردازش بر اساس نوع پیام
        switch (cmd.unicode()) { // استفاده از cmd.unicode() برای سوئیچ
        case 'L': handleLogin(fields); break;
        case 'S': handleSignup(fields); break;
        case 'R': handleRecover(fields); break;
        case 'P': handlePreGame(fields); break;
        case 'X': handleErrorCmd(fields); break;
        case 'C': handleUpdateProfile(fields); break;
        case 'G': handleGame(fields); break;
        default:
            qDebug() << "Unknown command:" << cmd;
            break;
        }
    }
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

void Client::handleUpdateProfile(const QStringList &fields)
{
    if (fields[0] != "CF") {
        QMessageBox::information(loginWindow, tr("Update Profile Failed"), tr("Invalid response format from server!"));
        return;
    }

    QString status = fields[1];

    if (status == "OK") {
        qDebug() << "fields from client handle:" << fields;
        qDebug() << "Player from client handle:" << player.firstName() << player.lastName() << player.email()
                  << player.phone() << player.username() << player.password();

        QMessageBox::information(loginWindow, fields[1], fields[2]);
        player.SetInfo(fields[3], fields[4], fields[5], fields[6], fields[7], fields[8]);
        menu->show();
    } else {
        QMessageBox::information(loginWindow, fields[1], fields[2]);
    }
}

void Client::handleLogin(const QStringList &fields)
{
    if (fields.isEmpty() || fields[0] != "OK") {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow, tr("Login failed"), fields[1]);
        return;
    }

    if (fields.size() < 8) {
        QMessageBox::information(loginWindow, tr("Login"), tr("Login response bad format!"));
        return;
    }

    QMessageBox::information(loginWindow, tr("Welcome!"), tr("Successfully logged in"));
    player.SetInfo(fields[2], fields[3], fields[5], fields[4], fields[6], fields[7]);
    qDebug() << "player from login:" << player.firstName() << player.lastName() << player.email() << player.phone()
              << player.username() << player.password();

    loginWindow->Open_menu();
}

void Client::handleSignup(const QStringList &fields)
{
    if (fields[0] == "OK") {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow, tr("Signup"), fields[1]);
        if (m_signup) {
            m_signup->OnSuccesfullSignUp();
        }
        loginWindow->show();
    } else {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow, tr("Signup failed"), fields[1]);
    }
}

void Client::handleRecover(const QStringList &fields)
{
    if (fields[0] == "OK") {
        if (fields.size() >= 3)
            QMessageBox::information(loginWindow, tr("Password found"), QString("%1 : %2").arg(fields[1], fields[2]));
    } else {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow, tr("Recover failed"), fields[1]);
    }
}

void Client::handleErrorCmd(const QStringList &fields)
{
    if (fields.size() >= 2)
        QMessageBox::information(loginWindow, tr("Error"), fields[1]);
}

void Client::handlePreGame(const QStringList &f)
{
    if (f.size() < 2) return;

    int total = f[0].toInt();
    const QString &status = f[1];

    if (f.size() >= 3 && f[2] == QLatin1String("Searching for a match")) {
        int waiting = f[1].toInt();
        if (m_chooseMode) {
            m_chooseMode->onPreGameSearching(waiting, total);
        }
    } else if (status == QLatin1String("Found")) {
        QStringList others = f.mid(2);
        if (m_chooseMode) {
            m_chooseMode->onPreGameFound(others);
        }
    } else if (status == QLatin1String("FULL")) {
        QString msg = (f.size() >= 3 ? f[2] : tr("Server is full"));
        if (m_chooseMode) {
            m_chooseMode->onPreGameFull(msg);
        }
    }
}

void Client::handleGame(const QStringList &fields)
{
    // if (fields.isEmpty()) {
    //     QMessageBox::information(menu, tr("Game Error"), tr("Invalid game message format"));
    //     return;
    // }

    // QString command = fields[0];
    // if (command == "INITIAL_CARD") {
    //     if (fields.size() < 2) {
    //         QMessageBox::information(menu, tr("Game Error"), tr("Invalid initial card format"));
    //         return;
    //     }
    //     QString card = fields[1];
    //     if (menu) {
    //         menu->showInitialCard(card); // فرض بر وجود متد showInitialCard در MainMenu
    //     }
    // } else if (command == "CARDS_TO_STARTER" || command == "CARDS_TO_OPPONENT") {
    //     if (fields.size() < 2) {
    //         QMessageBox::information(menu, tr("Game Error"), tr("Invalid cards format"));
    //         return;
    //     }
    //     QStringList cards = fields.mid(1);
    //     if (menu) {
    //         if (command == "CARDS_TO_STARTER") {
    //             menu->showCardsToSelect(cards, true); // true نشان‌دهنده نوبت starter
    //         } else {
    //             menu->showCardsToSelect(cards, false); // false نشان‌دهنده نوبت opponent
    //         }
    //     }
    // } else if (command == "ROUND_WINNER") {
    //     if (fields.size() < 2) {
    //         QMessageBox::information(menu, tr("Game Error"), tr("Invalid round winner format"));
    //         return;
    //     }
    //     QString winner = fields[1];
    //     if (menu) {
    //         menu->showRoundWinner(winner); // فرض بر وجود متد showRoundWinner
    //     }
    // } else if (command == "GAME_FINISHED") {
    //     if (fields.size() < 2) {
    //         QMessageBox::information(menu, tr("Game Error"), tr("Invalid game finished format"));
    //         return;
    //     }
    //     QString winner = fields[1];
    //     if (menu) {
    //         menu->showGameWinner(winner); // فرض بر وجود متد showGameWinner
    //     }
    // } else if (command == "CHAT") {
    //     if (fields.size() < 3) {
    //         QMessageBox::information(menu, tr("Game Error"), tr("Invalid chat message format"));
    //         return;
    //     }
    //     QString sender = fields[1];
    //     QString message = fields[2];
    //     if (menu) {
    //         menu->showChatMessage(sender, message); // فرض بر وجود متد showChatMessage
    //     }
    // } else {
    //     QMessageBox::information(menu, tr("Game Error"), tr("Unknown game command"));
    // }
}

void Client::sendCardSelection(const QString &card)
{
    WriteToServer(QString("G[SELECT_CARD][%1]").arg(card));
}

void Client::sendChatMessage(const QString &message)
{
    WriteToServer(QString("G[CHAT][%1]").arg(message));
}

Player& Client::GetPlayer()
{
    return player;
}

void Client::WriteToServer(const QString &data)
{
    QByteArray out = data.toUtf8();
    qDebug() << "Sending data to server:" << data;
    m_socket->write(out);
    m_socket->flush();
}
