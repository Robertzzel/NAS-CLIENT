#ifndef FILE_H
#define FILE_H

#include <QObject>

class File
{
public:
    // File(File&);
    // File(const File&);
    File();
    QString name;
    qint64 size;
    bool isDir;
    QString type;
    qint64 created;
};

#endif
