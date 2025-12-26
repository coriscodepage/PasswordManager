#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "clearcommand.h"
#include "creddialog.h"
#include "credentialmodel.h"
#include "deletecommand.h"
#include "editcommand.h"
#include "insertcommand.h"
#include "movecommand.h"
#include "vault.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_clipboard = QGuiApplication::clipboard();
    // INFO: Model and sorting (QSortFilterProxyModel) setup
    m_model = new CredentialModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->TableView->setModel(m_proxy);
    ui->TableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->TableView->setSortingEnabled(true);

    // INFO: UI plumbing
    connect(ui->AddButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->ClearAllButton, &QPushButton::clicked, this, &MainWindow::onDeleteAllClicked);
    connect(ui->PasswordVisibilityButton, &QPushButton::clicked, m_model, &CredentialModel::changeVisibility);

    connect(ui->UseCountOrderingButton, &QPushButton::clicked, this, &MainWindow::onUsageOrdering);
    connect(ui->ManualOrderingButton, &QPushButton::clicked, this, &MainWindow::onManualOrdering);
    connect(ui->MoveUpButton, &QPushButton::clicked, this, &MainWindow::onMoveUp);
    connect(ui->MoveDownButton, &QPushButton::clicked, this, &MainWindow::onMoveDown);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNew);
    connect(ui->actionClose, &QAction::triggered, this, &QApplication::quit, Qt::QueuedConnection);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSave);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onSaveAs);
    connect(ui->actionInfo, &QAction::triggered, this, &MainWindow::onAbout);

    connect(ui->SearchEdit, &QLineEdit::textChanged, m_proxy, &QSortFilterProxyModel::setFilterFixedString); // INFO: Search using ONLY the Service column!

    connect(ui->TableView, &QTableView::doubleClicked, this, &MainWindow::onEditClicked);

    // INFO: Setting a CustomContextMenu and using the signal for password copy
    ui->TableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->TableView, &QTableView::customContextMenuRequested, this, &MainWindow::onCopyPassword);

    // INFO: If user clicks on the header disable the special sorting modes
    connect(ui->TableView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, [this]() {
        ui->TableView->horizontalHeader()->setSortIndicatorShown(true);
        ui->UseCountOrderingButton->setEnabled(true);
        ui->ManualOrderingButton->setEnabled(true);
        ui->MoveUpButton->setEnabled(false);
        ui->MoveDownButton->setEnabled(false);
    });

    // INFO: Undo stack and context menu setup
    m_commandStack = new QUndoStack(this);

    connect(m_commandStack, &QUndoStack::cleanChanged, this, &MainWindow::onCleanChanged);

    QAction *stack_undo = m_commandStack->createUndoAction(this, "Cofnij");
    QAction *stack_redo = m_commandStack->createRedoAction(this, "Powtórz");
    stack_undo->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z));
    stack_undo->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditUndo));
    stack_redo->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    stack_redo->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditRedo));
    ui->menuEdit->addAction(stack_undo);
    ui->menuEdit->addAction(stack_redo);

    // INFO: Show a message on undo/redo
    connect(stack_undo, &QAction::triggered, this, [this](){
        const QUndoCommand *cmd = m_commandStack->command(m_commandStack->index() - 1);
        if (cmd) {
            QString message = QString("Cofnięto %1").arg(cmd->text());
            setStatusMessage(message);
        }
    });
    connect(stack_redo, &QAction::triggered, this, [this](){
        const QUndoCommand *cmd = m_commandStack->command(m_commandStack->index() - 1);
        if (cmd) {
            QString message = QString("Powtórzono %1").arg(cmd->text());
            setStatusMessage(message);
        }
    });
}

MainWindow::~MainWindow()
{
    if (m_commandStack)
        m_commandStack->disconnect(this); // INFO: Please do not Seg Fault...

    delete ui;
}

void MainWindow::onAddClicked() {
    CredDialog *dialog = new CredDialog(this);
    dialog->show();
    if (dialog->exec() == QDialog::Accepted) {
        if (dialog->result() == CredDialog::Result::Saved) {
            Cred entry_to_add(dialog->getService(), dialog->getUsername(), Vault::getInstance().encrypt(dialog->getPassword()), m_model->getNextOrder());
            m_commandStack->push(new InsertCommand(m_model, entry_to_add));
            setStatusMessage("Dodano pozycję");
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
            setStatusMessage("Edytowano pozycję");
        } else if (dialog->result() == CredDialog::Result::Deleted) {
            m_commandStack->push(new DeleteCommand(m_model, row));
            setStatusMessage("Usunięto pozycję");
        }
    }
}

