#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void create_stuTable();

private slots:
    void on_button_insert_clicked();

    void on_button_query_clicked();

private:
    Ui::Widget *ui;
    QSqlDatabase db;
    QSqlTableModel *m;
};
#endif // WIDGET_H
