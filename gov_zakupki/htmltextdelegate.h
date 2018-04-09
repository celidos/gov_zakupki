#ifndef HTMLDELEGATE_H
#define HTMLDELEGATE_H

#include <QItemDelegate>
#include <QTextDocument>
#include <QTextEdit>
#include <QtGui>

// Taken from:
//   http://www.forum.crossplatform.ru/index.php?showtopic=2565
// Don't even fuck how it works

class HtmlDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    HtmlDelegate(QWidget *parent = 0, Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter) : QItemDelegate(parent), align(alignment) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

private:
	  Qt::Alignment align;
      QRectF layoutRect(const QTextDocument & td, QRect rect) const;
};

#endif

