#include "fittingparameterchart.h"
#include <QLabel>
#include <QHeaderView>
#include <cmath>
#include <QDebug>

FittingParameterChart::FittingParameterChart(QTableWidget* table, QObject *parent)
    : QObject(parent), m_table(table), m_modelManager(nullptr)
{
    // 初始化表格表头样式
    if (m_table) {
        m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
}

void FittingParameterChart::setModelManager(ModelManager *mgr)
{
    m_modelManager = mgr;
}

void FittingParameterChart::resetParams(ModelManager::ModelType type)
{
    if(!m_modelManager) return;

    // 获取 ModelManager 定义的默认值
    QMap<QString,double> defs = m_modelManager->getDefaultParameters(type);

    // 补充一些基础默认值，防止 ModelManager 中缺失
    if(!defs.contains("phi")) defs["phi"] = 0.05;
    if(!defs.contains("h")) defs["h"] = 20.0;
    if(!defs.contains("mu")) defs["mu"] = 0.5;
    if(!defs.contains("B")) defs["B"] = 1.05;
    if(!defs.contains("Ct")) defs["Ct"] = 5e-4;
    if(!defs.contains("q")) defs["q"] = 5.0;
    if(!defs.contains("nf")) defs["nf"] = 4.0;
    if(!defs.contains("gamaD")) defs["gamaD"] = 0.02;
    if(!defs.contains("reD")) defs["reD"] = 10.0;

    m_parameters.clear();
    QStringList orderedKeys = getParamOrder(type);

    for(const QString& key : orderedKeys) {
        FitParameter p;
        p.name = key;
        QString dummy1, dummy2, dummy3;
        // 获取参数的显示名称、符号、单位
        getParamDisplayInfo(key, p.displayName, p.symbol, dummy2, p.unit);

        p.value = defs.contains(key) ? defs[key] : 0.0;
        p.isFit = false;

        // 设置合理的上下限范围
        if (key == "kf" || key == "km") { p.min = 1e-6; p.max = 100.0; }
        else if (key == "L") { p.min = 10.0; p.max = 5000.0; }
        else if (key == "Lf") { p.min = 1.0; p.max = 1000.0; }
        else if (key == "rmD") { p.min = 1.0; p.max = 50.0; }
        else if (key == "omega1" || key == "omega2") { p.min = 0.001; p.max = 1.0; }
        else if (key == "lambda1") { p.min = 1e-9; p.max = 1.0; }
        else if (key == "cD") { p.min = 0.0; p.max = 5000.0; }
        else if (key == "S") { p.min = -5.0; p.max = 50.0; }
        else if (key == "gamaD") { p.min = 0.0; p.max = 1.0; }
        else if (key == "reD") { p.min = 1.1; p.max = 1000.0; } // reD 必须大于 1.0
        else if (key == "phi") { p.min = 0.001; p.max = 1.0; }
        else if (key == "h") { p.min = 1.0; p.max = 500.0; }
        else if (key == "mu") { p.min = 0.01; p.max = 1000.0; }
        else if (key == "B") { p.min = 0.5; p.max = 2.0; }
        else if (key == "Ct") { p.min = 1e-6; p.max = 1e-2; }
        else if (key == "q") { p.min = 0.1; p.max = 10000.0; }
        else if (key == "nf") { p.min = 1.0; p.max = 100.0; }
        else {
            // 通用逻辑
            if(p.value > 0) { p.min = p.value * 0.001; p.max = p.value * 1000.0; }
            else if (p.value == 0) { p.min = 0.0; p.max = 100.0; }
            else { p.min = -100.0; p.max = 100.0; }
        }
        m_parameters.append(p);
    }
    // 数据准备好后刷新表格
    loadParamsToTable();
}

void FittingParameterChart::switchModel(ModelManager::ModelType newType)
{
    // 1. 先确保 m_parameters 中存储的是当前界面上最新的值
    updateParamsFromTable();

    // 2. 将旧参数值暂存到 Map 中
    QMap<QString, double> oldValues;
    for(const auto& p : m_parameters) {
        oldValues.insert(p.name, p.value);
    }

    // 3. 重置参数列表为新模型的结构
    resetParams(newType);

    // 4. 遍历新参数列表，如果旧参数中有同名参数，则恢复旧值
    bool restoredAny = false;
    for(int i = 0; i < m_parameters.size(); ++i) {
        if(oldValues.contains(m_parameters[i].name)) {
            m_parameters[i].value = oldValues[m_parameters[i].name];
            restoredAny = true;
        }
    }

    // 5. 如果有参数被恢复，刷新表格
    if(restoredAny) {
        loadParamsToTable();
    }
}

void FittingParameterChart::loadParamsToTable()
{
    if (!m_table) return;

    m_table->setRowCount(0);
    m_table->blockSignals(true); // 阻止信号，防止在填充时触发变更事件
    for(int i=0; i<m_parameters.size(); ++i) {
        m_table->insertRow(i);
        QString htmlSym, uniSym, unitStr, dummyName;
        getParamDisplayInfo(m_parameters[i].name, dummyName, htmlSym, uniSym, unitStr);
        // 使用 HTML 格式显示带上下标的参数名
        QString nameStr = QString("<html>%1 (%2)</html>").arg(m_parameters[i].displayName).arg(htmlSym);

        QLabel* nameLabel = new QLabel(nameStr);
        nameLabel->setTextFormat(Qt::RichText);
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nameLabel->setContentsMargins(5, 0, 0, 0);
        m_table->setCellWidget(i, 0, nameLabel);

        // 存储参数的 key 到隐藏数据中，方便后续识别
        QTableWidgetItem* dummyItem = new QTableWidgetItem("");
        dummyItem->setData(Qt::UserRole, m_parameters[i].name);
        m_table->setItem(i, 0, dummyItem);

        // 值
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(m_parameters[i].value)));

        // 拟合勾选框
        QTableWidgetItem* chk = new QTableWidgetItem();
        chk->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        chk->setCheckState(m_parameters[i].isFit ? Qt::Checked : Qt::Unchecked);
        m_table->setItem(i, 2, chk);

        // 最小值
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(m_parameters[i].min)));
        // 最大值
        m_table->setItem(i, 4, new QTableWidgetItem(QString::number(m_parameters[i].max)));

        // 单位 (不可编辑)
        if(unitStr == "无因次" || unitStr == "小数") unitStr = "";
        QTableWidgetItem* unitItem = new QTableWidgetItem(unitStr);
        unitItem->setFlags(unitItem->flags() ^ Qt::ItemIsEditable);
        m_table->setItem(i, 5, unitItem);
    }
    m_table->blockSignals(false);
}

