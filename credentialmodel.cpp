#include "credentialmodel.h"
#include "vault.h"

CredentialModel::CredentialModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_nextOrder(0)
{}

QVariant CredentialModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case Columns::SERVICE:
            return "Serwis";
        case Columns::USERNAME:
            return "Login";
        case Columns::PASSWORD:
            return "Hasło";
        default:
            return QVariant();
    }
}

int CredentialModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.count();
}

int CredentialModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 5; // FIXME: Remember to set it correctly!
}

QVariant CredentialModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const auto &concrete_cred = m_data[index.row()];

    switch (index.column()) {
        case Columns::SERVICE:
            return concrete_cred.getService();
        case Columns::USERNAME:
            return concrete_cred.getUsername();
        case Columns::PASSWORD:
            if (m_visibility) {
                if (m_decryptedCache.contains(index.row()))
                    return m_decryptedCache[index.row()];

                QString cachable_passwd = Vault::getInstance().decrypt(concrete_cred.getPasswordBlob());
                m_decryptedCache[index.row()] = cachable_passwd;
                return cachable_passwd;
            }
            return "********";
        case Columns::COUNT:
            return concrete_cred.getUseCount();
        case Columns::ORDER:
            return concrete_cred.getManualOrder();
        default:
            return QVariant();
    }
}

void CredentialModel::setCredentials(const QVector<Cred> &data) {
    beginResetModel();
    m_data = data;
    endResetModel();
}

void CredentialModel::clearCredentials() {
    beginResetModel();
    m_data.clear();
    endResetModel();
}

void CredentialModel::addCredential(const Cred &cred) {
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.append(cred);
    endInsertRows();
}

void CredentialModel::insertCredential(const Cred &cred, int row) {
    if (row < 0 || row >= m_data.count()) return;
    beginInsertRows(QModelIndex(), row, row);
    m_data.insert(row, cred);
    endInsertRows();
}

void CredentialModel::updateCredential(const Cred &cred, int row) {
    if (row < 0 || row >= m_data.count()) return;
    m_data[row] = cred;
    QModelIndex top_left = index(row, 0);
    QModelIndex bottom_right = index(row, columnCount() - 1);
    emit dataChanged(top_left, bottom_right, {Qt::DisplayRole, Qt::EditRole});
}

void CredentialModel::removeCredential(int row) {
    if (row < 0 || row >= m_data.count()) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_data.remove(row);
    endRemoveRows();
}

void CredentialModel::changeVisibility() {
    setVisibility(!m_visibility);
}

void CredentialModel::setVisibility(bool visibility) {
    if (m_visibility == visibility) return;

    m_visibility = visibility;

    if (!visibility)
        m_decryptedCache.clear();

    if (!m_data.isEmpty()) {
        QModelIndex top_left = index(0, Columns::PASSWORD);
        QModelIndex bottom_right = index(m_data.size() - 1, Columns::PASSWORD);
        emit dataChanged(top_left, bottom_right, {Qt::DisplayRole});
    }
}

void CredentialModel::incrementUseCount(int row) {
    if (row < 0 || row >= m_data.count()) return;
    m_data[row].incUseCount();
    QModelIndex top_left = index(row, Columns::COUNT);
    QModelIndex bottom_right = index(row, Columns::COUNT);
    emit dataChanged(top_left, bottom_right, {Qt::DisplayRole});
}

void CredentialModel::swapManualOrder(int row_a, int row_b) {
    if (row_a < 0 || row_b < 0 || row_a >= m_data.size() || row_b >= m_data.size()) return;
    int r1 = m_data[row_a].getManualOrder();
    m_data[row_a].setManualOrder(m_data[row_b].getManualOrder());
    m_data[row_b].setManualOrder(r1);
    QModelIndex top_left = index(qMin(row_a, row_b), 0);
    QModelIndex bottom_right = index(qMax(row_a, row_b), columnCount() - 1);
    emit dataChanged(top_left, bottom_right, {Qt::DisplayRole});
}


