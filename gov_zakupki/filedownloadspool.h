#ifndef FILEDOWNLOADSPOOL_H
#define FILEDOWNLOADSPOOL_H

#include "filedownloader.h"

#include <QDebug>

// SINGLETON FILEDOWNLOADSPOOL -------------------------------------------------

class FileDownloadsPool : public QObject
{
    Q_OBJECT

public:
    static FileDownloadsPool *instance();

    virtual ~FileDownloadsPool();

    void addDownload(QString req, QObject *obj, const char *slot,
                     void *info = nullptr);

    QByteArray extractDataAndFree(FileDownloader * ploader);

private:
    static FileDownloadsPool *_instance;

    explicit FileDownloadsPool(QObject *parent = 0);
    Q_DISABLE_COPY(FileDownloadsPool)

    QVector<FileDownloader *> pool;

signals:
    void fileDownloaded(QByteArray data);
};

#endif // FILEDOWNLOADSPOOL_H
