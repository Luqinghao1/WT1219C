#ifndef WT_PROJECTWIDGET_H
#define WT_PROJECTWIDGET_H

#include <QWidget>
#include <QString>
#include "newprojectdialog.h"

// 项目管理界面
namespace Ui {
class WT_ProjectWidget;
}

class WT_ProjectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WT_ProjectWidget(QWidget *parent = nullptr);
    ~WT_ProjectWidget();

    void init();
    void createNewProject();

signals:
    // 新项目创建或打开成功信号 (用于通知 MainWindow 解锁功能)
    void newProjectCreated();

    // 文件读取成功信号，携带文件路径和类型 (这是用于导入数据的)
    void fileLoaded(const QString& filePath, const QString& fileType);

private slots:
    // 点击"新建"按钮的槽函数
    void onNewProjectClicked();

    // 点击"打开"按钮的槽函数
    void onOpenProjectClicked();

    // 点击"读取"按钮的槽函数
    void onLoadFileClicked();

private:
    Ui::WT_ProjectWidget *ui;
};

#endif // WT_PROJECTWIDGET_H
