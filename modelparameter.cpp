#include "modelparameter.h"
#include <QFile>
#include <QJsonDocument>
#include <QFileInfo>
#include <QDebug>

ModelParameter* ModelParameter::m_instance = nullptr;

ModelParameter::ModelParameter(QObject* parent) : QObject(parent), m_hasLoaded(false)
{
    // 初始化默认值
    m_phi = 0.05;
    m_h = 20.0;
    m_mu = 0.5;
    m_B = 1.05;
    m_Ct = 5e-4;
    m_q = 50.0;
    m_rw = 0.1;
    m_projectPath = "";
}

ModelParameter* ModelParameter::instance()
{
    if (!m_instance) {
        m_instance = new ModelParameter();
    }
    return m_instance;
}

void ModelParameter::setParameters(double phi, double h, double mu, double B, double Ct, double q, double rw, const QString& path)
{
    m_phi = phi;
    m_h = h;
    m_mu = mu;
    m_B = B;
    m_Ct = Ct;
    m_q = q;
    m_rw = rw;

    m_projectFilePath = path; // 保存完整文件路径

    // 提取目录路径
    QFileInfo fi(path);
    if (fi.isFile()) {
        m_projectPath = fi.absolutePath();
    } else {
        m_projectPath = path;
    }

    m_hasLoaded = true;

    // 如果是新建项目，初始化一个空的JSON结构
    if (m_fullProjectData.isEmpty()) {
        QJsonObject reservoir;
        reservoir["porosity"] = m_phi;
        reservoir["thickness"] = m_h;
        reservoir["wellRadius"] = m_rw;
        reservoir["productionRate"] = m_q;

        QJsonObject pvt;
        pvt["viscosity"] = m_mu;
        pvt["volumeFactor"] = m_B;
        pvt["compressibility"] = m_Ct;

        m_fullProjectData["reservoir"] = reservoir;
        m_fullProjectData["pvt"] = pvt;
    }
}

bool ModelParameter::loadProject(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开项目文件:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "项目文件格式错误";
        return false;
    }

    m_fullProjectData = doc.object(); // 缓存整个JSON

    QJsonObject reservoir = m_fullProjectData["reservoir"].toObject();
    QJsonObject pvt = m_fullProjectData["pvt"].toObject();

    // 读取参数
    m_q = reservoir["productionRate"].toDouble(50.0);
    m_phi = reservoir["porosity"].toDouble(0.05);
    m_h = reservoir["thickness"].toDouble(20.0);
    m_rw = reservoir["wellRadius"].toDouble(0.1);

    m_Ct = pvt["compressibility"].toDouble(5e-4);
    m_mu = pvt["viscosity"].toDouble(0.5);
    m_B = pvt["volumeFactor"].toDouble(1.05);

    // 保存项目路径
    m_projectFilePath = filePath;
    QFileInfo fi(filePath);
    m_projectPath = fi.absolutePath();

    m_hasLoaded = true;
    qDebug() << "项目参数加载成功, 路径:" << m_projectPath;
    return true;
}

// [新增] 保存拟合结果
void ModelParameter::saveFittingResult(const QJsonObject& fittingData)
{
    if (m_projectFilePath.isEmpty()) return;

    // 更新内存中的 fitting 字段
    m_fullProjectData["fitting"] = fittingData;

    // 写回文件
    QFile file(m_projectFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(m_fullProjectData);
        file.write(doc.toJson());
        file.close();
        qDebug() << "拟合结果已保存到:" << m_projectFilePath;
    } else {
        qDebug() << "保存拟合结果失败，无法写入文件";
    }
}

// [新增] 获取拟合结果
QJsonObject ModelParameter::getFittingResult() const
{
    if (m_fullProjectData.contains("fitting")) {
        return m_fullProjectData["fitting"].toObject();
    }
    return QJsonObject();
}
