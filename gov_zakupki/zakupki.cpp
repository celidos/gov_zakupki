#include "zakupki.h"

const QStringList zakupki::coloumn_headers_ret()
{
    QStringList res;
    res << "Поле" << "Значение";
    return res;
}

QStringList zakupki::cc_fields_headers_ret()
{
    QStringList res;
    res << "Идентификационный код закупки" << "Полное наименование заказчика" << "Сокращенное наименование заказчика"
        << "ИНН" << "КПП" << "Уровень бюджета" << "Дата заключения контракта"
        << "Номер контракта"
        << "Цена контракта" << "Поставщик" << "ИНН поставщика" << "КПП поставщика"
        << "Наименование объекта закупки" << "Код бюджетной классификации";
    return res;
}

QStringList zakupki::ag_fields_headers_ret()
{
    QStringList res;
    res << "Идентификационный код закупки" << "Номер соглашения" << "Наименование ГБРС" << "Получатель"
        << "Дата заключения" << "Размер субсидии" << "ГРБС полное наименование"
        << "ГРБС краткое наименование" << "ГРБС ИНН" << "ГРБС КПП"
        << "Код ГБРС" << "Номер лицевого счета" << "ИНН получателя субсидии"
        << "КПП получателя субсидии";

    return res;
}

QStringList zakupki::docs_fields_headers_ret()
{
    QStringList res;
    res << "Файл" << "Открыть";
    return res;
}

QStringList zakupki::transfer_info_fields_headers_ret()
{
    QStringList res;
    res << "Номер платежного документа" << "Дата" << "Сумма перечисления";
    return res;
}