void MainWindow::onDeleteAllClicked() {
    if(m_model->getCredentials().empty()) return;
    int res = QMessageBox::question(this, "Usuwanie danych", "Czy wyczyścić listę haseł?");
    if (res == QMessageBox::Yes) {
        m_commandStack->push(new ClearCommand(m_model));
        setStatusMessage("Usunięto dane");
    }
}

void MainWindow::onCleanChanged(bool clean) {
    setWindowModified(!clean);
    m_clean = clean;
}

void MainWindow::onCopyPassword() {
    QModelIndexList selection = ui->TableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) return;

    QModelIndex proxyIndex = selection.first();
    QModelIndex source_index = m_proxy->mapToSource(proxyIndex);
    int row = source_index.row();
    if (row < 0) return;
    Cred credentials = m_model->getCredentials().at(row);
    QString password = Vault::getInstance().decrypt(credentials.getPasswordBlob());
    m_model->incrementUseCount(row);
    m_clipboard->setText(password);
    setStatusMessage("Hasło skopiowane");
}

void MainWindow::onMoveUp(){
    QModelIndexList selection = ui->TableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) return;

    QModelIndex current_proxy_idx = selection.first();
    int visual_row = current_proxy_idx.row();

    if (visual_row > 0) {
        QModelIndex a_idx = m_proxy->index(visual_row - 1, 0);

        int source_row_current = m_proxy->mapToSource(current_proxy_idx).row();
        int source_row_above = m_proxy->mapToSource(a_idx).row();

        QPersistentModelIndex persistent_idx(current_proxy_idx);

        m_commandStack->push(new MoveCommand(m_model, m_proxy, source_row_current, source_row_above, ui->TableView));

        ui->TableView->selectRow(persistent_idx.row());
    }
    ui->TableView->setFocus();
}

void MainWindow::onMoveDown() {
    QModelIndexList selection = ui->TableView->selectionModel()->selectedRows();
    if (selection.isEmpty()) return;

    QModelIndex current_proxy_idx = selection.first();
    int visual_row = current_proxy_idx.row();

    if (visual_row < m_proxy->rowCount() - 1) {
        QModelIndex b_idx = m_proxy->index(visual_row + 1, 0);

        int source_row_current = m_proxy->mapToSource(current_proxy_idx).row();
        int source_row_below = m_proxy->mapToSource(b_idx).row();

        QPersistentModelIndex persistent_idx(current_proxy_idx);

        m_commandStack->push(new MoveCommand(m_model, m_proxy, source_row_below, source_row_current, ui->TableView));

        ui->TableView->selectRow(persistent_idx.row());
    }
    ui->TableView->setFocus();
}

void MainWindow::onManualOrdering() {
    ui->ManualOrderingButton->setEnabled(false);
    ui->UseCountOrderingButton->setEnabled(true);
    ui->MoveUpButton->setEnabled(true);
    ui->MoveDownButton->setEnabled(true);
    m_proxy->sort(CredentialModel::Columns::ORDER, Qt::AscendingOrder);
    ui->TableView->horizontalHeader()->setSortIndicatorShown(false);
}

void MainWindow::onUsageOrdering() {
    ui->ManualOrderingButton->setEnabled(true);
    ui->UseCountOrderingButton->setEnabled(false);
    ui->MoveUpButton->setEnabled(false);
    ui->MoveDownButton->setEnabled(false);
    m_proxy->sort(CredentialModel::Columns::COUNT, Qt::DescendingOrder);
    ui->TableView->horizontalHeader()->setSortIndicatorShown(true);
}

void MainWindow::setStatusMessage(const QString &message) {
    statusBar()->showMessage(message, 2000);

}

void MainWindow::onNew() {
    m_commandStack->clear();
    m_clean = true;
    m_model->clearCredentials();
    Vault::getInstance().resetSessionKey();
    setStatusMessage("Utworzono nowy plik");
}
void MainWindow::onSave() {}
void MainWindow::onSaveAs() {}
void MainWindow::onOpen() {}

void MainWindow::onAbout() {
    QMessageBox::about(this, "O nas", "Menadżer haseł.\nAutor: Coriscodepage.\nProject under the GPL License.\n©2025.");
}
