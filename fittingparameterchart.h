#ifndef FITTINGPARAMETERCHART_H
#define FITTINGPARAMETERCHART_H

#include <QObject>
#include <QTableWidget>
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "modelmanager.h" // 包含 ModelManager::ModelType 定义

// 参数结构体 (从 fittingwidget.h 移动至此)
struct FitParameter {
    QString name;        // 参数内部名称 (如 kf)
    QString displayName; // 显示名称 (如 内区渗透率)
    QString symbol;      // HTML 符号 (用于显示)
    QString unit;        // 单位
    double value;        // 当前值
    bool isFit;          // 是否参与拟合
    double min;          // 最小值
    double max;          // 最大值
};

/**
 * @brief FittingParameterChart 类
 * 负责管理拟合参数列表、参数默认值初始化以及与 UI 表格的交互。
 */
class FittingParameterChart : public QObject
{
    Q_OBJECT
public:
    explicit FittingParameterChart(QTableWidget* table, QObject *parent = nullptr);

    // 设置模型管理器 (用于获取默认参数)
    void setModelManager(ModelManager* mgr);

    // 重置参数为默认值 (根据模型类型)
    void resetParams(ModelManager::ModelType type);

    // 切换模型类型 (尝试保留同名参数的旧值)
    void switchModel(ModelManager::ModelType newType);

    // 将参数加载到 UI 表格中
    void loadParamsToTable();

    // 从 UI 表格更新参数值到 m_parameters
    void updateParamsFromTable();

    // 获取当前参数列表
    QList<FitParameter> getParameters() const;

    // 设置参数列表 (用于加载状态)
    void setParameters(const QList<FitParameter>& params);

    // 获取参数的显示信息 (静态辅助函数，供外部如导出报告时使用)
    static void getParamDisplayInfo(const QString& key, QString& outName, QString& outSymbol, QString& outUnicodeSymbol, QString& outUnit);

private:
    QTableWidget* m_table;         // 指向 UI 上的参数表格控件
    ModelManager* m_modelManager;  // 模型管理器指针
    QList<FitParameter> m_parameters; // 参数列表数据

    // 获取特定模型的参数顺序
    QStringList getParamOrder(ModelManager::ModelType type);
};

#endif // FITTINGPARAMETERCHART_H
