#include "player.h"

Player::Player()
    : m_firstname(""),
    m_lastname(""),
    m_email(""),
    m_phone(""),
    m_username(""),
    m_password("")
{}

QString Player::fullName() const
{
    return m_firstname + " " + m_lastname;
}

void Player::SetInfo(const QString &firstname,
                     const QString &lastname,
                     const QString &email,
                     const QString &phone,
                     const QString &username,
                     const QString &password)
{
    m_firstname = firstname;
    m_lastname  = lastname;
    m_email     = email;
    m_phone     = phone;
    m_username  = username;
    m_password  = password;
}

bool Player::operator==(const Player &other) const
{
    return m_firstname == other.m_firstname &&
           m_lastname  == other.m_lastname  &&
           m_email     == other.m_email     &&
           m_phone     == other.m_phone     &&
           m_username  == other.m_username  &&
           m_password  == other.m_password;
}
