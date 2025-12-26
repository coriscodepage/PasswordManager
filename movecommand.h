#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "credentialmodel.h"
#include <QUndoCommand>
#include <qheaderview.h>
#include <qsortfilterproxymodel.h>
#include <QTableView>

class MoveCommand : public QUndoCommand {
    CredentialModel *m_model;
    QSortFilterProxyModel *m_proxy;
    QTableView *m_tableView;
    int m_rowA, m_rowB;

public:
    MoveCommand(CredentialModel *model, QSortFilterProxyModel *proxy, int a, int b, QTableView *view)
        : m_model(model)
        , m_rowA(a)
        , m_rowB(b)
        , m_proxy(proxy)
        , m_tableView(view)
    {
        setText("Przesuń wiersze");
    }

    void redo() override {
        m_proxy->sort(CredentialModel::ORDER, Qt::AscendingOrder);
        m_tableView->horizontalHeader()->setSortIndicatorShown(false);
        m_model->swapManualOrder(m_rowA, m_rowB);
    }
    void undo() override {
        m_proxy->sort(CredentialModel::ORDER, Qt::AscendingOrder);
        m_tableView->horizontalHeader()->setSortIndicatorShown(false);
        m_model->swapManualOrder(m_rowA, m_rowB);
    }
};
#endif // MOVECOMMAND_H
