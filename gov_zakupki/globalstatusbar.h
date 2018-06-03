#ifndef GLOBALSTATUSBAR_H
#define GLOBALSTATUSBAR_H

#include <QLabel>
#include <QString>

// SINGLETON WORKSTATUS --------------------------------------------------------

class workstatus : public QObject
{
    Q_OBJECT

public:
    static void setOutput(QLabel *label) {
        statuslabel = label;
    }

    static void upd(QString text="<font color=""green"">Готово</font>",
                    bool green=false, bool red=false) {
        if (statuslabel) {
            if (!green && !red)
                statuslabel->setText(text);
            else {
                if (green)
                    statuslabel->setText("<font color=""green"">" + text + "</font>");
                else if (red)
                    statuslabel->setText("<font color=""red"">" + text + "</font>");
            }
        }
    }

private:
    static QLabel *statuslabel;
    Q_DISABLE_COPY(workstatus)

};

#endif // GLOBALSTATUSBAR_H
