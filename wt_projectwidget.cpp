#include "wt_projectwidget.h"
#include "ui_wt_projectwidget.h"
#include "newprojectdialog.h"
#include "modelparameter.h" // 引入全局参数类

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QPalette>

WT_ProjectWidget::WT_ProjectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WT_ProjectWidget)
{
    ui->setupUi(this);
    init();
}

WT_ProjectWidget::~WT_ProjectWidget()
{
    delete ui;
}

void WT_ProjectWidget::init()
{
    qDebug() << "初始化项目管理界面...";

    // 设置一个空的顶部样式
    QString topPicStyle = "";
    QString topName = "  ";

    // 设置整体背景为透明
    this->setStyleSheet("background-color: transparent;");
    ui->widget_5->setStyleSheet("background-color: transparent;");

    // 调整按钮之间的间距
    ui->gridLayout_3->setHorizontalSpacing(30);
    ui->gridLayout_3->setVerticalSpacing(10);

    // 创建大字体
    QFont bigFont;
    bigFont.setPointSize(16);
    bigFont.setBold(true);

    // 创建背景颜色
    QColor backgroundColor(148, 226, 255);

    QString forceStyle = QString(
        "MonitoStateW { "
        "background-color: rgb(148, 226, 255); "
        "border-radius: 10px; "
        "padding: 10px; "
        "} "
        "MonitoStateW * { "
        "background-color: transparent; "
        "} "
        "MonitoStateW:hover { "
        "background-color: rgb(120, 200, 240); "
        "} "
        "QLabel { "
        "color: #333333; "
        "font-weight: bold; "
        "margin-top: 5px; "
        "background-color: transparent; "
        "}"
        );

    // "新建"按钮设置
    QString centerPicStyle = "border-image: url(:/new/prefix1/Resource/Mon1.png);";
    QString bottomName = "新建";
    ui->MonitState1->setTextInfo(centerPicStyle, topPicStyle, topName, bottomName);
    ui->MonitState1->setFixedSize(128, 160);
    ui->MonitState1->setStyleSheet(forceStyle);
    ui->MonitState1->setAutoFillBackground(true);
    QPalette palette1 = ui->MonitState1->palette();
    palette1.setColor(QPalette::Window, backgroundColor);
    ui->MonitState1->setPalette(palette1);
    ui->MonitState1->setFont(bigFont);
    ui->MonitState1->setMouseTracking(true);
    connect(ui->MonitState1, SIGNAL(sigClicked()), this, SLOT(onNewProjectClicked()));

    // "打开"按钮设置
    centerPicStyle = "border-image: url(:/new/prefix1/Resource/Mon2.png);";
    bottomName = "打开";
    ui->MonitState2->setTextInfo(centerPicStyle, topPicStyle, topName, bottomName);
    ui->MonitState2->setFixedSize(128, 160);
    ui->MonitState2->setStyleSheet(forceStyle);
    ui->MonitState2->setAutoFillBackground(true);
    QPalette palette2 = ui->MonitState2->palette();
    palette2.setColor(QPalette::Window, backgroundColor);
    ui->MonitState2->setPalette(palette2);
    ui->MonitState2->setFont(bigFont);
    ui->MonitState2->setMouseTracking(true);
    // 连接打开按钮信号
    connect(ui->MonitState2, SIGNAL(sigClicked()), this, SLOT(onOpenProjectClicked()));

    // "读取"按钮设置
    centerPicStyle = "border-image: url(:/new/prefix1/Resource/Mon3.png);";
    bottomName = "读取";
    ui->MonitState3->setTextInfo(centerPicStyle, topPicStyle, topName, bottomName);
    ui->MonitState3->setFixedSize(128, 160);
    ui->MonitState3->setStyleSheet(forceStyle);
    ui->MonitState3->setAutoFillBackground(true);
    QPalette palette3 = ui->MonitState3->palette();
    palette3.setColor(QPalette::Window, backgroundColor);
    ui->MonitState3->setPalette(palette3);
    ui->MonitState3->setFont(bigFont);
    ui->MonitState3->setMouseTracking(true);
    connect(ui->MonitState3, SIGNAL(sigClicked()), this, SLOT(onLoadFileClicked()));

    // "退出"按钮设置
    centerPicStyle = "border-image: url(:/new/prefix1/Resource/Mon4.png);";
    bottomName = "退出";
    ui->MonitState4->setTextInfo(centerPicStyle, topPicStyle, topName, bottomName);
    ui->MonitState4->setFixedSize(128, 160);
    ui->MonitState4->setStyleSheet(forceStyle);
    ui->MonitState4->setAutoFillBackground(true);
    QPalette palette4 = ui->MonitState4->palette();
    palette4.setColor(QPalette::Window, backgroundColor);
    ui->MonitState4->setPalette(palette4);
    ui->MonitState4->setFont(bigFont);
    ui->MonitState4->setMouseTracking(true);
    connect(ui->MonitState4, &MonitoStateW::sigClicked, this, [=]() {
        QApplication::quit();
    });
}

void WT_ProjectWidget::onNewProjectClicked()
{
    qDebug() << "新建按钮被点击...";
    NewProjectDialog* dialog = new NewProjectDialog(this);
    int result = dialog->exec();

    if (result == QDialog::Accepted) {
        qDebug() << "新项目创建成功";

        // 获取新项目数据并更新全局参数
        ProjectData data = dialog->getProjectData();
        ModelParameter::instance()->setParameters(
            data.porosity, data.thickness, data.viscosity,
            data.volumeFactor, data.compressibility, data.productionRate,
            data.wellRadius, data.fullFilePath
            );

        emit newProjectCreated();
    }
    delete dialog;
}

void WT_ProjectWidget::onOpenProjectClicked()
{
    qDebug() << "打开按钮被点击...";
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("打开项目"),
        "",
        tr("WellTest Project (*.pwt)")
        );

    if (filePath.isEmpty()) return;

    // 使用全局参数类加载项目
    if (ModelParameter::instance()->loadProject(filePath)) {

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("成功");
        msgBox.setText("项目已成功打开！");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #ffffff; color: #000000; }"
            "QLabel { color: #000000; background-color: transparent; }"
            "QPushButton { "
            "   color: #000000; "
            "   background-color: #f0f0f0; "
            "   border: 1px solid #c0c0c0; "
            "   border-radius: 4px; "
            "   padding: 5px 15px; "
            "}"
            "QPushButton:hover { background-color: #e0e0e0; }"
            "QPushButton:pressed { background-color: #d0d0d0; }"
            );
        msgBox.exec();

        // 发送信号通知 MainWindow 解锁其他功能
        emit newProjectCreated();
    } else {
        QMessageBox::critical(this, "错误", "无法打开或解析项目文件。");
    }
}

void WT_ProjectWidget::onLoadFileClicked()
{
    qDebug() << "读取按钮被点击...";
    QString filter = tr("Excel Files (*.xlsx *.xls);;Text Files (*.txt);;All Files (*.*)");
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择要读取的数据文件"), QString(), filter);

    if (filePath.isEmpty()) return;

    QString fileType = "unknown";
    if (filePath.endsWith(".xlsx", Qt::CaseInsensitive) || filePath.endsWith(".xls", Qt::CaseInsensitive)) {
        fileType = "excel";
    } else if (filePath.endsWith(".txt", Qt::CaseInsensitive)) {
        fileType = "txt";
    }

    emit fileLoaded(filePath, fileType);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("文件读取"));
    msgBox.setText(tr("文件已成功读取，正在准备显示数据..."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet("QLabel{color: black;}");
    msgBox.exec();
}
