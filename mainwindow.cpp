#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->startPaint, &QPushButton::clicked, this, [=](){
        //把抛物线的参数传给变量p
        p = ui->Parameter_p->text().toInt();
        //qDebug()<<p;
        //把椭圆半长轴的参数传给变量a
        a = ui->Parameter_a->text().toInt();
        //把椭圆半短轴的参数传给变量b
        b = ui->Parameter_b->text().toInt();
    });

    connect(ui->Display, &QPushButton::clicked, this, [=](){
        //清空归零
        ui->NCDisplay->clear();
        //设置NCDispaly这个文本框不要自动换行
        ui->NCDisplay->setWordWrapMode(QTextOption::NoWrap);
        ui->NCDisplay->append("N1 G54 G90 M03 M08 S10000 F200");
        ui->NCDisplay->append("N2 G00 G42 D01 X0 Y0");
        ui->NCDisplay->insertPlainText("\n");

        double j = 3;
        int j1 = 0;

        QVector<int> xCoords;
        QVector<int> yCoords;

        // 提取x和y的坐标并添加到xCoords和yCoords中
        for (const QPoint& point : polygon1)
        {
            xCoords.append(point.x());
            yCoords.append(point.y());
//            qDebug()<<point.x();
//            qDebug()<<point.y();
        }

        int end = 0;

        for (int i = 1; end == 0; i++)
        {
            int endX = xCoords[i - 1];
            int endY = yCoords[i - 1];

            if (endX != 1234 && endY != 1234)
            {
                //获取行数
                ui->NCDisplay->insertPlainText("N");
                QString text = QString::number(j);
                ui->NCDisplay->insertPlainText(text);
                j += 1;

                //获取X和Y坐标
                QString coordinate = QString("X %1").arg(xCoords[i - 1]);
                QString coordinate1 = QString("Y %1").arg(yCoords[i - 1]);

                ui->NCDisplay->insertPlainText(" G01 ");
                ui->NCDisplay->insertPlainText(coordinate);
                ui->NCDisplay->insertPlainText(coordinate1);
                ui->NCDisplay->insertPlainText("\n");
                //polygon1.clear();
            }
            else
            {
                end = 1;
                j1 = 2 * (j - 3) - 1;
                polygon1.clear();
            }
        }
        //结尾
        ui->NCDisplay->insertPlainText("N");
        QString text2 = QString::number(j);
        j += 1;
        ui->NCDisplay->insertPlainText(text2);
        ui->NCDisplay->insertPlainText(" G40 X0 Y0");
        ui->NCDisplay->insertPlainText("\n");
        ui->NCDisplay->insertPlainText("N");
        QString text3 = QString::number(j);
        j = 3;
        ui->NCDisplay->insertPlainText(text3);
        ui->NCDisplay->insertPlainText("M30");
        ui->NCDisplay->insertPlainText("\n");
        ui->NCDisplay->insertPlainText("逼近的直线段数为：");
        QString text4 = QString::number(j1);
        ui->NCDisplay->insertPlainText(text4);
    });
}


