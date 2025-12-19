#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include <QStandardItemModel>
#include "modelmanager.h"

class NavBtn;
class WT_ProjectWidget; // [修改] 使用 WT_ProjectWidget
class DataEditorWidget;
class PlottingWidget;
class FittingPage;
class SettingsWidget;

struct WellTestData;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();

    void initProjectForm(); // [修改]
    void initDataEditorForm();
    void initModelForm();
    void initPlottingForm();
    void initFittingForm();

private slots:
    void onProjectCreated(); // 兼顾新建和打开项目成功
    void onFileLoaded(const QString& filePath, const QString& fileType);
    void onPlotAnalysisCompleted(const QString &analysisType, const QMap<QString, double> &results);
    void onDataReadyForPlotting();
    void onTransferDataToPlotting();
    void onDataEditorDataChanged();
    void onSystemSettingsChanged();
    void onAutoSaveIntervalChanged(int interval);
    void onBackupSettingsChanged(bool enabled);
    void onPerformanceSettingsChanged();
    void onModelCalculationCompleted(const QString &analysisType, const QMap<QString, double> &results);

    // 拟合进度信号（可选保留用于状态栏）
    void onFittingProgressChanged(int progress);

private:
    Ui::MainWindow *ui;
    WT_ProjectWidget* m_ProjectWidget; // [修改] 变量名更新
    DataEditorWidget* m_DataEditorWidget;
    ModelManager* m_ModelManager;
    PlottingWidget* m_PlottingWidget;
    FittingPage* m_FittingPage;
    SettingsWidget* m_SettingsWidget;
    QMap<QString,NavBtn*>::Iterator item; // [修正] Iterator 成员需要移除或局部化，这里保留NavBtnMap即可
    QMap<QString,NavBtn*> m_NavBtnMap;
    QTimer m_timer;
    bool m_hasValidData = false;

    // 是否已加载项目（新建或打开）
    bool m_isProjectLoaded = false;

    void transferDataFromEditorToPlotting();
    void updateNavigationState();
    void transferDataToFitting();

    QStandardItemModel* getDataEditorModel() const;
    QString getCurrentFileName() const;
    bool hasDataLoaded();
    WellTestData createDemoWellTestData();
};

#endif // MAINWINDOW_H
