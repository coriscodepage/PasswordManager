#include "filemanager.h"
#include <qjsonarray.h>
#include "fileexception.h"
#include "vault.h"
#include <QFile>

FileManager::FileManager() : m_path("") {}

void FileManager::saveJson(const QVector<Cred> &creds) {
    if (!isPathSet()) throw FileException("Ścieżka nie została ustawiona.");
    QJsonArray enties_array;
    for (auto &c : creds) {
        enties_array.append(serializeEntry(c));
    }
    qDebug() << enties_array;
    QJsonDocument json_doc;
    json_doc.setArray(enties_array);

    QFile out_file(m_path);
    if(!out_file.open(QIODevice::WriteOnly)) {
        qDebug() << "[saveJson] File open error";
        throw FileException("Błąd w otwieraniu pliku.");
    }
    qDebug() << "[saveJson] Opened on path";
    EncryptedObject encrypted_json = Vault::getInstance().encryptJsonWithPassword(json_doc);
    qDebug() << "[saveJson] Encrypted";
    QDataStream out(&out_file);
    out << MAGIC;
    out << VERSION;
    out << encrypted_json.iv;
    out << encrypted_json.salt;
    out << encrypted_json.tag;
    out << encrypted_json.data;
}

QVector<Cred> FileManager::openJson() {
    if (!isPathSet()) throw FileException("Ścieżka nie została ustawiona.");
    QFile in_file(m_path);
    if(!in_file.open(QIODevice::ReadOnly)) {
        qDebug() << "[openJson] File open error";
        throw FileException("Błąd w otwieraniu pliku.");
    }
    QDataStream in(&in_file);
    quint16 magic;
    in >> magic;
    if (magic != MAGIC) {
        qWarning() << "[openJson] Magic invalid";
        throw FileException("Plik uszkodzony!");
    }
    quint8 version;
    in >> version;
    if (version > VERSION) {
        qWarning() << "[openJson] Version invalid";
        throw FileException("Niezgodna wersja pliku!");
    }
    QByteArray iv, salt, tag, cipher_text;
    in >> iv >> salt >> tag >> cipher_text;
    EncryptedObject in_data(iv, salt, tag, cipher_text);
    QJsonDocument decrypted_json = Vault::getInstance().decryptJsonWithPassword(in_data);
    QVector<Cred> credentials;
    for (const auto &c : decrypted_json.array()) {
        credentials.append(deserializeEntry(c.toObject()));
    }
    qDebug() << "[openJson] Decrypted";
    return credentials;
}

QJsonObject FileManager::serializeEntry(const Cred &cred) {
    QJsonObject entry;
    entry.insert("Service", cred.getService());
    entry.insert("Username", cred.getUsername());
    entry.insert("Password", Vault::getInstance().decrypt(cred.getPasswordBlob()));
    entry.insert("Uses", cred.getUseCount());
    entry.insert("Order", cred.getManualOrder());
    return entry;
}


Cred FileManager::deserializeEntry(const QJsonObject &object) {
    QString service = object["Service"].toString();
    QString username= object["Username"].toString();
    QByteArray password_blob = Vault::getInstance().encrypt(object["Password"].toString());
    int uses = object["Uses"].toInt();
    int order = object["Order"].toInt();
    return Cred(service, username, password_blob, order, uses);
}
