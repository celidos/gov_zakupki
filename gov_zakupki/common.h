#ifndef COMMON_H
#define COMMON_H

#include <QMessageBox>
#include <QApplication>
#include <QString>
#include <QTextDocument>

void failwith(QString err, bool hard = false);
QString removeHtml(QString htmlString);

#endif // COMMON_H
