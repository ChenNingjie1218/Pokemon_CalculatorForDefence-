#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTextCodec>//字符编码转换头文件
#define cout qDebug()
QTextCodec *codec;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    codec = QTextCodec::codecForName("GBK");//初始化
    ui->pushButton_2->setEnabled(false);
    setWindowIcon(QIcon("1.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int h_Rv, b_Rv, d_Rv;//种族值
int h_Iv, b_Iv, d_Iv;//个体值
int level;//等级
double correct_B, correct_D;//修正系数
int EV;//可分配努力值
int Bmin, Dmin;//特殊要求
double Bweight, Dweight;//B、D的权重
int correctFlag;//修正物防为1，修正特防为0

#include <stack>
using namespace std;
struct BestEv
{
    int Hbest, Bbest, Dbest, correctFlag;
    BestEv(int a, int b, int c, int d):Hbest(a),Bbest(b),Dbest(c),correctFlag(d){};
};
stack<BestEv> st;

//计算按钮
void MainWindow::on_pushButton_clicked()
{
    ui->pushButton_2->setEnabled(false);
    clear();
    if(!getRvIv())
    {
        return;
    }
   getLevel();
   setLowerlimit();
   setWeight();
   Item();
   Ability();
    int Hbest = 0;
    if(ui->checkBox_4->isChecked() && !static_H(Hbest))//固定HP
    {
        return;
    }
    double maxans = 0;//最佳努力分配
    cal(Hbest, maxans);
    if(st.empty())
    {
        ui->lineEdit_11->setText(codec->toUnicode("分配失败！"));
    }
    else
    {
        Output();
    }
}

//清空槽
void MainWindow::clear()
{
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    ui->lineEdit_9->clear();
    ui->lineEdit_15->clear();
    ui->lineEdit_16->clear();
    ui->lineEdit_17->clear();
}

//获取种族值和个体值
bool MainWindow::getRvIv()
{
    h_Rv = ui->lineEdit->text().toInt();
    b_Rv = ui->lineEdit_2->text().toInt();
    d_Rv = ui->lineEdit_3->text().toInt();
    h_Iv = ui->lineEdit_4->text().toInt();
    b_Iv = ui->lineEdit_5->text().toInt();
    d_Iv = ui->lineEdit_6->text().toInt();
    EV = ui->lineEdit_10->text().toInt();
    if(h_Iv > 31 || b_Iv > 31 || d_Iv > 31 || h_Iv < 0 || b_Iv < 0 || d_Iv < 0)
    {
        ui->lineEdit_11->setText(codec->toUnicode("个体值错误!"));
        return false;
    }
    if(EV > 510 || EV <4)
    {
        ui->lineEdit_11->setText(codec->toUnicode("努力值错误!"));
        return false;
    }
    return true;
}

//获取等级
void MainWindow::getLevel()
{
    if(ui->comboBox_4->currentText() == codec->toUnicode("50"))
    {
        level = 50;
    }
    else
    {
        level = 100;
    }
}
//设置下限
void MainWindow::setLowerlimit()
{
    Bmin = 0;
    Dmin = 0;
    if(ui->checkBox->isChecked())
    {
        Bmin = ui->lineEdit_12->text().toInt();
    }
    if(ui->checkBox_2->isChecked())
    {
        Dmin = ui->lineEdit_13->text().toInt();
    }
}

//设置权重
void MainWindow::setWeight()
{
    Bweight = 1;
    Dweight = 1;
    if(ui->checkBox_3->isChecked())
    {
        Bweight = ui->lineEdit_14->text().toDouble();
    }
    if(ui->checkBox_7->isChecked())
    {
        Dweight = ui->lineEdit_19->text().toDouble();
    }
}

//道具修正
void MainWindow::Item()
{
    correct_B = 1;
    correct_D = 1;
    if(ui->checkBox_5->isChecked())
    {
        if(ui->comboBox_2->currentText() == codec->toUnicode("突击背心/精神种子/薄雾种子/光苔"))
        {
            correct_D *= 1.5;
        }
        else if(ui->comboBox_2->currentText() == codec->toUnicode("青草种子/电气种子"))
        {
            correct_B *= 1.5;
        }
        else if(ui->comboBox_2->currentText() == codec->toUnicode("进化奇石"))
        {
            correct_B *= 1.5;
            correct_D *= 1.5;
        }
    }
}

//特性修正
void MainWindow::Ability()
{
    if(ui->checkBox_6->isChecked())
    {
        if(ui->comboBox_3->currentText() == codec->toUnicode("神奇鳞片/草之毛皮/不屈之盾"))
        {
            correct_B *= 1.5;
        }
        else if(ui->comboBox_3->currentText() == codec->toUnicode("毛皮大衣"))
        {
            correct_B *= 2;
        }
        else if(ui->comboBox_3->currentText() == codec->toUnicode("冰鳞粉"))
        {
            correct_D *= 2;
        }else if(ui->comboBox_3->currentText() == codec->toUnicode("灾祸之简"))
        {
            correct_B *= 4.0/3;
        }else if(ui->comboBox_3->currentText() == codec->toUnicode("灾祸之鼎"))
        {
            correct_D *= 4.0/3;
        }else if(ui->comboBox_3->currentText() == codec->toUnicode("威吓"))
        {
            correct_B *= 1.5;
        }
    }
}

//固定HP
bool MainWindow::static_H(int &Hbest)
{
    Hbest = ((ui->lineEdit_18->text().toInt() - level - 10) * 100 / level - h_Iv - h_Rv * 2) * 4;
    if(Hbest > 255 || Hbest < 0)
    {
        ui->lineEdit_11->setText(codec->toUnicode("分配失败！"));
        return false;
    }
    return true;
}

double MainWindow::get_ans(int H, int B, int D)
{
    return 1.0 * H / (Bweight / (B * correct_B) + Dweight / (D * correct_D)); //综合耐久公式：HP*（物防权重/物防+特防权重/特防）
}

void MainWindow::cal(int &Hbest, double &maxans)
{
    while(!st.empty())
    {
        st.pop();
    }
    int H, B, D, tmpFlag = 1;
    double ans, tmp_ans;
    int i = 0, j = 0, k = 0; //i,j,k为分配的努力

    //固定HP
    if(ui->checkBox_4->isChecked())
    {
        EV -= Hbest;
        H = (h_Rv * 2 + h_Iv + Hbest / 4) * level / 100 + 10 + level;
        j = 0;
        while(j < 255)
        {
            if(j > EV)
            {
                break;
            }
            k = 0;
            while(k < 255)
            {
                if(j + k > EV)
                {
                    break;
                }
                B = (b_Rv * 2 + b_Iv + j / 4) * level / 100 + 5;
                D = (d_Rv * 2 + d_Iv + k / 4) * level / 100 + 5;
                if(ui->comboBox->currentText() == codec->toUnicode("无修正"))
                {
                    ans = get_ans(H, B, D);
                    if(B*H < Bmin || D*H < Dmin){
                        ans = -1;
                    }
                }
                else if(ui->comboBox->currentText() == codec->toUnicode("正向修正"))
                {
                    ans = get_ans(H, B*1.1, D);
                    if((B*1.1*H) < Bmin || (D*H) < Dmin){
                        ans = -1;
                    }
                    tmp_ans = get_ans(H, B, 1.1*D);
                    if((B*H) < Bmin || (D*1.1*H) < Dmin){
                        tmp_ans = -1;
                    }
                    if(ans > tmp_ans)
                    {
                        tmpFlag = 1;
                        B *= 1.1;
                    }
                    else
                    {
                        ans = tmp_ans;
                        tmpFlag = 0;
                        D *= 1.1;
                    }
                }
                else
                {
                    ans = get_ans(H, B*0.9, D);
                    if((B*0.9*H) < Bmin || (D*H) < Dmin){
                        ans = -1;
                    }
                    tmp_ans = get_ans(H, B, D*0.9);
                    if((B*H) < Bmin || (D*0.9*H) < Dmin){
                        tmp_ans = -1;
                    }
                    if(ans > tmp_ans)
                    {
                        tmpFlag = 1;
                        B *= 0.9;
                    }
                    else
                    {
                        tmpFlag = 0;
                        ans = tmp_ans;
                        D *= 0.9;
                    }
                }
                if(ans > maxans)
                {
                    maxans = ans;
                    while(!st.empty())
                    {
                        st.pop();
                    }
                    st.push(BestEv(Hbest,j,k,tmpFlag));
                }
                else if(ans == maxans)
                {
                    st.push(BestEv(Hbest,j,k,tmpFlag));
                }
                if(k && level == 50)
                {
                    k += 4;
                }
                k += 4;
            }
            if(j && level == 50)
            {
                j += 4;
            }
            j += 4;
        }
    }
    else
    {
        //计算能力值
        while(i < 255)
        {
            if(i > EV)
            {
                break;
            }
            j = 0;
            while(j < 255)
            {
                if(i + j > EV)
                {
                    break;
                }
                k = 0;
                while(k < 255)
                {
                    if(i + j + k > EV)
                    {
                        break;
                    }
                    H = (h_Rv * 2 + h_Iv + i / 4) * level / 100 + 10 + level;
                    B = (b_Rv * 2 + b_Iv + j / 4) * level / 100 + 5;
                    D = (d_Rv * 2 + d_Iv + k / 4) * level / 100 + 5;
                    if(ui->comboBox->currentText() == codec->toUnicode("无修正"))
                    {
                        ans = get_ans(H, B, D);
                        if(B*H < Bmin || D*H < Dmin){
                            ans = -1;
                        }
                    }
                    else if(ui->comboBox->currentText() == codec->toUnicode("正向修正"))
                    {
                        ans = get_ans(H, B*1.1, D);
                        if((B*1.1*H) < Bmin || (D*H) < Dmin){
                            ans = -1;
                        }
                        tmp_ans = get_ans(H, B, 1.1*D);
                        if((B*H) < Bmin || (D*1.1*H) < Dmin){
                            tmp_ans = -1;
                        }
                        if(ans > tmp_ans)
                        {
                            tmpFlag = 1;
                            B *= 1.1;
                        }
                        else
                        {
                            ans = tmp_ans;
                            tmpFlag = 0;
                            D *= 1.1;
                        }
                    }
                    else
                    {
                        ans = get_ans(H, B*0.9, D);
                        if((B*0.9*H) < Bmin || (D*H) < Dmin){
                            ans = -1;
                        }
                        tmp_ans = get_ans(H, B, D*0.9);
                        if((B*H) < Bmin || (D*0.9*H) < Dmin){
                            tmp_ans = -1;
                        }
                        if(ans > tmp_ans)
                        {
                            tmpFlag = 1;
                            B *= 0.9;
                        }
                        else
                        {
                            tmpFlag = 0;
                            ans = tmp_ans;
                            D *= 0.9;
                        }
                    }
                    if(ans > maxans)
                    {
                        maxans = ans;
                        while(!st.empty())
                        {
                            st.pop();
                        }
                        st.push(BestEv(i,j,k,tmpFlag));
                    }
                    else if(ans == maxans)
                    {
                        st.push(BestEv(i,j,k,tmpFlag));
                    }
                    if(k && level == 50)
                    {
                        k += 4;
                    }
                    k += 4;
                }
                if(j && level == 50)
                {
                    j += 4;
                }
                j += 4;
            }
            if(i && level == 50)
            {
                i += 4;
            }
            i += 4;
        }
    }
}

//结果输出
void MainWindow::Output()
{
    BestEv tmp = st.top();
    st.pop();
    if(!st.empty())
    {
        ui->pushButton_2->setEnabled(true);
    }
    else
    {
        ui->pushButton_2->setEnabled(false);
    }
    int Hbest = tmp.Hbest, Bbest = tmp.Bbest, Dbest = tmp.Dbest, correctFlag = tmp.correctFlag;
    int B, D;
    ui->lineEdit_7->setText(QString::number(Hbest));
    ui->lineEdit_8->setText(QString::number(Bbest));
    ui->lineEdit_9->setText(QString::number(Dbest));
    int H = (int)((h_Rv * 2 + h_Iv + Hbest / 4) * level / 100 + 10 + level);
    ui->lineEdit_15->setText(QString::number(H));
    if(ui->comboBox->currentText() == codec->toUnicode("无修正"))
    {
        B = (int)(((b_Rv * 2 + b_Iv + Bbest / 4) * level / 100 + 5));
        D = (int)(((d_Rv * 2 + d_Iv + Dbest / 4) * level / 100 + 5));
        ui->lineEdit_16->setText(QString::number(B));
        ui->lineEdit_17->setText(QString::number(D));
        ui->lineEdit_11->setText(codec->toUnicode("分配成功！"));
    }
    else if(ui->comboBox->currentText() == codec->toUnicode("正向修正"))
    {
        if(correctFlag)
        {
           B = (int)(((b_Rv * 2 + b_Iv + Bbest / 4) * level / 100 + 5)*1.1);
           D = (int)(((d_Rv * 2 + d_Iv + Dbest / 4) * level / 100 + 5));
           ui->lineEdit_16->setText(QString::number(B));
           ui->lineEdit_17->setText(QString::number(D));
           ui->lineEdit_11->setText(codec->toUnicode("分配成功，性格修正物防"));
        }
        else
        {
            B = (int)(((b_Rv * 2 + b_Iv + Bbest / 4) * level / 100 + 5));
            D = (int)(((d_Rv * 2 + d_Iv + Dbest / 4) * level / 100 + 5)*1.1);
            ui->lineEdit_16->setText(QString::number(B));
            ui->lineEdit_17->setText(QString::number(D));
            ui->lineEdit_11->setText(codec->toUnicode("分配成功，性格修正特防"));
        }
    }
    else
    {
        if(correctFlag)
        {
            B = (int)(((b_Rv * 2 + b_Iv + Bbest / 4) * level / 100 + 5)*0.9);
            D = (int)(((d_Rv * 2 + d_Iv + Dbest / 4) * level / 100 + 5));
            ui->lineEdit_16->setText(QString::number(B));
            ui->lineEdit_17->setText(QString::number(D));
            ui->lineEdit_11->setText(codec->toUnicode("分配成功，性格修正物防"));
        }
        else
        {
            B = (int)(((b_Rv * 2 + b_Iv + Bbest / 4) * level / 100 + 5));
            D = (int)(((d_Rv * 2 + d_Iv + Dbest / 4) * level / 100 + 5)*0.9);
            ui->lineEdit_16->setText(QString::number(B));
            ui->lineEdit_17->setText(QString::number(D));
            ui->lineEdit_11->setText(codec->toUnicode("分配成功，性格修正特防"));
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if(!st.empty())
    {
        Output();
    }
}
