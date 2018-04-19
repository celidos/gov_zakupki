#include "requestgroup.h"

// SINGLE REQUEST --------------------------------------------------------------

SingleRequest::SingleRequest(RequestParams _req, QObject *obj, const char *slot,
                             QObject *parent) :
    QObject(parent), responseObject(obj), responseSlot(slot), req(_req)
{
    connect(this, SIGNAL(ready(SingleRequest*)), obj, slot);
    record = new zakupki::contract_record();
}

SingleRequest::~SingleRequest()
{

}

void SingleRequest::activate()
{
    qWarning()  << "Processing request" << this->req.id;

    QString strreq = req.id;
    FileDownloadsPool *filePool = FileDownloadsPool::instance();

    // case CONTRACT CARD
    if ((strreq.length() == zakupki::CC_REQ_FULL_LEN && strreq.mid(0, 8) != "00000000")
        || strreq.length() == zakupki::CC_REQ_SHORT_LEN) {
        if (strreq.length() == zakupki::CC_REQ_FULL_LEN) {
            strreq.truncate(zakupki::CC_REQ_SHORT_LEN);
        }
        qWarning()  << "Добавляем задачу zakupki - SEARCH скачивание!";
        this->record->rtype = zakupki::RT_ZAKUPKI;
        downloadEveryPage(QString("http://zakupki.gov.ru/epz/contract/contractCard/")
                          + "common-info.html?reestrNumber="
                          + strreq);
    }
    // case AGREEMENT
    else if (strreq.length() == zakupki::AG_REQ_SHORT_LEN ||
             strreq.length() == zakupki::AG_REQ_FULL_LEN) {
        if (strreq.length() == zakupki::AG_REQ_FULL_LEN)
            strreq = strreq.mid(8, zakupki::AG_REQ_SHORT_LEN);
        if (strreq.length() == zakupki::AG_REQ_SHORT_LEN) {
            this->record->rtype = zakupki::RT_BUDGET;
            qDebug() << "Добавляем задачу budget - скачивания!";

            QString full_url = QString("http://budget.gov.ru/epbs/registry/grants/")
                    + "data?pageNum=1&pageSize=5&filterchangenumstart="
                    + QUrl::toPercentEncoding(strreq)
                    + "___&sortField=renewdate&sortDir=desc&blocks=doc_0_1000,info,"
                    + "grbs,rcv,change";

            if (req.needTransferDetails){
                full_url += ",payment";
            }

            filePool->addDownload(full_url,
                this, SLOT(parseBudgetData(FileDownloader *)));
        } else {
            qWarning()  << "Incorrect request.";
            failwith("Неверный формат запроса.");
        }
    }
    else {
        qWarning()  << "Incorrect request.";
        failwith("Неверный формат запроса.");
    }
}

zakupki::contract_record* SingleRequest::getData()
{
    return this->record;
}

QString SingleRequest::extractParam1(QString &html, QString param, int startpos)
{
    qWarning() << "looking for param" << param;
    int pos = startpos;
    pos = html.indexOf(param, pos);

    if (pos >= 0) {
        int newpos = html.indexOf("<td>", pos);
        QString newlink = html.mid(newpos + 4, html.indexOf("</td>", newpos + 4)
                                   - newpos - 4);

        qWarning() << "get newlink" << newlink;
        return  newlink;
    }
    return QString("<font color=""red"">not found</font>");
}

QString SingleRequest::extractParam2(QString &html, int &pos)
{
    int newpos = html.indexOf(QString("epz_aware"), pos);
    if (newpos != -1) {
        newpos = html.indexOf("href=", newpos);
        int index2  =  html.indexOf('"', newpos);
        qDebug() << "help: " << html.indexOf('"', index2 + 1);
        QString newlink = html.mid(index2+1, html.indexOf('"', index2 + 1) - index2 - 1);
        pos = index2 + 6;
        return newlink;
    }
    pos = -1;
    return QString("");
}

