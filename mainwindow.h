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
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:

    void clear();
    bool getRvIv();
    void getLevel();
    void setLowerlimit();
    void setWeight();
    void Item();
    void Ability();
    bool static_H();
    bool static_H(int &Hbest);
    void cal(int &Hbest, double &maxans);
    double get_ans(int H, int B, int D);
    void Output();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
