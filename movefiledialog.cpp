#include "movefiledialog.h"
#include "ui_movefiledialog.h"

moveFIleDialog::moveFIleDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::moveFIleDialog)
{
    ui->setupUi(this);
}


moveFIleDialog::moveFIleDialog(QWidget *parent, QString path): QDialog(parent)
    , ui(new Ui::moveFIleDialog)
{
    ui->setupUi(this);
    this->ui->lineEdit_2->setText(path);
}


moveFIleDialog::~moveFIleDialog()
{
    delete ui;
}


void moveFIleDialog::on_okButton_clicked()
{
    emit moveFile(this->ui->lineEdit_2->text());
    accept();
}


void moveFIleDialog::on_cancelButton_clicked()
{
    accept();
}

