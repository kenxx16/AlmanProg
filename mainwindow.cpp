#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <stdlib.h>
#include <cstdio>
#include <qfiledialog.h>
#include <stdio.h>
#include <QStringList>
#include <QString>
#include <QIODevice>
#include <locale.h>
#include <QFile>
#include <QPlainTextEdit>
#include <QtSql\qsql.h>
#include <QtSql\qsqldatabase.h>
#include <QSqlQuery>
#include <QMessageBox>
#include <QObject>
#include <QtSql/QSqlError>
#include <QPushButton>
#include <qlineedit.h>

using namespace std;
//-----------------------------------------------
struct TALM_GLN {
#pragma pack(1)
    __int16 NOM_KA;  /*НОМЕР КА*/
    __int16 CHAN;    /*ЧАСТОТНЫЙ КАНАЛ;*/
    __int16 HEALTH;  /*ПРИЗНАК ЗДОРОВЬЯ;*/
    float E;          /*!ЭКСЦЕНТРИСИТЕТ*/
    __int32 N_DAY;   /*НОМЕР ДНЯ 4-Х ЛЕТНЕГО ПЕРИОДА*/
    float I_CORR;    /*ПОПРАВКА К НАКЛОНЕНИЮ (РАД)*/
    float LAMBDA;    /*ДОЛГОТА 1-ГО В СУТКАХ ВОСХОДЯЩЕГО УЗЛА
                     (РАД)*/
    float T_LAMBDA;  /*ВРЕМЯ (СЕК) ОТ НАЧАЛА СУТОК 1-ГО ВОСХ.
                     УЗЛА (РАД)*/
    float OMEGA;    /*АРГУМЕНТ ПЕРИГЕЯ (РАД) */
    float DT_DRAC;   /*ПОПРАВКА К ПЕРИОДУ 43200 (СЕК)*/
    float AF1;      /*СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК)*/
    float AF0;      /*УХОД ВРЕМЕНИ БОРТА (СЕК)*/
    __int16 CHECK_;  /*КОНТРОЛЬНАЯ СУММА*/
    double MJD;     /* MJD (ТАЙМЕР ПК)*/
};
//-----------------------------------------------
struct TALM_GPS {
#pragma pack(1)
    __int16 NOM_KA;   //НОМЕР КА
    __int16 HEALTH;   //ПРИЗНАК ЗДОРОВЬЯ
    float E;          //ЭКСЦЕНТРИСИТЕТ
    __int32 TOA;      //ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА (СЕК)
    float I_ANGLE;    //НАКЛОНЕНИЕ (РАД)
    float OMEGADOT;   //СКОРОСТЬ ПРЯМОГО ВОСХОЖДЕНИЯ (РАД/СЕК)
    double ROOTA;     //SQRT(A), КОРЕНЬ КВ.(МЕТР)
    double OMEGA0;    //ДОЛГОТА ВОСХОДЯЩЕГО УЗЛА (РАД)
    double OMEGA;     //АРГУМЕНТ ПЕРИГЕЯ (РАД)
    double M0;        //СРЕДНЯЯ АНОМАЛИЯ НА ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА(РАД)
    float AF0;        //УХОД ВРЕМЕНИ БОРТА (СЕК)
    float AF1;        //СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК)
    __int16 WNA;      //НОМЕР НЕДЕЛИ АЛЬМАНАХА
    __int16 WN;       //НОМЕР НЕДЕЛИ
    __int32 TOW;      //ВРЕМЯ В СЕКУНДАХ ОТ НАЧАЛА GPS-НЕДЕЛИ
    __int16 CHECK_;   //КОНТРОЛЬНАЯ СУММА
    double MJD;       //MJD (ТАЙМЕР ПК)
};
//-----------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//-----------------------------------------------
bool FileIsExist(std::string filePath)
{
    bool isExist = false;
    std::ifstream fin(filePath.c_str());

    if (fin.is_open())
        isExist = true;

    fin.close();
    return isExist;
}


