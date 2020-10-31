#ifndef UUID_9698CE08_223B_4AA6_A7BB_8166BCCB38ED
#define UUID_9698CE08_223B_4AA6_A7BB_8166BCCB38ED

#include <QtWidgets/QWidget>

class DrawWidget : public QWidget
{
    Q_OBJECT
public:
    using QWidget::QWidget;
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* /*event*/) override;
private:
    QList<QPointF> points_;
};

#endif
