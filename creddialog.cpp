#include "creddialog.h"
#include "ui_creddialog.h"

CredDialog::CredDialog(QWidget *parent, bool edit_mode)
    : QDialog(parent)
    , ui(new Ui::EditDialog),
    m_editMode(edit_mode)
{
    ui->setupUi(this);

}

CredDialog::~CredDialog()
{
    delete ui;
}

