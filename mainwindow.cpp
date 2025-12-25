#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "clearcommand.h"
#include "creddialog.h"
#include "credentialmodel.h"
#include "deletecommand.h"
#include "editcommand.h"
#include "insertcommand.h"
#include "vault.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // INFO: Model and sorting (QSortFilterProxyModel) setup
    m_model = new CredentialModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->TableView->setModel(m_proxy);
    ui->TableView->setSortingEnabled(true);

    // INFO: UI plumbing
    connect(ui->AddButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->ClearAllButton, &QPushButton::clicked, this, &MainWindow::onDeleteAllClicked);
    connect(ui->PasswordVisibilityButton, &QPushButton::clicked, m_model, &CredentialModel::changeVisibility);
    connect(ui->SearchEdit, &QLineEdit::textChanged, m_proxy, &QSortFilterProxyModel::setFilterFixedString); // INFO: Search using ONLY the Service column!
    connect(ui->TableView, &QTableView::doubleClicked, this, &MainWindow::onEditClicked);


    // INFO: Undo stack and context menu setup (could be simpler but I insist on using actions from the designer)
    m_commandStack = new QUndoStack(this);

    connect(m_commandStack, &QUndoStack::cleanChanged, this, &MainWindow::onCleanChanged);

    QAction *stack_undo = m_commandStack->createUndoAction(this, "Cofnij");
    QAction *stack_redo = m_commandStack->createRedoAction(this, "Powtórz");

    connect(stack_undo, &QAction::changed, this, [this, stack_undo]() {
        ui->actionUndo->setText(stack_undo->text());
        ui->actionUndo->setEnabled(stack_undo->isEnabled());
    });

    connect(stack_redo, &QAction::changed, this, [this, stack_redo]() {
        ui->actionRedo->setText(stack_redo->text());
        ui->actionRedo->setEnabled(stack_redo->isEnabled());
    });

    connect(ui->actionUndo, &QAction::triggered, stack_undo, &QAction::trigger);
    connect(ui->actionRedo, &QAction::triggered, stack_redo, &QAction::trigger);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddClicked() {
    CredDialog *dialog = new CredDialog(this);
    dialog->show();
    if (dialog->exec() == QDialog::Accepted) {
        if (dialog->result() == CredDialog::Result::Saved) {
            Cred entry_to_add(dialog->getService(), dialog->getUsername(), Vault::getInstance().encrypt(dialog->getPassword()));
            m_commandStack->push(new InsertCommand(m_model, entry_to_add));
        }
    }
}

void MainWindow::onEditClicked(const QModelIndex &proxyIndex) {
    if (!proxyIndex.isValid()) return;
    QModelIndex source_index = m_proxy->mapToSource(proxyIndex);
    int row = source_index.row();
    if (row < 0) return;
    Cred current_entry = m_model->getCredentials().at(row);
    CredDialog *dialog = new CredDialog(this, true);
    dialog->setData(current_entry.getService(), current_entry.getUsername(), Vault::getInstance().decrypt(current_entry.getPasswordBlob()));
    dialog->show();
    if (dialog->exec() == QDialog::Accepted) {
        if (dialog->result() == CredDialog::Result::Saved) {
            Cred entry_to_edit(dialog->getService(), dialog->getUsername(), Vault::getInstance().encrypt(dialog->getPassword()));
            m_commandStack->push(new EditCommand(m_model, row, entry_to_edit));
        } else if (dialog->result() == CredDialog::Result::Deleted) {
            m_commandStack->push(new DeleteCommand(m_model, row));
        }
    }
}

void MainWindow::onDeleteAllClicked() {
    if(m_model->getCredentials().empty()) return;
    int res = QMessageBox::question(this, "Usuwanie danych", "Czy wyczyścić listę haseł?");
    if (res == QMessageBox::Yes)
        m_commandStack->push(new ClearCommand(m_model));
}

void MainWindow::onCleanChanged(bool clean) {
    setWindowModified(!clean);
    m_clean = clean;
}
