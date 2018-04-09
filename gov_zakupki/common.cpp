#include "common.h"

void failwith(QString err, bool hard)
{
    QMessageBox messageBox;
    messageBox.critical(0, "Error", err);
    if (hard)
        QApplication::quit();
}

QString removeHtml(QString htmlString)
{
    QTextDocument text;
    text.setHtml(htmlString);
    return text.toPlainText();
}
