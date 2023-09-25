//
// Created by Dong on 2023/9/20.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FileWindow.h" resolved

#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QFileDialog>
#include <QInputDialog>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QT-AES/qaesencryption.h>
#include "filewindow.h"
#include "Forms/ui_FileWindow.h"

#include <QDebug>

FileWindow::FileWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::FileWindow) {
    ui->setupUi(this);

    // 加载该窗口之后，先隐藏获取数据之后的 widget
    afterFileWidget = ui->afterFileWidget;
    afterFileWidget->setVisible(false);

    // QListView初始化，创建 QStandardItemModel 模型
    listViewModel = new QStandardItemModel(this);
    // 将模型设置给 QListView
    ui->listView->setModel(listViewModel);

    // beforeFileWidget 添加事件
    beforeFileWidget = ui->beforeFileWidget;
    beforeFileWidget->installEventFilter(this);

    // 处理当文件被添加时，造成 selectedFiles 长度发生改变，处理方法
    connect(this, &FileWindow::setChanged, this, &FileWindow::onSetChanged);

    // 当点击删除某行的事件发生时，处理的事件
    connect(ui->deleteButton, &QPushButton::clicked, this, &FileWindow::onDeleteButtonClicked);

    // 当点击添加按钮的点击事件发生时，处理的事件
    connect(ui->addButton, &QPushButton::clicked, this, &FileWindow::onAddButtonClicked);

    // 当加密按钮被点击时，触发的事件
    connect(ui->encryptButton, &QPushButton::clicked, this, &FileWindow::encryptFile);

    // 当解密按钮被点击时，触发的事件
    connect(ui->decryptButton, &QPushButton::clicked, this, &FileWindow::decryptFile);
}

FileWindow::~FileWindow() {
    delete ui;
}

bool FileWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == beforeFileWidget && event->type() == QEvent::MouseButtonPress) {
        // 当 beforeFileWidget 窗口被点击时，弹出文件选择窗口
        // 该方法已经发送过数据修改信号
        onAddButtonClicked();
        // 事件被处理
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

// 实现获取所选文件的路径列表的函数
QSet<QString> FileWindow::getSelectedFiles() const {
    return selectedFiles;
}

// 处理拖拽进入事件
void FileWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

// 处理拖拽释放事件
void FileWindow::dropEvent(QDropEvent *event) {
    // 获取拖拽的文件路径
    QList<QUrl> urls = event->mimeData()->urls();

    for (const QUrl &url: urls) {
        selectedFiles.insert(url.toLocalFile());
    }
    // 发射 selectedFiles 已经被改变的信号
    emit setChanged();
    event->acceptProposedAction();
}

void FileWindow::onSetChanged() {
    // 获取当前selectedFiles的大小
    int currSize = selectedFiles.size();

    // 检查是否发生了大小变化
    if (currSize != 0) {
        // 显示文件被加载后的列表状态
        beforeFileWidget->setVisible(false);
        afterFileWidget->setVisible(true);

        // 清空模型，以确保不重复添加数据
        listViewModel->clear();

        for (const QString &filePath: selectedFiles) {
            QFileInfo fileInfo(filePath);
            if (fileInfo.isFile()) {
                // 这是一个文件的时候添加
                QStandardItem *item;
                item = new QStandardItem(filePath);
                listViewModel->appendRow(item);
            }
        }
    } else {
        // 显示文件被加载前的列表状态
        beforeFileWidget->setVisible(true);
        afterFileWidget->setVisible(false);

        // 清空模型，以确保不重复添加数据
        listViewModel->clear();
    }
}

void FileWindow::onDeleteButtonClicked() {
    // 获取选中的项目的索引列表，因为选择的可能是多个
    QModelIndexList selectedIndexes = ui->listView->selectionModel()->selectedIndexes();

    // 从模型和 selectedFiles 中删除选中的项目
    for (const QModelIndex &index: selectedIndexes) {
        QString filePath = listViewModel->data(index).toString();
        listViewModel->removeRow(index.row());
        selectedFiles.remove(filePath);
    }
    // 发射 selectedFiles 已经被改变的信号
    emit setChanged();
}

void FileWindow::onAddButtonClicked() {
    // 弹出文件选择对话框
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "选择文件", "", "所有文件 (*.*);;文本文件 (*.txt)");

    // 将所选文件的路径添加到 selectedFiles 列表中
    for (int i = 0; i < fileNames.size(); ++i) {
        selectedFiles.insert(fileNames.at(i));
    }
    // 发射 selectedFiles 已经被改变的信号
    emit setChanged();
}

