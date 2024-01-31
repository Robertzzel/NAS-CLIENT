#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "messagehandler.h"
#include "file.h"
#include "filewidget.h"

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
    void DisplayFiles();
    bool ServerLogin();
private slots:
    void on_uploadButton_clicked();
    void on_createDirectoryButton_clicked();
    void on_informationsButton_clicked();
    void on_searchButton_clicked();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_downloadButton_clicked();
    void on_deleteButton_clicked();
    void on_moveButton_clicked();
    void fileSelected();
private:
    Ui::MainWindow *ui;
    MessageHandler socket;
    QVector<File> files;
    FileWidget *selectedFile = nullptr;
    QString username = "Robertzzel";
    QString password = "123456";
    void resetConnection();
};
#endif // MAINWINDOW_H
