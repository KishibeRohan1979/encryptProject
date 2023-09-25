//
// Created by admin on 2023/9/20.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ChooseWindow.h" resolved

#include <QMessageBox>
#include "choosewindow.h"
#include "Forms/ui_ChooseWindow.h"
#include "filewindow.h"


ChooseWindow::ChooseWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::ChooseWindow) {
    ui->setupUi(this);

    // rect().width()获取窗口宽，size().height()获取窗口高
    // 设置窗口不能调整大小，固定值为宽330，高度166，当然这里是动态获取，可以在ui里随意更改
    setFixedSize(rect().width(), size().height());

    // 获取文件加密按钮
    fileButton = ui->fileButton;
    // 连接按钮的点击事件到槽函数
    QObject::connect(fileButton, &QPushButton::clicked, [&]() {
//        QMessageBox::warning(this, "警告", "按钮被点击了！");
        // 打开文件加解密窗口
        FileWindow *fileWindow;
        fileWindow = new FileWindow;
        fileWindow->show();
        // 先开后关，任务栏看着舒服
        // 关闭当前窗口，hide 比 close 的占用内存高一些，但是调用方便，时间性能快
        this->hide();
    });

    // 获取字符串加密按钮
    stringButton = ui->stringButton;
    // 连接按钮的点击事件到槽函数
    QObject::connect(stringButton, &QPushButton::clicked, [&]() {
        // 关闭当前窗口，hide 比 close 的占用内存高一些，但是调用方便，时间性能快
        this->hide();
        // 打开字符串加解密窗口
    });
}

ChooseWindow::~ChooseWindow() {
    delete ui;
}
