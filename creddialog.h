#ifndef CREDDIALOG_H
#define CREDDIALOG_H

#include <QDialog>

namespace Ui {
class EditDialog;
}

class CredDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Result {
        Saved,
        Deleted,
        Cancelled
    };
    explicit CredDialog(QWidget *parent = nullptr, bool edit_mode = false);
    ~CredDialog();

    Result result() { return m_result; }
    QString getService();
    QString getUsername();
    QString getPassword();
    void setData(const QString &service, const QString &username, const QString &password);

private slots:
    void onSave();
    void onCancel();
    void onDelete();
    void onGenPasswd();
    void updatePasswordStrength(const QString &password);
private:
    Ui::EditDialog *ui;
    bool m_editMode;
    bool m_passwordDirty;
    Result m_result;
};

#endif // CREDDIALOG_H