//-----------------------------------------------
void createsaltab(QString tabname, QString err)
{
    QSqlQuery query;

    QString sq = "CREATE TABLE "+tabname+"("
                            "nom_ka INTEGER PRIMARY KEY NOT NULL,"
                            "health INTEGER,"
                            "e FLOAT,"
                            "toa INTEGER,"
                            "i_angle FLOAT,"
                            "omegadot FLOAT,"
                            "roota FLOAT ,"
                            "omega0 FLOAT ,"
                            "omega FLOAT ,"
                            "m0 FLOAT ,"
                            "af0 FLOAT,"
                            "af1 FLOAT,"
                            "wna INTEGER,"
                            "wn INTEGER,"
                            "tow INTEGER,"
                            "chek INTEGER,"
                            "mjd FLOAT,"
                            ");";
    if (!query.exec(sq))
    {
        cout<<"Very bad("<<endl;
        err = query.lastError().text();
        cout<<err.toStdString()<<endl;
    }
    else err = tabname;
}


//-----------------------------------------------
void createsagtab(QString tabname, QString err)
{
   QSqlQuery query;
   QString sq = "CREATE TABLE "+tabname+"("
                                "nom_ka INTEGER PRIMARY KEY NOT NULL,"
                                "chan INTEGER,"
                                "health INTEGER,"
                                "e FLOAT,"
                                "n_day INTEGER,"
                                "i_corr FLOAT,"
                                "lambda FLOAT,"
                                "t_lambda FLOAT,"
                                "omega FLOAT,"
                                "dt_drac FLOAT,"
                                "af1 FLOAT,"
                                "af0 FLOAT,"
                                "chek INTEGER,"
                                "mjd FLOAT,"
                                ");";
    if (!query.exec(sq))
    {
        cout<<"Very bad("<<endl;
        err = query.lastError().text();
        cout<<err.toStdString()<<endl;
    }
    else err = tabname;
}