void FittingParameterChart::updateParamsFromTable()
{
    if (!m_table) return;

    for(int i=0; i<m_table->rowCount(); ++i) {
        if(i < m_parameters.size()) {
            QString key = m_table->item(i,0)->data(Qt::UserRole).toString();
            // 确保行对应正确的参数
            if(m_parameters[i].name == key) {
                m_parameters[i].value = m_table->item(i,1)->text().toDouble();
                m_parameters[i].isFit = (m_table->item(i,2)->checkState() == Qt::Checked);
                m_parameters[i].min = m_table->item(i,3)->text().toDouble();
                m_parameters[i].max = m_table->item(i,4)->text().toDouble();
            }
        }
    }
}

QList<FitParameter> FittingParameterChart::getParameters() const
{
    // 注意：外部调用前最好确保先调用了 updateParamsFromTable，这里直接返回内部缓存
    return m_parameters;
}

void FittingParameterChart::setParameters(const QList<FitParameter>& params)
{
    m_parameters = params;
    loadParamsToTable();
}

void FittingParameterChart::getParamDisplayInfo(const QString& key, QString& outName, QString& outSymbol, QString& outUnicodeSymbol, QString& outUnit)
{
    QString unitMd = "mD"; QString unitM = "m"; QString unitDimless = "无因次";
    QString unitM3D = "m³/d"; QString unitVis = "mPa·s"; QString unitComp = "MPa⁻¹";
    outName = key; outSymbol = key; outUnicodeSymbol = key; outUnit = "";

    if (key == "kf") { outName = "内区渗透率"; outSymbol = "k<sub>f</sub>"; outUnicodeSymbol = "k_f"; outUnit = unitMd; }
    else if (key == "km") { outName = "外区渗透率"; outSymbol = "k<sub>m</sub>"; outUnicodeSymbol = "kₘ"; outUnit = unitMd; }
    else if (key == "L") { outName = "水平井长度"; outSymbol = "L"; outUnicodeSymbol = "L"; outUnit = unitM; }
    else if (key == "Lf") { outName = "裂缝半长"; outSymbol = "L<sub>f</sub>"; outUnicodeSymbol = "L_f"; outUnit = unitM; }
    else if (key == "rmD") { outName = "复合半径"; outSymbol = "r<sub>mD</sub>"; outUnicodeSymbol = "rₘᴅ"; outUnit = unitDimless; }
    else if (key == "omega1") { outName = "内区储容比"; outSymbol = "ω<sub>1</sub>"; outUnicodeSymbol = "ω₁"; outUnit = unitDimless; }
    else if (key == "omega2") { outName = "外区储容比"; outSymbol = "ω<sub>2</sub>"; outUnicodeSymbol = "ω₂"; outUnit = unitDimless; }
    else if (key == "lambda1") { outName = "窜流系数"; outSymbol = "λ<sub>1</sub>"; outUnicodeSymbol = "λ₁"; outUnit = unitDimless; }
    else if (key == "cD") { outName = "井筒储存"; outSymbol = "C<sub>D</sub>"; outUnicodeSymbol = "Cᴅ"; outUnit = unitDimless; }
    else if (key == "S") { outName = "表皮系数"; outSymbol = "S"; outUnicodeSymbol = "S"; outUnit = unitDimless; }
    else if (key == "gamaD") { outName = "压敏系数"; outSymbol = "γ<sub>D</sub>"; outUnicodeSymbol = "γᴅ"; outUnit = unitDimless; }
    else if (key == "reD") { outName = "外边界半径"; outSymbol = "r<sub>eD</sub>"; outUnicodeSymbol = "reD"; outUnit = unitDimless; }

    else if (key == "phi") { outName = "孔隙度"; outSymbol = "φ"; outUnicodeSymbol = "φ"; outUnit = "小数"; }
    else if (key == "h") { outName = "厚度"; outSymbol = "h"; outUnicodeSymbol = "h"; outUnit = unitM; }
    else if (key == "mu") { outName = "粘度"; outSymbol = "μ"; outUnicodeSymbol = "μ"; outUnit = unitVis; }
    else if (key == "B") { outName = "体积系数"; outSymbol = "B"; outUnicodeSymbol = "B"; outUnit = ""; }
    else if (key == "Ct") { outName = "综合压缩系数"; outSymbol = "C<sub>t</sub>"; outUnicodeSymbol = "Cₜ"; outUnit = unitComp; }
    else if (key == "q") { outName = "产量"; outSymbol = "q"; outUnicodeSymbol = "q"; outUnit = unitM3D; }
    else if (key == "nf") { outName = "裂缝条数"; outSymbol = "n<sub>f</sub>"; outUnicodeSymbol = "n_f"; outUnit = unitDimless; }
}

