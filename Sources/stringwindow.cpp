//
// Created by admin on 2023/9/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_StringWindow.h" resolved

#include <QMessageBox>
#include <QInputDialog>
#include <QT-AES/qaesencryption.h>
#include <QRandomGenerator>
#include "stringwindow.h"
#include "Forms/ui_StringWindow.h"


StringWindow::StringWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::StringWindow) {
    ui->setupUi(this);

    // 设置输出为可复制
    ui->outputPlainTextEdit->setReadOnly(true); // 设置为只读
    ui->outputPlainTextEdit->setTextInteractionFlags(Qt::TextSelectableByMouse); // 可以复制文本

    // 当加密按钮被点击时，触发的事件
    connect(ui->encryptButton, &QPushButton::clicked, this, &StringWindow::encryptString);

    // 当加密按钮被点击时，触发的事件
    connect(ui->decryptButton, &QPushButton::clicked, this, &StringWindow::decryptString);
}

StringWindow::~StringWindow() {
    delete ui;
}

void StringWindow::encryptString() {
    QString inputString = ui->inputPlainTextEdit->toPlainText();
    if (inputString.isEmpty()) {
        QMessageBox::warning(this, "警告", "输入框内容为空！");
    } else {
        bool ok;
        QString defaultKey = getRandomString(16);
        QString key = showInputDialog("输入对应位数密钥", "请输入16位或24、32位密钥：", defaultKey, &ok);
        if (ok) {
            // 在这里处理加密操作，使用用户输入的密钥（key）
            if (key.size() == 16 || key.size() == 24 || key.size() == 32) {
                QString encryptString = encryptStringByKey(inputString, key);
                // 将字符串设置到QPlainTextEdit
                ui->outputPlainTextEdit->setPlainText(encryptString);
            } else {
                QMessageBox::warning(this, "警告", "请输入16位或24、32位密钥！");
            }
        }
    }
}

void StringWindow::decryptString() {
    QString inputString = ui->inputPlainTextEdit->toPlainText();
    if (inputString.isEmpty()) {
        QMessageBox::warning(this, "警告", "输入框内容为空！");
    } else {
        bool ok;
        QString key = showInputDialog("输入对应位数密钥", "请输入16位或24、32位密钥：", "", &ok);
        if (ok) {
            // 在这里处理加密操作，使用用户输入的密钥（key）
            if (key.size() == 16 || key.size() == 24 || key.size() == 32) {
                QString decryptString = decryptStringByKey(inputString, key);
                // 将字符串设置到QPlainTextEdit
                ui->outputPlainTextEdit->setPlainText(decryptString);
            } else {
                QMessageBox::warning(this, "警告", "请输入16位或24、32位密钥！");
            }
        }
    }
}

// 显示输入对话框，参数1：窗口 title 显示文本；参数2；提示显示文本；参数3：默认值；参数4：确认用户点的是不是“确认”按钮
QString StringWindow::showInputDialog(const QString &title, const QString &labelText, const QString &defaultValue, bool *ok) {
    QString key = defaultValue;
    // 弹出对话框
    QInputDialog dialog(this);
    dialog.setOkButtonText("确定");
    dialog.setCancelButtonText("取消");
    dialog.setWindowTitle(title);
    dialog.setLabelText(labelText);
    dialog.setTextEchoMode(QLineEdit::Normal);
    dialog.setTextValue(defaultValue);
    if (dialog.exec() == QDialog::Accepted) {
        *ok = true;
    } else {
        // 用户点了关闭或者取消
        *ok = false;
    }
    key = dialog.textValue();
    return key.trimmed();
}

// 加密字符串
QString StringWindow::encryptStringByKey(const QString &inputValue, const QString &key) {
    QByteArray inputBytes = inputValue.toUtf8();
    QByteArray keyBytes = key.toUtf8();

    QAESEncryption::Aes level;
    if (key.size() == 32) {
        level = QAESEncryption::AES_256;
    } else if (key.size() == 24) {
        level = QAESEncryption::AES_192;
    } else {
        level = QAESEncryption::AES_128;
    }
    QAESEncryption encryption(level, QAESEncryption::ECB, QAESEncryption::PKCS7);

    QByteArray encryptedBytes = encryption.encode(inputBytes, keyBytes);

    // 将加密后的数据转换为十六进制字符串
    QString encryptedString;
    encryptedString = encryptedBytes.toHex();

    return encryptedString;
}

// 解密字符串
QString StringWindow::decryptStringByKey(const QString &encryptedValue, const QString &key) {
    // 将十六进制字符串转换回字节数组
    QByteArray encryptedBytes = QByteArray::fromHex(encryptedValue.toUtf8());
    QByteArray keyBytes = key.toUtf8();

    QAESEncryption::Aes level;
    if (key.size() == 32) {
        level = QAESEncryption::AES_256;
    } else if (key.size() == 24) {
        level = QAESEncryption::AES_192;
    } else {
        level = QAESEncryption::AES_128;
    }
    QAESEncryption encryption(level, QAESEncryption::ECB, QAESEncryption::PKCS7);

    QByteArray decryptedBytes = encryption.decode(encryptedBytes, keyBytes);

    // 将解密后的字节数组转换为字符串
    QString decryptedString = QString::fromUtf8(decryptedBytes);

    return decryptedString;
}

QString StringWindow::getRandomString(int length) {
    const QString characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%_-+=?";

    QString randomString;

    for (int i = 0; i < length; ++i) {
        // 生成一个随机索引，用于选择字符
        int randomIndex = QRandomGenerator::global()->bounded(characters.length());
        QChar randomChar = characters.at(randomIndex);
        randomString.append(randomChar);
    }

    return randomString;
}
