#include "requestpool.h"

RequestPool::RequestPool(ReqDocumentManager *manager,
                         QObject *parent) :
    QObject(parent), docmanager(manager), progressbar(nullptr),
    budgetDbAcceptedPagesCount(0), budgetDbSubmittedPagesCount(0),
    budgetDbReady(false), zakupkiOrgDbAcceptedPagesCount(0),
    zakupkiOrgDbMaxPagesCount(0), zakupkiOrgDbSubmittedPagesCount(0),
    zakupkiOrgDbReady(false)
{
    fpool = FileDownloadsPool::instance();
}

RequestPool::~RequestPool()
{

}

void RequestPool::setPool(FileDownloadsPool *pool)
{
    this->fpool = pool;
}

void RequestPool::setProgressBar(QProgressBar *pbar)
{
    progressbar = pbar;
}

void RequestPool::setDocManager(ReqDocumentManager *docmanager)
{
    this->docmanager = docmanager;
}

void RequestPool::addSingleReq(QString req, QObject *obj, const char *slot,
                               bool needFiles, void *info)
{
    qWarning() << "LOLKEK";
    QStringList reqs(req);
    addGroup(reqs, obj, slot, needFiles, info);
}

void RequestPool::addGroup(QStringList &reqs, QObject *obj, const char *slot,
                           bool needFiles, void *info)
{
    qWarning() << "Adding new rgroup...";
    RequestGroup *group = new RequestGroup(reqs, obj, slot, needFiles, docmanager, this);
    rpool.append(group);
    connect(group, SIGNAL(ready(RequestGroup*)), obj, slot);

    if (progressbar) {
        progressbar->setMaximum(group->length());
        connect(rpool.back(), SIGNAL(gotElement()), this, SLOT(acceptProgress()));
    }

    rpool.back()->activate();
}



void RequestPool::addZakupkiFilterReq(FilterRequestParams &rp, QObject *obj,
                               const char *slot, void *info)
{
    filterreq_lastobj = obj;
    filterreq_lastslot = slot;
    QString customer_organization_name;
    rp.customer_inn = rp.customer_inn.trimmed();
    if (!rp.customer_inn.isEmpty()) {
        // looking for certain customer inn
        // let's know organization name firstly

        QString customer_inn_url = QString("http://zakupki.gov.ru/epz/")
            + "organization/chooseOrganization/chooseOrganizationTable.html?"
            + "placeOfSearch=FZ_94&organizationType=ALL&searchString="
            + rp.customer_inn
            + "&page=1";

        fpool->addDownload(customer_inn_url, this,
                           SLOT(acceptCustomerInn(FileDownloader*)),
                           nullptr);
    }
}

int find_index(QVector<int> &ind, int lookingFor)
{
    int t = 0;
    for (t = 0; t < ind.size(); ++t)
        if (ind[t] == lookingFor)
            return t;
    return 0;
}


bool match_str(QString &str, QString &pattern) {
    if (str.length() != str.length())
        return false;

    for (int j = 0; j < str.length(); ++j) {
        if (pattern[j] != BLANK_CHAR && str[j] != pattern[j])
            return false;
    }

    return true;
}

bool match(FilterRequestParams &rp, zakupki::contract_record &x) {
    if (rp.recordtype == zakupki::RT_BUDGET) {
        if (!rp.gbrs_inn.isEmpty()) {
            int t = find_index(x.indices, zakupki::AG_INDEX_GRBS_INN);
            if (!match_str(x.values[t], rp.gbrs_inn))
                return false;
        }
        if (!rp.gbrs_kpp.isEmpty()) {
            int t = find_index(x.indices, zakupki::AG_INDEX_GRBS_KPP);
            if (!match_str(x.values[t], rp.gbrs_kpp))
                return false;
        }

        if (rp.dateStartUsed) {
            int t = find_index(x.indices, zakupki::AG_INDEX_DATE_AGREEM);

            qWarning() << "okay, " << x.values[t].split(QRegExp("\\s+"),
                                                        QString::SkipEmptyParts)[0];
            QDate curdate = QDate::fromString(x.values[t].split(QRegExp("\\s+"),
                QString::SkipEmptyParts)[0].trimmed(), "yyyy-MM-dd");

            qWarning() << "startDate looking at :" << curdate.toString("dd.mm.yyyy");

            if (curdate < rp.dateStart)
                return false;
        }

        if (rp.dateFinishUsed) {
            int t = find_index(x.indices, zakupki::AG_INDEX_DATE_AGREEM);

            qWarning() << "hmmm, " << x.values[t].split(QRegExp("\\s+"),
                                                        QString::SkipEmptyParts)[0];

            QDate curdate = QDate::fromString(x.values[t].split(QRegExp("\\s+"),
                QString::SkipEmptyParts)[0].trimmed(), "yyyy-MM-dd");

            qWarning() << "finishDate looking at :" << curdate.toString("dd.mm.yyyy");

            if (curdate > rp.dateFinish)
                return false;
        }

        int t = find_index(x.indices, zakupki::AG_INDEX_CURRENCY_SUM);
        long double sum = ((long double) x.values[t].toDouble());
        if (rp.minSum > sum || rp.maxSum < sum) {
            return false;
        }

        return true;
    } else {
        return false;
    }
}

