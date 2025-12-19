#include "chartsetting1.h"
#include "mousezoom.h" // 引入 MouseZoom 以使用通用菜单功能
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>

ChartSetting1::ChartSetting1(QCustomPlot* plot, QCPTextElement* titleElement, QWidget* parent)
    : QDialog(parent), m_plot(plot), m_titleElement(titleElement)
{
    setWindowTitle("图表显示设置");
    resize(450, 400); //稍微调大一点以适应表格

    // 1. 备份初始状态 (保持不变)
    if (m_titleElement) {
        m_initialState.titleText = m_titleElement->text();
        m_initialState.titleFontSize = m_titleElement->font().pointSize();
    } else {
        m_initialState.titleText = "";
        m_initialState.titleFontSize = 12;
    }
    m_initialState.xLabel = m_plot->xAxis->label();
    m_initialState.yLabel = m_plot->yAxis->label();
    m_initialState.labelFontSize = m_plot->xAxis->labelFont().pointSize();
    m_initialState.tickFontSize = m_plot->xAxis->tickLabelFont().pointSize();
    m_initialState.isScientific = (m_plot->xAxis->numberFormat().contains("e"));
    m_initialState.isGridVisible = m_plot->xAxis->grid()->visible();

    // 2. 构建界面 (改为 TableWidget)
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_table = new QTableWidget(8, 2, this);
    m_table->setHorizontalHeaderLabels(QStringList() << "属性" << "设置值");
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionMode(QAbstractItemView::NoSelection); // 控件交互为主，不选行

    // 调用 MouseZoom 的通用方法添加右键菜单
    MouseZoom::addTableContextMenu(m_table);

    // 初始化控件
    m_editTitle = new QLineEdit(m_initialState.titleText, this);
    m_spinTitleFont = new QSpinBox(this); m_spinTitleFont->setRange(8, 72); m_spinTitleFont->setValue(m_initialState.titleFontSize);
    m_editXLabel = new QLineEdit(m_initialState.xLabel, this);
    m_editYLabel = new QLineEdit(m_initialState.yLabel, this);
    m_spinLabelFont = new QSpinBox(this); m_spinLabelFont->setRange(6, 48); m_spinLabelFont->setValue(m_initialState.labelFontSize);
    m_spinTickFont = new QSpinBox(this); m_spinTickFont->setRange(6, 48); m_spinTickFont->setValue(m_initialState.tickFontSize);
    m_chkScientific = new QCheckBox("启用", this); m_chkScientific->setChecked(m_initialState.isScientific);
    m_chkGrid = new QCheckBox("显示", this); m_chkGrid->setChecked(m_initialState.isGridVisible);

    // 填入表格
    addSettingRow(0, "图表标题", m_editTitle);
    addSettingRow(1, "标题字号", m_spinTitleFont);
    addSettingRow(2, "X轴 标题", m_editXLabel);
    addSettingRow(3, "Y轴 标题", m_editYLabel);
    addSettingRow(4, "轴标题字号", m_spinLabelFont);
    addSettingRow(5, "刻度字号", m_spinTickFont);
    addSettingRow(6, "科学计数法", m_chkScientific);
    addSettingRow(7, "网格线", m_chkGrid);

    mainLayout->addWidget(m_table);

    // 按钮布局 (位于表格下方)
    QHBoxLayout* btnLayout = new QHBoxLayout;
    QPushButton* btnPreview = new QPushButton("预览", this);
    QPushButton* btnOk = new QPushButton("确定", this);
    QPushButton* btnCancel = new QPushButton("取消", this);
    btnOk->setDefault(true);
    btnLayout->addStretch();
    btnLayout->addWidget(btnPreview); btnLayout->addWidget(btnOk); btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    connect(btnPreview, &QPushButton::clicked, this, &ChartSetting1::onPreview);
    connect(btnOk, &QPushButton::clicked, this, &ChartSetting1::onAccept);
    connect(btnCancel, &QPushButton::clicked, this, &ChartSetting1::onReject);
}

void ChartSetting1::addSettingRow(int row, const QString& label, QWidget* editor) {
    QTableWidgetItem* item = new QTableWidgetItem(label);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable); // 只读
    m_table->setItem(row, 0, item);

    // 将控件放入单元格
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->addWidget(editor);
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_table->setCellWidget(row, 1, container);
}

void ChartSetting1::onPreview() { applySettingsToPlot(); }
void ChartSetting1::onAccept() { applySettingsToPlot(); accept(); }
void ChartSetting1::onReject() { restoreStateToPlot(); reject(); }

void ChartSetting1::applySettingsToPlot() {
    if(!m_plot) return;

    if(m_titleElement) {
        m_titleElement->setText(m_editTitle->text());
        QFont f = m_titleElement->font(); f.setPointSize(m_spinTitleFont->value());
        m_titleElement->setFont(f);
    }
    m_plot->xAxis->setLabel(m_editXLabel->text());
    m_plot->yAxis->setLabel(m_editYLabel->text());

    QFont lf = m_plot->xAxis->labelFont(); lf.setPointSize(m_spinLabelFont->value());
    m_plot->xAxis->setLabelFont(lf); m_plot->yAxis->setLabelFont(lf);

    QFont tf = m_plot->xAxis->tickLabelFont(); tf.setPointSize(m_spinTickFont->value());
    m_plot->xAxis->setTickLabelFont(tf); m_plot->yAxis->setTickLabelFont(tf);

    QString fmt = m_chkScientific->isChecked() ? "eb" : "g";
    m_plot->xAxis->setNumberFormat(fmt); m_plot->yAxis->setNumberFormat(fmt);
    m_plot->xAxis2->setNumberFormat(fmt); m_plot->yAxis2->setNumberFormat(fmt);

    m_plot->xAxis->grid()->setVisible(m_chkGrid->isChecked());
    m_plot->yAxis->grid()->setVisible(m_chkGrid->isChecked());

    m_plot->replot();
}

void ChartSetting1::restoreStateToPlot() {
    if(!m_plot) return;
    if(m_titleElement) {
        m_titleElement->setText(m_initialState.titleText);
        QFont f = m_titleElement->font(); f.setPointSize(m_initialState.titleFontSize);
        m_titleElement->setFont(f);
    }
    m_plot->xAxis->setLabel(m_initialState.xLabel);
    m_plot->yAxis->setLabel(m_initialState.yLabel);
    // ... (rest same as before)
    QFont lf = m_plot->xAxis->labelFont(); lf.setPointSize(m_initialState.labelFontSize);
    m_plot->xAxis->setLabelFont(lf); m_plot->yAxis->setLabelFont(lf);
    QFont tf = m_plot->xAxis->tickLabelFont(); tf.setPointSize(m_initialState.tickFontSize);
    m_plot->xAxis->setTickLabelFont(tf); m_plot->yAxis->setTickLabelFont(tf);
    QString fmt = m_initialState.isScientific ? "eb" : "g";
    m_plot->xAxis->setNumberFormat(fmt); m_plot->yAxis->setNumberFormat(fmt);
    m_plot->xAxis2->setNumberFormat(fmt); m_plot->yAxis2->setNumberFormat(fmt);
    m_plot->xAxis->grid()->setVisible(m_initialState.isGridVisible);
    m_plot->yAxis->grid()->setVisible(m_initialState.isGridVisible);
    m_plot->replot();
}
