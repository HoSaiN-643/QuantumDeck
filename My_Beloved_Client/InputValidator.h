#pragma once
#include <QString>
#include <QRegularExpression>
#include <QStringList>

class InputValidator
{
public:
    static QString validateEmail(const QString& email);
    static QString validatePhone(const QString& phone);
    static QString validateUsername(const QString& username);
    static QString validatePassword(const QString& password);
    static QString validateName(const QString& name, const QString& fieldName);
    static const QStringList commonPasswords;
};
