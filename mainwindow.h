#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "credentialmodel.h"
#include "filemanager.h"
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <qundostack.h>
#include <QClipboard>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddClicked();
    void onEditClicked(const QModelIndex &proxyIndex);
    void onDeleteAllClicked();
    void onCleanChanged(bool clean);
    void onCopyPassword();
    void onMoveUp();
    void onMoveDown();
    void onManualOrdering();
    void onUsageOrdering();
    void onNew();
    void onSave();
    void onSaveAs();
    void onOpen();
    void onAbout();
    void resetTableUi();

private:
    Ui::MainWindow *ui;
    CredentialModel *m_model;
    QSortFilterProxyModel *m_proxy;
    QUndoStack *m_commandStack;
    QClipboard *m_clipboard;
    bool m_clean;
    FileManager *m_fileManager;
    void setStatusMessage(const QString &message);
    void clearContext();

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
