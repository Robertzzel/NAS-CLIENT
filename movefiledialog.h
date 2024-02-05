#ifndef MOVEFILEDIALOG_H
#define MOVEFILEDIALOG_H

#include <QDialog>

namespace Ui {
class moveFIleDialog;
}

class moveFIleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit moveFIleDialog(QWidget *parent = nullptr);
    moveFIleDialog(QWidget *parent, QString path);
    ~moveFIleDialog();

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::moveFIleDialog *ui;

signals:
    void moveFile(QString);
};

#endif // MOVEFILEDIALOG_H
