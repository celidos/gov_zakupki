#ifndef ZAKUPKI_H
#define ZAKUPKI_H

#include <cstdlib>

#include <QStringList>
#include <QVector>

using std::size_t;

const char BLANK_CHAR = '%';
const int  PARALLEL_LOADING_NUM_THREADS = 10;

namespace zakupki {

const size_t BUDGET_DB_PART_SIZE = 50;

// /!\ this is crucial, if it is less than amount of used lines in TableWidget,
//     you'll get out of bounds error
const size_t MAX_FIELDS = 13;

const QStringList coloumn_headers_ret();
const QStringList COLUMN_HEADERS(coloumn_headers_ret());

// -----------------------------------------------------------------------------
// cc = contract card
// ag = agreement

const size_t CC_MAX_FIELDS = 13;
const size_t CC_MAX_AUTOMATED_FIELDS = 8;

const size_t CC_INDEX_PROVIDER_NAME = 8;
const size_t CC_INDEX_PROVIDER_INN = 9;
const size_t CC_INDEX_PROVIDER_KPP = 10;
const size_t CC_INDEX_PURCHASE_NAME = 11;
const size_t CC_INDEX_PURCHASE_CODE = 12;

static QString CC_FIELDS_KEYWORDS[] = {
    "Полное наименование заказчика", "Сокращенное наименование заказчика",
    ">ИНН", ">КПП", "Уровень бюджета", "Дата заключения контракта", "Номер контракта",
    "Цена контракта<"
};

QStringList cc_fields_headers_ret();
const QStringList CC_FIELDS_HEADERS(cc_fields_headers_ret());

const size_t CC_REQ_FULL_LEN = 20;
const size_t CC_REQ_SHORT_LEN = 19;

const size_t AG_REQ_FULL_LEN = 20;
const size_t AG_REQ_SHORT_LEN = 11;

const size_t AG_MAX_FIELDS = 13;
QStringList ag_fields_headers_ret();
const QStringList AG_FIELDS_HEADERS(ag_fields_headers_ret());

const size_t AG_INDEX_NUM_AGREEM = 0;
const size_t AG_INDEX_GRBS_NAME = 1;
const size_t AG_INDEX_RECEIVER = 2;
const size_t AG_INDEX_DATE_AGREEM = 3;
const size_t AG_INDEX_CURRENCY_SUM = 4;
const size_t AG_INDEX_GRBS_FULL_NAME = 5;
const size_t AG_INDEX_GRBS_SHORT_NAME = 6;
const size_t AG_INDEX_GRBS_INN = 7;
const size_t AG_INDEX_GRBS_KPP = 8;
const size_t AG_INDEX_GRBS_CODE = 9;
const size_t AG_INDEX_GRBS_DEPOSIT_NUMBER = 10;
const size_t AG_INDEX_RECEIVER_INN = 11;
const size_t AG_INDEX_RECEIVER_KPP = 12;

const size_t DOCS_MAX_DOCS = 2000;
QStringList docs_fields_headers_ret();
const QStringList DOCS_COLUMNS_HEADERS(docs_fields_headers_ret());

enum ContractRecordType {RT_ZAKUPKI, RT_BUDGET};

struct contract_record {
    ContractRecordType rtype;
    QVector<QString> values;
    QVector<int> indices;
    void add_param(int index, QString value) {
        values.append(value);
        indices.append(index);
    }

    bool isReady() {
        if ((rtype == RT_ZAKUPKI && values.length() == CC_MAX_FIELDS) ||
            (rtype == RT_BUDGET  && values.length() == AG_MAX_FIELDS))
            return true;
        return false;
    }
};

} // namespace zakupki

#endif // ZAKUPKI_H
