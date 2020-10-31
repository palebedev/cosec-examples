#ifndef UUID_03264C2A_3733_4119_9574_3F9292463C0A
#define UUID_03264C2A_3733_4119_9574_3F9292463C0A

#include <QtCore/QAtomicInt>
#include <QtCore/QObject>

class FileCopier : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
signals:
    void progressChanged(int percent);
    void done(const QString& result);
public:
    void setCancelled(bool value);
public slots:
    void copyFile(const QString& source,const QString& destination);
private:
    // Qt has not atomic bool, we're using an int instead.
    QAtomicInt cancelled_ = 0;
};

#endif // FILECOPIER_HPP
