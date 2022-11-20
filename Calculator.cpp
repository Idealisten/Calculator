#include "Calculator.h"
#include <QButtonGroup>
#include <String>
#include <iostream>
#include <queue>
#include <stack>
#include <set>
#include <sstream>
#include <QMessageBox>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalculatorClass())
{
    ui->setupUi(this);
    initUI();
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::initUI()
{
    //1.找到所有按钮，并把所有的按钮放到一个按钮组里，批量处理点击事件
    QButtonGroup* buttonGroup = new QButtonGroup();
    auto btnList = findChildren<QPushButton*>();
    for (auto btn : btnList)
    {
        buttonGroup->addButton(btn);
    }
    //2.关联按钮组的点击信号
    connect(buttonGroup, &QButtonGroup::buttonClicked, this, &Calculator::onButtonGroupClicked);
}

void Calculator::onButtonGroupClicked(QAbstractButton* btn)
{
    //输出到控制台，项目-属性-链接器-系统-子系统-控制台/窗口
    qInfo() << btn->text();
    //根据不同的按钮点击处理不同的逻辑
    //将按钮的上的数字、小数点、运算符直接显示在lineEdit
    QString name = btn->text();
    if (name != "DEL" && name != "C" && name != "=")
    {
        ui->lineEdit->insert(btn->text());
    }
    else if (name == "C")
    {
        ui->lineEdit->clear();
    }
    else if (name == "DEL")
    {
        ui->lineEdit->setCursorPosition(ui->lineEdit->cursorPosition() - 1);
        ui->lineEdit->del();
    }
    else//按下等号，执行计算  
    {
        //获取lineEdit的文本
        std::string medialExp;
        QString qsExp = ui->lineEdit->text();
        medialExp = qsExp.toStdString();
        std::queue<std::string> suffixExp;  //声明后缀表达式队列
        std::stack<char> operatorStack; //符号栈
        std::stack<double> numStack;  //数字栈
        const QString title = "错误";
        const QString text = "计算式输入有误！请重新输入";
        std::set<char> ops = { '+', '-', '*', '/' };
        double a, b;
        bool bug = false;
        std::cout << medialExp << std::endl;
        for (auto x : medialExp)
        {
            std::cout << x;
        }
        if (medialExp.length() > 0)
        {   
            try
            {
                //转为后缀表达式
                //如果表达式第一个数字是负数，在表达式前面加0
                if (medialExp[0] == '-')
                {
                    medialExp = '0' + medialExp;
                }
                int lens = medialExp.length();  //中缀表达式的长度
                //逐位循环，判断当前位是运算符还是数字
                //如果是数字，继续向后循环判断下一位是不是数字或小数点，直到不是数字，将每一位数字（0~9）合并为一个数字，并记录下当前数字的长度，下次循环直接跳过当前循环过的数字
                size_t i = 0;
                while (i < lens)
                {
                    //当前位是数字
                    if ((medialExp[i] >= '0' && medialExp[i] <= '9') || medialExp[i] == '.')
                    {
                        unsigned short int j = 1; //试探最大的数字位数，用起始下标加j可得到当前下标，用来判断当前位是否为数字或小数点
                        unsigned short int x = i; //用x记录当前一轮循环的起始下标
                        std::string num;
                        num.push_back(medialExp[i]);
                        while (j < lens - x)
                        {
                            if ((medialExp[x + j] >= '0' && medialExp[x + j] <= '9') || medialExp[x + j] == '.')
                            {
                                num.push_back(medialExp[x + j]);
                                i += 1;
                                j += 1;
                            }
                            else
                            {
                                break;
                            }
                        }
                        suffixExp.push(num);
                        i += 1;
                    }
                    //当前位是运算符或括号
                    else
                    {
                        if (medialExp[i] == '(')
                        {
                            operatorStack.push(medialExp[i]);
                            if (i + 1 >= lens)
                            {
                                throw "计算式错误";
                            }
                            if (medialExp[i + 1] == '-')
                            {
                                suffixExp.push("0");
                            }
                        }
                        else if (medialExp[i] == ')')
                        {

                            if (operatorStack.empty())
                            {
                                throw "计算式有误";
                            }
                            while (operatorStack.top() != '(')
                            {
                                if (operatorStack.empty())
                                {
                                    throw "计算式有误";
                                }
                                char op = operatorStack.top();
                                std::string opS;
                                opS.push_back(op);
                                suffixExp.push(opS);
                                operatorStack.pop();
                                if (operatorStack.empty())
                                {
                                    throw "计算式错误";
                                }
                            }


                            if (operatorStack.empty())
                            {
                                throw "计算式有误";
                            }
                            operatorStack.pop();

                        }
                        else  //当前索引值的符号是“+-*/”
                        {
                            if (operatorStack.empty())//符号栈此时为空，直接入栈
                            {
                                operatorStack.push(medialExp[i]);
                            }
                            else//符号栈此时非空
                            {
                                if (medialExp[i] == '*' or medialExp[i] == '/')//中缀表达式当前索引值为*或/
                                {
                                    while (!operatorStack.empty())
                                    {
                                        if (operatorStack.top() == '*' || operatorStack.top() == '/')
                                        {
                                            suffixExp.push(std::string(1, operatorStack.top()));
                                            operatorStack.pop();
                                        }
                                        else {
                                            break;
                                        }
                                    }
                                    operatorStack.push(medialExp[i]);//将当前运算符入栈
                                }
                                else//中缀表达式当前索引值为+或-
                                {

                                    while (!operatorStack.empty())
                                    {
                                        if (ops.find(operatorStack.top()) != ops.end())
                                        {
                                            suffixExp.push(std::string(1, operatorStack.top()));
                                            operatorStack.pop();
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }
                                    operatorStack.push(medialExp[i]);//将当前运算符入栈
                                }
                            }
                        }
                        i += 1;
                    }

                }
                //将运算符栈中剩下的符号加入后缀表达式

                if (operatorStack.empty())
                {
                    throw "计算式有误！";
                }
                while (!operatorStack.empty())
                {
                    suffixExp.push(std::string(1, operatorStack.top()));
                    operatorStack.pop();
                }
                //进行计算，如果表达式不合法弹出对话框提示
                while (!suffixExp.empty())
                {
                    char item = suffixExp.front().c_str()[0];
                    if (ops.find(item) == ops.end())//如果是数字则入数字栈
                    {

                        numStack.push(std::stod(suffixExp.front()));


                    }
                    else
                    {

                        if (numStack.empty())
                        {
                            throw "计算式有误";
                        }
                        b = numStack.top();
                        numStack.pop();
                        if (numStack.empty())
                        {
                            throw "计算式有误";
                        }
                        a = numStack.top();
                        numStack.pop();

                        if (item == '+')
                        {
                            numStack.push(a + b);
                        }
                        else if (item == '-')
                        {
                            numStack.push(a - b);
                        }
                        else if (item == '*')
                        {
                            numStack.push(a * b);
                        }
                        else if(item == '/')
                        {

                            if (b == 0)
                            {
                                throw 0;
                            }
                            numStack.push(a / b);

                        }
                        else
                        {
                            throw "计算式有误";
                        }
                    }
                    suffixExp.pop();
                }
                if (!bug)
                {
                    //删除当前lineEdit内容，显示计算结果
                    qInfo() << QString::fromStdString(std::to_string(numStack.top()));
                    ui->lineEdit->clear();
                    std::ostringstream oss;
                    oss << numStack.top();
                    ui->lineEdit->setText(QString::fromStdString(oss.str()));
                    //ui->lineEdit->insert("=");
                    //ui->lineEdit->insert(QString::fromStdString(std::to_string(numStack.top())));
                    numStack.pop();
                }
            }
            catch (const char*)
            {
                QMessageBox::critical(this, title, text, QMessageBox::Ok);
            }
            catch (int)
            {
                const QString text = "0不能做除数！";
                QMessageBox::critical(this, title, text, QMessageBox::Ok);
            }
        }
        else
        {
            //没有输入任何算式
        }   
    }
}