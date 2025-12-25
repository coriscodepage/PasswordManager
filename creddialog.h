#ifndef CREDDIALOG_H
#define CREDDIALOG_H

#include "cred.h"
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
    static std::optional<Cred> addMode(QWidget *parent = nullptr);
    static std::pair<Result, std::optional<Cred>> editMode(const Cred &existing, QWidget *parent = nullptr);

private slots:
    void onSave();
    void onCancel();
    void onDelete();
    void onGenPasswd();
private:
    Ui::EditDialog *ui;
    bool m_editMode;
};

#endif // CREDDIALOG_H
