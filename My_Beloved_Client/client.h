#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class Player;
class Login;
class Connect;
class Signup;
class Choose_mode;
class MainMenu;
class GameWindow;
class History;
class Change_profile;

class Client : public QObject
{
    Q_OBJECT

public:
    void WriteToServer(const QString &data);
    QStringList extractFields(const QString &payload) const;
    QTcpSocket* m_socket;
    explicit Client(QObject *parent = nullptr);
    ~Client();

    void ConnectToServer(const QHostAddress &host, quint16 port);
    void DisconnectFromServer();
    Player& GetPlayer();

    void sendCardSelection(const QString &card);
    void sendChatMessage(const QString &message);

    void SetConnect(Connect* connect) { m_connect = connect; }
    void SetSignup(Signup* signup) { m_signup = signup; }
    void SetLogin(Login* login) { loginWindow = login; }
    void SetChooseMode(Choose_mode* chooseMode) { m_chooseMode = chooseMode; }
    void SetMainMenu(MainMenu* mainMenu) { menu = mainMenu; }
    void SetGameWindow(GameWindow* gameWindow) { gw = gameWindow; }
    void SetHistory(History* history) { this->history = history; }
    void SetChangeProfile(Change_profile* changeProfile) { cf = changeProfile; }

    Connect* GetConnect() const { return m_connect; }
    Signup* GetSignup() const { return m_signup; }
    Login* GetLogin() const { return loginWindow; }
    Choose_mode* GetChooseMode() const { return m_chooseMode; }
    MainMenu* GetMainMenu() const { return menu; }
    GameWindow* GetGameWindow() const { return gw; }
    History* GetHistory() const { return history; }
    Change_profile* GetChangeProfile() const { return cf; }

    QTcpSocket* getSocket() const { return m_socket; }

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void OnReadyRead();

private:
    Player* player;
    Login* loginWindow;
    Connect* m_connect;
    Signup* m_signup;
    Choose_mode* m_chooseMode;
    Change_profile* cf;
    MainMenu* menu;
    GameWindow* gw;
    History* history;
    QByteArray readBuffer;

    void handleLogin(const QStringList &fields);
    void handleSignup(const QStringList &fields);
    void handleRecover(const QStringList &fields);
    void handleErrorCmd(const QStringList &fields);
    void handlePreGame(const QStringList &fields);
    void handleGame(const QStringList &fields);
    void handleUpdateProfile(const QStringList &fields);
};

#endif // CLIENT_H
