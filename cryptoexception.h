#ifndef CRYPTOEXCEPTION_H
#define CRYPTOEXCEPTION_H

#include<QException>

class CryptoException : public QException
{
public:
    explicit CryptoException(const QString &message) : m_message(message) {}

    void raise() const override { throw *this; }
    CryptoException *clone() const override { return new CryptoException(*this); }
    virtual const char *what() const noexcept override { return m_message.toUtf8().constData(); }

private:
    QString m_message;
};

#endif // CRYPTOEXCEPTION_H
