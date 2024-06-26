#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "file.h"
#include "filewidget.h"
#include "commands.h"

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
    void on_uploadButton_clicked();
    void on_informationsButton_clicked();
    void on_searchButton_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_downloadButton_clicked();
    void on_deleteButton_clicked();
    void fileSelected();
    void fileDoubleClicked();
    void on_backButton_clicked();
    void createDirectory(QString);
    void moveFile(QString);
    void resizeEvent(QResizeEvent *event);
    void on_createBtn_clicked();

    void on_cancelCreateBtn_clicked();

    void on_renameBtn_clicked();

    void on_cancelRenameBtn_clicked();

    void on_loginBtn_clicked();

    void on_createDirectoryButton_clicked();

    void on_moveButton_clicked();

    void on_actionsButton_clicked();

    void on_actionsBackButton_clicked();

private:
    Ui::MainWindow *ui;
    Command* commands;
    QList<File> files;
    FileWidget *selectedFile = nullptr;
    QString currentPath = "/";
    QString username = "Robertzzel";
    QString password = "123456";

    void redrawFiles();
    bool updateFiles();
    void disableFileActionButtons();
    void enableFileActionButtons();
    void displayFiles();
    void cleanLayout(QLayout* layout);
    bool serverLogin();
    void setStatus(bool,QString);
};
#endif // MAINWINDOW_H
