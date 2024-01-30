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

    QVBoxLayout *nameAndSizeLayout = new QVBoxLayout;
    QLabel *nameLabel = new QLabel(file.name);
    QLabel *sizeLabel = new QLabel(QString::number(file.size));
    nameAndSizeLayout->addWidget(nameLabel);
    nameAndSizeLayout->addWidget(sizeLabel);
    fileLayout->addLayout(nameAndSizeLayout);

    this->setLayout(fileLayout);
    this->file = file;
}

void FileWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}
