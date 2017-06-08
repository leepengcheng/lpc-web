#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "QcustomPlot.h"//ªÊÕº¿‡
#include "pugixml.hpp"//Xml¿‡
#include "ui_dataplot.h"
using namespace pugi;

namespace Ui {
class MainWindow;
}

class AnsysDataPlot : public QMainWindow
{
    Q_OBJECT	
public:
    explicit AnsysDataPlot(QWidget *parent = 0);
	void getEarthQuakePlot();
    int parseXYData(const char *xmlstring);
    ~AnsysDataPlot();
private slots:
    void showData(QCPLegend*);
    void showPointXY(QMouseEvent*);
    void createGraph(xml_node node);
private:
    Ui::MainWindow *ui;
    QRect geometry;
    //ÕºœÒ√˚≥∆
    QString title;
    //X÷·±Í«©
    QString xlabel;
    //Y÷·±Í«©
    QString ylabel;
    QVector<double> x_max;
    QVector<double> y_max;
    QVector<double> x_min;
    QVector<double> y_min;
};

#endif // MAINWINDOW_H
