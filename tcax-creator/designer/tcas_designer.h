#ifndef TCAS_DESIGNER_H
#define TCAS_DESIGNER_H

#include "../com/common.h"
#include "../com/tcax_helper.h"

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class TcasDesigner;
}

class TcasDesigner : public QWidget
{
    Q_OBJECT

public:
    explicit TcasDesigner(QWidget *parent = nullptr);
    ~TcasDesigner();
    friend class MainWindow;
    friend class ScriptDesigner;
    QWidget *mainUi;
    void reload(const QString &a_filename);

private slots:
    void on_buttonStartShifting_clicked();

private:
    Ui::TcasDesigner *ui;
    QString m_filename;
};

#endif // TCAS_DESIGNER_H
