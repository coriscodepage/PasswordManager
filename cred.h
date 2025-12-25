#ifndef CRED_H
#define CRED_H

#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <QStringList>

class ValidationResult;

class Cred {

private:
    QString m_service;
    QString m_uname;
    QByteArray m_passwordBlob;

public:
    Cred(QString website, QString uname, QByteArray password_blob)
        : m_service(std::move(website))
        , m_uname(std::move(uname))
        , m_passwordBlob(std::move(password_blob))
    {}
    Cred() = default;
    QString getService() const {return m_service;}
    void setWebsite(const QString &website) {m_service = website;}
    QString getUsername() const {return m_uname;}
    void setUsername(const QString &uname) {m_uname = uname;}
    QByteArray getPasswordBlob() const {return m_passwordBlob;}
    void setPasswordBlob(const QByteArray &password_blob) {m_passwordBlob = password_blob;}
    static ValidationResult validate(const QString &service, const QString &user, const QString &password);
};

struct PasswordStrength {
    enum Ratings {
        WEAK = 30,
        MEDIUM = 50,
        GOOD = 90,
        VERYGOOD,
    };

    int score;
    QString rating;
    QStringList suggestions;

    PasswordStrength() : score(0), rating("Słabe") {}
    PasswordStrength(const QString &password);
};

struct ValidationResult {
    bool result;
    QString message;
    ValidationResult() : result(false), message("") {}
    bool isVaid() { return result; }
};

#endif // CRED_H
