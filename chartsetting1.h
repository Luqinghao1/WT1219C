#ifndef CHARTSETTING1_H
#define CHARTSETTING1_H

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include "qcustomplot.h"

/**
 * @brief 图表设置对话框 (重构版)
 * 使用 QTableWidget 布局，统一风格。
 */
class ChartSetting1 : public QDialog {
    Q_OBJECT
public:
    explicit ChartSetting1(QCustomPlot* plot, QCPTextElement* titleElement, QWidget* parent = nullptr);

private slots:
    void onPreview();
    void onAccept();
    void onReject();

private:
    void applySettingsToPlot();
    void restoreStateToPlot();

    // 辅助：向表格添加一行设置项
    void addSettingRow(int row, const QString& label, QWidget* editor);

    QCustomPlot* m_plot;
    QCPTextElement* m_titleElement;

    // UI 控件
    QTableWidget* m_table; // [新增] 主表格
    QLineEdit* m_editTitle;
    QSpinBox* m_spinTitleFont;
    QLineEdit* m_editXLabel;
    QLineEdit* m_editYLabel;
    QSpinBox* m_spinLabelFont;
    QSpinBox* m_spinTickFont;
    QCheckBox* m_chkScientific;
    QCheckBox* m_chkGrid;

    struct State {
        QString titleText;
        int titleFontSize;
        QString xLabel, yLabel;
        int labelFontSize, tickFontSize;
        bool isScientific;
        bool isGridVisible;
    } m_initialState;
};

#endif // CHARTSETTING1_H
