#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QString>
#include <QRegularExpression>

class InputValidator {
public:
    static const QStringList commonPasswords;

    static QString validateEmail(const QString& email);
    static QString validatePhone(const QString& phone);
    static QString validatePassword(const QString& password);
    static QString validateAddress(const QString& address);
    static QString validatePort(const QString& port);
};

#endif // INPUTVALIDATOR_H