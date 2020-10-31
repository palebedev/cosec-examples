#ifndef UUID_2BF6D915_A7CF_4867_8AFC_3076122D261B
#define UUID_2BF6D915_A7CF_4867_8AFC_3076122D261B

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class TestWidget;
}
QT_END_NAMESPACE

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget* parent = nullptr);
    ~TestWidget() override;
private slots:
    void on_widgetButton_clicked();
    void on_viewButton_clicked();
private:
    QScopedPointer<Ui::TestWidget> ui_;
};

#endif
