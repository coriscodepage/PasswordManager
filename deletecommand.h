#ifndef DELETECOMMAND_H
#define DELETECOMMAND_H

#include "credentialmodel.h"
#include <QUndoCommand>

class DeleteCommand : public QUndoCommand
{
public:
    CredentialModel *m_model;
    Cred m_data;
    int m_row;

public:
    DeleteCommand(CredentialModel *model, int row) : m_model(model), m_row(row) {
        m_data = model->getCredentials().at(m_row);
        setText("Usuń pozycję");
    }
    void undo() override {
        m_model->insertCredential(m_data, m_row);

    }
    void redo() override {
        m_model->removeCredential(m_row);
    }
};

#endif // DELETECOMMAND_H
