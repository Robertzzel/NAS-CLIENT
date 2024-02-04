#include "createdirectorydialog.h"
#include "ui_createdirectorydialog.h"

CreateDirectoryDialog::CreateDirectoryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateDirectoryDialog)
{
    ui->setupUi(this);
}

CreateDirectoryDialog::~CreateDirectoryDialog()
{
    delete ui;
}

void CreateDirectoryDialog::on_okButton_clicked()
{
    emit this->createDirectory(this->ui->lineEdit->text());
    accept();
}


void CreateDirectoryDialog::on_pushButton_clicked()
{
    accept();
}

