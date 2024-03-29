#include "filedownloadspool.h"

FileDownloadsPool* FileDownloadsPool::_instance = nullptr;

FileDownloadsPool::FileDownloadsPool(QObject *parent) :
    QObject(parent)
{
}

FileDownloadsPool *FileDownloadsPool::instance()
{
    if (!_instance) {
        _instance = new FileDownloadsPool();
    }
    return _instance;
}

FileDownloadsPool::~FileDownloadsPool()
{
    for (auto &it : pool) {
        if (it)
            delete it;
    }
}

void FileDownloadsPool::addDownload(QString req, QObject *obj, const char *slot,
                                    void *info)
{

    pool.append(new FileDownloader(QUrl(req), this));
    pool.back()->info = info;
    connect(pool.back(), SIGNAL(downloaded(FileDownloader *, QByteArray *)),  obj, slot);

    pool.back()->activate();                                                    /// ### thread unsafe
}

QByteArray FileDownloadsPool::extractDataAndFree(FileDownloader *ploader)
{
    QByteArray puredata = *(ploader->getData());
    delete ploader;

    QMutableVectorIterator<FileDownloader *> it(pool);
    while (it.hasNext()) {
        if (it.next() == ploader)
            it.remove();
    }
    return puredata;
}

