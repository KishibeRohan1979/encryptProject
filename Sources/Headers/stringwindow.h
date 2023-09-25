//
// Created by admin on 2023/9/25.
//

#ifndef ENCRYPTPROJECT_STRINGWINDOW_H
#define ENCRYPTPROJECT_STRINGWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class StringWindow; }
QT_END_NAMESPACE

class StringWindow : public QWidget {
Q_OBJECT

public:
    explicit StringWindow(QWidget *parent = nullptr);

    ~StringWindow() override;

private:
    Ui::StringWindow *ui;

    void encryptString();

    static QString getRandomString(int length);

    QString showInputDialog(const QString &title, const QString &labelText, const QString &defaultValue, bool *ok);

    static QString encryptStringByKey(const QString &inputValue, const QString &key);

    void decryptString();

    QString decryptStringByKey(const QString &encryptedValue, const QString &key);
};


#endif //ENCRYPTPROJECT_STRINGWINDOW_H
