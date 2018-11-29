#ifndef LUA_HELPER_H
#define LUA_HELPER_H

#include "common.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <stdexcept>
#include <string>

#include <QObject>
#include <QDebug>

#define LUA_QRC_ENTRY ":/strings/lua/"

namespace LuaHelper
{
    bool luaDoFile(const QString &a_luaFile, const QString &a_funcName, const QVariant &a_arg1, const QVariant &a_arg2, bool a_fromRes = true);

    bool lyricToAss(const QString &a_srcPath, const QString &a_tarPath);
    bool lyricToAssSafe(const QString &a_srcPath, const QString &a_tarPath);
};

#endif // LUA_HELPER_H
