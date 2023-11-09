#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QAbstractButton>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();
    void initUI();
public slots:
    //QAbstractButton 是 Qt 中的一个抽象基类，用于表示按钮控件的通用属性和行为。这意味着 onButtonGroupClicked 函数可以处理派生自 QAbstractButton 的各种按钮控件，例如 QPushButton 或 QRadioButton。
    void onButtonGroupClicked(QAbstractButton* btn);
private:
    Ui::Calculator *ui;
};
#endif // CALCULATOR_H
