#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <QObject>

class validators
{
public:
    validators();
    static bool IsAddressValid(QString);
    static bool IsPortValid(QString);
};

#endif // VALIDATORS_H
