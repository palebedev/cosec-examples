#ifndef UUID_3B79EAE7_BE3E_4315_B82A_EF17B8F1B331
#define UUID_3B79EAE7_BE3E_4315_B82A_EF17B8F1B331

#include <QtCore/QScopedPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtPositioning/QGeoPositionInfoSource>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget();
private:
    QScopedPointer<Ui::MainWidget> ui_;
    QGeoCoordinate coord_;
    QNetworkAccessManager nam_;
    QDate date_;
    int tries_;

    void sendDowRequest();
private slots:
    void error(QGeoPositionInfoSource::Error error);
    void updateTimeout();
    void positionUpdated(const QGeoPositionInfo& gpi);
    void on_doButton_clicked();
    void replyFinished(QNetworkReply* reply);
};

#endif
