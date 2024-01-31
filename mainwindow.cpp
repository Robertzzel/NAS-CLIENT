#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include "commands.h"

bool MainWindow::ServerLogin() {
    QString stringMessage = this->username + ";" + this->password;
    QByteArray rawMessage = stringMessage.toUtf8();
    rawMessage.prepend(Commnad::Login);
    this->socket.Write(rawMessage);

    rawMessage = this->socket.Read();
    if(rawMessage.size() < 1){
        return false;
    }
    return rawMessage[0] == '\x00';
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!this->socket.Connect("127.0.0.1", 8000)) {
        qDebug() << "Cannot connect\n";
        return;
    }

    bool loggedIn = ServerLogin();
    if(!loggedIn) {
        qDebug() << "Cannot login";
        return;
    }

    QString listStringMessage = "/";
    QByteArray listRawMessage = listStringMessage.toUtf8();
    listRawMessage.prepend(Commnad::ListFilesAndDirectories);
    this->socket.Write(listRawMessage);

    QByteArray listMessage = this->socket.Read();
    if(listMessage.size() < 1 || listMessage[0] != '\x00'){
        qDebug() << "Cannot list";
        return;
    }
    listMessage = listMessage.removeAt(0);

    QString stringMessage = QString::fromUtf8(listMessage);
    QStringList messageFiles = stringMessage.split('\x1c');

    this->files.clear();
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
    this->DisplayFiles();
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

void MainWindow::DisplayFiles()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this->ui->filesContents);

    for (int i = 0; i < this->files.size(); ++i)
    {
        FileWidget *fileWidget = new FileWidget(this->ui->filesContents, this->files[i]);
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

void MainWindow::on_downloadButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select one or more files to open");

    if (directory.isEmpty()) {
        return;
    }

    QDir dir(directory);
    QString filePath = dir.filePath(this->selectedFile->file.isDir ? this->selectedFile->file.name + ".zip" : this->selectedFile->file.name);
    qDebug() << "Saving file " << filePath;

    QFile file(filePath);
    if(file.exists()){
        qDebug() << "File exists" << filePath;
        return;
    }

    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file " << filePath;
        return;
    }

    QByteArray message = this->selectedFile->file.name.toUtf8();
    message.prepend(Commnad::DownloadFileOrDirectory);
    socket.Write(message);

    auto msg = socket.Read();
    if(msg[0] != '\x00') {
        qDebug() << "Negative message at receive" << filePath;
        return;
    }

    if(!socket.ReadFile(file)){
        qDebug() << "Fail to receive" << filePath;
        return;
    }

    file.close();
    this->resetConnection();
}


void MainWindow::on_deleteButton_clicked()
{

}


void MainWindow::on_moveButton_clicked()
{

}

void MainWindow::resetConnection() {
    this->socket.ResetConnection("127.0.0.1", 8000);

    bool loggedIn = ServerLogin();
    if(!loggedIn) {
        qDebug() << "Cannot login";
        return;
    }
}

