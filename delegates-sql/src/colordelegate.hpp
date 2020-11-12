#ifndef UUID_B62D325D_2AC6_48AC_AF11_CA6530236DFB
#define UUID_B62D325D_2AC6_48AC_AF11_CA6530236DFB

#include <QtWidgets/QStyledItemDelegate>
#include <qstyleditemdelegate.h>

class ColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QWidget* createEditor(QWidget* parent,const QStyleOptionViewItem& option,
                          const QModelIndex &index) const override;
    void paint(QPainter* painter,const QStyleOptionViewItem &option,
               const QModelIndex& index) const override;
};

#endif
