#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>

/// http://amin-ahmadi.com/2016/04/05/check-internet-connection-availability-status-qt/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget* window = new QWidget(this);
    infotable = new AdvancedTableWidget(window);
    QWidget* window2 = new QWidget(this);
    doctable = new AdvancedTableWidget(window2);

    docmanager = new ReqDocumentManager(this, doctable);

    rpool = new RequestPool(docmanager, this);

    ui->horizontalLayout_4->addWidget(infotable);
    window->show();
    ui->horizontalLayout_3->addWidget(doctable);
    window2->show();

    configureUi();

    connectionChecker = new InternetConnectionChecker(pwInternetStatusLabel, this);

    qDebug() << "Is ok?";
}

MainWindow::~MainWindow()
{
    // memory leakage

    delete ui;
}

void MainWindow::configureUi()
{
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(process()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(process()));

    // configuaring table
    infotable->configureForZakupki(this, SLOT(copyButtonClicked(int)));

    // internet status label
    pwInternetStatusLabel = new QLabel(this);
    pwInternetStatusLabel->setFixedWidth(270);
    pwInternetStatusLabel->setText("Проверка интернет-соединения...");
    ui->statusBar->addWidget(pwInternetStatusLabel);

    // documents download progress bar
    pwDownloadFilesProgressBar = new QProgressBar(this);
    pwDownloadFilesProgressBar->setFixedWidth(160);
    pwDownloadFilesProgressBar->setMaximum(0);
    pwDownloadFilesProgressBar->setMinimum(1);
    pwDownloadFilesProgressBar->setValue(0);
    pwDownloadFilesProgressBar->hide();

    //pwDownloadFilesProgressBar->setText("Проверка интернет-соединения...");
    ui->statusBar->addWidget(pwDownloadFilesProgressBar);

//    ui->scrollArea->setWidget(ui->widget);

    // -------------------------------------------------------------------------

    connect(ui->pushButton_4, SIGNAL(clicked(bool)),
            this, SLOT(searchButtonPressed()));

    connect(ui->pushButton_6, SIGNAL(clicked()),
            this, SLOT(exportToExcelMultipleRequestButtonPressed()));

    connect(ui->pushButton_5, SIGNAL(clicked(bool)),
            this, SLOT(clearTextEditButtonPressed()));

    connect(ui->pushButton_3, SIGNAL(clicked()),
            this, SLOT(filterSearchButtonPressed()));

    connect(ui->pushButton_8, SIGNAL(clicked()),
            this, SLOT(loadDatabaseButtonPressed()));

    connect(ui->pushButton_9, SIGNAL(clicked()),
            this, SLOT(budgetFilterSearchButtonPressed()));

    connect(ui->pushButton_11, SIGNAL(clicked()),
            this, SLOT(exportToExcelBudgetFilterButtonPressed()));

    connect(ui->pushButton_7, SIGNAL(clicked()),
            this, SLOT(exportToExcelZakupkiFilterButtonPressed()));

    connect(ui->pushButton_2, SIGNAL(clicked()),
            this, SLOT(clearFilterZakupkiButtonPressed()));

    rpool->setProgressBar(ui->progressBar);

    // -------------------------------------------------------------------------

    configureTransferInfoTable();
}

void MainWindow::configureTransferInfoTable()
{
    ui->tableWidget->clear();

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableWidget->setColumnCount(3);
//    setColumnWidth(0, int(this->width() * 0.6));

    ui->tableWidget->setRowCount(1);

    ui->tableWidget->setHorizontalHeaderLabels(zakupki::TRANSFER_INFO_COLUMNS_HEADERS);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

//    for (size_t i = 0; i < zakupki::AG_MAX_FIELDS; ++i) {
//        setCellWidget(i, 1, button_widgets[i]);
//        button_widgets[i]->setVisible(true);
//        setItem(i, 0, tableitems_widgets[i]);
    //    }
}

void MainWindow::postFilterZakupki(FilterRequestParams *rp)
{

}

// processing data -------------------------------------------------------------

