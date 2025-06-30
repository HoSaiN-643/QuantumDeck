
#include <QJsonValue>
#include "player.h"

// constructor
Player::Player() : m_firstname(""), m_lastname(""), m_email(""), m_phone(""), m_username(""), m_password("") {}

Player::Player(const QString &firstname,
               const QString &lastname,
               const QString &email,
               const QString &phone,
               const QString &username,
               const QString &password)
    : m_firstname(firstname),
    m_lastname (lastname),
    m_email    (email),
    m_phone    (phone),
    m_username (username),
    m_password (password)
{
}

// نام کامل
QString Player::fullName() const
{
    return m_firstname + " " + m_lastname;
}

// تبدیل به QJsonObject
QJsonObject Player::toJson() const
{
    QJsonObject obj;
    obj["firstname"] = m_firstname;
    obj["lastname"]  = m_lastname;
    obj["email"]     = m_email;
    obj["phone"]     = m_phone;
    obj["username"]  = m_username;
    obj["password"]  = m_password;
    return obj;
}

// ساخت از QJsonObject
Player Player::fromJson(const QJsonObject &obj)
{
    return Player(
        obj.value("firstname").toString(),
        obj.value("lastname").toString(),
        obj.value("email").toString(),
        obj.value("phone").toString(),
        obj.value("username").toString(),
        obj.value("password").toString()
        );
}

// اپراتور ==
bool Player::operator==(const Player &o) const
{
    return m_firstname == o.m_firstname
           && m_lastname  == o.m_lastname
           && m_email     == o.m_email
           && m_phone     == o.m_phone
           && m_username  == o.m_username
           && m_password  == o.m_password;
}
