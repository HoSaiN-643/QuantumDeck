#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player
{
public:
    explicit Player(); // سازنده پیشفرض

    ~Player() = default;

    // --- getters ---
    QString firstName () const { return m_firstname; }
    QString lastName  () const { return m_lastname;  }
    QString email     () const { return m_email;     }
    QString phone     () const { return m_phone;     }
    QString username  () const { return m_username;  }
    QString password  () const { return m_password;  }

    // --- setters ---
    void setFirstName (const QString &v) { m_firstname = v; }
    void setLastName  (const QString &v) { m_lastname  = v; }
    void setEmail     (const QString &v) { m_email     = v; }
    void setPhone     (const QString &v) { m_phone     = v; }
    void setUsername  (const QString &v) { m_username  = v; }
    void setPassword  (const QString &v) { m_password  = v; }

    QString fullName() const;

    void SetInfo(const QString &firstname,
                 const QString &lastname,
                 const QString &email,
                 const QString &phone,
                 const QString &username,
                 const QString &password);

    bool operator==(const Player &other) const;
    bool operator!=(const Player &other) const { return !(*this == other); }

private:
    QString m_firstname;
    QString m_lastname;
    QString m_email;
    QString m_phone;
    QString m_username;
    QString m_password;
};

#endif // PLAYER_H
