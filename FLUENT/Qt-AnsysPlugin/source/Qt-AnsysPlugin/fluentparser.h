#ifndef FLUENTPARSER_H
#define FLUENTPARSER_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QRegExp>

namespace Ui {
class FluentParser;
}

class FluentParser : public QMainWindow
{
    Q_OBJECT

public:
    explicit FluentParser(QWidget *parent = 0);
    ~FluentParser();

private:
    Ui::FluentParser *ui;
    QString     separator;//结果文件分隔符
    QStringList results;//结果类型
    QStringList resultList;
    QStringList getDataItems(QString line);
    QHash<QString,QString> resultMapTransformer;
    QMultiHash<int,QVector<int>> meshData;//解析cas文件得到的单元信息
    QHash<QString,int>  boundaryData;//解析cas文件得到的边界单元信息
    QVector<QVector<double>>   nodeData;//解析cas文件得到的节点信息
    const QVector<int> boundaryTypeVector=QVector<int>({4,5,10,20});//边界类型代号及意义
//    4	pressure-inlet, inlet-vent, intake-fan
//    5	pressure-outlet, exhaust-fan, outlet-vent
//    10	velocity-inlet
//    20	mass-flow-inlet
    bool isFluentResultFileInvalid();//判断flunet txt结果文件格式是否正确
    QVector<int> getExportIndexs();
    void exportResult(QString,QString);//导出XML
    void exportMesh(QString,QString);//导出CDB
    void Initialize();//初始化
    void getCasMeshText(QFile&);//解析Cas文件
    void exportProperty(QFile &out);//导出项目信息
    void exportEBlock(QFile &out);//导出单元信息到文件
    void exportNBlock(QFile &out);//导出节点信息到文件
    void exportBBlock(QFile &out);//导出边界单元信息
    QString ValidateInputData();//输入信息验证
};

#endif // FLUENTPARSER_H
