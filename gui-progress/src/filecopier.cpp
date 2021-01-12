#include "filecopier.hpp"

#include <QtCore/QByteArray>
#include <QtCore/QFile>

void FileCopier::setCancelled(bool value)
{
    cancelled_ = value;
}

void FileCopier::copyFile(const QString& source,const QString& destination)
{
    QFile sourceFile(source);
    if(!sourceFile.open(QFile::ReadOnly)){
        emit done(tr("Failed to open source file: %1").arg(sourceFile.errorString()));
        return;
    }
    QFile destinationFile(destination);
    if(!destinationFile.open(QFile::WriteOnly)){
        emit done(tr("Failed to open destination file: %1").arg(destinationFile.errorString()));
        return;
    }
    int progress = 0;
    emit progressChanged(0);
    qint64 size = sourceFile.size();
    QByteArray buffer(1024*1024,'\0');
    qint64 count,copied = 0;
    // Manually check whether operation was cancelled after every block.
    while((count = sourceFile.read(buffer.data(),buffer.size()))>0&&!cancelled_){
        if(destinationFile.write(buffer.data(),count)!=count){
            emit done(tr("Failed to write to destination file: %1").arg(
                destinationFile.errorString()));
            return;
        }
        copied += count;
        int newProgress = int(100*copied/size);
        if(progress!=newProgress){
            // We send an update only for whoole percent changes
            // to avoid spending too much time sending UI updates.
            progress = newProgress;
            emit progressChanged(progress);
        }
    }
    if(cancelled_){
        emit done(tr("File copy cancelled."));
        return;
    }
    if(count<0){
        emit done(tr("Failed to read source file: %1").arg(sourceFile.errorString()));
        return;
    }
    emit done(tr("File copied successfully."));
}
