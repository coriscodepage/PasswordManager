#ifndef VAULT_H
#define VAULT_H

#include "cred.h"
#include <QObject>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QDebug>

struct EncryptedObject;

class Vault : public QObject
{

private:
    Q_OBJECT
    explicit Vault(QObject *parent = nullptr);
    QByteArray m_masterPassword;

    const int AES_KEY_LENGTH = 32;
    const int AES_IV_LENGTH = 16;
    const int SALT_LENGTH = 16;
    const int ITERATIONS = 100000;

    QByteArray& sessionKey() {
        static QByteArray key;
        if (key.isEmpty()) {
            key.resize(AES_KEY_LENGTH);
            RAND_bytes(reinterpret_cast<unsigned char*>(key.data()), AES_KEY_LENGTH);
            qDebug() << "[VAULT] Session key created";
        }
        return key;
    }

public:
    static Vault &getInstance() {
        static Vault _instance;
        return _instance;
    }

    Vault(Vault const&) = delete;
    void operator=(Vault const&) = delete;

    EncryptedObject encryptJsonWithPassword(const QJsonDocument &json);
    QJsonDocument decryptJsonWithPassword(const EncryptedObject &data);
    QVector<Cred> load(const QString &path);
    QByteArray encrypt(const QString &text);
    QString decrypt(const QByteArray &cypher);
    bool isMasterPasswordSet() const { return !m_masterPassword.isEmpty(); }
    void setMasterPassword(const QString &password) { m_masterPassword = encrypt(password); }

    void resetSession() {
        QByteArray& key = sessionKey();

        if (!key.isEmpty())
            OPENSSL_cleanse(key.data(), key.size());
        if (!m_masterPassword.isEmpty())
            OPENSSL_cleanse(m_masterPassword.data(), m_masterPassword.size());
        m_masterPassword.clear();
        key.clear();
        qDebug() << "[VAULT] Session renewed";
    }

signals:
};

struct EncryptedObject {
    QByteArray iv;
    QByteArray salt;
    QByteArray tag;
    QByteArray data;

    EncryptedObject(const QByteArray &iv, const QByteArray &salt, const QByteArray &tag, const QByteArray &data) : salt(salt), tag(tag), iv(iv), data(data) {}
};

#endif // VAULT_H
