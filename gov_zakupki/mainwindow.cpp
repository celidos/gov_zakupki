﻿#include "mainwindow.h"
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

    // -------------------------------------------------------------------------

    connect(ui->pushButton_4, SIGNAL(clicked(bool)),
            this, SLOT(searchButtonPressed()));

    connect(ui->pushButton_6, SIGNAL(clicked()),
            this, SLOT(exportToExcelMultipleRequest()));

    connect(ui->pushButton_5, SIGNAL(clicked(bool)),
            this, SLOT(clearTextEditButtonPressed()));

    connect(ui->pushButton_3, SIGNAL(clicked()),
            this, SLOT(filterSearchButtonPressed()));

    connect(ui->pushButton_8, SIGNAL(clicked()),
            this, SLOT(loadDatabaseButtonPressed()));

    connect(ui->pushButton_9, SIGNAL(clicked()),
            this, SLOT(budgetFilterSearchButtonPressed()));

    rpool->setProgressBar(ui->progressBar);
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

void MainWindow::loadOrganizationsNamesButtonPressed()
{
    rpool->loadOrganizationsFromZakupki();
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
    qWarning() << "Gay zone!";

    FilterRequestParams rp;
    rp.customer_inn = ui->lineEdit_4->text();

    rpool->addZakupkiFilterReq(rp, this, SLOT(acceptMultipleRequest(RequestGroup*)));

}

void MainWindow::clearTextEditButtonPressed()
{
    ui->plainTextEdit->clear();
}

void MainWindow::acceptSingleRequest(RequestGroup *pgroup)
{
    Group data = rpool->extractDataAndFree(pgroup);
    zakupki::contract_record record = data[0];

    if (record.rtype == zakupki::RT_ZAKUPKI) {
        infotable->configureForZakupki(this, SLOT(copyButtonClicked(int)));
    } else if (record.rtype == zakupki::RT_BUDGET) {
        infotable->configureForBudget(this, SLOT(copyButtonClicked(int)));
    }

    for (size_t i = 0; i < record.values.length(); ++i) {
        infotable->safeSetItem(record.indices[i], 0, record.values[i]);
    }

    //    qWarning() << "OLOLO, we are here!!!";
}

void MainWindow::acceptMultipleRequest(RequestGroup *pgroup)
{
    lastgroup = rpool->extractDataAndFree(pgroup);
//    zakupki::contract_record record = data[0];


    for (auto &it : lastgroup){
        ui->plainTextEdit_2->appendPlainText(QString("# Index : ") + QString::number(it.indices[0])
                + QString(", value =") + it.values[0]);
    }



    //    qWarning() << "OLOLO, we are here!!!";
}

void MainWindow::acceptFilterRequest(RequestGroup *pgroup)
{
    qWarning() << "we are doing nothing here!";
}

void MainWindow::exportToExcelMultipleRequest()
{
//    NumberDuck::Workbook workbook("");
//    Worksheet* pWorksheet = workbook.GetWorksheetByIndex(0);

//    Cell* pCell = pWorksheet->GetCellByAddress("A1");
//    pCell->SetString("Totally cool spreadsheet!");

//    pWorksheet->GetCell(1,1)->SetFloat(3.1417f);

//    workbook.Save("SimpleExample.xls");
    QXlsx::Document xlsx;
//    xlsx.write("A1", "Hello Qt!");
//    xlsx.saveAs("Test.xlsx");

    for (size_t i = 0; i < lastgroup.size(); ++i) {
        zakupki::contract_record &curr_record = lastgroup[i];

        for (size_t j = 0; j < curr_record.values.size(); ++j) {
            int column_index = curr_record.indices[j];
            xlsx.write(numToLetter(column_index) + QString::number(i + 1), curr_record.values[j]);
        }
    }

    xlsx.saveAs(ui->lineEdit_10->text() + ".xlsx");
}

void MainWindow::process()
{
    docmanager->current_rec = ui->lineEdit->text();
    docmanager->current_rec.remove(QRegExp(
        QString::fromUtf8("[\\s-`~!@#$%^&*()_—+=|:;<>«»?/{}\'\"\\\[\\\]\\\\]")));

    rpool->addSingleReq(docmanager->current_rec, this,
                        SLOT(acceptSingleRequest(RequestGroup*)),
                        ui->checkBox->isChecked());
}

QString numToLetter(int x)
{
    if (x < 26)
        return QString('A' + x);

    int a = x / 26  - 1;
    int b = x % 26;
    return QString('A' + char(a)) + QString('A' + char(b));
}