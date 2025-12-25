#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "credentialmodel.h"
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <qundostack.h>

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

// ADDED METHODS
    void onAddClicked();

private:
    Ui::MainWindow *ui;
    CredentialModel *m_model;
    QSortFilterProxyModel *m_proxy;
    QUndoStack *m_commandStack;
};
#endif // MAINWINDOW_H
