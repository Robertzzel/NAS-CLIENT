#ifndef CREATEDIRECTORYDIALOG_H
#define CREATEDIRECTORYDIALOG_H

#include <QDialog>

namespace Ui {
class CreateDirectoryDialog;
}

class CreateDirectoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDirectoryDialog(QWidget *parent = nullptr);
    ~CreateDirectoryDialog();

private slots:

    void on_okButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::CreateDirectoryDialog *ui;

signals:
    void createDirectory(QString name);
};

#endif // CREATEDIRECTORYDIALOG_H