void MainWindow::copyButtonClicked(int index)
{
    QString text = removeHtml(infotable->item(index, 0)->text());
    QApplication::clipboard()->setText(text);
}

// fast f

void MainWindow::refreshDataBase()
{
    rpool->updateBudgetDatabase();
}

void MainWindow::loadDatabaseButtonPressed()
{
    rpool->loadDatabaseToMemory();
}

void MainWindow::budgetFilterSearchButtonPressed()
{
    FilterRequestParams rp;
    rp.recordtype = zakupki::RT_BUDGET;
    rp.gbrs_inn = ui->lineEdit_5->text().trimmed();
    rp.gbrs_kpp = ui->lineEdit_6->text().trimmed();

    if (ui->checkBox_4->isChecked()) {
        rp.dateStartUsed = true;
        rp.dateStart = ui->dateEdit_4->date();
    } else {
        rp.dateStartUsed = false;
    }

    if (ui->checkBox_5->isChecked()) {
        rp.dateFinishUsed = true;
        rp.dateFinish = ui->dateEdit_3->date();
    } else {
        rp.dateFinishUsed = false;
    }

    rp.minSum = (long double) (ui->lineEdit_7->text().toDouble());
    rp.maxSum = (long double) (ui->lineEdit_8->text().toDouble());

    QVector<int> ans = rpool->addBudgetFilter(rp);
    for (auto &it: ans) {
        qWarning() << "Good index! " << it;
    }
}

QStringList getNumberFromQString(const QString &xString)
{
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");
    int pos = 0;
    QStringList res;
    while ((pos = xRegExp.indexIn(xString, pos)) != -1) {
        //
        res.append(xRegExp.capturedTexts()[0]);
        pos += xRegExp.matchedLength();
    }
    return res;
}

void MainWindow::searchButtonPressed()
{
    QStringList req_set = getNumberFromQString(ui->plainTextEdit->toPlainText());
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(req_set.length());

    rpool->addGroup(req_set, this, SLOT(acceptMultipleRequest(RequestGroup*)));
}

void MainWindow::filterSearchButtonPressed()
{
    if (rp)
        delete rp;

    rp = new FilterRequestParams();
    rp->recordtype =zakupki::RT_ZAKUPKI;
    rp->customer_inn = ui->lineEdit_4->text().trimmed();
    rp->gbrs_inn = ui->lineEdit_9->text().trimmed();
    rp->gbrs_kpp = ui->lineEdit_12->text().trimmed();

    rp->dateStartUsed  = ui->checkBox_7->isChecked();
    if (rp->dateStartUsed) {
        rp->dateStart = ui->dateEdit_5->date();
    }

    rp->dateFinishUsed = ui->checkBox_6->isChecked();
    if (rp->dateFinishUsed) {
        rp->dateFinish = ui->dateEdit_6->date();
    }

    rp->minSum = ui->lineEdit_14->text().toLongLong();
    rp->maxSum = ui->lineEdit_13->text().toLongLong();

    qWarning() << "POINTER: " << rp;

    rpool->addZakupkiFilterReq(rp, this, SLOT(acceptFilterZakupkiRequest(RequestGroup*)));
}

void MainWindow::clearTextEditButtonPressed()
{
    ui->plainTextEdit->clear();
}

void MainWindow::clearFilterZakupkiButtonPressed()
{
    ui->lineEdit_4->clear();
    ui->lineEdit_9->clear();
    ui->lineEdit_12->clear();
    ui->checkBox_7->setChecked(false);
    ui->checkBox_6->setChecked(false);
    ui->lineEdit_14->setText("0");
    ui->lineEdit_13->setText("200000000000");
    ui->dateEdit_6->setDate(QDate::fromString("01.01.2008", "dd.MM.yyyy"));
    ui->dateEdit_5->setDate(QDateTime::currentDateTime().date());
}

