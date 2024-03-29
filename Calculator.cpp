#include "Calculator.h"
#include "./ui_Calculator.h"
#include <QButtonGroup>
#include <string>
#include <iostream>
#include <queue>
#include <stack>
#include <set>
#include <sstream>
#include <QMessageBox>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator())
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
        ui->lineEdit->del();    //删除光标右侧的字符
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
                size_t left_count = 0;
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
                                if(medialExp[x+j]=='.'){
                                    if(x+j-1 > 0)
                                    {
                                        if((medialExp[x+j-1] < '0' || medialExp[x+j-1] > '9'))
                                            throw "计算式有误";
                                    }
                                }
                                num.push_back(medialExp[x + j]);
                                i += 1;
                                j += 1;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if(num.length() == 1 && num == "."){
                            throw"运算表达式错误";
                        }
                        suffixExp.push(num);
                        i += 1;
                    }
                    //当前位是运算符或括号
                    else
                    {
                        if (medialExp[i] == '(')
                        {
                            left_count += 1;
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
                            if (left_count == 0){
                                throw "括号不匹配错误";
                            }
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
                            operatorStack.pop();    //(出栈
                            left_count--;
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
                    char& c = operatorStack.top();
                    if(c == '('){
                        left_count++;
                    }
                    else if(operatorStack.top() == ')'){
                        left_count--;
                    }
                    suffixExp.push(std::string(1, operatorStack.top()));
                    operatorStack.pop();
                }
                if(left_count!=0){
                    throw "括号不匹配错误";
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
/*
"""
    中缀表达式转后缀表达式：
    对于普通中缀表达式的计算，我们可以将其转化为后缀表达式再进行计算。转换方法也十分简单。只要建立一个用于存放运算符的栈，扫描该中缀表达式：
    如果遇到数字，直接将该数字输出到后缀表达式（以下部分用「输出」表示输出到后缀表达式）；
    如果遇到左括号，入栈；
    如果遇到右括号，不断输出栈顶元素，直至遇到左括号（左括号出栈，但不输出）；
    如果遇到其他运算符，不断去除所有运算优先级大于等于当前运算符的运算符，输出到后缀表达式。最后，新的符号入栈；
    把栈中剩下的符号依次弹出到后缀表达式，表达式转换结束
"""
"""
    如何处理负数：
    如果中缀表达式第一个符号是”-“，则这个符号是负号，在表达式最前面加一个0将其转化为减号
    如果左括号（后是”-“，则这个符号是负号，在”（“后面加一个0，将负号转化为减号，此时并不需要真的在中缀表达式中加一个0，只需要在将”（“入栈后在后缀表达式中加一个0
    除以上两种情况外”-“均为减号
"""
"""
    后缀表达式的计算：
    后缀表达式的计算就是从左到右扫描表达式，遇到数字就将其压入栈，遇到操作符表示可以计算，这时取出栈顶的两个元素进行操作，然后再次将结果压入栈，
    最后栈里会留下一个元素，该元素就是运行结果
"""
*/
