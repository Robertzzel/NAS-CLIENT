#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!this->socket.Connect("127.0.0.1", 8000)) {
        qDebug() << "Cannot connect\n";
        return;
    }

    if(!this->login()) {
        qDebug() << "Cannot login";
        return;
    }

    QString listStringMessage = "/";
    QByteArray listRawMessage = listStringMessage.toUtf8();
    listRawMessage.prepend('\x06');
    this->socket.Write(listRawMessage);

    QByteArray listMessage = this->socket.Read();
    if(listMessage[0] != '\x00'){
        qDebug() << "Cannot list";
        return;
    }

    listMessage = listMessage.removeAt(0);

    QString stringMessage = QString::fromUtf8(listMessage);
    QStringList messageFiles = stringMessage.split('\x1c');

    this->files.clear();
    this->files.reserve(messageFiles.length());
    for(int i = 0; i < messageFiles.length(); ++i) {
        QStringList fileAndDetails = messageFiles[i].split('\n');
        File file;
        file.name = fileAndDetails[0];
        file.size = fileAndDetails[1].toULongLong();
        file.isDir = fileAndDetails[2] == "true";
        file.type = fileAndDetails[3];
        file.created = fileAndDetails[4].toULongLong();
        files.append(file);
    }

    this->displayFiles();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_uploadButton_clicked()
{

}


void MainWindow::on_createDirectoryButton_clicked()
{

}


void MainWindow::on_informationsButton_clicked()
{

}


void MainWindow::on_searchButton_clicked()
{

}


void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{

}

void MainWindow::displayFiles()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this->ui->filesContents);

    for (const File& file : files)
    {
        FileWidget *fileWidget = new FileWidget(this->ui->filesContents, file);
        connect(fileWidget, SIGNAL(clicked()), this, SLOT(fileSelected()));
        mainLayout->addWidget(fileWidget);
    }

    this->ui->filesContents->setLayout(mainLayout);
}

void MainWindow::fileSelected() {
    FileWidget* fileWidget = qobject_cast<FileWidget*>(sender());
    this->selectedFile = fileWidget;

    this->ui->downloadButton->setEnabled(true);
    this->ui->deleteButton->setEnabled(true);
    this->ui->moveButton->setEnabled(true);

    qDebug() << fileWidget->file.name << "\n";
}

bool MainWindow::login() {
    QString stringMessage = "Robertzzel;123456";
    QByteArray rawMessage = stringMessage.toUtf8();
    rawMessage.prepend('\x05');
    this->socket.Write(rawMessage);

    rawMessage = this->socket.Read();
    return rawMessage[0] == '\x00';
}

void MainWindow::on_downloadButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select one or more files to open");

    if (directory.isEmpty()) {
        return;
    }

    QDir dir(directory);
    QString filePath = dir.filePath(this->selectedFile->file.name);

    QFile file(filePath);
    if(file.exists()){
        return;
    }

    if(!file.open(QIODevice::WriteOnly)) {
        return;
    }

    QByteArray message = this->selectedFile->file.name.toUtf8();
    message.prepend('\x01');
    socket.Write(message);

    socket.ReadFile(file);

    file.close();
}


void MainWindow::on_deleteButton_clicked()
{

}


void MainWindow::on_moveButton_clicked()
{

}

