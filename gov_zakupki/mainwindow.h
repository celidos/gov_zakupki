#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringRef>
#include <QRegExp>
#include <QVector>
#include <QHBoxLayout>
#include <QClipboard>
#include <QLabel>
#include <QTimer>
#include <QMimeDatabase>
#include <QDebug>
#include <QProgressBar>

//#include <iostream>

#include "filedownloader.h"
#include "multiplepushbutton.h"
#include "filedownloadspool.h"
#include "htmltextdelegate.h"
#include "zakupki.h"
#include "reqdocumentmanager.h"
#include "advancedtablewidget.h"
#include "internetconnectionchecker.h"
#include "requestpool.h"
#include "requestgroup.h"

#include "xlsxdocument.h"

using std::size_t;
//using namespace NumberDuck;

namespace Ui {
class MainWindow;
}

/// http://doc.qt.io/qt-5/json.html
/// https://doc.qt.io/archives/qq/qq10-signalmapper.html

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    ReqDocumentManager *docmanager;

    RequestPool *rpool;

    AdvancedTableWidget *infotable;
    AdvancedTableWidget *doctable;
    InternetConnectionChecker *connectionChecker;

    QLabel *pwInternetStatusLabel;
    QProgressBar *pwDownloadFilesProgressBar;

    int currentPageIndex;

    void downloadData();

    void configureUi();

    Group lastgroup;

private slots:
    void process();
    void searchButtonPressed();

    void filterSearchButtonPressed();

    void copyButtonClicked(int index);

    // f fast

    void refreshDataBase();
    void loadDatabaseButtonPressed();
    void budgetFilterSearchButtonPressed();

    void loadOrganizationsNamesButtonPressed();

    // LIST SEARCH -------------------------------------------------------------
public slots:

    void clearTextEditButtonPressed();

    void acceptSingleRequest  (RequestGroup *pgroup);
    void acceptMultipleRequest(RequestGroup *pgroup);
    void acceptFilterRequest  (RequestGroup *pgroup);

    void exportToExcelMultipleRequest();

    void

    // f fast


};

QString numToLetter(int x);

#endif // MAINWINDOW_H
