#include "client.h"
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QMessageBox>
#include "login.h"
#include "player.h"
#include <connect.h>
#include <login.h>
#include <choose_mode.h>
#include <signup.h>

Client::Client(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this)),
    loginWindow(new Login(player, this)),
    m_connect(nullptr),
    m_signup(nullptr),
    m_chooseMode(nullptr)
{
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &Client::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::OnReadyRead);
}

Client::~Client()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

void Client::ConnectToServer(const QHostAddress &host, quint16 port)
{
    m_socket->connectToHost(host, port);
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
        m_connect->OnConnected(); // Direct call instead of signal
    }
}

void Client::onDisconnected()
{
    qDebug() << "Disconnected";
    // Optionally call a method in Connect if needed
}

void Client::onError(QAbstractSocket::SocketError)
{
    QString err = m_socket->errorString();
    qDebug() << "Error:" << err;
    if (m_connect) {
        m_connect->OnErrorOccurred(err); // Direct call instead of signal
    }
}

QStringList Client::extractFields(const QString &msg)
{
    QStringList result;
    int start = msg.indexOf('[');
    while (start != -1) {
        int end = msg.indexOf(']', start);
        if (end == -1) break;
        result << msg.mid(start + 1, end - start - 1);
        start = msg.indexOf('[', end);
    }
    return result;
}

void Client::OnReadyRead()
{
    readBuffer = m_socket->readAll();
    QByteArray line = readBuffer;
    readBuffer.clear();

    if (line.isEmpty())
        return;

    char cmd = line.at(0);
    QByteArray body = line.mid(1);
    QString payload = QString::fromUtf8(body);
    QStringList fields = extractFields(payload);
    if (fields.isEmpty())
        return;

    switch (cmd) {
    case 'L': handleLogin(fields); break;
    case 'S': handleSignup(fields); break;
    case 'R': handleRecover(fields); break;
    case 'P': handlePreGame(fields); break;
    case 'X': handleErrorCmd(fields); break;
    case 'C': handleUpdateProfile(fields); break;
    default: break;
    }
}
void Client::handleUpdateProfile(const QStringList &fields)
{
    if (fields.size() != 9  || fields[0] != "CF") {
        QMessageBox::information(loginWindow, tr("Update Profile Failed"), tr("Invalid response format from server!"));
        return;
    }

    QString status = fields[1];

    if (status == "OK") {

        QMessageBox::information(loginWindow, fields[1], fields[2]);
        // به‌روزرسانی اطلاعات بازیکن با داده‌های جدید دریافت‌شده از سرور
        // فرض می‌کنیم فیلدهای 3 تا 8 به ترتیب firstname, lastname, email, phone, username, password هستند
        player.SetInfo(fields[3], fields[4], fields[5], fields[6], fields[7], fields[8]);
    }
    else if (status == "NOTFOUND") {
        QString message = (fields.size() >= 4) ? fields[3] : tr("Old username not found in database.");
        QMessageBox::information(loginWindow, tr("Update Profile Failed"), message);
    }
    else if (status == "DUPLICATE") {
        QString message = (fields.size() >= 4) ? fields[3] : tr("New username already exists.");
        QMessageBox::information(loginWindow, tr("Update Profile Failed"), message);
    }
    else if (status == "ERROR") {
        QString message = (fields.size() >= 4) ? fields[3] : tr("An error occurred while updating profile.");
        QMessageBox::information(loginWindow, tr("Update Profile Failed"), message);
    }
    else {
        QString message = (fields.size() >= 4) ? fields[3] : tr("Unknown response from server.");
        QMessageBox::information(loginWindow, tr("Update Profile Failed"), message);
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
    player.SetInfo(fields[2], fields[3], fields[4], fields[5], fields[6], fields[7]);
    loginWindow->Open_menu(); // Assuming this is a direct call to show the menu
}

void Client::handleSignup(const QStringList &fields)
{
    if (fields[0] == "OK") {
        if (fields.size() >= 2)
            QMessageBox::information(loginWindow, tr("Signup"), fields[1]);
        if (m_signup) {
            m_signup->OnSuccesfullSignUp(); // Direct call instead of signal
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

Player& Client::GetPlayer()
{
    return player;
}

void Client::WriteToServer(const QString &data)
{
    QByteArray out = data.toUtf8();
    qDebug() << "sending data to sever : " << data;
    m_socket->write(out);
}

void Client::handlePreGame(const QStringList &f)
{
    if (f.size() < 2) return;

    int total = f[0].toInt();
    const QString &status = f[1];

    if (f.size() >= 3 && f[2] == QLatin1String("Searching for a match")) {
        int waiting = f[1].toInt();
        if (m_chooseMode) {
            m_chooseMode->onPreGameSearching(waiting, total); // Direct call
        }
    } else if (status == QLatin1String("Found")) {
        QStringList others = f.mid(2);
        if (m_chooseMode) {
            m_chooseMode->onPreGameFound(others); // Direct call
        }
    } else if (status == QLatin1String("FULL")) {
        QString msg = (f.size() >= 3 ? f[2] : tr("Server is full"));
        if (m_chooseMode) {
            m_chooseMode->onPreGameFull(msg); // Direct call
        }
    }
}