void MainWindow::acceptSingleRequest(RequestGroup *pgroup)
{
    qWarning() << "ACCEPTING SINGLE REQUEST!";
    Group data = rpool->extractDataAndFree(pgroup);
    zakupki::contract_record record = data[0];

    if (record.rtype == zakupki::RT_ZAKUPKI) {
        infotable->configureForZakupki(this, SLOT(copyButtonClicked(int)));
    } else if (record.rtype == zakupki::RT_BUDGET) {
        infotable->configureForBudget(this, SLOT(copyButtonClicked(int)));

        if (!record.ag_transfer_num.isEmpty()) {

            ui->tableWidget->clear();

            QTableWidget *tw = ui->tableWidget;

            for (int i = 0; i < record.ag_transfer_num.size(); ++i) {
                QString newtext;

                ui->tableWidget->insertRow(i);

                newtext = QString::number(record.ag_transfer_num[i]);
                QTableWidgetItem *pitem1(tw->item(0, i));
                if (pitem1) {
                    pitem1->setText(newtext);
                } else {
                    tw->setItem(0, i, new QTableWidgetItem(newtext));
                }

                newtext = record.ag_transfer_date[i];
                QTableWidgetItem *pitem2(tw->item(1, i));
                if (pitem2) {
                    pitem2->setText(newtext);
                } else {
                    tw->setItem(1, i, new QTableWidgetItem(newtext));
                }

                newtext = QString::number(record.ag_transfer_sum[i]);
                QTableWidgetItem *pitem3(tw->item(2, i));
                if (pitem3) {
                    pitem3->setText(newtext);
                } else {
                    tw->setItem(2, i, new QTableWidgetItem(newtext));
                }
            }
        }
    }

    for (size_t i = 0; i < record.values.length(); ++i) {
        infotable->safeSetItem(record.indices[i], 0, record.values[i]);
    }

    //    qWarning() << "OLOLO, we are here!!!";
}

void MainWindow::acceptMultipleRequest(RequestGroup *pgroup)
{
    lastmultiplegroup = rpool->extractDataAndFree(pgroup);
//    zakupki::contract_record record = data[0];

    for (auto &it : lastmultiplegroup){
        ui->plainTextEdit_2->appendPlainText(QString("# Index : ") + QString::number(it.indices[0])
                + QString(", value =") + it.values[0]);
    }
}


void MainWindow::acceptFilterZakupkiRequest(RequestGroup *pgroup)
{
    qWarning() << "we are doing nothing here!";

    lastzakupkigroup = rpool->extractDataAndFree(pgroup);

    qWarning() << "Before Filtering last zakupki group size = "<< lastzakupkigroup.size();

    Group newlist;
//    lastzakupkigroup.clear();
    for (int i = 0; i < lastzakupkigroup.size(); ++i) {
        zakupki::contract_record &el = lastzakupkigroup[i];
        if (match(*rp, el)) {
            newlist.append(el);
        }
    }
    lastzakupkigroup = newlist;

    qWarning() << "zakupki group size " << lastzakupkigroup.size();
}

void MainWindow::exportToExcelMultipleRequestButtonPressed()
{
    exportGroupToExcel(lastmultiplegroup, ui->lineEdit_10->text() + ".xlsx");
}

void MainWindow::exportToExcelBudgetFilterButtonPressed()
{
    qWarning() << "button?";
    rpool->exportToExcelBudgetFilter(ui->lineEdit_11->text());
}

void MainWindow::exportToExcelZakupkiFilterButtonPressed()
{
    exportGroupToExcel(lastzakupkigroup, ui->lineEdit_15->text() + ".xlsx");
}

void MainWindow::process()
{
    docmanager->current_rec = ui->lineEdit->text();
    docmanager->current_rec.remove(QRegExp(
        QString::fromUtf8("[\\s-`~!@#$%^&*()_—+=|:;<>«»?/{}\'\"\\\[\\\]\\\\]")));

    ui->tableWidget->clear();

    rpool->addSingleReq(docmanager->current_rec, this,
                        SLOT(acceptSingleRequest(RequestGroup*)),
                        ui->checkBox->isChecked(), ui->checkBox_2->isChecked());
}

