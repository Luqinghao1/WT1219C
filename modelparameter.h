#ifndef MODELPARAMETER_H
#define MODELPARAMETER_H

#include <QString>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMutex>

// 项目参数单例类，用于在不同模块间共享项目基础信息及读写项目文件
class ModelParameter : public QObject
{
    Q_OBJECT

public:
    static ModelParameter* instance();

    // 加载项目文件 (.wtproj)
    bool loadProject(const QString& filePath);

    // 设置参数（通常用于新建项目后立即更新）
    void setParameters(double phi, double h, double mu, double B, double Ct, double q, double rw, const QString& path);

    // 获取参数的接口
    double getPhi() const { return m_phi; }
    double getH() const { return m_h; }
    double getMu() const { return m_mu; }
    double getB() const { return m_B; }
    double getCt() const { return m_Ct; }
    double getQ() const { return m_q; }
    double getRw() const { return m_rw; }
    QString getProjectPath() const { return m_projectPath; }

    // [新增] 保存拟合结果到项目文件
    void saveFittingResult(const QJsonObject& fittingData);

    // [新增] 获取项目文件中存储的拟合结果
    QJsonObject getFittingResult() const;

    // 判断是否已加载有效项目
    bool hasLoadedProject() const { return m_hasLoaded; }

private:
    explicit ModelParameter(QObject* parent = nullptr);
    static ModelParameter* m_instance;

    bool m_hasLoaded;
    QString m_projectPath; // 项目所在目录路径
    QString m_projectFilePath; // 项目文件完整路径 (.wtproj)

    // 缓存完整的JSON对象，以便保存时不丢失其他信息
    QJsonObject m_fullProjectData;

    // 基础参数
    double m_phi; // 孔隙度
    double m_h;   // 厚度
    double m_mu;  // 粘度
    double m_B;   // 体积系数
    double m_Ct;  // 综合压缩系数
    double m_q;   // 产量
    double m_rw;  // 井筒半径
};

#endif // MODELPARAMETER_H
