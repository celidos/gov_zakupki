#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

/// http://wiki.qt.io/Download_Data_from_URL

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

/// http://www.mathnet.ru/images/sem_rus.jpg

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QUrl url, QObject *parent = 0);
    virtual ~FileDownloader();
    QByteArray *downloadedData() const;

    void activate();
    QByteArray *getData();
    void *info;

signals:
    void downloaded(FileDownloader *pointer, QByteArray *pdata);

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QUrl imageUrl;
    QNetworkAccessManager *m_WebCtrl;
    QByteArray *p_DownloadedData;
};

#endif // FILEDOWNLOADER_H
