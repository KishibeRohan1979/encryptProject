#include <QApplication>
#include "choosewindow.h"

int main(int argc, char *argv[]) {
    // 隐藏CMD窗口
//    FreeConsole();

    QApplication a(argc, argv);
    ChooseWindow chooseWindow;
    chooseWindow.show();

    return QApplication::exec();
}