#ifndef VAULT_H
#define VAULT_H

#include "cred.h"
#include <QObject>
#include <openssl/evp.h>
#include <openssl/rand.h>

class Vault : public QObject
{

private:
    Q_OBJECT
    explicit Vault(QObject *parent = nullptr);
    QByteArray m_file_key;

    const int AES_KEY_LENGTH = 32;
    const int AES_IV_LENGTH = 16;
    const int SALT_LENGTH = 16;
    const int ITERATIONS = 100000;

    QByteArray& sessionKey() {
        static QByteArray key;
        if (key.isEmpty()) {
            key.resize(AES_KEY_LENGTH);
            RAND_bytes(reinterpret_cast<unsigned char*>(key.data()), AES_KEY_LENGTH);
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

    void encryptJsonWithPassword(const QJsonDocument &json, const QString &password);
    QVector<Cred> load(const QString &path);
    QByteArray encrypt(const QString &text);
    QString decrypt(const QByteArray &cypher);

    void resetSessionKey() {
        QByteArray& key = sessionKey();

        if (!key.isEmpty()) {
            OPENSSL_cleanse(key.data(), key.size());
        }

        key.clear();
    }

signals:
};

#endif // VAULT_H
