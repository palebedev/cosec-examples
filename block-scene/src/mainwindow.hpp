#ifndef UUID_AECAA7D5_DE70_4E1D_8D17_57DF9461CA80
#define UUID_AECAA7D5_DE70_4E1D_8D17_57DF9461CA80

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class BlockPropertiesModel;
class BlockScene;
class BlockTypesModel;

class MainWindow : public QWidget
{
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private:
    QScopedPointer<Ui::MainWindow> ui_;
    BlockTypesModel* btm_;
    BlockScene* scene_;
    BlockPropertiesModel* bpm_;
};

#endif
