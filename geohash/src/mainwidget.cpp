#include "mainwidget.hpp"

#include "ui_mainwidget.h"

#include <QtCore/QByteRef>
#include <QtCore/QCryptographicHash>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkReply>

MainWidget::MainWidget(QWidget* parent)
    : QWidget{parent},
      ui_{new Ui::MainWidget}
{
    ui_->setupUi(this);
    if(auto source = QGeoPositionInfoSource::createDefaultSource(this)){
        source->startUpdates();
        connect(source,qOverload<QGeoPositionInfoSource::Error>(&QGeoPositionInfoSource::error),
                this,&MainWidget::error);
        connect(source,&QGeoPositionInfoSource::updateTimeout,this,&MainWidget::updateTimeout);
        updateTimeout();
        connect(source,&QGeoPositionInfoSource::positionUpdated,this,&MainWidget::positionUpdated);
        //
        //QTimer::singleShot(3000,[=,this]{
        //    QGeoPositionInfo gpi;
        //    gpi.setCoordinate({53.8851943,37.6571295,179.2});
        //    gpi.setTimestamp(QDateTime::currentDateTimeUtc());
        //    positionUpdated(gpi);
        //});
        //
        connect(&nam_,&QNetworkAccessManager::finished,this,&MainWidget::replyFinished);
    }
}

MainWidget::~MainWidget() = default;

void MainWidget::error(QGeoPositionInfoSource::Error error)
{
    QString s;
    switch(error){
        case QGeoPositionInfoSource::Error::AccessError:
            s = tr("Insufficient privileges to use geolocation.");
            break;
        case QGeoPositionInfoSource::Error::ClosedError:
            s = tr("Access to geolocation has been disabled");
            break;
        default:
            s = tr("Unknown geolocation error has occured.");
    }
    ui_->log->appendPlainText(s);
    updateTimeout();
}

void MainWidget::updateTimeout()
{
    ui_->locationLabel->setText(tr("Location: (unknown)"));
    ui_->doButton->setEnabled(false);
}

void MainWidget::positionUpdated(const QGeoPositionInfo& gpi)
{
    if(!gpi.isValid()){
        updateTimeout();
        return;
    }
    coord_ = gpi.coordinate();
    ui_->locationLabel->setText(tr("Location: (%1,%2)").arg(coord_.latitude()).arg(coord_.longitude()));
    ui_->doButton->setEnabled(true);
}

void MainWidget::on_doButton_clicked()
{
    if(!coord_.isValid())
        return;
    date_ = QDate::currentDate();
    if(coord_.longitude()>=-30)
        date_ = date_.addDays(-1);
    tries_ = 0;
    sendDowRequest();
}

void MainWidget::sendDowRequest()
{
    auto url = QStringLiteral("http://geo.crox.net/djia/%1/%2/%3")
        .arg(date_.year()).arg(date_.month()).arg(date_.day());
    ui_->log->appendPlainText(tr("Sending request to %1...").arg(url));
    nam_.get(QNetworkRequest(QUrl(url)));
}

void MainWidget::replyFinished(QNetworkReply* reply)
{
    if(reply->error()!=QNetworkReply::NoError){
        ui_->log->appendPlainText(reply->errorString());
        return;
    }
    bool ok;
    auto ba = reply->readAll();
    QString dow_s = QString::fromUtf8(ba);
    reply->deleteLater();
    ba.toDouble(&ok);
    if(!ok){
        ui_->log->appendPlainText(tr("Received not a double: %1").arg(dow_s));
        if(++tries_>3)
            ui_->log->appendPlainText(tr("Invalid dow received after retries."));
        else{
            date_ = date_.addDays(-1);
            sendDowRequest();
        }
        return;
    }
    QString date_s = date_.toString(Qt::ISODate),
            hash_s = date_s+QLatin1Char('-')+dow_s;
    auto hash = QCryptographicHash::hash(hash_s.toUtf8(),QCryptographicHash::Md5);
    ui_->log->appendPlainText(tr("dow(%1)=%2").arg(date_s).arg(dow_s));
    ui_->log->appendPlainText(tr("md5(%1)=%2").arg(hash_s).arg(QString::fromUtf8(hash.toHex())));
    auto half_size = std::size_t(hash.size()/2);
    auto hex2fraction = [](const char* s,std::size_t n){
        double r = 0,w = 1;
        for(std::size_t i=0;i<n;++i){
            w *= 0.0625;
            r += (uint8_t(s[i])>>4)*w;
            w *= 0.0625;
            r += (uint8_t(s[i])&0xf)*w;
        }
        return r;
    };
    auto latitude = int(coord_.latitude())+hex2fraction(hash.data(),half_size),
         longitude = int(coord_.longitude())+hex2fraction(hash.data()+half_size,half_size);
    ui_->log->appendPlainText(tr("Result: (%1,%2)").arg(latitude).arg(longitude));
    auto url = QStringLiteral("https://yandex.ru/maps/?ll=%2,%1&spn=1,1&l=skl").arg(latitude).arg(longitude);
    ui_->log->appendPlainText(tr("Navigating to %1...").arg(url));
    ui_->mapView->setUrl(QUrl(url));
}
