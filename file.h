#ifndef FILE_H
#define FILE_H

#include <QObject>

class File
{
public:
    // File(File&);
    // File(const File&);
    File();
    QString name; // nume + sortare
    qint64 size; // size
    bool isDir; // clickable daca e dir
    QString type; // poza
    qint64 created; // sortare
};

#endif
