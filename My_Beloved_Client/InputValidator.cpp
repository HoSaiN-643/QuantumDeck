#include "InputValidator.h"

const QStringList InputValidator::commonPasswords = {
    "password",
    "12345678",
    "123456789",
    "qwerty123",
    "abc12345",
    "iloveyou",
    "admin123",
    "letmein1",
    "welcome1",
    "password1",
    "1234567890",
};

// Name validator: min 2 char
// Name validator: فقط حروف الفبا (فارسی یا انگلیسی)، حداقل ۲ کاراکتر
QString InputValidator::validateName(const QString& name, const QString& fieldName) {
    QString trimmed = name.trimmed();
    if (trimmed.length() < 2)
        return QString("%1 must be at least 2 characters.").arg(fieldName);

    // regex: فقط حروف الفبای فارسی یا انگلیسی
    QRegularExpression r(R"(^[a-zA-Zآ-ی]{2,}$)"); // ۲ کاراکتر یا بیشتر
    if (!r.match(trimmed).hasMatch())
        return QString("%1 must contain only letters (a-z, A-Z, or آ-ی).").arg(fieldName);

    return "";
}

// Email validator
QString InputValidator::validateEmail(const QString& email) {
    QRegularExpression r(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!r.match(email).hasMatch())
        return "Invalid email format.";
    return "";
}
// Phone validator
QString InputValidator::validatePhone(const QString& phone) {
    QRegularExpression r(R"(^0\d{10,14}$)");
    if (!r.match(phone).hasMatch())
        return "Phone number must start with 0 and be 11-15 digits.";
    return "";
}
// Username validator: only a-zA-Z0-9_, 5-20 chars
QString InputValidator::validateUsername(const QString& username) {
    QRegularExpression r(R"(^[a-zA-Z0-9_]{5,20}$)");
    if (!r.match(username).hasMatch())
        return "Username must be 5–20 English letters, numbers, or underscores.";
    return "";
}
// Password validator
QString InputValidator::validatePassword(const QString& password) {
    if (password.length() < 8)
        return "Password must be at least 8 characters.";
    if (commonPasswords.contains(password))
        return "Password is too weak.";
    return "";
}
