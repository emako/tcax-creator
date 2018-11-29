#ifndef TCC_DESIGNER_H
#define TCC_DESIGNER_H

#include "../com/tcax_helper.h"

#include <QWidget>
#include <QObject>
#include <QMenu>
#include <QFontDatabase>
#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExpValidator>

#define TCC_COLOR_LENGTH (7)
#define TCC_COLOR_LAST_CHAR L'\0'
#define TCC_ALIGNMENT_DEFAULT (7)

#define TCC_JSON_PY_FILE "file"
#define TCC_JSON_BEG_LINE "from"
#define TCC_JSON_END_LINE "to"

#define TCC_K_MODE_NORMAL "normal"
#define TCC_K_MODE_SYLLABLE "syllable"
#define TCC_K_MODE_ADVANCED "advanced"

#define TCC_TEXT_LAYOUT_HOR "horizontal"
#define TCC_TEXT_LAYOUT_VER "vertical"

#define TCC_PROP_ALIGNMENT "tcc_alignment"

#define TCC_COLOR_INIT_MOJI "#"

#define TCC_PY_LIST_JSON_ARRAY_PROP "py_json_array_prop"
#define TCC_PY_LIST_MENU_ADD QObject::tr("Add")
#define TCC_PY_LIST_MENU_DELETE QObject::tr("Delete")
#define TCC_PY_LIST_MENU_MOVE_UP QObject::tr("Move Up")
#define TCC_PY_LIST_MENU_MOVE_DOWN QObject::tr("Move Down")

#define TCC_INDEX_OFFSET (1)

#define TCC_LIB_NEO

namespace Ui {
class TccDesigner;
}

class TccDesigner : public QWidget
{
    Q_OBJECT

public:
    explicit TccDesigner(QWidget *parent = nullptr);
    ~TccDesigner();
    friend class MainWindow;
    friend class ScriptDesigner;
    QWidget *mainUi;
    void reload(const QString &a_filename);
    bool save(const QString &a_filename);
    bool isChanged(void);

    void setTccMap(TccMap a_tccMap);
    TccMap getTccMap(void);

    QT_DEPRECATED void setTccAttr(TCC_Attributes a_tccAttr);
    QT_DEPRECATED TCC_Attributes getTccAttr(void);
    QT_DEPRECATED void freeTccAttr(TCC_pAttributes a_pTccAttr);

private slots:
    void colorChanged(QString a_colorString);
    void lineChanged(void);

    void on_comboBoxFontName_activated(const QString &a_text);
    void on_listWidgetLine_customContextMenuRequested(const QPoint &);
    void on_buttonBrowseSource_clicked();
    void on_buttonBrowsePython_clicked();

private:
    void addCustomActions(void);
    void setPythonLines(const QJsonArray &a_jsonArray);
    QString getSrcFilename(const QString &a_filename);

private:
    Ui::TccDesigner *ui;
    QButtonGroup *m_pButtonGroupAlignment;
    QMenu *m_pPyListMenu;
    QRegExpValidator *m_pRegExpValidator;
};

#endif // TCC_DESIGNER_H
