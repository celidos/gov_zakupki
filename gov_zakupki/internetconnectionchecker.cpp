#include "internetconnectionchecker.h"


InternetConnectionChecker::InternetConnectionChecker(QLabel *plabel, QObject *parent) :
    QObject(parent), label(plabel)
{
    reply = nullptr;
    tmr = new QTimer(this);
    tmr->setInterval(5000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(checkInternetConnection()));
    tmr->start();
}

InternetConnectionChecker::~InternetConnectionChecker()
{

}

void InternetConnectionChecker::checkInternetConnection()
{
    bool ans = false;
    QNetworkAccessManager nam;
    QNetworkRequest req(QUrl("http://www.google.com"));
    reply = nam.get(req);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->bytesAvailable()) {
        if (label)
            label->setText("<font color=""green"">online</font>");
        ans = true;
    } else {
        if (label)
            label->setText("<font color=""red"">Нет соединения</font>");
        ans = false;
    }
    delete reply;
    reply = nullptr;
//    return ans;
}
