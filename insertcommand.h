#ifndef INSERTCOMMAND_H
#define INSERTCOMMAND_H

#include "credentialmodel.h"
#include <QUndoCommand>

class InsertCommand : public QUndoCommand
{
    CredentialModel *m_model;
    Cred m_data;
    int m_row;

public:
    InsertCommand(CredentialModel *model, const Cred &data) : m_data(data), m_model(model), m_row(m_model->rowCount()) {
        setText("Dodaj pozycję");
    }
    void undo() override {
        m_model->removeCredential(m_row);
    }
    void redo() override {
        m_model->addCredential(m_data);
    }
};

#endif // INSERTCOMMAND_H
