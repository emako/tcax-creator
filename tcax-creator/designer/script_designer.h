#ifndef SCRIPT_DESIGNER_H
#define SCRIPT_DESIGNER_H

#include "../com/tcax_helper.h"

#include <QWidget>

namespace Ui {
class ScriptDesigner;
}

class ScriptDesigner : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptDesigner(QWidget *parent = nullptr);
    void setMainUi(QWidget *a_pWidget);
    ~ScriptDesigner();
    QWidget *mainUi;
    friend class MainWindow;

    enum DesignScriptType {
        TcaxDesign,
        TccDesign,
        ScriptDesignMax,
    };

    void clear(void);
    void reload(DesignScriptType a_type, const QString &a_filename);
    void reloadTcas(const QString &a_filename);
    void reloadTcc(const QString &a_filename);

private:
    Ui::ScriptDesigner *ui;
};

#endif // SCRIPT_DESIGNER_H