QVector<int> RequestPool::addBudgetFilter(FilterRequestParams &rp)
{
    if (budgetDbReady) {
        QVector<int> ans;

        for (int i = 0; i < budget_db.size(); ++i) {
            zakupki::contract_record &el = budget_db[i];
            if (match(rp, el)) {
                ans.append(i);
            }
        }
        return ans;
    }
    return QVector<int>();
}

Group RequestPool::extractDataAndFree(RequestGroup *pgroup)
{
    Group puredata = *(pgroup->getData());
    delete pgroup;

    QMutableVectorIterator<RequestGroup *> it(rpool);
    while (it.hasNext()) {
        if (it.next() == pgroup)
            it.remove();
    }

    //    return puredata;

    return puredata;
}

void RequestPool::updateBudgetDatabase()
{

}

void RequestPool::loadDatabaseToMemory()
{
    qWarning() << "we are here";
    fpool->addDownload(QString("http://budget.gov.ru/epbs/registry/grants/data?")
                       + "pageNum=1&pageSize="
                       +  QString::number(zakupki::BUDGET_DB_PART_SIZE)
                       + "&sortField=renewdate&sortDir=desc",
                       this, SLOT(acceptBudgetDbInitPage(FileDownloader*)));

}

void RequestPool::loadOrganizationsFromZakupki()
{
//    qWarning() << "Got to acceptZakupki";

//    QJsonDocument d = QJsonDocument::fromJson(json_text.toUtf8());
//    QJsonObject obj = d.object();

    zakupkiOrgDbMaxPagesCount = 14507;

    qWarning() << "we think that here " << zakupkiOrgDbMaxPagesCount << "pages!";

    zakupkiOrgDbAcceptedPagesCount = 0;
    zakupkiOrgDbSubmittedPagesCount = 0;
    budget_db.clear();

    if (progressbar) {
        progressbar->setMaximum(14507);
        progressbar->setValue(0);
    }




    for (int i = 1; i <= 10; ++i) {
        fpool->addDownload(QString("http://zakupki.gov.ru/epz/organization/")
            + "chooseOrganization/chooseOrganizationTable.html?placeOfSearch=FZ_94"
            + "&organizationType=ALL&searchString=&page="
            + QString::number(zakupkiOrgDbSubmittedPagesCount),
            this, SLOT(acceptBudgetDbPartialData(FileDownloader*)));

        ++zakupkiOrgDbSubmittedPagesCount;
    }
}

void RequestPool::getLastUpdateDate()
{
//    QFile file("./budget_db/last_update.txt");
//    if(!file.open(QIODevice::ReadOnly)) {
//        QMessageBox::information(0, "error", file.errorString());
//    }

//    QTextStream in(&file);

//    while(!in.atEnd()) {
//        QString line = in.readLine();
//        QStringList fields = line.split(",");
//        model->appendRow(fields);
//    }

//    file.close();
}

void RequestPool::acceptProgress()
{
    if (progressbar) {
        int curValue = progressbar->value();
        if (curValue < progressbar->maximum())
            progressbar->setValue(curValue + 1);
    }
}

