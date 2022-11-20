#pragma once

#include <QtWidgets/QWidget>
#include "ui_Calculator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CalculatorClass; };
QT_END_NAMESPACE

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();
    void initUI();
public slots:
    void onButtonGroupClicked(QAbstractButton* btn);
private:
    Ui::CalculatorClass *ui;
};
