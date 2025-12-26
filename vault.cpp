#include "vault.h"
#include "cryptoexception.h"
#include <qjsondocument.h>

Vault::Vault(QObject *parent)
    : QObject{parent} {
    qDebug() << "[VAULT] OpenSSL Version:" << OPENSSL_VERSION_TEXT;
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
    int len, final_len;
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len,
                      reinterpret_cast<const unsigned char*>(data.constData()), data.size());

    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + len, &final_len);

    cipher_text = iv + out.left(len + final_len);

    EVP_CIPHER_CTX_free(ctx);
    qDebug() << "[VAULT] Encrypted plain text";
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

    QByteArray plain_text(encrypted.size(), 0);
    int len;
    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plain_text.data()), &len,
                      reinterpret_cast<const unsigned char*>(encrypted.constData()), encrypted.size());

    EVP_CIPHER_CTX_free(ctx);
    qDebug() << "[VAULT] Decrypted cypher";
    return QString::fromUtf8(plain_text.left(len));
}

EncryptedObject Vault::encryptJsonWithPassword(const QJsonDocument &json) {

    QByteArray salt(SALT_LENGTH, 0);
    QByteArray iv(AES_IV_LENGTH, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(salt.data()), SALT_LENGTH);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), AES_IV_LENGTH);
    QString password = decrypt(m_masterPassword);
    QByteArray key(AES_KEY_LENGTH, 0);
    PKCS5_PBKDF2_HMAC(password.toUtf8().constData(), password.length(),
                      reinterpret_cast<const unsigned char*>(salt.constData()), SALT_LENGTH,
                      ITERATIONS, EVP_sha256(), AES_KEY_LENGTH,
                      reinterpret_cast<unsigned char*>(key.data()));

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL,
                       reinterpret_cast<unsigned char*>(key.data()),
                       reinterpret_cast<unsigned char*>(iv.data()));

    QByteArray plain_text = json.toJson(QJsonDocument::Compact);
    QByteArray out(plain_text.size(), 0);
    QByteArray tag(16, 0);
    int len, final_len;
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len,
                      reinterpret_cast<const unsigned char*>(plain_text.constData()), plain_text.size());
    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + len, &final_len);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data());
    EVP_CIPHER_CTX_free(ctx);

    return EncryptedObject(iv, salt, tag, out);
}

QJsonDocument Vault::decryptJsonWithPassword(const EncryptedObject &enc_data) {

    QByteArray key(AES_KEY_LENGTH, 0);
    QString password = decrypt(m_masterPassword);
    PKCS5_PBKDF2_HMAC(password.toUtf8().constData(), password.length(),
                      reinterpret_cast<const unsigned char*>(enc_data.salt.constData()), SALT_LENGTH,
                      ITERATIONS, EVP_sha256(), AES_KEY_LENGTH,
                      reinterpret_cast<unsigned char*>(key.data()));

    QByteArray plain_text(enc_data.data.size(), 0);

    int len, final_len;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL,
                       reinterpret_cast<const unsigned char*>(key.constData()),
                       reinterpret_cast<const unsigned char*>(enc_data.iv.constData()));

    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plain_text.data()), &len,
                      reinterpret_cast<const unsigned char*>(enc_data.data.constData()), enc_data.data.size());

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, const_cast<char*>(enc_data.tag.data()));

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plain_text.data()) + len, &final_len) <= 0) {
        EVP_CIPHER_CTX_free(ctx);
        plain_text.fill(0);
        qWarning() << "[VAULT] File decrypt failed";
        throw CryptoException("Niepoprawne hasło lub uszkodzony plik!");
    }
    EVP_CIPHER_CTX_free(ctx);
    QJsonDocument doc = QJsonDocument::fromJson(plain_text.left(len + final_len));
    return doc;
}
