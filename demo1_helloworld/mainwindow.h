#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT // Q_OBJECT宏，允许类中使用信号和槽的机制
              // Q_OBJECT宏必须出现在类的私有部分的最开始
              // Q_OBJECT宏之后，必须有一个信号和槽的声明，否则会出现编译错误

public:
    MainWindow(QWidget *parent = nullptr); // 构造函数
    ~MainWindow(); // 析构函数

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
