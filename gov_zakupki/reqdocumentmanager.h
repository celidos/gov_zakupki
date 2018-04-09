#ifndef REQDOCUMENTMANAGER_H
#define REQDOCUMENTMANAGER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QTableWidget>
#include <QRegExp>
#include <QHBoxLayout>
#include <QDir>
#include <QDesktopServices>
#include <QProgressBar>

#include "filedownloadspool.h"
#include "htmltextdelegate.h"
#include "multiplepushbutton.h"
#include "advancedtablewidget.h"

enum DocumentClass {DC_CONTRACT_INFO, DC_CONTRACT_CHANGE,
                    DC_CONTRACT_EXE,  DC_CONTRACT_ANNUL, DC_UNCLASSIFIED};

struct FileRecord {

    QString title;
    QString filename;
    QString ext;
    int tableIndex;
    QString link;
    DocumentClass docClass;
    FileRecord() {loaded = false; toload = false;}
    FileRecord(QString _filename, QString _link, int _index, DocumentClass documentClass=DC_UNCLASSIFIED) {
        ext = _filename.mid(_filename.lastIndexOf('.')).simplified();
//        qDebug() << fileext.simplified();
        filename = _filename.left(_filename.lastIndexOf('.'));
        tableIndex = _index;
        link = _link;
        docClass = documentClass;
        loaded = false;
        toload = false;
    }
    QString getFullPath(QString current_rec);
    bool loaded;
    bool toload;
};

class ReqDocumentManager : public QObject {
    Q_OBJECT
public:
    explicit ReqDocumentManager(QObject *parent = 0);
    ReqDocumentManager(QObject *parent,
                       AdvancedTableWidget *ptablewidget,
                       QProgressBar *pprogressbar=nullptr);
    virtual ~ReqDocumentManager();

    void init();

    void addFileRecord(FileRecord filerec);
    void addDirt(QString acut, DocumentClass dc);
    void downloadAll();
    void clear();
    void processLastRecord();

    QString current_rec;
    size_t getSize();

    void openFileWithSystemProgram(FileRecord &fr);

private:
    FileDownloadsPool *filePool;
    AdvancedTableWidget *ptable;
    QProgressBar *ppbar;

    QVector<FileRecord> files;
    void clearRecords();
    void clearCache();
public slots:
    void saveDocumentToDisk(FileDownloader *ploader);
    void openFile(int index);
};

#endif // REQDOCUMENTMANAGER_H
