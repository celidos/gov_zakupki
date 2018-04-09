#ifndef REQUESTPOOL_H
#define REQUESTPOOL_H

#include <QObject>
#include <QVector>
#include <QStringList>
#include <QProgressBar>

#include "zakupki.h"
#include "requestgroup.h"
#include "reqdocumentmanager.h"
#include "filedownloadspool.h"

#include "xlsxdocument.h"

struct OrganizationInfo {
    QString fillname;
};

class RequestPool : public QObject
{
    Q_OBJECT
public:
    explicit RequestPool(ReqDocumentManager *manager=nullptr,
                         QObject *parent = 0);
    virtual ~RequestPool();

    void setPool(FileDownloadsPool *pool);
    void setProgressBar(QProgressBar *pbar);
    void setDocManager(ReqDocumentManager *docmanager);
    void addSingleReq(QString req,  QObject *obj, const char *slot,
                      bool needFiles, void *info = nullptr);
    void addGroup(QStringList &reqs, QObject *obj, const char *slot,
                  bool needFiles=false, void *info = nullptr);
    void addGroup(RequestGroup &group, QObject *obj, const char *slot, void *info = nullptr);

    void addZakupkiFilterReq(FilterRequestParams &rp, QObject *obj, const char *slot, void *info = nullptr);

    QVector<int> addBudgetFilter(FilterRequestParams &rp);

    Group extractDataAndFree(RequestGroup *pgroup);

    // f fast

    void updateBudgetDatabase();
    void loadDatabaseToMemory();

    void loadOrganizationsFromZakupki();

private:
    FileDownloadsPool *fpool;
    ReqDocumentManager *docmanager;

    QProgressBar *progressbar;

    QVector<RequestGroup *> rpool;

    QObject *filterreq_lastobj;
    const char *filterreq_lastslot;

    QVector<zakupki::contract_record> budget_db;

    void getLastUpdateDate(); /// ### not working, todo

    int budgetDbAcceptedPagesCount;
    int budgetDbSubmittedPagesCount;
    int budgetDbMaxPagesCount;
    bool budgetDbReady;

    int zakupkiOrgDbAcceptedPagesCount;
    int zakupkiOrgDbSubmittedPagesCount;
    int zakupkiOrgDbMaxPagesCount;
    bool zakupkiOrgDbReady;

private slots:
    void acceptProgress();

    void acceptCustomerInn(FileDownloader *ploader);
    void acceptFilterSearchResults(FileDownloader *ploader);

    void acceptBudgetDbInitPage(FileDownloader *ploader);
    void acceptBudgetDbPartialData(FileDownloader *ploader);

    void acceptZakupkiOrganizationInfoPage(FileDownloader *ploader);

signals:

};

#endif // REQUESTPOOL_H
