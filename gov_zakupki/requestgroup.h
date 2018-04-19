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
#include "xlsxdocument.h"

// struct REQUEST PARAMS -------------------------------------------------------

typedef QVector<zakupki::contract_record> Group;

void exportGroupToExcel(Group &group, QString fullFilename);

struct RequestParams {
    QString id;
    QString customer_inn;
    bool needFiles;
    bool needTransferDetails;
    ReqDocumentManager *docmanager;

    RequestParams(QString req, bool needfiles=false, bool needTransferInfo=false,
                  ReqDocumentManager *_docmanager=nullptr) {
        this->id = req;
        this->customer_inn = QString();
        this->needFiles = needfiles;
        this->needTransferDetails = needTransferInfo;
        this->docmanager = _docmanager;

    }
    RequestParams() {}
};

struct OrganizationInfo {
    QString fullname;
    QString cpz;
    QString fz94id;
    QString fz223id;
    QString inn;
    QString kpp;
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

    double maxSum;
    double minSum;

    OrganizationInfo orginfo;

    zakupki::ContractRecordType recordtype;
    int page_num;

    FilterRequestParams() {}
    ~FilterRequestParams() {}

    QString constructZakupkiUrl(int pagenum);
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
                          bool needFiles=false, bool needTransferInfo=false,
                          ReqDocumentManager *docmanager=nullptr, QObject *parent=0);
    explicit RequestGroup(QStringList &reqs, QObject *obj, const char *slot,
                          bool needFiles=false, bool needTransferInfo=false,
                          ReqDocumentManager *docmanager=nullptr, QObject *parent=0);
//    explicit RequestGroup(const RequestGroup& copy) = default;

    virtual ~RequestGroup();

    void activate();
    const int length();

    Group *getData();

    FilterRequestParams *rp;

private:
    QVector<RequestParams> requests_params;
    QVector<SingleRequest *> single_reqs;

    Group accepted;

    QObject *responseObject;
    const char *responseSlot;

    int activatedReqs;
    int acceptedReqs;
    int totalReqs;

public slots:
    void acceptSingleRequest(SingleRequest *psingle);

signals:
    void ready(RequestGroup *pgroup);
    void gotElement();
};

QString numToLetter(int x);
void updateRecordGeneralInfoWithJson(zakupki::contract_record *record, QJsonObject &mainobj);

#endif // REQUESTGROUP_H
