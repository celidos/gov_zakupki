#include "advancedtablewidget.h"


AdvancedTableWidget::AdvancedTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
}

AdvancedTableWidget::~AdvancedTableWidget()
{

}

void AdvancedTableWidget::configure(size_t max_fields,
                                    QString btn_text, QObject *obj, const char *slot)
{
        btntext = btn_text;
        custom_slot = slot;
        custom_object = obj;

        setEditTriggers(QAbstractItemView::NoEditTriggers);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setColumnCount(2);
        setColumnWidth(0, int(this->width() * 0.6));

        size_t fields_indexes[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    //    item delegate for possible html in cells
    //    setItemDelegate(new HtmlDelegate(this, Qt::AlignLeft));
    //    setEditTriggers(QAbstractItemView::DoubleClicked |
    //                    QAbstractItemView::SelectedClicked);
    //    setSelectionBehavior(QAbstractItemView::SelectRows);

        // start filling table
        for (size_t i = 0; i < max_fields; ++i) {
            // add buttons
            QWidget* pWidget = new QWidget(this);
            MultipleButton* btn_edit = new MultipleButton(fields_indexes[i], pWidget);
            btn_edit->setText(btntext);
            QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
            pLayout->addWidget(btn_edit);
            pLayout->setAlignment(Qt::AlignCenter);
            pLayout->setContentsMargins(0, 0, 0, 0);
            pWidget->setLayout(pLayout);
            pWidget->setVisible(false);
            button_widgets.append(pWidget);
            connect(btn_edit, SIGNAL(clicked(int)), obj, slot);

            tableitems_widgets.append(new QTableWidgetItem());
        }
}

void AdvancedTableWidget::configureForZakupki(QObject *obj, const char *slot)
{
    clear();

    // custom code


//    QStandardItemModel *model= new QStandardItemModel(row,1);
//    model->setColumnCount(1);
//    QString readString;
//    for (int r = 0; r < row; r++) {
//    for (int column = 0; column < 1; column++) {
//    QModelIndex mindex = model->index(r, column);
//    model->setData(mindex,readString);
//    }
//    }
//    HtmlDelegate* delegate = new HtmlDelegate();
//    setModel(model);
//    QHeaderView* hhdr = ui->tableView->horizontalHeader();
//    QHeaderView* vhdr = ui->tableView->verticalHeader();
//    for (int r = 0; r < row; r++) {
//    for (int column = 0; column < 1; column++) {
//    vhdr->setSectionResizeMode(r, QHeaderView::ResizeToContents);
//    hhdr->setSectionResizeMode(column, QHeaderView::Stretch);
//    }
//    }
//    setWordWrap(true);
//    setTextElideMode(Qt::ElideLeft);
//    resizeColumnsToContents();
//    resizeRowsToContents();

//    setItemDelegate(delegate);



    // custom code


//    verticalHeader()->setSectionResizeMode(0);


    configure(zakupki::CC_MAX_FIELDS, "Copy", obj, slot);

    setRowCount(zakupki::CC_MAX_FIELDS);

    setVerticalHeaderLabels(zakupki::CC_FIELDS_HEADERS);
    setHorizontalHeaderLabels(zakupki::COLUMN_HEADERS);
    horizontalHeader()->setStretchLastSection(true);

    verticalHeader()->setFixedWidth(350);
//    set   ColumnWidth(0, int(this->width() * 0.6));
    setWordWrap(true);
    resizeRowsToContents();

    for (size_t i = 0; i < zakupki::CC_MAX_FIELDS; ++i) {
        setCellWidget(i, 1, button_widgets[i]);
        button_widgets[i]->setVisible(true);
        setItem(i, 0, tableitems_widgets[i]);
    }
}

void AdvancedTableWidget::configureForBudget(QObject *obj, const char *slot)
{
    clear();

    configure(zakupki::AG_MAX_FIELDS, "Copy", obj, slot);

    setRowCount(zakupki::AG_MAX_FIELDS);
    setVerticalHeaderLabels(zakupki::AG_FIELDS_HEADERS);
    setHorizontalHeaderLabels(zakupki::COLUMN_HEADERS);
    horizontalHeader()->setStretchLastSection(true);

    for (size_t i = 0; i < zakupki::AG_MAX_FIELDS; ++i) {
        setCellWidget(i, 1, button_widgets[i]);
        button_widgets[i]->setVisible(true);
        setItem(i, 0, tableitems_widgets[i]);
    }
}

void AdvancedTableWidget::configureForDocs(QObject *obj, const char *slot)
{
    clear();

    size_t emptyheaders[] = {};

    configure(0, "Open", obj, slot);

    setRowCount(0);
    setVerticalHeaderLabels(zakupki::DOCS_COLUMNS_HEADERS);
    horizontalHeader()->setStretchLastSection(true);

//    for (size_t i = 0; i < zakupki::AG_MAX_FIELDS; ++i) {
//        setCellWidget(zakupki::AG_FIELDS_INDEXES[i], 1, button_widgets[i]);
//        button_widgets[i]->setVisible(true);
//        setItem(zakupki::AG_FIELDS_INDEXES[i], 0, tableitems_widgets[i]);
//    }
}

void AdvancedTableWidget::clear()
{
    for (auto &it : button_widgets)
        if (it) delete it;

    for (auto &it : tableitems_widgets)
        if (it) delete it;

    setRowCount(0);

    button_widgets.clear();
    tableitems_widgets.clear();
}

void AdvancedTableWidget::safeSetItem(int x, int y, QString newtext)
{
    QTableWidgetItem *pitem(item(x, y));
    if (pitem) {
        pitem->setText(newtext);
    } else {
        tableitems_widgets.append(new QTableWidgetItem(newtext));
        setItem(x, y, tableitems_widgets.back());
    }
}

void AdvancedTableWidget::appendItem(QString newtext)
{
    insertRow(rowCount());
    tableitems_widgets.append(new QTableWidgetItem(newtext));

    QWidget* pWidget = new QWidget(this);
    MultipleButton* btn_edit = new MultipleButton(button_widgets.size(), pWidget);
    btn_edit->setText(btntext);
    QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
    pLayout->addWidget(btn_edit);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pWidget->setLayout(pLayout);
    pWidget->setVisible(false);

    button_widgets.append(pWidget);
    setItem(rowCount() - 1, 0, tableitems_widgets.back());
    setCellWidget(button_widgets.size() - 1, 1, button_widgets.back());
    connect(btn_edit, SIGNAL(clicked(int)), custom_object, custom_slot);
}