//调用绘图事件函数
//在qt的绘图里面，y轴正方向朝下，x轴正方向朝右
void MainWindow::paintEvent(QPaintEvent *)
{
    qDebug()<<p;
    //实例化画家对象
    QPainter painterBoard(this);
    //设置画刷
    QBrush brush(Qt::white);
    //让画家使用画刷
    painterBoard.setBrush(brush);
    //抗锯齿
    painterBoard.setRenderHint(QPainter::Antialiasing);
    //绘制画板的边框
    painterBoard.drawRect(QRect(50,50+20,600,600));
    //让画家返回初始状态
    //painterBoard.restore();

    QPainter painterCurve(this);
    QPainter painterRound(this);
    QPainter painterline(this);

    int scale = 20;     //缩放因子
    const double width = ui->Board->width();//将绘图窗口的宽度赋给width
    const double height = ui->Board->height();//将绘图窗口的高度赋给height
    //给初始误差圆圆心的坐标赋值
    xo = 0;
    yo = 0;
    r = 10;
    int xMove;
    int yMove;

    if ( p != 0 )
    {
        //绘制抛物线曲线
        for ( xStart = -width/2; xStart <= width/2; xStart += 1)
        {
            const float yStart = -p * xStart * xStart / (scale * scale);

            if (yStart + height/2 < 70)
            {
                continue;
            }

            painterCurve.drawPoint(xStart + width/2 + 50, yStart + height/2);
        }

        //绘制抛物线曲线的逼近直线
        //初始的误差圆定义为圆心(0,0)的圆
        for ( dxMove = xo + r + 1; dxMove < width/2; dxMove += 1)
        {
            //设置笔的颜色为红色
            QPen pen(QColor(255, 0, 0));
            //设置笔的类型为虚线
            pen.setStyle(Qt::SolidLine);
            //让画家使用这支笔
            painterRound.setPen(pen);
            xDisplay = xo;
            yDisplay = yo / (scale * scale);
            xDisplay_1 = -xo;
            yDisplay_1 = yo / (scale * scale);
            //绘制右半抛物线的节点圆
            painterRound.drawEllipse(QRect(xDisplay + width/2 + 50 - r, yDisplay + height/2 - r, 2*r, 2*r));
            //绘制左半抛物线的节点圆
            painterRound.drawEllipse(QRect(xDisplay_1 + width/2 + 50 - r, yDisplay_1 + height/2 - r, 2*r, 2*r));
            //painterRound.restore();
            update();

            double k = 2 * p * dxMove;
            double b = - p * pow(dxMove, 2);
            double temp = fabs(k * xo + b - yo);
            double d = temp / pow((k * k + 1), (1.0 / 2));
            double temp1 = fabs(d - r);

            if (temp1 < 0.5)
            {
                //将找到的节点圆的坐标放入polygon中便于后续textedit显示
                polygon1.append(QPoint(xo, yo));

                xMove = dxMove;
                //计算最终获得的节点圆的y坐标
                yMove = - p * pow(xMove, 2);
//                qDebug()<<xMove;
//                qDebug()<<yMove;

                double temp2 = dxMove - xMove;
                //将最终求得的解四舍五入
                if (temp2 > 0.5)
                {
                    xMove += 1;
                }

                //将double类型的动点x重新赋予一个int的动点x的值
                dxMove = xMove;
                xDisplay1 = xMove;
                yDisplay1 = yMove / (scale * scale);
                xDisplay1_1 = -xMove;
                yDisplay1_1 = yMove / (scale * scale);

                //防止超出绘图板的上方
                if (yDisplay1 + height/2 < 70)
                {
                    if (yDisplay1 + height/2 >= 70 || xDisplay1 + width/2 + 50 <= 650)
                    {
                    polygon1.append(QPoint(xMove, yMove));
                    }
                    polygon1.append(QPoint(1234, 1234));
                    break;
                }
                else
                {
                    //绘制节点圆和新节点之间的直线
                    //绘制抛物线的右半部分
                    painterline.drawLine
                        (QPoint(xDisplay + width/2 + 50, yDisplay + height/2),
                         QPoint(xDisplay1 + width/2 + 50, yDisplay1 + height/2));
                    //绘制抛物线的左半部分
                    painterline.drawLine
                        (QPoint(xDisplay_1 + width/2 + 50, yDisplay_1 + height/2),
                         QPoint(xDisplay1_1 + width/2 + 50, yDisplay1_1 + height/2));
                }
                xo = xMove;
                yo = yMove;
            }
            else
            {
                continue;
            }
        }
//        //一元四次方程参数的处理
//        xo = yo = 0;
//        b4 = -4 * xo;
//        c4 = 2 * pow(xo, 2) - 4 * pow(r, 2);
//        d4 = 4 * pow(xo, 3);
//        e4 = pow(xo, 4) - pow(r, 2) / pow(p, 2);
//        //一元四次降阶三次方程的处理
//        a3 = -1;
//        b3 = c4;
//        c3 = 4 * e4 - b4 * d4;
//        d3 = pow(d4, 2) + e4 * pow(b4, 2) - 4 * e4 * c4;
//        p3 = (3 * a3 * c3 - pow(b3, 2)) / (3 * pow(a3, 2));
//        q3 = (27 * pow(a3, 2) * d3 - 9 * a3 * b3 * c3 + 2 * pow(b3, 3)) / (27 * pow(a3, 3));
//        y3_1 = pow(q3, 3) / 4 + pow(p3, 3) / 27;
//        y3_2 = pow(y3_1, (1 / 2));
//        y3 = -1 * (b3 / (3 * a3)) + pow((-q3 / 2 + y3_2), (1/3)) +pow((-q3 / 2 - y3_2), (1/3));
//        //一元三次方程降阶二次处理
//        //第一个一元二次方程
//        a2_1 = 1;
//        b2_1 = b4 / 2 - pow((pow(b4, 2) - 4 * c4 + 4 * y3), (1 / 2)) / 2;
//        c2_1 = y3 / 2 - (b4 * y3 - 2 * d4) / pow(2 * (pow(b4, 2) - 4 * c4 + 4 * y3),(1 / 2));
//        triangle1 = pow(b2_1, 2) - 4 * a2_1 * c2_1;
//        //第二个一元二次方程
//        a2_2 = 1;
//        b2_2 = b4 / 2 + pow((pow(b4, 2) - 4 * c4 + 4 * y3), (1 / 2)) / 2;
//        c2_2 = y3 / 2 + (b4 * y3 - 2 * d4) / pow(2 * (pow(b4, 2) - 4 * c4 + 4 * y3),(1 / 2));
//        triangle2 = pow(b2_2, 2) - 4 * a2_2 * c2_2;
//        //解出的四个根
//        x2_1_1 = (-b2_1 - pow(triangle1, (1 / 2))) / 2 * a2_1;
//        x2_1_2 = (-b2_1 + pow(triangle1, (1 / 2))) / 2 * a2_1;
//        x2_2_1 = (-b2_2 - pow(triangle2, (1 / 2))) / 2 * a2_2;
//        x2_2_2 = (-b2_2 + pow(triangle2, (1 / 2))) / 2 * a2_2;

//        int k2_1_1 = 2 * p * x2_1_1;
//        int compare()
//        {

//        }
    }

    //绘制椭圆曲线
    if (a != 0 && b != 0)
    {

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}



