#include "colordelegate.hpp"

#include <QtGui/QPainter>
#include <QtWidgets/QColorDialog>

QWidget* ColorDelegate::createEditor(QWidget* parent,const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    QVariant data = index.data();
    if(!data.canConvert<QColor>())
        return QStyledItemDelegate::createEditor(parent,option,index);
    QColor c = QColorDialog::getColor(data.value<QColor>(),parent,tr("Color"));
    if(c.isValid())
        const_cast<QAbstractItemModel*>(index.model())->setData(index,c,Qt::EditRole);
    return nullptr;
}

void ColorDelegate::paint(QPainter* painter,const QStyleOptionViewItem& option,
                          const QModelIndex& index) const
{
    QVariant data = index.data();
    if(!data.canConvert<QColor>())
        return QStyledItemDelegate::paint(painter,option,index);
    QColor c = data.value<QColor>();
    painter->fillRect(option.rect,c);
    painter->setPen(c.lightnessF()>0.5?Qt::black:Qt::white);
    painter->drawText(option.rect,Qt::AlignHCenter|Qt::AlignVCenter,
                      QStringLiteral("#%1").arg(c.rgb(),6,16,QLatin1Char('0')));
}
