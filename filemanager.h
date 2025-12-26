#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "cred.h"
#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <QJsonObject>

class FileManager
{
public:
    FileManager();
    bool isPathSet() const { return !m_path.isEmpty(); }
    void setPath(const QString &path) { m_path = path; }
    void clearPath() { m_path.clear(); }
    void saveJson(const QVector<Cred> &data);
    QVector<Cred> openJson();

private:
    QString m_path;
    QJsonObject serializeEntry(const Cred &cred);
    Cred deserializeEntry(const QJsonObject &object);

    const quint16 MAGIC = 0xFAAF;
    const quint8 VERSION = 1;
};

#endif // FILEMANAGER_H
