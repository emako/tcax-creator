#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include <QObject>

#include <Windows.h>
#include <atlstr.h>
#include <ShlObj.h>

#define OK 0
#define NG -1

bool assoc(void);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget mainUi;
    bool ret = false;

    mainUi.setWindowIcon(QIcon(":/icons/tcax.ico"));

    ret = assoc();
    if(ret)
    {
        QMessageBox::information(&mainUi, QObject::tr("TCAX - info"), QObject::tr("TCAX file association successfully executed.\n\nTCAX program file association successfully executed, please confirm."));
    }
    else
    {
        QMessageBox::critical(&mainUi, QObject::tr("TCAX - error"), QObject::tr("TCAX file association failed executed.\n\nTCAX program file association failed executed, please try again."));
    }

    return (ret ? OK : NG);
}

bool assoc(void)
{
    int      argc;
    LPWSTR  *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    HKEY     hKey;
    DWORD    dwDisp;
    CStringW wcsPath = argv[0];
    WCHAR    szSysDir[256];
    CStringW wcsSysDir;

    LocalFree(argv);
    wcsPath = wcsPath.Mid(0, wcsPath.ReverseFind(L'\\') + 1);    // note: '\\' is included in the Path
    GetSystemDirectoryW(szSysDir, 256);
    wcsSysDir = szSysDir;

    // --- for TCC file
    // --- Reg ".tcc" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L".tcc", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"TCC_File_Type", 0);
    RegCloseKey(hKey);

    // --- Reg "shell\open\command" of "TCC_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"TCC_File_Type\\shell\\open\\command", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"\"" + wcsPath + + L"\\tcax-creator.exe\" \"%1\"", 0);
    RegCloseKey(hKey);

    // --- Reg "shell\edit\command" of "TCC_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"TCC_File_Type\\shell\\edit\\command", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"\"" + wcsPath + + L"\\tcax-creator.exe\" \"%1\"", 0);
    RegCloseKey(hKey);

    // --- Reg "shell\parse" of "TCC_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"TCC_File_Type\\shell\\parse", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"Parse", 0);
    RegCloseKey(hKey);

    // --- Reg "shell\parse\command" of "TCC_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"TCC_File_Type\\shell\\parse\\command", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"\"" + wcsPath + L"tcax\\tcax.exe\" \"%L\" -debug", 0);
    RegCloseKey(hKey);

    // --- Reg "DefaultIcon" of "TCC_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"TCC_File_Type\\DefaultIcon", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, wcsPath + L"tcax\\icons\\tcc.ico", 0);
    RegCloseKey(hKey);

    // --- for ASS file
    // --- Reg ".ass" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L".ass", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"ASS_File_Type", 0);
    RegCloseKey(hKey);

    // --- Reg "DefaultIcon" of "ASS_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"ASS_File_Type\\DefaultIcon", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, wcsPath + L"tcax\\icons\\ass.ico", 0);
    RegCloseKey(hKey);

    // --- for tcas file
    // --- Reg ".tcas" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L".tcas", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"TCAS_File_Type", 0);
    RegCloseKey(hKey);

    // --- Reg "shell\open\command" of "TCAS_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"TCAS_File_Type\\shell\\open\\command", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }
    RegSetValueW(hKey, nullptr, REG_SZ, L"\"" + wcsPath + L"tcax-creator.exe\" \"%L\"", 0);
    RegCloseKey(hKey);

    // --- Reg "DefaultIcon" of "TCAS_File_Type" in "HKEY_CLASSES_ROOT"
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, L"TCAS_File_Type\\DefaultIcon", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, &dwDisp) != ERROR_SUCCESS)
    {
        return false;
    }

    RegSetValueW(hKey, nullptr, REG_SZ, wcsPath + L"tcax\\icons\\tcas.ico", 0);
    RegCloseKey(hKey);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSHNOWAIT, nullptr, nullptr);

    return true;
}