void SingleRequest::parseZakupkiGeneralInfoPage(FileDownloader *loader)
{
    qDebug() << " ------------ Parsing data page! -----------------";

    FileDownloadsPool *filePool = FileDownloadsPool::instance();
    QString html = QString(filePool->extractDataAndFree(loader));

    record->add_param(zakupki::CC_INDEX_UID, req.id);

    qWarning() << "$$$ here";

    for (size_t i = 1; i < zakupki::CC_MAX_AUTOMATED_FIELDS; ++i) {
        QString newparam = extractParam1(html, zakupki::CC_FIELDS_KEYWORDS[i]);
        record->add_param(i, removeHtml(newparam));
    }


    int provider_pos = html.indexOf("Информация о поставщиках");
    int provider_title_pos = html.indexOf("tdHead", provider_pos + 1);
    provider_title_pos = html.indexOf("<tr>", provider_title_pos + 1);
    provider_title_pos = html.indexOf("<tr>", provider_title_pos + 1);
    provider_title_pos = html.indexOf("<br>", provider_title_pos + 1);
    QString provider_title = html.mid(provider_title_pos + 4,
        html.indexOf("participant", provider_title_pos + 2) - provider_title_pos - 16);
    provider_title = removeHtml(provider_title);
    record->add_param(zakupki::CC_INDEX_PROVIDER_NAME, provider_title);
    record->add_param(zakupki::CC_INDEX_PROVIDER_INN,
        extractParam1(html, "ИНН", provider_pos));
    record->add_param(zakupki::CC_INDEX_PROVIDER_KPP,
        removeHtml(extractParam1(html, "КПП", provider_pos).split('\n')[0]));

    checkReady();
}

void SingleRequest::parseZakupkiPurchaseInfo(FileDownloader *loader)
{
    qDebug() <<" ------------ Parsing purchase info page! -----------------";

    FileDownloadsPool *filePool = FileDownloadsPool::instance();
    QString html = QString(filePool->extractDataAndFree(loader));

    qDebug() << html;

    int purchase_block = html.indexOf("Объект закупки");
    int purchase_title_pos = html.indexOf("tdHead", purchase_block + 1);
    purchase_title_pos = html.indexOf("<tr>", purchase_title_pos + 1);
    purchase_title_pos = html.indexOf("<tr>", purchase_title_pos + 1);
    purchase_title_pos = html.indexOf("<td>", purchase_title_pos + 1);

    QString purchase_name = html.mid(purchase_title_pos + 4,
        html.indexOf("</td>", purchase_title_pos + 2) - purchase_title_pos - 4);
    purchase_name = removeHtml(purchase_name);

    qWarning() << "GAYGAYGAY" << purchase_name;

    record->add_param(zakupki::CC_INDEX_PURCHASE_NAME,  purchase_name);

    // budgetary funds
    int budgetary_block = html.indexOf("За счет бюджетных средств");
    int budgetary_code_pos = html.indexOf("subTdHead", budgetary_block + 1);
    budgetary_code_pos = html.indexOf("<tr>", budgetary_code_pos + 1);
    budgetary_code_pos = html.indexOf("<tr>", budgetary_code_pos + 1);
    budgetary_code_pos = html.indexOf("<td>", budgetary_code_pos + 1);

    QString purchase_code = html.mid(budgetary_code_pos + 4,
        html.indexOf("</td>", budgetary_code_pos + 2) - budgetary_code_pos - 4);
    purchase_code = removeHtml(purchase_code);

    record->add_param(zakupki::CC_INDEX_PURCHASE_CODE,  purchase_code);
    checkReady();
}

void SingleRequest::parseZakupkiDocumentsPage(FileDownloader *loader)
{
    qDebug() << "------------ Ищем документацию по закупке! ------------";
    FileDownloadsPool *filePool = FileDownloadsPool::instance();
    QString html = QString(filePool->extractDataAndFree(loader));

    qDebug() << html;

    qDebug() << "Найденные файлы:";

    int pos_1 = 0;
    int pos;
    int endpos_1;
    {
    pos_1 = html.indexOf("noticeTabBoxWrapper", pos_1);
    pos_1 = html.indexOf("notice-documents", pos_1);
    pos_1 = html.indexOf("vertical-align: top; width: 250px; padding-top: 0", pos_1+1);
    endpos_1 = html.indexOf("noticeTabBoxWrapper", pos_1);

    pos = pos_1;
    while (pos < endpos_1) {
        pos = html.indexOf("class=\"documentsInfoContract", pos+1);
        if (pos == -1) break;
        pos = html.indexOf("class=\"documentsInfoContract", pos+1);
        if (pos == -1) break;
        if (pos >= endpos_1)
            break;
        pos = html.indexOf("<a", pos);

        QString newurl = html.mid(pos + 3, html.indexOf("</a>", pos + 1) - pos - 3);
        req.docmanager->addDirt(newurl, DC_CONTRACT_INFO);
        qDebug() << "* " << newurl;
        qDebug() << pos << endpos_1 << pos_1;
    }

    }
    // ---------------------------------------------------------------------------

    {
    pos_1 = html.indexOf("notice-documents", endpos_1);
    pos_1 = html.indexOf("vertical-align: top; width: 250px; padding-top: 0", pos_1+1);
    endpos_1 = html.indexOf("Информация об исполнении", pos_1);

    pos = pos_1;
    while (pos < endpos_1) {
        pos = html.indexOf("class=\"documentsInfoContract", pos+1);
        if (pos == -1) break;
        pos = html.indexOf("class=\"documentsInfoContract", pos+1);
        if (pos == -1) break;
        if (pos >= endpos_1)
            break;
        pos = html.indexOf("<a", pos);

        QString newurl = html.mid(pos + 3, html.indexOf("</a>", pos + 1) - pos - 3);
        req.docmanager->addDirt(newurl, DC_CONTRACT_INFO);
    }
    }

//    if (ui->checkBox->isChecked())
        req.docmanager->downloadAll();
}

