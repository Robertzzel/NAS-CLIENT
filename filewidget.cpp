#include "filewidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>

FileWidget::FileWidget(QWidget *parent, File file) : QWidget{parent}
{
    QString imgUrl = this->getFileIcon(file);

    QHBoxLayout *fileLayout = new QHBoxLayout;

    QPixmap icon(imgUrl);
    QLabel *iconLabel = new QLabel();
    //iconLabel->setFixedHeight(this->height() / 2);
    iconLabel->setPixmap(icon.scaledToHeight(this->height() * 1.5));
    iconLabel->setAlignment(Qt::AlignCenter);
    fileLayout->addWidget(iconLabel);

    QLabel *nameLabel = new QLabel(file.name);
    nameLabel->setAlignment(Qt::AlignCenter);
    QLabel *sizeLabel = new QLabel(QString::number(file.size));
    sizeLabel->setAlignment(Qt::AlignCenter);
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

QString FileWidget::getFileIcon(File& file){
    if(file.isDir) {
        return ":/images/directory.png";
    } else if(file.name.contains("zip") || file.name.contains("compressed")){
        return ":/images/zip.png";
    } else if(file.name.contains("pdf")){
        return ":/images/pdf.png";
    } else if(file.name.contains("html")){
        return ":/images/html.png";
    } else if(file.name.contains("xml")){
        return ":/images/xml.png";
    } else if(file.name.contains("image")){
        return ":/images/image.png";
    }
    return ":/images/file.png";
}
