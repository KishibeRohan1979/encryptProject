//
// Created by Dong on 2023/9/20.
//

#ifndef ENCRYPTPROJECT_FILEWINDOW_H
#define ENCRYPTPROJECT_FILEWINDOW_H

#include <QWidget>
#include <QSet>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class FileWindow; }
QT_END_NAMESPACE

class FileWindow : public QWidget {
Q_OBJECT

public:
    explicit FileWindow(QWidget *parent = nullptr);

    ~FileWindow() override;

    // 添加一个公共函数来获取所选文件的路径列表
    QSet<QString> getSelectedFiles() const;

signals:
    // 声明一个信号，用于通知selectedFiles发生变化
    void setChanged();

private:
    Ui::FileWindow *ui;

    QWidget *beforeFileWidget;

    QWidget *afterFileWidget;

    // QListView内部
    QStandardItemModel *listViewModel;

    // 添加一个私有成员变量来存储所选文件的路径列表
    QSet<QString> selectedFiles;

    // 自定义对象，自定义事件的触发
    bool eventFilter(QObject *obj, QEvent *event) override;

    // 拖动事件
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

    // 监测 selectedFiles 文件列表数据发生的变化
    void onSetChanged();

    // 用于处理删除按钮被点击时产生的事件
    void onDeleteButtonClicked();

    // 用于处理添加按钮被点击时产生的事件
    void onAddButtonClicked();

    // AES加密文件
    void encryptFile();

    // AES解密文件
    void decryptFile();

    QString showInputDialog(const QString &title, const QString &labelText, const QString &defaultValue, bool *ok);

    static QString getRandomString(int length);

    static bool decryptFileBySet(const QSet<QString> &fileSet, const QString &key);

    static bool encryptFileBySet(const QSet<QString> &fileSet, const QString &key);
};


#endif //ENCRYPTPROJECT_FILEWINDOW_H