//-----------------------------------------------
void MainWindow::on_pushButton_clicked()
{
    ui-> textBrowser-> clear();
    QStringList filename = QFileDialog::getOpenFileNames(this, "Open Dialog", QDir::homePath(), "*.sag.alm *.sal.alm");


    int argc = 0;
        TALM_GPS y;
        TALM_GLN x;
        int Count = filename.count();

        while (argc!=Count)
        {
            FILE* in;
            fstream tx;

            string filepath = filename[argc].toStdString();
            char * fchar = new char[filepath.size() + 1];
            std::copy(filepath.begin(), filepath.end(), fchar);
            fchar[filepath.size()] = '\0';

            cout<<fchar<<endl;

            fopen_s(&in,fchar,"rb");

            if (FileIsExist(fchar))
            {
                ui->textBrowser->insertPlainText("Открыт файл: \n");
                ui->textBrowser->insertPlainText(fchar);
                ui->textBrowser->insertPlainText("\n");
            }
            else ui->textBrowser->insertPlainText("файл не существует! \n");


            filepath = filepath + ".txt" ;
            char * ftxt = new char[filepath.size() + 1];
            std::copy(filepath.begin(), filepath.end(), ftxt);
            ftxt[filepath.size()] = '\0';

            tx.open(ftxt, ios::out);

            if (FileIsExist(ftxt))
            {
                ui->textBrowser->insertPlainText("Файл перезаписан: \n");
                ui->textBrowser->insertPlainText(ftxt);
                ui->textBrowser->insertPlainText("\n");
            }
            else
            {
                ui->textBrowser->insertPlainText("файл создан: \n");
                ui->textBrowser->insertPlainText(ftxt);
                ui->textBrowser->insertPlainText("\n");
            }
            int ex;
            int i;

            string ext = strrchr(ftxt, '.s');

            if (ext == "sal.alm.txt")
            {
                ex = 0;
                i = 0;
                QString tabname = "tab"+filename[argc].section("/",-1,-1);
                QString err;
                tabname.remove('.');
                cout << tabname.toStdString() << endl;

                createsaltab(tabname,err);
                ui->textBrowser->insertPlainText(err);
                ui->textBrowser->insertPlainText("\n");

                QSqlQuery query;
                QString sq = "INSERT INTO "+tabname+" (nom_ka, health, e, toa, i_angle, omegadot, roota,"
                                                      " omega0, omega, m0, af0, af1, wna, wn, tow, chek, mjd)"
                             "VALUES (:nom_ka, :health, :e, :toa, :i_angle, :omegadot, :roota,"
                                    " :omega0, :omega, :m0, :af0, :af1, :wna, :wn, :tow, :chek, :mjd)";
                query.prepare(sq);
                cout<<query.lastError().text().toStdString()<<endl;


                while (ex != 1)
                {
                    fseek(in, i * 78, SEEK_SET);               //GPS
                    if (!fread(&y, 78, 1, in))
                    {
                        ex = 1;
                    }
                    else
                    {
                        double e = y.E;
                        double i_angle = (double)y.I_ANGLE;
                        double omegadot = (double)y.OMEGADOT;
                        double roota = y.ROOTA;
                        double omega0 = y.OMEGA0;
                        double omega = y.OMEGA;
                        double m0 = y.M0;
                        double af0 = (double)y.AF0;
                        double af1 = (double)y.AF1;
                        double mjd = y.MJD;


                        query.bindValue(":nom_ka",y.NOM_KA);
                        query.bindValue(":health",y.HEALTH);
                        query.bindValue(":e",e);
                        query.bindValue(":toa",y.TOA);
                        query.bindValue(":i_angle",i_angle);
                        query.bindValue(":omegadot",omegadot);
                        query.bindValue(":roota",roota);
                        query.bindValue(":omega0",omega0);
                        query.bindValue(":omega",omega);
                        query.bindValue(":m0",m0);
                        query.bindValue(":af0",af0);
                        query.bindValue(":af1",af1);
                        query.bindValue(":wna",y.WNA);
                        query.bindValue(":wn",y.WN);
                        query.bindValue(":tow",y.TOW);
                        query.bindValue(":chek",y.CHECK_);
                        query.bindValue(":mjd",mjd);
                        query.exec();


                        //cout << qw << endl;

                      tx << "НОМЕР КА ========================================== " << y.NOM_KA << endl <<
                            "ПРИЗНАК ЗДОРОВЬЯ ================================== " << y.HEALTH << endl <<
                            "ЭКСЦЕНТРИСИТЕТ ==================================== " << y.E << endl <<
                            "ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА (СЕК) ===================== " << y.TOA << endl <<
                            "НАКЛОНЕНИЕ (РАД) ================================== " << y.I_ANGLE << endl <<
                            "СКОРОСТЬ ПРЯМОГО ВОСХОЖДЕНИЯ (РАД/СЕК) ============ " << y.OMEGADOT << endl <<
                            "SQRT(A), КОРЕНЬ КВ.(МЕТР) ========================= " << y.ROOTA << endl <<
                            "ДОЛГОТА ВОСХОДЯЩЕГО УЗЛА (РАД) ==================== " << y.OMEGA0 << endl <<
                            "АРГУМЕНТ ПЕРИГЕЯ (РАД) ============================ " << y.OMEGA << endl <<
                            "СРЕДНЯЯ АНОМАЛИЯ НА ВРЕМЯ ЗАДАНИЯ АЛЬМАНАХА(РАД) == " << y.M0 << endl <<
                            "УХОД ВРЕМЕНИ БОРТА (СЕК) ========================== " << y.AF0 << endl <<
                            "СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК) ============ " << y.AF1 << endl <<
                            "НОМЕР НЕДЕЛИ АЛЬМАНАХА ============================ " << y.WNA << endl <<
                            "НОМЕР НЕДЕЛИ ====================================== " << y.WN << endl <<
                            "ВРЕМЯ В СЕКУНДАХ ОТ НАЧАЛА GPS-НЕДЕЛИ ============= " << y.TOW << endl <<
                            "КОНТРОЛЬНАЯ СУММА ================================= " << y.CHECK_ << endl <<
                            "MJD (ТАЙМЕР ПК) =================================== " << y.MJD << endl <<
                            " " << endl;
                        i++;
                    }
                }
            cout<<"ok \n";
            }
            else
            {
                i = 0;
                ex = 0;

                QString tabname ="tab"+filename[argc].section("/",-1,-1);
                QString err;
                tabname.remove('.');
                cout << tabname.toStdString() << endl;

                createsagtab(tabname,err);
                ui->textBrowser->insertPlainText(err);
                ui->textBrowser->insertPlainText("\n");

                QSqlQuery query;
                QString sq = "INSERT INTO "+tabname+"(nom_ka, chan, health, e, n_day, i_corr, lambda, t_lambda,"
                                                      " omega, dt_drac, af1, af0, chek, mjd)"
                             "VALUES (:nom_ka, :chan, :health, :e, :n_day, :i_corr, :lambda, :t_lambda,"
                                     " :omega, :dt_drac, :af1, :af0, :chek, :mjd);";
                query.prepare(sq);
                cout<<query.lastError().text().toStdString()<<endl;

                while (ex != 1)
                {
                    fseek(in, i * 52, SEEK_SET);                  //GLN
                    if (!fread(&x, 52, 1, in))
                    {
                        ex = 1;
                    }
                    else
                    {
                        double e = x.E;
                        double i_corr = x.I_CORR;
                        double lambda = x.LAMBDA;
                        double t_lambda = x.T_LAMBDA;
                        double af1 = x.AF1;
                        double af0 = x.AF0;
                        double omega = x.OMEGA;
                        double dt_drac = x.DT_DRAC;




                        query.bindValue(":nom_ka",x.NOM_KA);
                        query.bindValue(":chan",x.CHAN);
                        query.bindValue(":health",x.HEALTH);
                        query.bindValue(":e",e);
                        query.bindValue(":n_day",x.N_DAY);
                        query.bindValue(":i_corr",i_corr);
                        query.bindValue(":lambda",lambda);
                        query.bindValue(":t_lambda",t_lambda);
                        query.bindValue(":omega",omega);
                        query.bindValue(":dt_drac",dt_drac);
                        query.bindValue(":af1",af1);
                        query.bindValue(":af0",af0);
                        query.bindValue(":chek",x.CHECK_);
                        query.bindValue(":mjd",x.MJD);
                        query.exec();

                      tx << "НОМЕР КА ========================================== " << x.NOM_KA << endl <<
                            "ЧАСТОТНЫЙ КАНАЛ =================================== " << x.CHAN << endl <<
                            "ПРИЗНАК ЗДОРОВЬЯ ================================== " << x.HEALTH << endl <<
                            "ЭКСЦЕНТРИСИТЕТ ==================================== " << x.E << endl <<
                            "НОМЕР ДНЯ 4-Х ЛЕТНЕГО ПЕРИОДА ===================== " << x.N_DAY << endl <<
                            "ПОПРАВКА К НАКЛОНЕНИЮ (РАД) ======================= " << x.I_CORR << endl <<
                            "ДОЛГОТА 1-ГО В СУТКАХ ВОСХОДЯЩЕГО УЗЛА (РАД) ====== " << x.LAMBDA << endl <<
                            "ВРЕМЯ (СЕК) ОТ НАЧАЛА СУТОК 1-ГО ВОСХ. УЗЛА(РАД) == " << x.T_LAMBDA << endl <<
                            "АРГУМЕНТ ПЕРИГЕЯ (РАД) ============================ " << x.OMEGA << endl <<
                            "ПОПРАВКА К ПЕРИОДУ 43200 (СЕК) ==================== " << x.DT_DRAC << endl <<
                            "СКОРОСТЬ УХОДА ВРЕМЕНИ БОРТА (СЕК/СЕК) ============ " << x.AF1 << endl <<
                            "УХОД ВРЕМЕНИ БОРТА ================================ " << x.AF0 << endl <<
                            "КОНТРОЛЬНАЯ СУММА ================================= " << x.CHECK_ << endl <<
                            "MJD (ТАЙМЕР ПК) =================================== " << x.MJD << endl <<
                            " " << endl;
                        i++;
                    }
                }
            cout<<"ok \n";
            }
            fclose(in);
            tx.close();
            argc++;
    }
}




void MainWindow::on_pushButton_2_clicked()
{
    QString driver = "QODBC";
    QSqlDatabase db = QSqlDatabase::addDatabase(driver);
    QString dbname = ui->lineEdit->text();
    db.setDatabaseName(dbname);
    QString User = ui->lineEdit_2->text();
    db.setUserName(User);
    db.setHostName("localhost");
    QString Pass = ui->lineEdit_3->text();
    db.setPassword(Pass);


    if (!db.open())
    {

       QMessageBox::critical(NULL,QObject::tr("Ошибка"),"Ошибка подключения");

    }
    else QMessageBox::information(NULL,QObject::tr(""),"Подключение выполнено");
    QStringList ls = db.tables();
    int i1=0;
    int lent = ls.count();
    ui->textBrowser->insertPlainText("Существующие таблицы: \n");
    while (i1!=lent)
    {
    ui->textBrowser->insertPlainText(ls[i1]);
    ui->textBrowser->insertPlainText("\n");
    i1++;
    }
}


//-----------------------------------------------


void MainWindow::on_pushButton_3_clicked()
{

}