void updateRecordGeneralInfoWithJson(zakupki::contract_record *record, QJsonObject &mainobj) {
    QJsonObject info = mainobj["info"].toObject();
    QJsonObject grbs = mainobj["grbs"].toObject();
    QJsonObject receiver = mainobj["receiver"].toArray().takeAt(0).toObject();

    record->add_param(zakupki::AG_INDEX_UID, info["regNum"].toString());
    record->add_param(zakupki::AG_INDEX_NUM_AGREEM, info["numAgreem"].toString());
    record->add_param(zakupki::AG_INDEX_DATE_AGREEM, info["dateAgreem"].toString());
    record->add_param(zakupki::AG_INDEX_CURRENCY_SUM, info["currencySum"].toString());
    record->add_param(zakupki::AG_INDEX_GRBS_NAME, grbs["fullName"].toString());
    record->add_param(zakupki::AG_INDEX_RECEIVER, receiver["fullName"].toString());
    record->add_param(zakupki::AG_INDEX_GRBS_FULL_NAME, grbs["fullName"].toString());
    record->add_param(zakupki::AG_INDEX_GRBS_SHORT_NAME, grbs["shortName"].toString());
    record->add_param(zakupki::AG_INDEX_GRBS_INN, grbs["inn"].toString());
    record->add_param(zakupki::AG_INDEX_GRBS_KPP, grbs["kpp"].toString());
    record->add_param(zakupki::AG_INDEX_GRBS_CODE, grbs["kbkInput"].toString());
    record->add_param(zakupki::AG_INDEX_GRBS_DEPOSIT_NUMBER, grbs["grbsAccount"].toString());
    record->add_param(zakupki::AG_INDEX_RECEIVER_INN, receiver["inn"].toString());
    record->add_param(zakupki::AG_INDEX_RECEIVER_KPP, receiver["kpp"].toString());
}

void updateRecordTransferInfoWithJson(zakupki::contract_record *record, QJsonObject &mainobj) {
    QJsonArray pay = mainobj["payments"].toArray();
    foreach (const QJsonValue & value, pay) {
        QJsonObject obj = value.toObject();
        record->ag_transfer_date.append(QDateTime::fromString(obj["date"].toString(), "yyyyMMdd").
                toString("dd.MM.yyyy"));

        record->ag_transfer_num.append(obj["num"].toString().toLongLong());
        record->ag_transfer_sum.append(obj["sum"].toString().toLongLong());
    }
}

void SingleRequest::parseBudgetData(FileDownloader *loader)
{
//    docmanager->clear();
    qWarning() << "Parsing budget data...";

    FileDownloadsPool *filePool = FileDownloadsPool::instance();
    QString json_text = QString(filePool->extractDataAndFree(loader));

    QJsonDocument d = QJsonDocument::fromJson(json_text.toUtf8());
    QJsonObject obj = d.object();
    QJsonArray data = obj["data"].toArray();
    QJsonObject mainobj = data.takeAt(0).toObject();
    QJsonArray docs = mainobj["documents"].toArray();

    record->add_param(zakupki::AG_INDEX_UID, req.id);

    updateRecordGeneralInfoWithJson(record, mainobj);

    if (req.needTransferDetails) {
        updateRecordTransferInfoWithJson(record, mainobj);
    }

    foreach (const QJsonValue & value, docs) {
        QJsonObject newobj = value.toObject();
        req.docmanager->addFileRecord(FileRecord(newobj["name"].toString(),
                                  "http://budget.gov.ru/epbs/registry/grants/attachment?id=" +
                                  newobj["id"].toString(), req.docmanager->getSize()));
    }
//    if (ui->checkBox->isChecked())
        req.docmanager->downloadAll();
    checkReady();

    qWarning() << "end!";
}

void SingleRequest::checkReady()
{
    if (record->isReady()) {
        qWarning() << "Oh, record is ready!";
        emit ready(this);
    }
}

