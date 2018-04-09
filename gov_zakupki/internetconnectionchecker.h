#ifndef INTERNETCONNECTIONCHECKER_H
#define INTERNETCONNECTIONCHECKER_H

#include <QTimer>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QLabel>
#include <QNetworkReply>

class InternetConnectionChecker : public QObject
{
    Q_OBJECT
public:
    explicit InternetConnectionChecker(QLabel *plabel, QObject *parent = nullptr);
    virtual ~InternetConnectionChecker();
private:
    QTimer *tmr;
    QNetworkReply *reply;
    QEventLoop loop;
    QLabel *label;

public slots:
    void checkInternetConnection();
};

#endif // INTERNETCONNECTIONCHECKER_H
