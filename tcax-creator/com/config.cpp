#include "config.h"

const static char SETTINGS_FILE_NAME[]           = "/tcax-creator.config";
const static char COMMON_GROUP[]                 = "common";

const char *c_config_common_key[Config::eCONFIG_COMMON_MAX] = {
    "recent_folder_list",               /* eCONFIG_COMMON_RECENT */
    "show_splash_screen",               /* eCONFIG_COMMON_SHOW_SPLASH_SCREEN */
    "editor_file_limit_enable",         /* eCONFIG_COMMON_EDITOR_FILE_LIMIT_ENABLE */
    "editor_file_limit_size",           /* eCONFIG_COMMON_EDITOR_FILE_LIMIT_SIZE */
    "style_factory_enable",             /* eCONFIG_COMMON_STYLE_FACTORY_ENABLE */
    "style_factory",                    /* eCONFIG_COMMON_STYLE_FACTORY */
    "editor_font_point_size",           /* eCONFIG_COMMON_EDITOR_FONT_POINT_SIZE */
};

Config::Config()
{
    getConfigFile();
    initCommonConfig();
}

bool Config::getPortableMode(void)
{
    QString applicationDir = QCoreApplication::applicationDirPath();
    QString settingsFilePath = applicationDir + SETTINGS_FILE_NAME;
    QFileInfo settingsFileInfo(settingsFilePath);
    bool portableMode = (settingsFileInfo.exists() && settingsFileInfo.isWritable());
    return portableMode;
}

void Config::getConfigFile(void)
{
    QString applicationDir = QCoreApplication::applicationDirPath();

    if(getPortableMode())
    {
        m_settingsFilePath = applicationDir + SETTINGS_FILE_NAME;
    }
    else
    {
        m_settingsFilePath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + SETTINGS_FILE_NAME;
    }
    m_settingsFilePath = QDir::toNativeSeparators(m_settingsFilePath);

    qDebug() << "Config file:" << m_settingsFilePath;
}

QVariant Config::getConfig(ECONFIG_COMMON a_key)
{
    return m_config_common[a_key];
}

QVariant Config::getConfigDefault(ECONFIG_COMMON a_key)
{
    return m_config_common_default.at(a_key);
}

void Config::setConfig(ECONFIG_COMMON a_key, QVariant a_value)
{
    if(m_config_common.contains(a_key))
    {
        m_config_common.remove(a_key);
    }
    m_config_common.insert(a_key, a_value);
}

QVariant Config::valueInGroup(const QString & a_group, const QString & a_key, const QVariant & a_defaultValue) const
{
    QSettings settings(m_settingsFilePath, QSettings::IniFormat);
    settings.beginGroup(a_group);
    return settings.value(a_key, a_defaultValue);
}

bool Config::setValueInGroup(const QString & a_group, const QString & a_key, const QVariant & a_value)
{
    QSettings settings(m_settingsFilePath, QSettings::IniFormat);
    settings.beginGroup(a_group);
    settings.setValue(a_key, a_value);
    settings.sync();
    bool success = (QSettings::NoError == settings.status());
    return success;
}

bool Config::deleteValueInGroup(const QString & a_group, const QString & a_key)
{
    QSettings settings(m_settingsFilePath, QSettings::IniFormat);
    settings.beginGroup(a_group);
    settings.remove(a_key);
    settings.sync();
    bool success = (QSettings::NoError == settings.status());
    return success;
}

QVariant Config::value(const QString & a_key, const QVariant & a_defaultValue) const
{
    return valueInGroup(COMMON_GROUP, a_key, a_defaultValue);
}

bool Config::setValue(const QString & a_key, const QVariant & a_value)
{
    return setValueInGroup(COMMON_GROUP, a_key, a_value);
}

QStringList Config::groupKeys(const QString & a_group)
{
    QSettings settings(m_settingsFilePath, QSettings::IniFormat);
    settings.beginGroup(a_group);
    return settings.childKeys();
}

QString Config::settingsFilePath(void)
{
    return m_settingsFilePath;
}

bool Config::isShowSplashScreen(void)
{
    return getConfig(eCONFIG_COMMON_SHOW_SPLASH_SCREEN).toBool();
}

void Config::initCommonConfigDefault(void)
{
    m_config_common_default << QVariant(NULLSTR); /* eCONFIG_COMMON_RECENT */
    m_config_common_default << QVariant(true); /* eCONFIG_COMMON_SHOW_SPLASH_SCREEN */
    m_config_common_default << QVariant(true); /* eCONFIG_COMMON_EDITOR_FILE_LIMIT_ENABLE */
    m_config_common_default << QVariant(eINDEX_2); /* eCONFIG_COMMON_EDITOR_FILE_LIMIT_SIZE */
    m_config_common_default << QVariant(false); /* eCONFIG_COMMON_STYLE_FACTORY_ENABLE */
    m_config_common_default << QVariant(DEFAULT_STYLE_FACTORY); /* eCONFIG_COMMON_STYLE_FACTORY */
    m_config_common_default << QVariant(eINDEX_9); /* eCONFIG_COMMON_EDITOR_FONT_POINT_SIZE */
}

void Config::initCommonConfig(void)
{
    initCommonConfigDefault();
    for(int i = eINDEX_0; i < eCONFIG_COMMON_MAX; i++)
    {
        m_config_common.insert(static_cast<ECONFIG_COMMON>(i), value(c_config_common_key[static_cast<ECONFIG_COMMON>(i)], m_config_common_default.at(static_cast<ECONFIG_COMMON>(i))));
    }
    for(QMap<ECONFIG_COMMON, QVariant>::iterator i = m_config_common.begin(); i != m_config_common.end(); i++)
    {
        qDebug() << QString("%1:%2=").arg(i.key()).arg(c_config_common_key[i.key()]) << i.value();
    }
}

void Config::reset(void)
{
    for(int i = eINDEX_0; i < m_config_common_default.length(); i++)
    {
        if(static_cast<ECONFIG_COMMON>(i) != eCONFIG_COMMON_RECENT) // Recent can not be reset.
        {
            setConfig(static_cast<ECONFIG_COMMON>(i), m_config_common_default.at(i));
        }
    }
}

void Config::saveConfigAll(void)
{
    QString key;
    QVariant value;

    for(QMap<ECONFIG_COMMON, QVariant>::iterator i = m_config_common.begin(); i != m_config_common.end(); i++)
    {
        key = c_config_common_key[i.key()];
        value = i.value();
        if(key.isEmpty())
        {
            continue;
        }
        setValue(key, value);
    }
}
