#include "creddialog.h"
#include "cred.h"
#include "ui_creddialog.h"
#include <qregularexpression.h>
#include <qvalidator.h>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QInputDialog>

CredDialog::CredDialog(QWidget *parent, bool edit_mode)
    : QDialog(parent)
    , ui(new Ui::EditDialog),
    m_passwordDirty(false),
    m_editMode(edit_mode)
{
    ui->setupUi(this);
    setModal(true);
    setWindowTitle(m_editMode ? "Edytuj" : "Dodaj");
    QRegularExpression no_spaces("\\S*");
    ui->UsernameEdit->setValidator(new QRegularExpressionValidator(no_spaces));

    connect(ui->CancelButton, &QPushButton::clicked, this, &CredDialog::onCancel);
    connect(ui->SaveButton, &QPushButton::clicked, this, &CredDialog::onSave);
    connect(ui->DeleteButton, &QPushButton::clicked, this, &CredDialog::onDelete);
    connect(ui->GeneratePasswordButton, &QPushButton::clicked, this, &CredDialog::onGenPasswd);

    connect(ui->PasswordEdit, &QLineEdit::textEdited, this, &CredDialog::updatePasswordStrength);

    connect(ui->ServiceEdit, &QLineEdit::returnPressed, this, &CredDialog::onSave);
    connect(ui->UsernameEdit, &QLineEdit::returnPressed, this, &CredDialog::onSave);
    connect(ui->PasswordEdit, &QLineEdit::returnPressed, this, &CredDialog::onSave);

    if (m_editMode)
        ui->DeleteButton->show();
    else {
        updatePasswordStrength("");
        ui->DeleteButton->hide();
    }
}

void CredDialog::setData(const QString &service, const QString &username, const QString &password) {
    ui->ServiceEdit->setText(service);
    ui->UsernameEdit->setText(username);
    ui->PasswordEdit->setText(password);
    updatePasswordStrength(password);
    m_passwordDirty = false;
}

void CredDialog::onSave() {
    ValidationResult result = Cred::validate(ui->ServiceEdit->text().trimmed(), ui->UsernameEdit->text().trimmed(), ui->PasswordEdit->text());
    if (result.isVaid()) {
        if (m_passwordDirty) {
            PasswordStrength strength(ui->PasswordEdit->text());
            if (strength.score < PasswordStrength::WEAK) {
                int ret = QMessageBox::question(this, "Słabe hasło", "Słabe hasło, proszę zastanowić się nad implementacją poniższych sugestii:\n•" + strength.suggestions.join("\n•") + "\n Kontynuować?");
                if (ret == QMessageBox::No)
                    return;
           }
        }
        m_result = Result::Saved;
        accept();
    }
    else {
        QMessageBox::warning(this, "Błąd walidacji", result.message);
        return;
    }
}

void CredDialog::onCancel() {
    m_result = Result::Cancelled;
    reject();
}

void CredDialog::onDelete() {
    int res = QMessageBox::question(this, "Usuń", "Czy jesteś pewien?");
    if (res == QMessageBox::Yes) {
        m_result = Result::Deleted;
        accept();
    }
}

void CredDialog::onGenPasswd() {
    static QString charset = QString("QWERTYUIOPASDFGHJKLZXCVBNM") + QString("qwertyuiopasdfghjklzxcvbnm") + QString("0123456789") + QString("!@#$%^&*:;?.,~`/(){}[]<>- ");

    QString password;
    bool ok;
    int password_length = QInputDialog::getInt(this, "Długość hasła", "Proszę podać długość hasła:", 8, 1, 2048, 1, &ok);
    if (ok && password_length > 0) {
        for (int i = 0; i < password_length; ++i) {
            auto random_index = QRandomGenerator::global()->bounded(charset.length());
            password += charset[random_index];
        }
        ui->PasswordEdit->setText(password);
        updatePasswordStrength(password);
    }
}

void CredDialog::updatePasswordStrength(const QString &password) {
    PasswordStrength strength(password);
    ui->PasswordScoreAssesmentLabel->setText(strength.rating);
    ui->PasswordStrengthBar->setValue(qMin(strength.score, 100));
    m_passwordDirty = true;
}

QString CredDialog::getService() {
    return ui->ServiceEdit->text().trimmed();
}

QString CredDialog::getUsername() {
    return ui->UsernameEdit->text().trimmed();
}

QString CredDialog::getPassword() {
    return ui->PasswordEdit->text();
}

CredDialog::~CredDialog()
{
    delete ui;
}

