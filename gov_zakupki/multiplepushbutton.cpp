#include "multiplepushbutton.h"

MultipleButton::MultipleButton(int digit, QWidget *parent)
    : QPushButton(parent)
{
    myDigit = digit;
    connect(this, SIGNAL(clicked()), this, SLOT(reemitClicked()));
}

void MultipleButton::reemitClicked()
{
    emit clicked(myDigit);
}
