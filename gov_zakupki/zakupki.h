#ifndef ZAKUPKI_H
#define ZAKUPKI_H

#include <cstdlib>

#include <QStringList>
#include <QVector>
#include <QDebug>

using std::size_t;

const char BLANK_CHAR = '%';
const int  PARALLEL_LOADING_NUM_THREADS = 10;
const int  ZAKUPKI_MAX_PAGES_WITH_100_PER_PAGE = 10;

namespace zakupki {

const size_t BUDGET_DB_PART_SIZE = 50;

// /!\ this is crucial, if it is less than amount of used lines in TableWidget,
//     you'll get out of bounds error
const size_t MAX_FIELDS = 14;

const QStringList coloumn_headers_ret();
const QStringList COLUMN_HEADERS(coloumn_headers_ret());

// -----------------------------------------------------------------------------
// cc = contract card
// ag = agreement

const size_t CC_MAX_FIELDS = 35;
const size_t CC_MAX_AUTOMATED_FIELDS = 30;

const size_t CC_INDEX_UID = 0;
const size_t CC_INDEX_PROVIDER_NAME = 30;
const size_t CC_INDEX_PROVIDER_INN = 31;
const size_t CC_INDEX_PROVIDER_KPP = 32;
const size_t CC_INDEX_PURCHASE_NAME = 33;
const size_t CC_INDEX_PURCHASE_CODE = 34;

static QString CC_FIELDS_KEYWORDS[] = {"~~skip~~",
    "Статус контракта", "Номер извещения об осуществлении закупки",
    "Идентификационный код закупки (ИКЗ)", "Уникальный номер позиции плана-графика",
    "Способ определения поставщика (подрядчика, исполнителя)",
    "Дата размещения (по местному времени)", "Основание заключения контракта с единственным поставщиком",
    "Реквизиты документа, подтверждающего основание заключения контракта", // 9



    "Полное наименование заказчика", "Сокращенное наименование заказчика",

    "Идентификационный код заказчика", "Код организационно-правовой формы",
    "Код ОКПО", "Наименование внебюджетных средств", //15

    ">ИНН", ">КПП", "Уровень бюджета", "Дата заключения контракта", "Номер контракта",
    "Цена контракта<", // 21


    "Дата начала исполнения контракта", "Дата окончания исполнения контракта",
    "Номер реестровой записи реестра банковских гарантий (исполнение контракта)",
    "Размер обеспечения исполнения контракта",
    "В том числе НДС",
    "Срок, на который предоставляется гарантия",
    "Информация о требованиях к гарантийному обслуживанию товара",
    "Номер реестровой записи реестра банковских гарантий",
    "Размер обеспечения исполнения обязательств по предоставленной гарантии" // 30
};


QStringList cc_fields_headers_ret();
const QStringList CC_FIELDS_HEADERS(cc_fields_headers_ret());

const size_t CC_REQ_FULL_LEN = 20;
const size_t CC_REQ_SHORT_LEN = 19;



const size_t AG_REQ_FULL_LEN = 20;
const size_t AG_REQ_SHORT_LEN = 11;

const size_t AG_MAX_FIELDS = 14;
QStringList ag_fields_headers_ret();
const QStringList AG_FIELDS_HEADERS(ag_fields_headers_ret());

const size_t AG_INDEX_UID = 0;
const size_t AG_INDEX_NUM_AGREEM = 1;
const size_t AG_INDEX_GRBS_NAME = 2;
const size_t AG_INDEX_RECEIVER = 3;
const size_t AG_INDEX_DATE_AGREEM = 4;
const size_t AG_INDEX_CURRENCY_SUM = 5;
const size_t AG_INDEX_GRBS_FULL_NAME = 6;
const size_t AG_INDEX_GRBS_SHORT_NAME = 7;
const size_t AG_INDEX_GRBS_INN = 8;
const size_t AG_INDEX_GRBS_KPP = 9;
const size_t AG_INDEX_GRBS_CODE = 10;
const size_t AG_INDEX_GRBS_DEPOSIT_NUMBER = 11;
const size_t AG_INDEX_RECEIVER_INN = 12;
const size_t AG_INDEX_RECEIVER_KPP = 13;

const size_t DOCS_MAX_DOCS = 2000;
QStringList docs_fields_headers_ret();
const QStringList DOCS_COLUMNS_HEADERS(docs_fields_headers_ret());

enum ContractRecordType {RT_ZAKUPKI, RT_BUDGET};

struct contract_record {
    ContractRecordType rtype;
    QVector<QString> values;
    QVector<int> indices;

    QVector<qlonglong> ag_transfer_num;
    QVector<QString> ag_transfer_date;
    QVector<qlonglong> ag_transfer_sum;

    void add_param(int index, QString value) {
        values.append(value);
        indices.append(index);
    }

    bool isReady() {
        if ((rtype == RT_ZAKUPKI && values.length() == CC_MAX_FIELDS) ||
            (rtype == RT_BUDGET  && values.length() == AG_MAX_FIELDS)) {
//            qWarning() << "debug#4 " << values.length();

            return true;
        }
        return false;
    }
};

QStringList transfer_info_fields_headers_ret();
const QStringList TRANSFER_INFO_COLUMNS_HEADERS(transfer_info_fields_headers_ret());

static bool switcher_FILL_NOT_FOUND(true);

} // namespace zakupki

#endif // ZAKUPKI_H
