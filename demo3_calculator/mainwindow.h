#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_button_1_clicked();

    void on_button_2_clicked();

    void on_button_3_clicked();

    void on_button_4_clicked();

    void on_button_5_clicked();

    void on_button_6_clicked();

    void on_button_7_clicked();

    void on_button_8_clicked();

    void on_button_9_clicked();

    void on_button_zero_clicked();

    void on_button_leftbrace_clicked();

    void on_button_rightbrace_clicked();

    void on_button_multiply_clicked();

    void on_button_devide_clicked();

    void on_button_plus_clicked();

    void on_button_minus_clicked();

    void on_button_clear_clicked();

    void on_button_del_clicked();

private:
    Ui::MainWindow *ui;
    QString expression;
};
#endif // MAINWINDOW_H
