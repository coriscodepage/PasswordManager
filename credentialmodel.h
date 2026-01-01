#ifndef CREDENTIALMODEL_H
#define CREDENTIALMODEL_H

#include "cred.h"
#include <QAbstractTableModel>

class CredentialModel : public QAbstractTableModel
{
private:
    Q_OBJECT
    QVector<Cred> m_data;
    mutable QHash<int, QString> m_decryptedCache;
    bool m_visibility = false;
    int m_nextOrder;


public:
    enum Columns {
        SERVICE,
        USERNAME,
        PASSWORD,
        COUNT,
        ORDER,
    };
    explicit CredentialModel(QObject *parent = nullptr);

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // ADDED METHODS
    void setCredentials(const QVector<Cred> &data);
    void clearCredentials();
    void addCredential(const Cred &cred);
    void updateCredential(Cred &cred, int row);
    QVector<Cred> getCredentials() const { return m_data; }
    void insertCredential(const Cred &cred, int row);
    void removeCredential(int row);
    void changeVisibility();
    void setVisibility(bool visibility);
    void incrementUseCount(int row);
    void swapManualOrder(int row_a, int row_b);
    int getNextOrder() { return m_nextOrder++; }
    void setNextOrder(int ord) { m_nextOrder = ord; }
};

#endif // CREDENTIALMODEL_H
