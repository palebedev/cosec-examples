#ifndef UUID_D1438A3B_FCA7_4EE4_BD0F_AC64BB188F86
#define UUID_D1438A3B_FCA7_4EE4_BD0F_AC64BB188F86

#include <QtCore/QStringList>

QStringList getAvailableTranslations();
QStringList getFullTranslationNames(const QStringList& translations);
bool loadTranslation(const QString& language);
QString getTranslationClosestToUiLanguage();

#endif
