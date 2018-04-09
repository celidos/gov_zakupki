#include "reqdocumentmanager.h"


ReqDocumentManager::ReqDocumentManager(QObject *parent) :
    QObject(parent), ptable(nullptr), ppbar(nullptr)
{
    filePool = FileDownloadsPool::instance();
}

ReqDocumentManager::ReqDocumentManager(QObject *parent,
    AdvancedTableWidget *ptablewidget, QProgressBar *pprogressbar) :
    QObject(parent), ptable(ptablewidget), ppbar(pprogressbar)
{
    filePool = FileDownloadsPool::instance();
    init();
}

ReqDocumentManager::~ReqDocumentManager()
{

}

void ReqDocumentManager::init()
{
    ptable->configureForDocs(this, SLOT(openFile(int)));
}

void ReqDocumentManager::addFileRecord(FileRecord filerec)
{
    files.append(filerec);
    processLastRecord();
}

void ReqDocumentManager::addDirt(QString acut, DocumentClass dc)
{
    FileRecord rec;

    int pos = acut.indexOf("href=");
    pos = acut.indexOf('"', pos);
    rec.link = acut.mid(pos + 1, acut.indexOf('"', pos+2) - pos - 1);

    QString fileext;
    pos = acut.indexOf("title=");
    pos = acut.indexOf('"', pos);
    fileext = acut.mid(pos + 1, acut.indexOf('"', pos+2) - pos - 1);
    fileext = fileext.left(fileext.lastIndexOf('('));
    fileext = fileext.mid(fileext.lastIndexOf('.') + 1);
    rec.ext = fileext.simplified();
    qDebug() << fileext.simplified();

    pos = acut.indexOf(">");
    rec.filename = acut.mid(pos + 1);
    rec.filename.remove(QRegExp(QString::fromUtf8("[\\s-`~!@#$%^&*()_—+=|:;<>«»?/{}\'\"\\\[\\\]\\\\]")));
    qDebug() << "EJECTED URL:" << rec.link << rec.title;

    addFileRecord(rec);
}

void ReqDocumentManager::downloadAll()
{
    if (filePool == nullptr) {
        qDebug() << "No file pool connected.";
        return;
    }

    qDebug() << "МЫ ЗДЕСЬ!!!!!!!!!!!!!!!!!!!!!!";

    if (!files.empty()) {
        for (auto &it : files) {
            qDebug() << it.link;

            filePool->addDownload(it.link, this, SLOT(saveDocumentToDisk(FileDownloader*)),
                                  (void *)&it);
        }
    }
    else {
        qDebug() << "no fields!";
    }
}

void ReqDocumentManager::clear()
{
    clearRecords();
}

void ReqDocumentManager::processLastRecord()
{
    qDebug() << "ADDING " << files.back().filename;
    ptable->appendItem(files.back().filename + files.back().ext);

    if (ppbar) {
        ppbar->setMaximum(files.size());

    }
}

size_t ReqDocumentManager::getSize()
{
    return files.size();
}

void ReqDocumentManager::openFileWithSystemProgram(FileRecord &fr)
{
    QString file = fr.getFullPath(current_rec);
    qDebug() << "WE ARE NOW TRYING TO OPEN file" << file;
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}

void ReqDocumentManager::clearRecords()
{
    ptable->clear();

    files.clear();
}

void checkPath(QString path) {
    qDebug() << path;
    if (!QDir("./" + path).exists()) {
        QDir().mkdir(path);
    }
}

void ReqDocumentManager::saveDocumentToDisk(FileDownloader *ploader)
{
    qDebug() << "_____________________ FILE SAVING ON THE DISK ________________________________";
    FileRecord *rec = (FileRecord *) ploader->info;

    rec->loaded = true;
    QByteArray data = filePool->extractDataAndFree(ploader);


    QString path("cache/" + current_rec);
    checkPath("cache/");
    checkPath(path);

    QFile file("./" + path +"/" + rec->filename + "." + rec->ext);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    if (rec->toload) {
        openFileWithSystemProgram(*rec);
    }
}

void ReqDocumentManager::openFile(int index)
{
    if (index < files.size()) {
        FileRecord &rec = files[index];
        rec.toload = true;
        if (!rec.loaded) {
            filePool->addDownload(rec.link, this, SLOT(saveDocumentToDisk(FileDownloader*)),
                                  (void *)&files[index]);
        } else {
            openFileWithSystemProgram(rec);
        }
    }
}

QString FileRecord::getFullPath(QString current_rec)
{
    QString path("cache/" + current_rec);
    QString file("./" + path + "/" + filename + "." + ext);
    return file;
}
