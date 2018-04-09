#ifndef MULTIPLEPUSHBUTTON_H
#define MULTIPLEPUSHBUTTON_H

#include <QPushButton>

// buttons which send INDEXED clicked() signal

class MultipleButton : public QPushButton
{
    Q_OBJECT
public:
    MultipleButton(int digit, QWidget *parent);

signals:
    void clicked(int digit);

private slots:
    void reemitClicked();

private:
    int myDigit;
};

#endif // MULTIPLEPUSHBUTTON_H
