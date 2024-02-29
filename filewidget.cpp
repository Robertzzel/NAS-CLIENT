#include "filewidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>

FileWidget::FileWidget(QWidget *parent, File file) : QWidget{parent}
{
    QHBoxLayout *fileLayout = new QHBoxLayout;

    QString imgUrl;
    if(file.isDir) {
        imgUrl = ":/images/directory.png";
    } else if(file.name.contains("zip") || file.name.contains("compressed")){
        imgUrl = ":/images/zip.png";
    } else if(file.name.contains("pdf")){
        imgUrl = ":/images/pdf.png";
    } else if(file.name.contains("html")){
        imgUrl = ":/images/html.png";
    } else if(file.name.contains("xml")){
        imgUrl = ":/images/xml.png";
    } else if(file.name.contains("image")){
        imgUrl = ":/images/image.png";
    } else {
        imgUrl = ":/images/file.png";
    }
    QPixmap icon(imgUrl);
    QLabel *iconLabel = new QLabel();
    iconLabel->setFixedSize(120, 150);
    iconLabel->setPixmap(icon);
    fileLayout->addWidget(iconLabel);

    QLabel *nameLabel = new QLabel(file.name);
    QLabel *sizeLabel = new QLabel(QString::number(file.size));
    fileLayout->addWidget(nameLabel);
    fileLayout->addWidget(sizeLabel);

    this->setLayout(fileLayout);
    this->file = file;
}

FileWidget::~FileWidget() {
    if(this->lastPressed != NULL && this->lastPressed == this){
        this->lastPressed = NULL;
    }
    for(auto child: this->children()) {
        delete child;
    }
}

void FileWidget::mousePressEvent(QMouseEvent *event) {
    if(this->lastPressed == this){
        emit doubleClicked();
        return;
    }
    if(this->lastPressed != NULL){
        lastPressed->setStyleSheet("");
    }
    this->lastPressed = this;
    this->setStyleSheet("background-color: #393f45;");
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

void FileWidget::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        emit doubleClicked();
    }
}
