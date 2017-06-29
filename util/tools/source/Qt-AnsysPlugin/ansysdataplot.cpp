#include "ansysdataplot.h"
using namespace pugi;
using namespace std;

/*本模块用于绘制曲线 */
AnsysDataPlot::AnsysDataPlot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置图标
    this->setWindowIcon(QIcon(":/ANSYS.ico"));
}


//绘图及事件绑定
void AnsysDataPlot::getEarthQuakePlot()
{
    //添加箭头
    ui->customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    //绑定事件
    connect(ui->customPlot,SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)),\
            this,SLOT(showData(QCPLegend*)));
    connect(ui->customPlot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(showPointXY(QMouseEvent*)));
    //设置位置和大小
    this->setGeometry(geometry);
    this->centralWidget()->setFixedSize(geometry.size());
    //设置标题栏
    this->setWindowTitle(title);

    //设置坐标轴标签名称
    ui->customPlot->xAxis->setLabel(xlabel);
    ui->customPlot->yAxis->setLabel(ylabel);
	//设置坐标轴显示范围,否则我们只能看到默认的范围
    ui->customPlot->graph()->rescaleAxes();
    double x_maxval=*max_element(x_max.begin(),x_max.end());
    double y_maxval=*max_element(y_max.begin(),y_max.end());
    double x_minval=*min_element(x_min.begin(),x_min.end());
    double y_minval=*min_element(y_min.begin(),y_min.end());
    ui->customPlot->xAxis->setRange(x_minval*1.2,x_maxval*1.2);
    ui->customPlot->yAxis->setRange(y_minval*1.5,y_maxval*1.5);
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 150));
	//设置交互
    ui->customPlot->setInteractions(QCP::iSelectLegend | QCP::iSelectOther
                                    |QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables|QCP::iSelectItems|QCP::iMultiSelect);
	//重绘
	ui->customPlot->replot();
}


//解析传入的参数
int AnsysDataPlot::parseXYData(const char *xmlstring)
{
    xml_document doc;
    xml_parse_result result=doc.load_string(xmlstring);
    if(result.status!=xml_parse_status::status_ok)
    {
        return 0;
    }
    //根节点 root
    xml_node node_root=doc.document_element();
    //////////////////////窗口设置///////////////////////////////////////////
    //设置窗口标题
    title = QString::fromLocal8Bit(node_root.attribute("title").value());
    //X轴标签
    xlabel = QString::fromLocal8Bit(node_root.attribute("xlabel").value());
    //Y轴标签
    ylabel = QString::fromLocal8Bit(node_root.attribute("ylabel").value());
    //设置父窗体的位置和大小
    QString  geoString(node_root.attribute("geometry").value());
    geoString.replace("x","+");
    QStringList list=geoString.split("+");
    geometry.setRect(list[2].toInt(),list[3].toInt(),list[0].toInt(),list[1].toInt());
    //////////////////////////曲线设置////////////////////////////////////////////////////
    xpath_node_set graphs=doc.select_nodes("//graph");
    for(xpath_node_set::const_iterator graph=graphs.begin();graph!=graphs.end();graph++)
    {
        this->createGraph(graph->node());
    }
    return 1;
}

AnsysDataPlot::~AnsysDataPlot()
{
	delete ui;
}

//状态栏显示数据
void AnsysDataPlot::showData(QCPLegend* plottable)
{
    this->statusBar()->showMessage(plottable->objectName(),2000);
}

//显示点击点的位置
void AnsysDataPlot::showPointXY(QMouseEvent* event)
{
//    QCPGraphDataContainer* points = ui->customPlot->graph(0)->data().data();
//    for(int i=0;i<points->size();i++)
//    {
//        qDebug()<<points->at(i)->key;
//    }
    if(event->button()==Qt::MouseButton::LeftButton)
    {
        double x=ui->customPlot->xAxis->pixelToCoord(event->pos().x());
        double y=ui->customPlot->yAxis->pixelToCoord(event->pos().y());
        QString str=QString("%1--%2").arg(x).arg(y);
        ui->statusBar->showMessage(str,2000);
    }

}

//创建图形
void AnsysDataPlot::createGraph(xml_node node_graph)
{
    //添加一条曲线
    ui->customPlot->addGraph();
    //读取X Y轴数据
    xml_node node_x=node_graph.child("xlist");
    xml_node node_y=node_graph.child("ylist");
    QVector<double> xdata,ydata;
    //读取XML中的X轴和Y轴的字符串数据并进行分割
    QStringList xDataList=QString(node_x.child_value()).split(" ");
    QStringList yDataList=QString(node_y.child_value()).split(" ");
    //转换字符串数据为Double型
    for(int i=0;i<xDataList.length();i++)
    {
        xdata.append(xDataList[i].toDouble());
        ydata.append(yDataList[i].toDouble());
    }
    //将该曲线的最大值加入到x_max y_max列表
    x_max.append(*max_element(xdata.begin(),xdata.end()));
    y_max.append(*max_element(ydata.begin(),ydata.end()));
    //将该曲线的最小值加入到x_min y_min列表
    x_min.append(*min_element(xdata.begin(),xdata.end()));
    y_min.append(*min_element(ydata.begin(),ydata.end()));
    //添加数据
    ui->customPlot->graph()->setData(xdata, ydata);
    //风格配置节点
    xml_node node_style=node_graph.child("style");
     //设置数据的名称
    ui->customPlot->graph()->setName(QString::fromLocal8Bit(node_style.attribute("legend").value()));

    //设置线显示风格
    QString line(node_style.attribute("line").value());
    if (!line.isEmpty()) {
        QStringList linecolors=line.split(" ");
        //线颜色
        QColor line_pen(linecolors[0]);
        //笔刷颜色
        QColor line_brush(linecolors[1]);
        //笔刷透明度
        line_brush.setAlphaF(linecolors[2].toDouble());
        ui->customPlot->graph()->setPen(line_pen);
        ui->customPlot->graph()->setBrush(QBrush(QColor(line_brush)));
    } else {
        ui->customPlot->graph()->setLineStyle(QCPGraph::lsNone);
    }

   //设置数据点风格
    QString point(node_style.attribute("point").value());
    if (!point.isEmpty()) {
        QStringList pointcolors=point.split(" ");
        //点外圈颜色
        QColor point_pen(pointcolors[0]);
        //点内圈颜色
        QColor point_brush(pointcolors[1]);
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,QPen(point_pen),QBrush(point_brush),pointcolors[2].toInt()));
    } else
    {
        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDot));
    }
}


