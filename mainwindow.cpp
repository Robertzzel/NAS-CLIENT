#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include "commands.h"
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!this->socket.Connect("127.0.0.1", 8000)) {
        qDebug() << "Cannot connect\n";
        return;
    }

    bool loggedIn = this->socket.Login(this->username, this->password);
    if(!loggedIn) {
        qDebug() << "Cannot login";
        return;
    }

    this->ui->statusLabel->setVisible(false);
    this->ui->statusLabel->setText("Downloading...");

    this->updateFiles();
    this->displayFiles();
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

    QByteArray message = (this->currentPath + QFileInfo(file).fileName() + '\n' + QString::number(file.size())).toUtf8();
    auto msg = socket.WriteCommandAndRead(Commnad::UploadFile, message);
    if(msg[0] != '\x00') {
        qDebug() << "Negative message at receive" << filePath;
        return;
    }

    this->ui->statusLabel->setVisible(true);
    this->ui->statusLabel->setText("Uploading...");
    qApp->processEvents();

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

    this->ui->statusLabel->setVisible(false);
}


void MainWindow::on_createDirectoryButton_clicked()
{
    auto dialog = new CreateDirectoryDialog(this);
    connect(dialog, SIGNAL(createDirectory(QString)), this, SLOT(createDirectory(QString)));
    dialog->show();
}

void MainWindow::createDirectory(QString name) {
    QByteArray message = (this->currentPath + name).toUtf8();
    if(socket.WriteCommandAndRead(Commnad::CreateDirectory, message)[0] != '\x00') {
        qDebug() << "Could not create file";
        return;
    }

    this->updateFiles();
    this->redrawFiles();
}


void MainWindow::on_informationsButton_clicked()
{
    auto msg = QString("").toUtf8();
    auto response = this->socket.WriteCommandAndRead(Commnad::Info, msg);
    if(response[0] != '\x00') {
        qDebug() << "Could display files";
        return;
    }
    response = response.removeAt(0);

    QDialog secondWindow(this);
    QLabel *label = new QLabel(&secondWindow);
    label->setText("Memory Remaining: " + QString::fromUtf8(response));
    secondWindow.exec();
}


void MainWindow::on_searchButton_clicked()
{
    this->redrawFiles();
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

    this->redrawFiles();
}

void MainWindow::displayFiles()
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
    this->setWindowTitle("NAS - "+ this->currentPath);
    this->updateFiles();
    this->redrawFiles();
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
    auto msg = socket.WriteCommandAndRead(Commnad::DownloadFileOrDirectory, message);
    if(msg[0] != '\x00') {
        qDebug() << "Negative message at receive" << filePath;
        return;
    }

    this->ui->statusLabel->setVisible(true);
    this->ui->statusLabel->setText("Downloading...");
    qApp->processEvents();

    if(!socket.ReadFile(file)){
        qDebug() << "Fail to receive" << filePath;
        return;
    }
    this->ui->statusLabel->setVisible(false);

    file.close();
    this->resetConnection();
}


void MainWindow::on_deleteButton_clicked()
{
    QByteArray message = (this->currentPath + this->selectedFile->file.name).toUtf8();
    auto msg = socket.WriteCommandAndRead(Commnad::RemoveFileOrDirectory, message);
    if(msg[0] != '\x00') {
        qDebug() << "Could not delete file";
        return;
    }

    this->updateFiles();
    this->redrawFiles();
}


void MainWindow::on_moveButton_clicked()
{
    auto dialog = new moveFIleDialog(this, this->currentPath + this->selectedFile->file.name);
    connect(dialog, SIGNAL(moveFile(QString)), this, SLOT(moveFile(QString)));
    dialog->show();
}

void MainWindow::moveFile(QString name) {
    QByteArray message = (this->currentPath + this->selectedFile->file.name + '\n' + name).toUtf8();
    auto msg = socket.WriteCommandAndRead(Commnad::RenameFileOrDirectory, message);
    if(msg[0] != '\x00') {
        qDebug() << "Could not delete file";
        return;
    }

    this->updateFiles();
    this->redrawFiles();
}

void MainWindow::resetConnection() {
    this->socket.ResetConnection("127.0.0.1", 8000);
    this->socket.Login(this->username, this->password);
}

void MainWindow::cleanLayout(QLayout* layout) {
    if (layout == NULL)
        return;
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            cleanLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void MainWindow::redrawFiles() {
    this->cleanLayout(this->ui->filesContents->layout());
    delete this->ui->filesContents->layout();
    this->displayFiles();
    this->disableFileActionButtons();
}

bool MainWindow::updateFiles() {
    QByteArray listRawMessage = this->currentPath.toUtf8();
    QByteArray listMessage = this->socket.WriteCommandAndRead(Commnad::ListFilesAndDirectories, listRawMessage);;
    if(listMessage.size() < 1 || listMessage[0] != '\x00'){
        qDebug() << "Cannot list";
        return false;
    }
    listMessage = listMessage.removeAt(0);

    this->files.clear();
    QString stringMessage = QString::fromUtf8(listMessage);
    if(stringMessage == ""){
        return true;
    }
    QStringList messageFiles = stringMessage.split('\x1c');

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

    this->setWindowTitle("NAS - "+ this->currentPath);
    this->updateFiles();
    this->redrawFiles();
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

