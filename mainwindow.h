#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "credentialmodel.h"
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

private:
    Ui::MainWindow *ui;
    CredentialModel *m_model;
    QSortFilterProxyModel *m_proxy;
    QUndoStack *m_commandStack;
    QClipboard *m_clipboard;
    bool m_clean;
    void setStatusMessage(const QString &message);
};
#endif // MAINWINDOW_H
