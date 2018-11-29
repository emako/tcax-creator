#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

global Config *g_pConfig = nullptr;

int main(int argc, char *argv[])
{
    /* Application Setup */
    QApplication a(argc, argv);

    /* Application Current Path */
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    /* Global Configure First */
    g_pConfig = new Config(); // deleted when ~MainWindow()

    /* Splash Screen Show */
    QSplashScreen screen(QPixmap(":/images/tcax-logo.png"));
    if(g_pConfig->isShowSplashScreen())
    {
        screen.show();
    }

    MainWindow w;
    screen.finish(&w);
    w.show();

    return a.exec();
}
