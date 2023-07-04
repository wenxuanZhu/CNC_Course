#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QDebug>
#include<QPainter>          //调用画家类
#include<math.h>            //调用数学幂函数
#include<QPen>              //调用画笔
#include<QTextEdit>         //调用文本显示框
#include<QVector>           //调用容器

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //声明绘图画板事件函数
    void paintEvent(QPaintEvent *);

    //定义非圆曲线的起点坐标和终点坐标
    double xStart, dxMove;
    //定义抛物线的参数p
    int p;
    //定义椭圆半长轴的参数a
    int a;
    //定义椭圆半短轴的参数b
    int b;

    //定义抛物线中出现的误差圆圆心坐标、实际显示在窗口的圆心坐标和节点坐标
    int xo, yo, xDisplay, yDisplay, xDisplay1, yDisplay1,
        xDisplay_1, yDisplay_1, xDisplay1_1, yDisplay1_1, r;
    double xm, ym;
    //定义抛物线切线联立误差圆的一元四次方程参数
    double a4, b4, c4, d4, e4;
    //定义抛物线切线联立误差圆的一元四次降阶三次方程参数以及三次方程的一个解
    double a3, b3, c3, d3, p3, q3, y3, y3_1, y3_2;
    //定义抛物线切线联立误差圆的一元三次降阶二次方程参数以及他的四个解
    double a2_1, b2_1, c2_1, triangle1;
    double a2_2, b2_2, c2_2, triangle2;
    double x2_1_1, x2_1_2, x2_2_1, x2_2_2;

    //比较四个解出来的根的大小的函数声明：
    int compare(int a, int b, int c, int d);

     QPolygon polygon1;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
