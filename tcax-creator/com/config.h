#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

#include <QObject>
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

class Config
{
public:
    Config();

    enum ECONFIG_COMMON {
        eCONFIG_COMMON_RECENT,
        eCONFIG_COMMON_SHOW_SPLASH_SCREEN,
        eCONFIG_COMMON_EDITOR_FILE_LIMIT_ENABLE,
        eCONFIG_COMMON_EDITOR_FILE_LIMIT_SIZE,
        eCONFIG_COMMON_STYLE_FACTORY_ENABLE,
        eCONFIG_COMMON_STYLE_FACTORY,
        eCONFIG_COMMON_MAX,
    };

    bool getPortableMode(void);
    void getConfigFile(void);
    QString settingsFilePath(void);
    bool isShowSplashScreen(void);

    void initCommonConfigDefault(void);
    void initCommonConfig(void);
    void reset(void);
    void saveConfigAll(void);

    QVariant getConfig(ECONFIG_COMMON a_key);
    QVariant getConfigDefault(ECONFIG_COMMON a_key);
    void setConfig(ECONFIG_COMMON a_key, QVariant a_value);

    QVariant valueInGroup(const QString & a_group, const QString & a_key, const QVariant & a_defaultValue) const;
    bool setValueInGroup(const QString & a_group, const QString & a_key, const QVariant & a_value);
    bool deleteValueInGroup(const QString & a_group, const QString & a_key);
    QVariant value(const QString & a_key, const QVariant & a_defaultValue) const;
    bool setValue(const QString & a_key, const QVariant & a_value);
    QStringList groupKeys(const QString & a_group);

public:
    QMap<ECONFIG_COMMON, QVariant> m_config_common;

private:
    QString m_settingsFilePath;
    QList<QVariant> m_config_common_default;
};

extern Config *g_pConfig;
extern const char *c_config_common_key[Config::eCONFIG_COMMON_MAX];

#endif // CONFIG_H
