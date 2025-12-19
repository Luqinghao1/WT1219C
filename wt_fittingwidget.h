#ifndef WT_FITTINGWIDGET_H
#define WT_FITTINGWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QFutureWatcher>
#include <QJsonObject>
#include "modelmanager.h"
#include "mousezoom.h"
#include "chartsetting1.h"

// 引入新拆分的模块头文件
#include "fittingparameterchart.h"
#include "fittingobserveddata.h"

namespace Ui { class FittingWidget; }

class FittingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FittingWidget(QWidget *parent = nullptr);
    ~FittingWidget();

    void setModelManager(ModelManager* m);
    void setObservedData(const QVector<double>& t, const QVector<double>& p, const QVector<double>& d);

    // 基础参数更新接口
    void updateBasicParameters();

    // 从 JSON 数据加载拟合状态
    void loadFittingState(const QJsonObject& data = QJsonObject());

    // 获取当前拟合状态的 JSON 对象（用于保存）
    QJsonObject getJsonState() const;

signals:
    void fittingCompleted(ModelManager::ModelType modelType, const QMap<QString, double>& parameters);
    void sigIterationUpdated(double error, QMap<QString, double> currentParams, QVector<double> t, QVector<double> p, QVector<double> d);
    void sigProgress(int progress);
    void sigRequestSave();

private slots:
    void on_btnLoadData_clicked();
    void on_btnRunFit_clicked();
    void on_btnStop_clicked();
    void on_btnImportModel_clicked(); // 界面上的"刷新曲线"按钮
    void on_btnExportData_clicked();
    void on_btnExportChart_clicked();
    void on_btnResetParams_clicked();
    void on_btnResetView_clicked();
    void on_btnChartSettings_clicked();
    void on_btn_modelSelect_clicked();

    void on_btnSaveFit_clicked();
    void on_btnExportReport_clicked();

    void onIterationUpdate(double err, const QMap<QString,double>& p, const QVector<double>& t, const QVector<double>& p_curve, const QVector<double>& d_curve);
    void onFitFinished();

    // 滑块值改变槽函数
    void onSliderWeightChanged(int value);

private:
    Ui::FittingWidget *ui;
    ModelManager* m_modelManager;
    MouseZoom* m_plot;
    QCPTextElement* m_plotTitle;
    ModelManager::ModelType m_currentModelType;

    // --- 新增模块实例 ---
    FittingParameterChart* m_paramChart; // 负责参数表格管理
    FittingObservedData* m_dataLoader;   // 负责数据加载与处理

    // 本地缓存的观测数据
    QVector<double> m_obsTime;
    QVector<double> m_obsPressure;
    QVector<double> m_obsDerivative;

    bool m_isFitting;
    bool m_stopRequested;
    QFutureWatcher<void> m_watcher;

    void setupPlot();
    void initializeDefaultModel();
    void updateModelCurve();

    // 优化算法相关函数
    void runOptimizationTask(ModelManager::ModelType modelType, QList<FitParameter> fitParams, double weight);
    void runLevenbergMarquardtOptimization(ModelManager::ModelType modelType, QList<FitParameter> params, double weight);

    QVector<double> calculateResiduals(const QMap<QString, double>& params, ModelManager::ModelType modelType, double weight);
    QVector<QVector<double>> computeJacobian(const QMap<QString, double>& params, const QVector<double>& residuals, const QVector<int>& fitIndices, ModelManager::ModelType modelType, const QList<FitParameter>& currentFitParams, double weight);
    QVector<double> solveLinearSystem(const QVector<QVector<double>>& A, const QVector<double>& b);
    double calculateSumSquaredError(const QVector<double>& residuals);

    QString getPlotImageBase64();
    void plotCurves(const QVector<double>& t, const QVector<double>& p, const QVector<double>& d, bool isModel);
};

#endif // WT_FITTINGWIDGET_H
