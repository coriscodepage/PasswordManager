#ifndef CRED_H
#define CRED_H

#include <qcontainerfwd.h>
#include <qhashfunctions.h>
class Cred {

private:
    QString m_website;
    QString m_uname;
    QByteArray m_passwordBlob;

public:
    Cred(QString website, QString uname, QByteArray password_blob)
        : m_website(std::move(website))
        , m_uname(std::move(uname))
        , m_passwordBlob(std::move(password_blob))
    {}

    QString getWebsite() const {return m_website;}
    void setWebsite(const QString &website) {m_website = website;}
    QString getUsername() const {return m_uname;}
    void setUsername(const QString &uname) {m_uname = uname;}
    QByteArray getPasswordBlob() const {return m_passwordBlob;}
    void setPasswordBlob(const QByteArray &password_blob) {m_passwordBlob = password_blob;}
};

#endif // CRED_H
