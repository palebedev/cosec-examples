#ifndef UUID_BCE2667F_57E8_4EE0_9CE1_0BCEA03E33D0
#define UUID_BCE2667F_57E8_4EE0_9CE1_0BCEA03E33D0

#include <QtWidgets/QActionGroup>
#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
protected:
    void changeEvent(QEvent* event) override;
private slots:
    void on_openAction_triggered();
    void on_runAction_triggered();
    void on_aboutAction_triggered();
    void langActionSelected(QAction* action);
private:
    QScopedPointer<Ui::MainWindow> ui_;
    QActionGroup* languagesGroup_;

    void loadSystemLocale();
};

#endif