void SingleRequest::downloadEveryPage(QString url)
{
    qWarning()  << "Вызвано Download every page...";

    FileDownloadsPool *filePool = FileDownloadsPool::instance();

    record->rtype = zakupki::RT_ZAKUPKI;

    filePool->addDownload(url, this,
                          SLOT(parseZakupkiGeneralInfoPage(FileDownloader *)),
                          (void *)record);

    QString purchaseInfoPage = url;
    purchaseInfoPage.replace("common-info", "payment-info-and-target-of-order");
    filePool->addDownload(purchaseInfoPage, this,
                          SLOT(parseZakupkiPurchaseInfo(FileDownloader*)),
                          (void *)record);

    if (this->req.needFiles) {
        QString documentsPage = url;
        this->req.docmanager->clear();
        documentsPage.replace("common-info", "document-info");
        qWarning() << "NEW LINK:" << documentsPage;
        filePool->addDownload(documentsPage, this, SLOT(parseZakupkiDocumentsPage(FileDownloader *)));
    }
}

// REQUEST GROUP ---------------------------------------------------------------

RequestGroup::RequestGroup(QString singleReq, QObject *obj, const char *slot,
                           bool needFiles, bool needTransferInfo, ReqDocumentManager *docmanager,
                           QObject *parent):
    QObject(parent), responseObject(obj), responseSlot(slot),
    activatedReqs(0), acceptedReqs(0), totalReqs(0)
{
    requests_params.append(RequestParams(singleReq, needFiles, needTransferInfo, docmanager));
}

RequestGroup::RequestGroup(QStringList &reqs, QObject *obj, const char *slot,
                           bool needFiles, bool needTransferInfo, ReqDocumentManager *docmanager,
                           QObject *parent):
    QObject(parent), responseObject(obj), responseSlot(slot),
    activatedReqs(0), acceptedReqs(0), totalReqs(0)
{
    for (auto &it : reqs) {
        requests_params.append(RequestParams(it, needFiles, needTransferInfo, docmanager));
    }
}

RequestGroup::~RequestGroup()
{

}

void RequestGroup::activate()
{
    totalReqs = requests_params.length();
    activatedReqs = 0;

    for (auto &it : requests_params) {
        SingleRequest *newReq = new SingleRequest(it, this,
            SLOT(acceptSingleRequest(SingleRequest*)), this);
        single_reqs.append(newReq);
//        newReq->activate();
    }

    for (int i = totalReqs - 1; i >= std::max(0, totalReqs - PARALLEL_LOADING_NUM_THREADS); --i) {
        single_reqs[i]->activate();
        ++activatedReqs;
    }
}

const int RequestGroup::length()
{
    return requests_params.length();
}

Group* RequestGroup::getData()
{
    return &(this->accepted);
}

void RequestGroup::acceptSingleRequest(SingleRequest *psingle)
{
    emit gotElement();
    zakupki::contract_record puredata = *(psingle->getData());
    accepted.push_back(puredata);
    delete psingle;

    QMutableVectorIterator<SingleRequest *> it(single_reqs);
    while (it.hasNext()) {
        if (it.next() == psingle)
            it.remove();
    }
//    return puredata;

    ++acceptedReqs;
    if (activatedReqs < totalReqs && !single_reqs.isEmpty()) {
        single_reqs[totalReqs - activatedReqs - 1]->activate();
        ++activatedReqs;
    }

    if (requests_params.length() == accepted.length()) {
        emit ready(this);
    }

    // do nothing
    qWarning()  << "### TODO fix this";
}

QString FilterRequestParams::constructZakupkiUrl(int pagenum)
{
    FilterRequestParams *rp = this;
    QString new_url = QString("http://zakupki.gov.ru/epz/contract/extendedsearch/")
            + "results.html?morphology=on&openMode=USE_DEFAULT_PARAMS"
            + "&pageNumber=" + QString::number(pagenum)
            + "&sortDirection=false&recordsPerPage=_100"
            + "&sortBy=PO_DATE_OBNOVLENIJA&fz44=on"
            + "&priceFrom=" + QString::number(rp->minSum)
            + "&priceTo=" + QString::number(rp->maxSum)
            + "&advancePercentFrom=hint"
            + "&advancePercentTo=hint"
            + "&contractStageList_0=on"
            + "&contractStageList_1=on"
            + "&contractStageList_2=on"
            + "&contractStageList_3=on"
            + "&contractStageList=0%2C1%2C2%2C3"
            + "&customerTitle=" + QUrl::toPercentEncoding(rp->orginfo.fullname)
            + "&customerCode=" + rp->orginfo.cpz
            + "&customerFz94id=" + rp->orginfo.fz94id
            + "&customerInn=" + rp->orginfo.inn;

    if (rp->dateStartUsed) {
        new_url += "&contractDateFrom=" + rp->dateStart.toString("dd.MM.yyyy");
    }

    if (rp->dateFinishUsed) {
        new_url += "&contractDateTo=" + rp->dateFinish.toString("dd.MM.yyyy");
    }

    if (!rp->gbrs_inn.isEmpty()) {
        new_url += "&supplierTitle=" + rp->gbrs_inn;
    }

    return new_url;
}