void FileWindow::encryptFile() {
    if (selectedFiles.empty()) {
        QMessageBox::warning(this, "警告", "没有文件被加入列表！");
    } else {
        // 生成 16 位随机数
        QString defaultKey = getRandomString(16);
        bool ok;
        QString key = showInputDialog("输入对应位数密钥", "请输入16位或24、32位密钥：", defaultKey, &ok);
        if (ok) {
            // 在这里处理加密操作，使用用户输入的密钥（key）
            if (key.size() == 16 || key.size() == 24 || key.size() == 32) {
                bool success = encryptFileBySet(selectedFiles, key);
                if (success) {
                    QMessageBox::warning(this, "提示", "加密成功！");
                    selectedFiles.clear();
                    // 发射 selectedFiles 已经被改变的信号
                    emit setChanged();
                } else {
                    QMessageBox::warning(this, "警告", "加密失败！");
                }
            } else {
                QMessageBox::warning(this, "警告", "请输入16位或24、32位密钥！");
            }
        } else {
            qDebug() << "用户取消捏";
        }
    }
}

void FileWindow::decryptFile() {
    if (selectedFiles.empty()) {
        QMessageBox::warning(this, "警告", "没有文件被加入列表！");
    } else {
        bool ok;
        QString key = showInputDialog("输入对应位数密钥", "请输入16位或24、32位密钥：", "", &ok);
        if (ok) {
            // 在这里处理加密操作，使用用户输入的密钥（key）
            if (key.size() == 16 || key.size() == 24 || key.size() == 32) {
                bool success = decryptFileBySet(selectedFiles, key);
                if (success) {
                    QMessageBox::warning(this, "提示", "解密成功！");
                    selectedFiles.clear();
                    // 发射 selectedFiles 已经被改变的信号
                    emit setChanged();
                } else {
                    QMessageBox::warning(this, "警告", "解密失败！");
                }
            } else {
                QMessageBox::warning(this, "警告", "请输入16位或24、32位密钥！");
            }
        } else {
            qDebug() << "用户取消捏";
        }
    }
}

// 显示输入对话框，参数1：窗口 title 显示文本；参数2；提示显示文本；参数3：默认值
QString
FileWindow::showInputDialog(const QString &title, const QString &labelText, const QString &defaultValue, bool *ok) {
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
    return key;
}

// 产生指定长度的随机字符串
QString FileWindow::getRandomString(int length) {
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

// 文件加密
bool FileWindow::encryptFileBySet(const QSet<QString> &fileSet, const QString &key) {
    for (const QString &filePath: fileSet) {
        QFile inputFile(filePath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开文件：" << filePath;
            return false;
        }

        QFileInfo fileInfo(filePath);
        // 加密文件的文件名
        QString encryptedFileName = "en_" + fileInfo.fileName();
        // 加密文件的存储位置
        QString encryptedFilePath = fileInfo.absolutePath() + "/" + encryptedFileName;

        QFile outputFile(encryptedFilePath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qDebug() << "无法创建加密文件：" << encryptedFilePath;
            inputFile.close();
            return false;
        }

        QAESEncryption::Aes level;
        if (key.size() == 32) {
            level = QAESEncryption::AES_256;
        } else if (key.size() == 24) {
            level = QAESEncryption::AES_192;
        } else {
            level = QAESEncryption::AES_128;
        }

        QAESEncryption encryption(level, QAESEncryption::ECB, QAESEncryption::PKCS7);

        QByteArray inputBytes = inputFile.readAll();
        QByteArray encryptedBytes = encryption.encode(inputBytes, key.toUtf8());

        if (encryptedBytes.isEmpty()) {
            qDebug() << "加密失败";
            inputFile.close();
            outputFile.close();
            return false;
        }

        // 将加密后的数据写入输出文件
        if (outputFile.write(encryptedBytes) == -1) {
            qDebug() << "写入加密数据失败";
            inputFile.close();
            outputFile.close();
            return false;
        }

        inputFile.close();
        outputFile.close();
    }

    return true;
}

// 文件解密
bool FileWindow::decryptFileBySet(const QSet<QString> &fileSet, const QString &key) {
    for (const QString &filePath: fileSet) {
        QFile inputFile(filePath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开文件：" << filePath;
            return false;
        }

        QFileInfo fileInfo(filePath);
        // 加密文件的文件名
        QString decryptedFileName = "en_" + fileInfo.fileName();
        // 加密文件的存储位置
        QString decryptedFilePath = fileInfo.absolutePath() + "/" + decryptedFileName;

        QFile outputFile(decryptedFilePath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qDebug() << "无法创建解密文件：" << decryptedFilePath;
            inputFile.close();
            return false;
        }

        QAESEncryption::Aes level;
        if (key.size() == 32) {
            level = QAESEncryption::AES_256;
        } else if (key.size() == 24) {
            level = QAESEncryption::AES_192;
        } else {
            level = QAESEncryption::AES_128;
        }

        QAESEncryption encryption(level, QAESEncryption::ECB, QAESEncryption::PKCS7);
        QByteArray inputBytes = inputFile.readAll();
        QByteArray decryptedBytes = encryption.decode(inputBytes, key.toUtf8());

        if (decryptedBytes.isEmpty()) {
            qDebug() << "解密失败";
            inputFile.close();
            outputFile.close();
            return false;
        }

        // 将解密后的数据写入输出文件
        if (outputFile.write(decryptedBytes) == -1) {
            qDebug() << "写入解密数据失败";
            inputFile.close();
            outputFile.close();
            return false;
        }

        inputFile.close();
        outputFile.close();
    }

    return true;
}
