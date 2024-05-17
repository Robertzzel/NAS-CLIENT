#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QTcpSocket>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include "commands.h"
#include <QApplication>
#include <QScroller>
#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#endif

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QScroller::grabGesture(this->ui->files, QScroller::LeftMouseButtonGesture);
    this->ui->stackedWidget->setCurrentIndex(1);
    this->ui->statusLabel->setVisible(false);
    this->ui->serverAddressLineEdit->setText("127.0.0.1");
    this->ui->usernameLineEdit->setText("Robertzzel");
    this->ui->passwordLineEdit->setText("123456");

#ifdef Q_OS_ANDROID
    auto r = QtAndroidPrivate::checkPermission("android.permission.READ_EXTERNAL_STORAGE").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
            this->ui->statusLabel->setText("Permissions missing");
    }
    r = QtAndroidPrivate::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
            this->ui->statusLabel->setText("Permissions missing");
    }
    r = QtAndroidPrivate::checkPermission("android.permission.READ_MEDIA_IMAGES").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_MEDIA_IMAGES").result();
        if (r == QtAndroidPrivate::Denied)
            this->ui->statusLabel->setText("Permissions missing");
    }
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
    if(this->commands != nullptr){
        delete this->commands;
    }
}

void MainWindow::createDirectory(QString name) {
    QByteArray message = (this->currentPath + name).toUtf8();
    bool created;
    QString err = this->commands->CreateDirectory(message, created);
}

void MainWindow::on_informationsButton_clicked()
{
    QString info;
    QString err = this->commands->Info(info);
    if(err == "") {
        return;
    }

    QDialog secondWindow(this);
    QLabel *label = new QLabel(&secondWindow);
    label->setText("Memory Remaining: " + info);
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
        QString fileName = this->files[i].name;
        if(!fileName.contains(this->ui->filterLineEdit->text())){
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

    this->commands->Download(this->currentPath + this->selectedFile->file.name, filePath);
}

void MainWindow::on_uploadButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select one or more files to open");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if(!file.exists()){
        return;
    }

    this->commands->Upload(this->currentPath, filePath);
}


void MainWindow::on_deleteButton_clicked()
{
    QByteArray message = (this->currentPath + this->selectedFile->file.name).toUtf8();
    bool success;
    QString err = this->commands->Remove(message, success);

    this->updateFiles();
    this->redrawFiles();
}

void MainWindow::moveFile(QString name) {
    QString oldFile = this->currentPath + this->selectedFile->file.name;
    QString newFile = name;
    bool success;
    QString err = this->commands->Rename(oldFile, newFile, success);
}

void MainWindow::redrawFiles() {
    this->cleanLayout(this->ui->filesContents->layout());
    delete this->ui->filesContents->layout();
    this->displayFiles();
    this->disableFileActionButtons();
}

bool MainWindow::updateFiles() {
    this->files.clear();
    QString err = this->commands->List(this->currentPath.toUtf8(), this->files);
    return err == "";
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

// CREATE DIRECTORY

void MainWindow::on_createBtn_clicked()
{
    createDirectory(this->ui->directoryNameLineEdit->text());
    this->updateFiles();
    this->redrawFiles();
    this->ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_cancelCreateBtn_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

// END CREATE DIRECTORY

// RENAME

void MainWindow::on_renameBtn_clicked()
{
    moveFile(this->ui->newNameLineEdit->text());
    this->updateFiles();
    this->redrawFiles();
    this->ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_cancelRenameBtn_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

// END RENAME

// LOGIN

void MainWindow::on_loginBtn_clicked()
{
    QString error;

    QString address = this->ui->serverAddressLineEdit->text();
    this->commands = new Command(address, 8001);

    bool success;
    QString err = this->commands->Login(this->ui->usernameLineEdit->text(), this->ui->passwordLineEdit->text(), success);
    if(!success) {
        this->ui->statusLabel->setVisible(true);
        this->ui->statusLabel->setText("Cannot login, invalid credentials");
        return;
    }

    connect(this->commands, &Command::statusSet, this, &MainWindow::setStatus);

    this->ui->statusLabel->setVisible(false);
    this->username = this->ui->usernameLineEdit->text();
    this->password = this->ui->passwordLineEdit->text();

    this->updateFiles();
    this->redrawFiles();
    this->ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setStatus(bool enb, QString msg){
    this->ui->statusLabel->setVisible(enb);
    this->ui->statusLabel->setText(msg);
}

// END LOGIN

void MainWindow::on_createDirectoryButton_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_moveButton_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
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
