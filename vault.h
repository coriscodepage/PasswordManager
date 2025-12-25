#ifndef VAULT_H
#define VAULT_H

#include "cred.h"
#include <QObject>

class Vault : public QObject
{

private:
    Q_OBJECT
    explicit Vault(QObject *parent = nullptr);
    QString m_key;

public:
    static Vault &getInstance() {
        static Vault _instance;
        return _instance;
    }

    Vault(Vault const&) = delete;
    void operator=(Vault const&) = delete;

    void setKey(const QString &key) {m_key = key;}
    void save(const QString &path, const QVector<Cred> &data);
    QVector<Cred> load(const QString &path);


    QByteArray encrypt(const QString &text) { return text.toUtf8(); }               // TODO: Make ACTUAL encryption
    QString decrypt(const QByteArray &cypher) { return QString::fromUtf8(cypher); } // TODO: Make ACTUAL decryption


signals:
};

#endif // VAULT_H
