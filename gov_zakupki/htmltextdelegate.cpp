#include "htmltextdelegate.h"

void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QString str = index.data(Qt::DisplayRole).toString();
    QTextDocument td;
    td.setHtml(str);

    QStyleOptionViewItem opt = option;
    QRectF lr = layoutRect(td, opt.rect);

    painter->save();
    painter->translate(lr.topLeft());
    painter->setClipRect(lr.translated(-lr.x(), -lr.y()));
    td.drawContents(painter, QRectF());
    painter->restore();
}


QWidget *HtmlDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{
    QTextEdit *textEdit = new QTextEdit(parent);
    return textEdit;
}

void HtmlDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    QString str = index.data(Qt::DisplayRole).toString();
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(editor);
    textEdit->setHtml(str);
}

void HtmlDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(editor);
    QString str = textEdit->toHtml();
    model->setData(index, str, Qt::DisplayRole);
}

QRectF HtmlDelegate::layoutRect(const QTextDocument & td, QRect rect) const
{
    qreal xo;
    qreal yo;

    if (align & Qt::AlignLeft) {
        xo = 0;
    } else if(align & Qt::AlignRight) {
        qreal rw = td.documentLayout()->documentSize().width();
        xo = 0;
        xo = qMax((rect.width()-rw), qreal(0));
    } else if(align & Qt::AlignHCenter) {
        qreal rw = td.documentLayout()->documentSize().width();
        xo = 0;
        xo = qMax((rect.width()-rw)/2, qreal(0));
    }

    if (align & Qt::AlignTop) {
        yo = 0;
    } else if (align & Qt::AlignBottom) {
        qreal rh = td.documentLayout()->documentSize().height();
        yo = 0;
        yo = qMax((rect.height()-rh), qreal(0));
    } else if (align & Qt::AlignVCenter) {
        qreal rh = td.documentLayout()->documentSize().height();
        yo = 0;
        yo = qMax((rect.height()-rh)/2, qreal(0));
    }
    return QRectF(xo + rect.x(), yo + rect.y(), rect.width(), rect.height());
}

