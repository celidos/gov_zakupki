#ifndef REQUESTGROUP_H
#define REQUESTGROUP_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDate>

#include "common.h"
#include "zakupki.h"
#include "filedownloadspool.h"
#include "reqdocumentmanager.h"

// struct REQUEST PARAMS -------------------------------------------------------

typedef QVector<zakupki::contract_record> Group;

struct RequestParams {
    QString id;
    QString customer_inn;
    bool needFiles;
    ReqDocumentManager *docmanager;

    RequestParams(QString req, bool needfiles=false,
                  ReqDocumentManager *_docmanager=nullptr) {
        this->id = req;
        this->customer_inn = QString();
        this->needFiles = needfiles;
        this->docmanager = _docmanager;
    }
    RequestParams() {}
};

struct FilterRequestParams {
    QString id;
    QString customer_inn;
    QString gbrs_inn;
    QString gbrs_kpp;

    bool dateStartUsed;
    bool dateFinishUsed;

    QDate dateStart;
    QDate dateFinish;

    long double maxSum;
    long double minSum;

    zakupki::ContractRecordType recordtype;

    FilterRequestParams() {}
    ~FilterRequestParams() {}
};

// SINGLE REQUEST --------------------------------------------------------------

class SingleRequest : public QObject
{
    Q_OBJECT

public:
    explicit SingleRequest(RequestParams _req, QObject *obj, const char *slot,
                           QObject *parent=0);
    virtual ~SingleRequest();

    void activate();
    zakupki::contract_record *getData();

private:
    QObject *responseObject;
    const char *responseSlot;
    RequestParams req;

    zakupki::contract_record *record;

    void downloadEveryPage(QString url);

    QString extractParam1(QString &html, QString param, int startpos=0);
    QString extractParam2(QString &html, int &pos);

private slots:
    void parseZakupkiGeneralInfoPage(FileDownloader *loader);
    void parseZakupkiPurchaseInfo   (FileDownloader *loader);
    void parseZakupkiDocumentsPage  (FileDownloader *loader);

    void parseBudgetData(FileDownloader *loader);

    void checkReady();

signals:
    void ready(SingleRequest *psingle);
};

// REQUEST GROUP ---------------------------------------------------------------

class RequestGroup : public QObject
{
    Q_OBJECT

public:
    explicit RequestGroup(QString singleReq, QObject *obj, const char *slot,
                          bool needFiles=false, ReqDocumentManager *docmanager=nullptr, QObject *parent=0);
    explicit RequestGroup(QStringList &reqs, QObject *obj, const char *slot,
                          bool needFiles=false, ReqDocumentManager *docmanager=nullptr, QObject *parent=0);
//    explicit RequestGroup(const RequestGroup& copy) = default;

    virtual ~RequestGroup();

    void activate();
    const int length();

    Group *getData();

private:
    QVector<RequestParams> requests_params;
    QVector<SingleRequest *> single_reqs;

    Group accepted;

    QObject *responseObject;
    const char *responseSlot;

public slots:
    void acceptSingleRequest(SingleRequest *psingle);

signals:
    void ready(RequestGroup *pgroup);
    void gotElement();
};

void updateRecordWithJson(zakupki::contract_record *record, QJsonObject &mainobj);

#endif // REQUESTGROUP_H
