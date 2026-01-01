#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "cred.h"
#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <QJsonObject>
#include <QObject>

class FileManager : public QObject
{
    Q_OBJECT
public:
    FileManager();
    bool isPathSet() const { return !m_path.isEmpty(); }
    void setPath(const QString &path);
    void clearPath() { m_path.clear(); }
    void saveJson(const QVector<Cred> &data);
    QVector<Cred> openJson();

private:
    QString m_path;
    QJsonObject serializeEntry(const Cred &cred);
    Cred deserializeEntry(const QJsonObject &object);

    const quint16 MAGIC = 0xFAAF;
    const quint8 VERSION = 1;
signals:
    void pathChanged(const QString &path);
};

#endif // FILEMANAGER_H