QStringList getGoodOrganizationName(QString &html)
{
    int general_title_pos = html.indexOf("organizationMap[\"0\"] = {", 0);
    int jumper = html.indexOf("cpz: '", general_title_pos + 1);
    QString cpz = html.mid(jumper + 6,
        html.indexOf("'", jumper + 7) - jumper - 6);
    cpz = removeHtml(cpz);
    qWarning() << "cpz " << cpz;

    jumper = html.indexOf("fz94id: '", general_title_pos + 1);
    QString fz94id = html.mid(jumper + 9,
        html.indexOf("'", jumper + 10) - jumper - 9);
    fz94id = removeHtml(fz94id);
    qWarning() << "fz94id " << fz94id;

    jumper = html.indexOf("fz223id: '", general_title_pos + 1);
    QString fz223id = html.mid(jumper + 10,
        html.indexOf("'", jumper + 10) - jumper - 10);
    fz223id = removeHtml(fz223id);

    jumper = html.indexOf("inn: '", general_title_pos + 1);
    QString inn = html.mid(jumper + 6,
        html.indexOf("'", jumper + 6) - jumper - 6);
    inn = removeHtml(inn);

    qWarning() << "inn " << inn;

    // --------------------------------------------------

    int purchase_title_pos = html.indexOf("<tr class=", 0);
    purchase_title_pos = html.indexOf("<td>", purchase_title_pos + 1);
    purchase_title_pos = html.indexOf("<td>", purchase_title_pos + 1);

    QString customer_title = html.mid(purchase_title_pos + 4,
        html.indexOf("</td>", purchase_title_pos + 2) - purchase_title_pos - 4);
    customer_title = removeHtml(customer_title);
    qWarning() << customer_title;

//    purchase_title_pos = html.indexOf("<td>", purchase_title_pos + 1);
//    QString customer_inn = html.mid(purchase_title_pos + 4,
//        html.indexOf("</td>", purchase_title_pos + 2) - purchase_title_pos - 4);
//    customer_inn = removeHtml(customer_inn);

//    var organizationMap = new Object();
//        organizationMap["0"] = {
//            cpz: '03333000744',
//            fz94id: '767604',
//            fz223id: '',
//            inn: '<span class="pinkBg">3711013676</span>',
//            draftId: ''
//        };

//    purchase_title_pos = html.indexOf("<td>", purchase_title_pos + 1);
//    QString customer_inn = html.mid(purchase_title_pos + 4,
//        html.indexOf("</td>", purchase_title_pos + 2) - purchase_title_pos - 4);
//    customer_inn = removeHtml(customer_inn);

    QStringList sl;
    sl << customer_title << cpz << fz94id << inn;

    return sl;
}

void RequestPool::acceptCustomerInn(FileDownloader *ploader)
{
    FileDownloadsPool *filePool = FileDownloadsPool::instance();
    QString html = QString(filePool->extractDataAndFree(ploader));

    QStringList customer_params = getGoodOrganizationName(html);

    QString new_url = QString("http://zakupki.gov.ru/epz/contract/extendedsearch/")
            + "results.html?morphology=on&openMode=USE_DEFAULT_PARAMS"
            + "&pageNumber=1&sortDirection=false&recordsPerPage=_50"
            + "&sortBy=PO_DATE_OBNOVLENIJA&fz44=on"
            + "&priceFrom=0"
            + "&priceTo=200000000000"
            + "&advancePercentFrom=hint"
            + "&advancePercentTo=hint"
            + "&contractStageList_0=on"
            + "&contractStageList_1=on"
            + "&contractStageList_2=on"
            + "&contractStageList_3=on"
            + "&contractStageList=0%2C1%2C2%2C3"
            + "&customerTitle=" + QUrl::toPercentEncoding(customer_params[0])
            + "&customerCode=" + customer_params[1]
            + "&customerFz94id=" + customer_params[2]
            + "&customerInn=" + customer_params[3];

    qWarning() << "we got url :\n\n" << new_url;

    filePool->addDownload(new_url, this, SLOT(acceptFilterSearchResults(FileDownloader*)));
    //    qWarning() << html;
}

void RequestPool::acceptFilterSearchResults(FileDownloader *ploader)
{
    FileDownloadsPool *filePool = FileDownloadsPool::instance();
    QString html = QString(filePool->extractDataAndFree(ploader));

    QVector<int> positions;
    QStringList IDs;
    int pos = 0;
    while (pos >= 0) {
        pos = html.indexOf("descriptTenderTd", pos + 1);
        if (pos >= 0)
            positions.append(pos);
    }

    for (auto &it : positions) {
        int newpos = html.indexOf("<a href=", it);
        QString newid = html.mid(newpos, html.indexOf("</a>", newpos + 6) - newpos);
        IDs << removeHtml(newid).mid(2);
    }

    qWarning() << "Найденные ссылки:";
    for (auto &it : IDs) {
        qWarning() << "* " << it;
    }

    addGroup(IDs, filterreq_lastobj, filterreq_lastslot);
}

