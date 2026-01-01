#ifndef EDITCOMMAND_H
#define EDITCOMMAND_H

#include "credentialmodel.h"
#include <QUndoCommand>

class EditCommand : public QUndoCommand
{
    CredentialModel *m_model;
    Cred m_newData;
    Cred m_oldData;
    int m_row;

public:
    EditCommand(CredentialModel *model, int row, const Cred &data) : m_newData(data), m_model(model), m_row(row) {
        m_oldData = m_model->getCredentials().at(row);
        setText("Edytuj " + m_newData.getService());
    }
    void undo() override {
        m_model->updateCredential(m_oldData, m_row);
    }
    void redo() override {
        m_model->updateCredential(m_newData, m_row);
    }
};
#endif // EDITCOMMAND_H
