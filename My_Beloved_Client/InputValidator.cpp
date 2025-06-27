#include "InputValidator.h"
#include <QRegularExpression>

const QStringList InputValidator::commonPasswords = {
    "password",
    "123456",
    "123456789",
    "qwerty",
    "abc123"
};

QString InputValidator::validateEmail(const QString& email) {
    QRegularExpression emailRegex("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}");
    QRegularExpressionMatch match = emailRegex.match(email);
    if (!match.hasMatch()) {
        return "email format is not correct";
    }
    return "";
}

QString InputValidator::validatePhone(const QString& phone) {
    QString digits = phone.remove(QRegularExpression("\\D"));
    if (digits.length() < 10 || digits.length() > 15) {
        return " phone number format is not correct";
    }
    return "";
}

QString InputValidator::validatePassword(const QString& password) {
    if (password.length() < 8) {
        return " password  at least 8 charcters";
    }
    if (commonPasswords.contains(password)) {
        return "password is not strong"
    }
    return "";
}

QString InputValidator::validateAddress(const QString& address) {
    if (address.trimmed().isEmpty()) {
        return "address is empty"
    }
    return "";
}

QString InputValidator::validatePort(const QString& port) {
    bool ok;
    int p = port.toInt(&ok);
    if (!ok || p < 1 || p > 65535) {
        return "invalid port";
    }
    return "";
}