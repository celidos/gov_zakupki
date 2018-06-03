#ifndef ADVANCEDTABLEWIDGET_H
#define ADVANCEDTABLEWIDGET_H

#include <QTableWidget>
#include <QVector>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QString>

#include "common.h"
#include "htmltextdelegate.h"
#include "zakupki.h"
#include "multiplepushbutton.h"

class AdvancedTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit AdvancedTableWidget(QWidget *parent = 0);
    virtual ~AdvancedTableWidget();

    void configure(size_t max_fields, QString btn_text,
                   QObject *obj, const char *slot);

    void configureForZakupki(QObject *obj, const char *slot);
    void configureForBudget(QObject *obj, const char *slot);
    void configureForDocs(QObject *obj, const char *slot);

    void clear();
    void safeSetItem(int x, int y, QString newtext);
    void appendItem(QString newtext);

private:
    QString btntext;
    const char *custom_slot;
    QObject *custom_object;

    QVector<QWidget *> button_widgets;
    QVector<QTableWidgetItem *> tableitems_widgets;
};

#endif // ADVANCEDTABLEWIDGET_H
