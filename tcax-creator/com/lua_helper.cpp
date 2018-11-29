#include "lua_helper.h"

bool LuaHelper::luaDoFile(const QString &a_luaFile, const QString &a_funcName, const QVariant &a_arg1, const QVariant &a_arg2, bool a_fromRes)
{
    bool ret = true;
    int retLua = eINDEX_0;
    QString scriptFile;
    QString script;
    lua_State *lua = nullptr;

    if(a_fromRes)
    {
        scriptFile = QString(":/strings/lua/%1").arg(a_luaFile);
    }
    else
    {
        scriptFile = Common::findFirstFilePath(a_luaFile).remove(QT_DQM);
    }

    if(!QFileInfo(scriptFile).isFile())
    {
        qDebug() << "Lua none script!";
        return false;
    }

    script = Common::getFileText(scriptFile);
    if(script.isEmpty())
    {
        qDebug() << "Lua empty script!";
        return false;
    }

    lua = luaL_newstate();
    if(lua == nullptr)
    {
        qDebug() << "Lua new state failed!";
        ret = false;
    }

    luaL_openlibs(lua);

    if(lua == nullptr)
    {
        qDebug() << "Lua open libs failed!";
        ret = false;
    }

    retLua = luaL_dostring(lua, script.toStdString().c_str());

    if(retLua)
    {
        qDebug() << "Lua dofile failed!";
        ret = false;
    }

    lua_getglobal(lua, a_funcName.toStdString().c_str());

    if(a_arg1.type() == QVariant::Double)
    {
        lua_pushnumber(lua, a_arg1.toDouble());
    }
    else if(a_arg1.type() == QVariant::Bool)
    {
        lua_pushboolean(lua, a_arg1.toInt());
    }
    else
    {
        lua_pushstring(lua, a_arg1.toString().toStdString().c_str());
    }

    if(a_arg2.type() == QVariant::Double)
    {
        lua_pushnumber(lua, a_arg2.toDouble());
    }
    else if(a_arg2.type() == QVariant::Bool)
    {
        lua_pushboolean(lua, a_arg2.toInt());
    }
    else
    {
        lua_pushstring(lua, a_arg2.toString().toStdString().c_str());
    }

    retLua = lua_pcall(lua, 2, 1, 0);
    ret = QVariant(lua_toboolean(lua, -1)).toBool();

    if(retLua)
    {
        qDebug() << "Lua pcall error occured!";
        ret = false;
    }

    if(!ret)
    {
        qDebug() << "Lua return value is" << ret;
    }

    lua_close(lua);

    return ret;
}

bool LuaHelper::lyricToAssSafe(const QString &a_srcPath, const QString &a_tarPath)
{
    bool ret = false;
    QString tempFileSrc = Common::creatTempFilename(QFileInfo(a_srcPath).suffix());
    QString tempFileTar = Common::creatTempFilename(QFileInfo(a_tarPath).suffix());

    if(tempFileSrc.isEmpty() || tempFileTar.isEmpty())
    {
        return ret;
    }

    QFile::remove(tempFileSrc);

    if(QFile::copy(a_srcPath, tempFileSrc))
    {
        if(lyricToAss(tempFileSrc, tempFileTar))
        {
            QFile::remove(a_tarPath);

            if(QFile::copy(tempFileTar, a_tarPath))
            {
                QFile::remove(tempFileSrc);
                QFile::remove(tempFileTar);
                ret = true;
            }
        }
    }

    return ret;
}

bool LuaHelper::lyricToAss(const QString &a_srcPath, const QString &a_tarPath)
{
    return luaDoFile("lyric2ass.lua", "lyric_to_ass", a_srcPath, a_tarPath, true);
}