void RequestPool::acceptBudgetDbInitPage(FileDownloader *ploader)
{
    qWarning() << "Got to acceptBudget!";
    QString json_text = QString(fpool->extractDataAndFree(ploader));

    QJsonDocument d = QJsonDocument::fromJson(json_text.toUtf8());
    QJsonObject obj = d.object();

    budgetDbMaxPagesCount = 1;//obj["pageCount"].toInt();

    qWarning() << "we think that here " << budgetDbMaxPagesCount << "pages!";

    budgetDbAcceptedPagesCount = 0;
    budgetDbSubmittedPagesCount = 0;
    budget_db.clear();

    if (progressbar) {
        progressbar->setMaximum(budgetDbMaxPagesCount);
        progressbar->setValue(0);
    }

    for (int i = 1; i <= std::min(10, budgetDbMaxPagesCount); ++i) {
        fpool->addDownload(QString("http://budget.gov.ru/epbs/registry/grants/data?")
                           + "pageNum=" + QString::number(i) + "&pageSize="
                           + QString::number(zakupki::BUDGET_DB_PART_SIZE) + "&sortField=renewdate&sortDir=desc"
                           + "&blocks=info,grbs,rcv,change",
                           this, SLOT(acceptBudgetDbPartialData(FileDownloader*)));
        ++budgetDbSubmittedPagesCount;
    }
}

void RequestPool::acceptBudgetDbPartialData(FileDownloader *ploader)
{
    QString json_text = QString(fpool->extractDataAndFree(ploader));

    qWarning() << "accepted page " << budgetDbAcceptedPagesCount;
    ++budgetDbAcceptedPagesCount;

    if (progressbar) {
        progressbar->setValue(budgetDbAcceptedPagesCount);
    }

    QJsonDocument d = QJsonDocument::fromJson(json_text.toUtf8());
    QJsonObject obj = d.object();
    QJsonArray data = obj["data"].toArray();

    foreach (QJsonValue objvalue, data) {
        QJsonObject obj = objvalue.toObject();
        budget_db.append(zakupki::contract_record());
        updateRecordWithJson(&budget_db.back(), obj);
        QString fullstr = "";
        for (auto &it : budget_db.back().values)
            fullstr += "; " + it;
        qWarning() << fullstr << "\n";
    }

    if (budgetDbAcceptedPagesCount >= budgetDbMaxPagesCount) {
        budgetDbReady = true;
    }

    if (budgetDbSubmittedPagesCount < budgetDbMaxPagesCount) {
        fpool->addDownload(QString("http://budget.gov.ru/epbs/registry/grants/data?")
                           + "pageNum=" + QString::number(budgetDbSubmittedPagesCount+1) + "&pageSize="
                           + QString::number(zakupki::BUDGET_DB_PART_SIZE) + "&sortField=renewdate&sortDir=desc"
                           + "&blocks=info,grbs,rcv,change",
                           this, SLOT(acceptBudgetDbPartialData(FileDownloader*)));
        ++budgetDbSubmittedPagesCount;



    }

    qWarning() << budget_db.size();
}

void RequestPool::acceptZakupkiOrganizationInfoPage(FileDownloader *ploader)
{
    int general_title_pos = 0;

    while ((general_title_pos = html.indexOf("organizationMap[",
                                             general_title_pos + 1)) != -1) {
        int jumper = html.indexOf("cpz: '", general_title_pos + 1);
        QString cpz = html.mid(jumper + 6,
            html.indexOf("'", jumper + 7) - jumper - 6);
        cpz = removeHtml(cpz);
        qWarning() << "cpz " << cpz;

        jumper = html.indexOf("fz94id: '", general_title_pos + 1);
        QString fz94id = html.mid(jumper + 9,
            html.indexOf("'", jumper + 10) - jumper - 9);
        fz94id = removeHtml(fz94id);
        qWarning() << "fz94id " << fz94id;

        jumper = html.indexOf("fz223id: '", general_title_pos + 1);
        QString fz223id = html.mid(jumper + 10,
            html.indexOf("'", jumper + 10) - jumper - 10);
        fz223id = removeHtml(fz223id);

        jumper = html.indexOf("inn: '", general_title_pos + 1);
        QString inn = html.mid(jumper + 6,
            html.indexOf("'", jumper + 6) - jumper - 6);
        inn = removeHtml(inn);
    }

    qWarning() << "inn " << inn;

    // --------------------------------------------------

    int purchase_title_pos = html.indexOf("<tr class=", 0);
    purchase_title_pos = html.indexOf("<td>", purchase_title_pos + 1);
    purchase_title_pos = html.indexOf("<td>", purchase_title_pos + 1);

    QString customer_title = html.mid(purchase_title_pos + 4,
        html.indexOf("</td>", purchase_title_pos + 2) - purchase_title_pos - 4);
    customer_title = removeHtml(customer_title);
    qWarning() << customer_title;

    QStringList sl;
    sl << customer_title << cpz << fz94id << inn;

    return sl;
}
