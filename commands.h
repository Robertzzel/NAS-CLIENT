#ifndef COMMANDS_H
#define COMMANDS_H

enum Commnad
{
    UploadFile              = '\x00',
    DownloadFileOrDirectory = '\x01',
    CreateDirectory         = '\x02',
    RemoveFileOrDirectory   = '\x03',
    RenameFileOrDirectory   = '\x04',
    Login                   = '\x05',
    ListFilesAndDirectories = '\x06'
};

#endif // COMMANDS_H
