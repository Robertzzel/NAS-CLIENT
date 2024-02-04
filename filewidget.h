#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include "file.h"
#include <QWidget>

class FileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileWidget(QWidget *parent, File file);
    ~FileWidget();
    File file;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent * e);
signals:
    void clicked();
    void doubleClicked();
};

#endif // FILEWIDGET_H
