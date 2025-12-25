#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "credentialmodel.h"
#include "vault.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_model = new CredentialModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->TableView->setModel(m_proxy);
    ui->TableView->setSortingEnabled(true);

    connect(ui->AddButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->PasswordVisibilityButton, &QPushButton::clicked, m_model, &CredentialModel::changeVisibility);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddClicked() {
    Cred cred("test", "admin", Vault::getInstance().encrypt("adnim"));
    m_model->addCredential(cred);
}