QStringList FittingParameterChart::getParamOrder(ModelManager::ModelType type)
{
    QStringList order;
    // 基础参数 (所有模型通用)
    order << "phi" << "h" << "mu" << "B" << "Ct" << "q" << "nf";

    // 模型 1 (Infinite, Changing): 变井储，有 cD, S
    if (type == ModelManager::Model_1) {
        order << "kf" << "km" << "L" << "Lf" << "rmD" << "omega1" << "omega2" << "lambda1" << "gamaD" << "cD" << "S";
    }
    // 模型 2 (Infinite, Constant): 恒定井储，无 cD, S
    else if (type == ModelManager::Model_2) {
        order << "kf" << "km" << "L" << "Lf" << "rmD" << "omega1" << "omega2" << "lambda1" << "gamaD";
    }
    // 模型 3 (Closed, Changing): 封闭边界+变井储，有 cD, S, reD
    else if (type == ModelManager::Model_3) {
        order << "kf" << "km" << "L" << "Lf" << "rmD" << "omega1" << "omega2" << "lambda1" << "gamaD" << "reD" << "cD" << "S";
    }
    // 模型 4 (Closed, Constant): 封闭边界+恒定井储，有 reD, 无 cD, S
    else if (type == ModelManager::Model_4) {
        order << "kf" << "km" << "L" << "Lf" << "rmD" << "omega1" << "omega2" << "lambda1" << "gamaD" << "reD";
    }
    // 模型 5 (ConstPressure, Changing): 定压边界+变井储，有 cD, S, reD
    else if (type == ModelManager::Model_5) {
        order << "kf" << "km" << "L" << "Lf" << "rmD" << "omega1" << "omega2" << "lambda1" << "gamaD" << "reD" << "cD" << "S";
    }
    // 模型 6 (ConstPressure, Constant): 定压边界+恒定井储，有 reD, 无 cD, S
    else if (type == ModelManager::Model_6) {
        order << "kf" << "km" << "L" << "Lf" << "rmD" << "omega1" << "omega2" << "lambda1" << "gamaD" << "reD";
    }
    else {
        // 默认 fallback
        order << "kf" << "km" << "L" << "Lf" << "rmD" << "omega1" << "omega2" << "lambda1" << "cD" << "S";
    }

    return order;
}
