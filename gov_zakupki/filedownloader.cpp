#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl url, QObject *parent) :
    QObject(parent)
{
    m_WebCtrl = new QNetworkAccessManager();
    connect(m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
            this, SLOT (fileDownloaded(QNetworkReply*)));
    imageUrl = url;
}

FileDownloader::~FileDownloader() {
    if (p_DownloadedData)
        delete p_DownloadedData;
    m_WebCtrl->deleteLater();
}

void FileDownloader::fileDownloaded(QNetworkReply* pReply) {                    /// ### update some other
                                                                                ///     other network responses
    p_DownloadedData = new QByteArray(pReply->readAll());
    pReply->deleteLater();
    emit downloaded(this, p_DownloadedData);
}

QByteArray* FileDownloader::downloadedData() const {
    return p_DownloadedData;
}

void FileDownloader::activate()
{
    QNetworkRequest request(imageUrl);
    m_WebCtrl->get(request);
}

QByteArray *FileDownloader::getData()
{
    return p_DownloadedData;
}
