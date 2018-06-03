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
#include "globalstatusbar.h"

#include "xlsxdocument.h"



struct ZakupkiSearchParams {
    OrganizationInfo orginfo;
    double priceFrom;
    double priceTo;

    int pageNum;
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
                      bool needFiles=false, bool needTransferInfo=false, void *info = nullptr);
    void addGroup(QStringList &reqs, QObject *obj, const char *slot,
                  bool needFiles=false, bool needTransferInfo=false, void *info = nullptr);
    void addGroup(RequestGroup &group, QObject *obj, const char *slot, void *info = nullptr);

    void addZakupkiFilterReq(FilterRequestParams *rp, QObject *obj, const char *slot);

    QVector<int> addBudgetFilter(FilterRequestParams &rp);

    Group extractDataAndFree(RequestGroup *pgroup);

    void exportToExcelBudgetFilter(QString filename);

    // f fast

    void updateBudgetDatabase();
    void loadDatabaseToMemory();

    void loadOrganizationsFromZakupki();

private:
    FileDownloadsPool *fpool;
    ReqDocumentManager *docmanager;

    QProgressBar *progressbar;

    QVector<RequestGroup *> rpool;

    // zakupki

    QObject *filterzakupki_lastobj;
    const char *filterzakupki_lastslot;
    FilterRequestParams *filterzakupki_lastrp;
    QStringList last_zakupki_ids;
    Group last_zakupkiFilter;

    QVector<int> last_budgetFilter;

    QVector<zakupki::contract_record> budget_db;
    QVector<OrganizationInfo> org_db;

    void getLastUpdateDate(); /// ### not working, todo

    // budget

    int budgetDbAcceptedPagesCount;
    int budgetDbSubmittedPagesCount;
    int budgetDbMaxPagesCount;
    bool budgetDbReady;

    // zakupki

//
//    QObject *filterzakupki_lastobj;
//    const char *filterzakupki_lastslot;

    // not used

    int zakupkiOrgDbAcceptedPagesCount;
    int zakupkiOrgDbSubmittedPagesCount;
    int zakupkiOrgDbMaxPagesCount;
    bool zakupkiOrgDbReady;

private slots:
    void acceptProgress();

    void acceptCustomerInn(FileDownloader *ploader);
    void acceptZakupkiFilterSearchResults(FileDownloader *ploader);

    void acceptBudgetDbInitPage(FileDownloader *ploader);
    void acceptBudgetDbPartialData(FileDownloader *ploader);

    void acceptZakupkiOrganizationInfoPage(FileDownloader *ploader);

signals:
    void readyFilterZakupki();
};

bool match_str(QString &str, QString &pattern);
bool match(FilterRequestParams &rp, zakupki::contract_record &x);

#endif // REQUESTPOOL_H
