#include "mainwindow.hpp"

#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>

#include <cstdint>
#include <random>

namespace
{
    QString createDatabase()
    {
        auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
        db.setDatabaseName(QStringLiteral(":memory:"));
        auto error = [&](QString message){
            return message.arg(db.lastError().text());
        };
        if(!db.open())
            return error(qApp->translate("CreateDB","Couldn't create in-memory database: %1"));
        QSqlQuery query;
        if(!query.exec(QStringLiteral(
                "CREATE TABLE fabrics (ID INTEGER PRIMARY KEY,name TEXT,color INTEGER,avail_from INTEGER)")))
            return error(qApp->translate("CreateDB","Couldn't create table: %1"));
        if(!query.prepare(QStringLiteral("INSERT INTO fabrics (name,color,avail_from) VALUES (?,?,?)")))
            return error(qApp->translate("CreateDB","Couldn't prepare insert query"));
        constexpr std::size_t rows = 1000;
        std::mt19937_64 prng;
        QString vowels = QStringLiteral("aeiouwy"),
                consonants = QStringLiteral("bcdfghjklmnpqrstvxz");
        std::uniform_int_distribution<int> syllablesDist{3,7},
                                           syllableTypeDist{0,3}, // {v,cv,cvc,ccv}
                                           vowelsDist{0,vowels.size()-1},
                                           consonantsDist{0,consonants.size()-1};
        std::uniform_int_distribution<std::uint32_t> colorDist{0,0xffffff};
        std::uniform_int_distribution<std::int64_t> dateDist{
            QDate{2010,1,1}.startOfDay().toSecsSinceEpoch(),
            QDate{2020,1,1}.startOfDay().toSecsSinceEpoch()};
        QString name;
        for(std::size_t i=0;i<rows;++i){
            name.resize(0);
            int s = syllablesDist(prng);
            for(int j=0;j<s;++j)
                switch(syllableTypeDist(prng)){
                    case 3:
                        name.append(consonants[consonantsDist(prng)]);
                        [[fallthrough]] ;
                    case 1:
                        name.append(consonants[consonantsDist(prng)]);
                        [[fallthrough]] ;
                    case 0:
                        name.append(vowels[vowelsDist(prng)]);
                        break;
                    case 2:
                        name.append(consonants[consonantsDist(prng)]);
                        name.append(vowels[vowelsDist(prng)]);
                        name.append(consonants[consonantsDist(prng)]);
                }
            name[0] = name[0].toUpper();
            query.addBindValue(name);
            query.addBindValue(colorDist(prng));
            query.addBindValue(qlonglong(dateDist(prng)));
            if(!query.exec())
                return error(qApp->translate("CreateDB","Failed to insert row: %1"));
        }
        return {};
    }
}

int main(int argc,char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app{argc,argv};
    QString error = createDatabase();
    if(!error.isEmpty()){
        QMessageBox::critical(nullptr,app.translate("CreateDB","Error"),error);
        return 1;
    }
    MainWindow mw;
    mw.show();
    return app.exec();
}
