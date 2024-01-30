#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include "file.h"
#include <QWidget>

class FileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileWidget(QWidget *parent, File file);
    File file;
protected:
    void mousePressEvent(QMouseEvent *event);
signals:
    void clicked();
};

#endif // FILEWIDGET_H
