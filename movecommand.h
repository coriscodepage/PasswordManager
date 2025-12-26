#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "credentialmodel.h"
#include <QUndoCommand>
#include <qsortfilterproxymodel.h>

class MoveCommand : public QUndoCommand {
    CredentialModel *m_model;
    QSortFilterProxyModel *m_proxy;
    int m_rowA, m_rowB;

public:
    MoveCommand(CredentialModel *model, QSortFilterProxyModel *proxy, int a, int b)
        : m_model(model)
        , m_rowA(a)
        , m_rowB(b)
        , m_proxy(proxy)
    {
        setText("Przesuń wiersze");
    }

    void redo() override {
        m_proxy->sort(CredentialModel::ORDER, Qt::AscendingOrder);
        m_model->swapManualOrder(m_rowA, m_rowB);
    }
    void undo() override {
        m_proxy->sort(CredentialModel::ORDER, Qt::AscendingOrder);
        m_model->swapManualOrder(m_rowA, m_rowB);
    }
};
#endif // MOVECOMMAND_H
