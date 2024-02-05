#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include "commands.h"

bool MainWindow::ServerLogin() {
    QString stringMessage = this->username + ';' + this->password;
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

    this->UpdateFiles();
    this->DisplayFiles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_uploadButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select one or more files to open");

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if(!file.exists()){
        qDebug() << "File does not exist" << filePath;
        return;
    }

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file " << filePath;
        return;
    }

    QByteArray message = (this->currentPath + QFileInfo(file).fileName() + ";" + QString::number(file.size())).toUtf8();
    message.prepend(Commnad::UploadFile);
    socket.Write(message);

    auto msg = socket.Read();
    if(msg[0] != '\x00') {
        qDebug() << "Negative message at receive" << filePath;
        return;
    }

    if(!socket.WriteFile(file)){
        qDebug() << "Fail to send" << filePath;
        return;
    }

    file.close();

    msg = socket.Read();
    if(msg[0] != '\x00') {
        qDebug() << "Negative message at receive" << filePath;
        return;
    }
}


void MainWindow::on_createDirectoryButton_clicked()
{
    auto dialog = new CreateDirectoryDialog(this);
    connect(dialog, SIGNAL(createDirectory(QString)), this, SLOT(createDirectory(QString)));
    dialog->show();

}

void MainWindow::createDirectory(QString name) {
    QByteArray message = (this->currentPath + name).toUtf8();
    message.prepend(Commnad::CreateDirectory);
    socket.Write(message);

    auto msg = socket.Read();
    if(msg[0] != '\x00') {
        qDebug() << "Could not create file";
        return;
    }

    this->UpdateFiles();
    this->RedrawFiles();
}


void MainWindow::on_informationsButton_clicked()
{

}


void MainWindow::on_searchButton_clicked()
{
    this->RedrawFiles();
}


void MainWindow::on_comboBox_currentTextChanged(const QString &comboBoxText)
{
    if(comboBoxText == "Created ASC") {
        std::sort(this->files.begin(), this->files.end(), [](const File& a, const File& b) {
              return a.size > b.size;
        });
    } else if(comboBoxText == "Name ASC") {
        std::sort(this->files.begin(), this->files.end(), [](const File& a, const File& b) {
            return a.name > b.name;
        });
    } else if(comboBoxText == "Name DES") {
        std::sort(this->files.begin(), this->files.end(), [](const File& a, const File& b) {
            return a.name < b.name;
        });
    } else if(comboBoxText == "Created DES") {
        std::sort(this->files.begin(), this->files.end(), [](const File& a, const File& b) {
            return a.size < b.size;
        });
    }

    this->RedrawFiles();
}

void MainWindow::DisplayFiles()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this->ui->filesContents);

    for (int i = 0; i < this->files.size(); ++i)
    {
        if(!this->files[i].name.contains(this->ui->filterLineEdit->text())){
            continue;
        }
        FileWidget *fileWidget = new FileWidget(this->ui->filesContents, this->files[i]);
        connect(fileWidget, SIGNAL(clicked()), this, SLOT(fileSelected()));
        connect(fileWidget, SIGNAL(doubleClicked()), this, SLOT(fileDoubleClicked()));
        mainLayout->addWidget(fileWidget);
    }

    mainLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    this->ui->filesContents->setLayout(mainLayout);
}

void MainWindow::fileSelected() {
    FileWidget* fileWidget = qobject_cast<FileWidget*>(sender());
    this->selectedFile = fileWidget;

    this->enableFileActionButtons();

    qDebug() << fileWidget->file.name << "\n";
}

void MainWindow::fileDoubleClicked() {
    FileWidget* fileWidget = qobject_cast<FileWidget*>(sender());
    if(!fileWidget->file.isDir){
        return;
    }

    this->currentPath += fileWidget->file.name + "/";
    this->UpdateFiles();
    this->RedrawFiles();
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

    QByteArray message = (this->currentPath + this->selectedFile->file.name).toUtf8();
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
    QByteArray message = (this->currentPath + this->selectedFile->file.name).toUtf8();
    message.prepend(Commnad::RemoveFileOrDirectory);
    socket.Write(message);

    auto msg = socket.Read();
    if(msg[0] != '\x00') {
        qDebug() << "Could not delete file";
        return;
    }

    this->UpdateFiles();
    this->RedrawFiles();
}


void MainWindow::on_moveButton_clicked()
{
    auto dialog = new moveFIleDialog(this, this->currentPath + this->selectedFile->file.name);
    connect(dialog, SIGNAL(moveFile(QString)), this, SLOT(moveFile(QString)));
    dialog->show();
}

void MainWindow::moveFile(QString name) {
    QByteArray message = (this->currentPath + this->selectedFile->file.name + ";" + name).toUtf8();
    message.prepend(Commnad::RenameFileOrDirectory);
    socket.Write(message);

    auto msg = socket.Read();
    if(msg[0] != '\x00') {
        qDebug() << "Could not delete file";
        return;
    }

    this->UpdateFiles();
    this->RedrawFiles();
}

void MainWindow::resetConnection() {
    this->socket.ResetConnection("127.0.0.1", 8000);

    bool loggedIn = ServerLogin();
    if(!loggedIn) {
        qDebug() << "Cannot login";
        return;
    }
}

void MainWindow::CleanLayout(QLayout* layout) {
    if (layout == NULL)
        return;
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            CleanLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void MainWindow::RedrawFiles() {
    this->CleanLayout(this->ui->filesContents->layout());
    delete this->ui->filesContents->layout();
    this->DisplayFiles();
    this->disableFileActionButtons();
}

bool MainWindow::UpdateFiles() {
    QByteArray listRawMessage = this->currentPath.toUtf8();
    listRawMessage.prepend(Commnad::ListFilesAndDirectories);
    this->socket.Write(listRawMessage);

    QByteArray listMessage = this->socket.Read();
    if(listMessage.size() < 1 || listMessage[0] != '\x00'){
        qDebug() << "Cannot list";
        return false;
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

    return true;
}

void MainWindow::on_backButton_clicked()
{
    QStringList pathComponents = this->currentPath.split("/");
    if(!pathComponents.isEmpty()) {
        pathComponents.removeFirst();
    }
    if(!pathComponents.isEmpty()){
        pathComponents.removeLast();
    }
    if(!pathComponents.isEmpty()){
        pathComponents.removeLast();
    }
    this->currentPath = "/";
    for(int i = 0; i < pathComponents.size(); ++i){
        this->currentPath += pathComponents[i] + "/";
    }

    this->UpdateFiles();
    this->RedrawFiles();
}

void MainWindow::enableFileActionButtons() {
    this->ui->downloadButton->setEnabled(true);
    this->ui->deleteButton->setEnabled(true);
    this->ui->moveButton->setEnabled(true);
}

void MainWindow::disableFileActionButtons() {
    this->ui->downloadButton->setEnabled(false);
    this->ui->deleteButton->setEnabled(false);
    this->ui->moveButton->setEnabled(false);
}

