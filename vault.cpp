#include "vault.h"
#include <QDebug>

Vault::Vault(QObject *parent)
    : QObject{parent} {
    qDebug() << "OpenSSL Version:" << OPENSSL_VERSION_TEXT;
}

QByteArray Vault::encrypt(const QString &text) {
    QByteArray data = text.toUtf8();
    QByteArray iv(16, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), 16);

    QByteArray cipher_text;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL,
                       reinterpret_cast<unsigned char*>(sessionKey().data()),
                       reinterpret_cast<unsigned char*>(iv.data()));

    QByteArray out(data.size() + 16, 0);
    int len;
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len,
                      reinterpret_cast<const unsigned char*>(data.constData()), data.size());

    int final_len;
    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + len, &final_len);

    cipher_text = iv + out.left(len + final_len);

    EVP_CIPHER_CTX_free(ctx);
    return cipher_text;
}

QString Vault::decrypt(const QByteArray &cypher) {
    if (cypher.size() < 16) return QString();

    QByteArray iv = cypher.left(16);
    QByteArray encrypted = cypher.mid(16);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL,
                       reinterpret_cast<unsigned char*>(sessionKey().data()),
                       reinterpret_cast<unsigned char*>(iv.data()));

    QByteArray plainText(encrypted.size(), 0);
    int len;
    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plainText.data()), &len,
                      reinterpret_cast<const unsigned char*>(encrypted.constData()), encrypted.size());

    EVP_CIPHER_CTX_free(ctx);
    return QString::fromUtf8(plainText.left(len));
}

void Vault::encryptJsonWithPassword(const QJsonDocument &json, const QString &password) {

}
