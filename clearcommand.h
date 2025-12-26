#ifndef CLEARCOMMAND_H
#define CLEARCOMMAND_H

#include "credentialmodel.h"
#include <QUndoCommand>

class ClearCommand : public QUndoCommand
{
public:
    CredentialModel *m_model;
    QVector<Cred> m_data;
    int m_savedOrder;

public:
    ClearCommand(CredentialModel *model) : m_model(model), m_data(m_model->getCredentials()) {
        setText("Usuń wszystko");
    }
    void undo() override {
        m_model->setNextOrder(m_savedOrder);
        m_model->setCredentials(m_data);
    }
    void redo() override {
        m_savedOrder = m_model->getNextOrder();
        m_model->clearCredentials();
    }
};

#endif // CLEARCOMMAND_H
