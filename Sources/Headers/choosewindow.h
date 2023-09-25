//
// Created by admin on 2023/9/20.
//

#ifndef ENCRYPTPROJECT_CHOOSEWINDOW_H
#define ENCRYPTPROJECT_CHOOSEWINDOW_H

#include <QWidget>
#include <QPushButton>


QT_BEGIN_NAMESPACE
namespace Ui { class ChooseWindow; }
QT_END_NAMESPACE

class ChooseWindow : public QWidget {
Q_OBJECT

public:
    explicit ChooseWindow(QWidget *parent = nullptr);

    ~ChooseWindow() override;

private:
    Ui::ChooseWindow *ui;

    QPushButton *fileButton;

    QPushButton *stringButton;
};


#endif //ENCRYPTPROJECT_CHOOSEWINDOW_H
