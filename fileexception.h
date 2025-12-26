#ifndef FILEEXCEPTION_H
#define FILEEXCEPTION_H

#include<QException>

class FileException : public QException
{
public:
    explicit FileException(const QString &message) : m_message(message) {}

    void raise() const override { throw *this; }
    FileException *clone() const override { return new FileException(*this); }
    virtual const char *what() const noexcept override { return m_message.toUtf8().constData(); }

private:
    QString m_message;
};

#endif // FILEEXCEPTION_H
