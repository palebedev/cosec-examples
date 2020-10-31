#ifndef UUID_E60A5C31_8630_4126_807A_10DF3410C0FD
#define UUID_E60A5C31_8630_4126_807A_10DF3410C0FD

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class CopyWidget;
}
QT_END_NAMESPACE

class QThread;
class FileCopier;

class CopyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CopyWidget(QWidget* parent = nullptr);
    ~CopyWidget() override;
protected:
    void closeEvent(QCloseEvent* event) override;
private slots:
    void on_browseSourceButton_clicked();
    void on_browseDestinationButton_clicked();
    void on_runCancelButton_clicked();
    void done(const QString& result);
private:
    QScopedPointer<Ui::CopyWidget> ui_;
    // A thread to copy files in without blocking
    // event handling loop in main gui thread.
    QThread* thread_;
    FileCopier* fileCopier_;
    bool running_ = false;
};

#endif
