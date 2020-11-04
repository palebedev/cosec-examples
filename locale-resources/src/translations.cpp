#include "translations.hpp"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QStandardPaths>
#include <QtCore/QTranslator>
#include <QtWidgets/QApplication>

#include <locale-resources/config.hpp>

namespace
{
    QStringList getDataDirs()
    {
        static QStringList dirs;
        if(dirs.isEmpty()){
            // First, try the application directory.
            // QStandardPaths will add this only on Windows, but we want
            // to support resources as in binary dir everywhere.
            auto appPath = qApp->applicationDirPath();
            dirs.append(appPath);
            // Add relative prefix path that QStandardPaths doesn't know about.
            if(auto fp = QFileInfo{appPath+QStringLiteral("/" LOCALE_RESOURCES_REL_DATADIR)}.canonicalFilePath();
                    !fp.isEmpty())
                dirs.append(fp);
            // QStandardPaths uses only /usr prefix, but provides os-specific profile locations.
            for(const QString& fp:QStandardPaths::standardLocations(QStandardPaths::AppDataLocation))
                if(QFileInfo::exists(fp)&&!dirs.contains(fp))
                    dirs.append(fp);
        }
        return dirs;
    }

    QStringList getTranslationDirs()
    {
        QStringList dirs;
        if(dirs.isEmpty())
            for(const QString& fp:getDataDirs())
                if(QString tp = fp+QStringLiteral("/translations");QFileInfo::exists(tp))
                    dirs.append(tp);
        return dirs;
    }

    QString getQtTranslationsDir()
    {
        return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    }

    QTranslator appTranslator,qtTranslator;
}

QStringList getAvailableTranslations()
{
    QStringList translations;
    if(translations.isEmpty()){
        int appNameLength = qApp->applicationName().size();
        for(const QString& tp:getTranslationDirs())
            for(const QString& name:QDir(tp).entryList({
                    qApp->applicationName()+QStringLiteral("_*.qm")})){

                auto lang = name.mid(appNameLength+1,name.size()-appNameLength-4);
                if(!translations.contains(lang))
                    translations.append(lang);
            }
    }
    return translations;
}

QStringList getFullTranslationNames(const QStringList& translations)
{
    QStringList ftn;
    ftn.reserve(translations.size());
    for(const QString& language:translations)
        ftn.append(QLocale(language).nativeLanguageName());
    return ftn;
}

bool loadTranslation(const QString& language)
{
    qApp->removeTranslator(&appTranslator);
    qApp->removeTranslator(&qtTranslator);
    QString nameBase = qApp->applicationName()+QLatin1Char('_')+language,
            fileName = nameBase+QStringLiteral(".qm");
    for(const QString& tp:getTranslationDirs())
        if(QFileInfo::exists(tp+QLatin1Char('/')+fileName)){
            if(!appTranslator.load(nameBase,tp))
                return false;
            break;
        }
    qtTranslator.load(QStringLiteral("qt_")+language,getQtTranslationsDir());
    qApp->installTranslator(&appTranslator);
    qApp->installTranslator(&qtTranslator);
    QLocale::setDefault(QLocale(language));
    qApp->setApplicationDisplayName(qApp->translate("Global","Locale & Resources Test Application"));
    return true;
}
