#include "fluentparser.h"
#include "ui_fluentparser.h"
#include "pugixml.hpp"
using namespace pugi;
using namespace std;

/*本模块用于转换fluent数据 */

FluentParser::FluentParser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FluentParser)
{
    ui->setupUi(this);
    this->Initialize();
}

FluentParser::~FluentParser()
{
    delete ui;
}

//解析每一行数据
QStringList FluentParser::getDataItems(QString line)
{
    QStringList items;
    //去除空白项
    for(QString item:line.split(separator)){
        item=item.trimmed();
        if (!item.isEmpty()) {
            items.append(item);
        }
    }
    return items;
}


//检查文件格式是否正确,并返回分隔符和结果类型
bool FluentParser::isFluentResultFileInvalid()
{
    results.clear();//清除数据
    QFile file(ui->lineEdit_txtFile->text());
    if(!file.open(QIODevice::ReadOnly))
    {
        separator="";
        return true;
    }
    //第1行数据
    QString resultLine=file.readLine().trimmed();
    file.close();//关闭
    separator=resultLine.contains(",")?",":resultLine.contains(" ")?" ":"";
    for(QString result:this->getDataItems(resultLine))
    {
        results.append(resultMapTransformer.value(result,result));
    }
    return separator.isEmpty();

}

//查看哪些结果类型被选中
QVector<int> FluentParser::getExportIndexs()
{
    QVector<int> index;
    for (int i = 0; i < ui->treeView->topLevelItemCount(); ++i) {
        if(ui->treeView->topLevelItem(i)->checkState(0)==Qt::Checked)
        {
            index.push_back(i);
        }
    }
    return index;
}

//导出网格文件
void FluentParser::exportMesh(QString cas_file,QString cdb_file)
{
    QFile in(cas_file);
    QFile out(cdb_file);
    if(in.open(QIODevice::ReadOnly))
    {
        this->getCasMeshText(in);//cas解析
        if(out.open(QIODevice::WriteOnly))
        {
            this->exportProperty(out);//写出项目属性
            this->exportNBlock(out);//写出节点信息
            this->exportEBlock(out);//写出单元信息
            this->exportBBlock(out);//写出边界信息
            out.close();
        }
        in.close();
    }


}

//初始化
void FluentParser::Initialize()
{
    this->setFixedSize(this->width(),this->height());//固定窗口大小
    this->setWindowIcon(QIcon(":/ANSYS.ico"));//设置图标
    this->ui->btn_casFile->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    this->ui->btn_cdbFile->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    this->ui->btn_txtFile->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    this->ui->btn_xmlFile->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    this->setWindowTitle(QString::fromLocal8Bit("Fluent结果转换"));
    resultList=QStringList::fromStdList({"Id","Ux","Uy","Uz"});
    this->ui->treeView->setColumnCount(1);//列数
    this->ui->treeView->setColumnWidth(0,150);//宽度
    this->ui->treeView->setHeaderLabel(QString::fromLocal8Bit("结果类型"));//结果标题栏
    this->ui->comboBox_caseID->addItems({"1","2","3","4","5","6","7","8","9"});//工况填充ID
    this->ui->comboBox_Units->addItems({"MPA","SI"});//单位填充

    //将结果类型名称映射到XML元素名称
    resultMapTransformer["nodenumber"]="Id";
    resultMapTransformer["x-coordinate"]="Ux";
    resultMapTransformer["y-coordinate"]="Uy";
    resultMapTransformer["z-coordinate"]="Uz";
    resultMapTransformer["pressure"]="P";
    resultMapTransformer["pressure-coefficient"]="Cp";
    resultMapTransformer["dynamic-pressure"]="Dp";
    resultMapTransformer["absolute-pressure"]="Ap";
    resultMapTransformer["total-pressure"]="Tp";
    ui->checkBox_mesh->setChecked(true);
    ui->checkBox_result->setChecked(true);

    ////////////////////////////////////////信号槽事件/////////////////
    //// 选择fluent 结果输入txt文件
    connect(ui->btn_txtFile,&QPushButton::clicked,[this](){
        QString txtFile = QFileDialog::getOpenFileName(this, tr("Open Fluent Result Text File"), ".", "TxT Files(*.txt)").trimmed();
        ui->lineEdit_txtFile->setText(txtFile);//填充文件路径
        if(txtFile.isEmpty()){return;}//未选择文件
        //检查文件格式并
        if(isFluentResultFileInvalid())
        {
            QMessageBox::information(this,tr("Tips"),tr("Please Check the Format of Fluent Result File!"));
            ui->lineEdit_txtFile->setFocus();//高亮文件路径
            return;
        }
        ui->treeView->clear();
        for(QString result:results)
        {
            QTreeWidgetItem* result_item=new QTreeWidgetItem(QStringList::fromStdList({tr(result.toStdString().c_str())}));
            bool isUseless=resultList.contains(result);
            result_item->setCheckState(0,isUseless?Qt::Unchecked:Qt::Checked);
            ui->treeView->addTopLevelItem(result_item);
            result_item->setHidden(isUseless);
        }
    });
    ////选择ANSYS的xml结果文件保存路径
    connect(ui->btn_xmlFile,&QPushButton::clicked,[this](){
        QString ansysResultFile = QFileDialog::getSaveFileName(this, tr("Open Ansys Result Xml File"), ".", "Xml Files(*.xml)");
        ui->lineEdit_xmlFile->setText(ansysResultFile);
    });
    ////选择导入的fluent CAS文件路径
    connect(ui->btn_casFile,&QPushButton::clicked,[this](){
        QString casFile = QFileDialog::getOpenFileName(this, tr("Open Fluent cas File"), ".", "Cas Files(*.cas)");
        ui->lineEdit_casFile->setText(casFile);
    });
    ////选择导入的ansys CDB文件路径
    connect(ui->btn_cdbFile,&QPushButton::clicked,[this](){
        QString cdbFile = QFileDialog::getSaveFileName(this, tr("Open Ansys cdb File"), ".", "Cdb Files(*.cdb)");
        ui->lineEdit_cdbFile->setText(cdbFile);
    });

    connect(ui->checkBox_mesh,&QCheckBox::clicked,[this](bool ischecked){
        this->ui->groupBox_mesh->setEnabled(ischecked);
    });

    connect(ui->checkBox_result,&QCheckBox::clicked,[this](bool ischecked){
        this->ui->groupBox_result->setEnabled(ischecked);
    });

    //// 导出ansys 结果xml文件和cdb网格
    connect(ui->btn_Export,&QPushButton::clicked,[this](){
        QString msg=this->ValidateInputData();
        if(!msg.isEmpty())
        {
            QMessageBox::information(this, tr("Error"),msg);
            return;
        }
        if(ui->checkBox_mesh->isChecked())
        {
            this->exportMesh(ui->lineEdit_casFile->text(),ui->lineEdit_cdbFile->text());
        }
        if(ui->checkBox_result->isChecked())
        {
            this->exportResult(ui->lineEdit_txtFile->text(),ui->lineEdit_xmlFile->text());
        }

        QMessageBox::information(this, tr("Tips"),tr("Succeccfully Export!"));
    });

    ////退出
    connect(ui->btn_Exit,&QPushButton::clicked,[this](){
        this->close();
    });
}

//解析Cas模型文件中的单元面数据
void FluentParser::getCasMeshText(QFile &in)
{
    nodeData.swap(QVector<QVector<double>>());//清空节点数据
    meshData.swap(QMultiHash<int,QVector<int>>());//清空单元数据
    boundaryData.swap(QHash<QString,int>());//清空边界单元数据
    QRegExp rx("\\(.*\\((.*)?\\)");
    while (!in.atEnd()) {
        QString line=in.readLine();
        //匹配面数据的位置
        if(line.startsWith("(13") && !line.endsWith("))\n"))
        {
            line.indexOf(rx);//正则匹配
            QStringList infoList=rx.cap(1).split(" ");//分割
            int face_start=infoList[1].toInt(0,16);//获得面起始编号
            int face_end=infoList[2].toInt(0,16);//获得面终止编号
            int face_type=infoList[3].toInt(0,16);//获得面的类型
            int node_num=infoList[4].toInt();//获得单元节点数目
            in.readLine();//跳过下一行的")"
            for (int i = face_start; i <= face_end; ++i) {
                line=in.readLine();//读取1行数据
                QVector<int> face;//面
                QStringList nodes=line.trimmed().replace(")","").split(" ");//分割字符串
                int count=node_num?node_num:nodes[0].toInt();//如果node_num=0,单元节点数等于第1个数
                int j=node_num?0:1;//如果node_num=0,则从1开始计数
                for (; j < nodes.size(); ++j) {
                    int node_id=nodes[j].toInt(0,16);//节点ID
                    if(count>0)
                    {
                        face.push_back(node_id);//插入节点到面
                    }
                    else
                    {
                        if(node_id!=0)
                        {
                            meshData.insert(node_id,face);//插入单元面,最后node_id为单元号
                            //插入边界单元，4-入口面  5-出口面  10-速度进口
                            if (boundaryTypeVector.contains(face_type)) {
                                QStringList strList;
                                QVector<int> boundaryFace(face);//复制边界面
                                qSort(boundaryFace.begin(),boundaryFace.end(),qGreater<int>());//排序
                                for (int node:boundaryFace) {
                                    strList.append(QString("%1").arg(node));//转换为字符列表
                                }

                                boundaryData[strList.join("-")]=face_type;//将面节点排序后的字符串作为键值存入哈希表
                            }

                        }
                    }
                    count--;
                }
            }
        }
        //匹配边界信息的位置
        else if(line.startsWith("(10 (") && !line.endsWith("))\n"))
        {
            line.indexOf(rx);//正则匹配
            QStringList infoList=rx.cap(1).split(" ");//分割
            int node_start=infoList[1].toInt(0,16);//获得节点起始编号
            int node_end=infoList[2].toInt(0,16);//获得节点终止编号
            in.readLine();//跳过下一行的")"
            for (int i = node_start; i <= node_end; ++i) {
                line=in.readLine();//读取1行数据
                QVector<double> xyz;
                QStringList nodes=line.trimmed().replace(")","").split(" ");//分割字符串
                for (QString node:nodes) {
                    if(!node.trimmed().isEmpty())
                    {
                        xyz.push_back(node.toDouble());
                    }
                }
                nodeData.push_back(xyz);
            }
        }
    }
}

//写出项目信息到xml
void FluentParser::exportProperty(QFile &out)
{
    QString projectType("!FLUENT EXPORT\n");
    QString projectName("*SET,_PROJECT_NAME,'%1'\n");
    QString projectProperty("*SET,_PROJECT_PROPERTY,'%1'\n");

    out.write(projectType.toStdString().c_str());//项目类型Fluent
    out.write(projectName.arg(ui->lineEdit_projectName->text()).toStdString().c_str());//项目名称
    out.write(projectProperty.arg(ui->lineEdit_projectDetail->text()).toStdString().c_str());//项目描述
}

//写出单元拓扑关系到CDB
void FluentParser::exportEBlock(QFile &out)
{
    int elementid=1;
    //线段排序
    QVector<QVector<int>> orders{{{0,1},{1,2},{2,3},{3,0}}};
    //节点映射
    QHash<int,int> map;
    char buffer[256];//每一个单元的输出行
    QSet<int> keys=meshData.keys().toSet();
    int ele_sum=keys.size();
    //写出EBLOCK标识
    sprintf(buffer,"EBLOCK,%d,SOLID,%10d,%10d\n",19,ele_sum,ele_sum);
    out.write(buffer);
    out.write("(19i9)\n");
    for(int key:keys)
    {
        QList<QVector<int>> ele=meshData.values(key);//单个单元
        int face_num=ele.size();//单元面个数
        QVector<int> f0;
        int n;//计数
        switch (face_num) {
        //根据6组面节点的顺序重构6面体
        case 6:
            map.clear();
            f0=ele[0];//第0个单元面
            for (n= 1; n < 6; ++n)
            {
                for(auto order:orders)
                {
                    int p1=ele[n].at(order[0]);//线段的端点1
                    int p2=ele[n].at(order[1]);//线段的端点2
                    //如果端点1在f0上,端点2不在f0上
                    if(f0.contains(p1) && !f0.contains(p2))
                    {
                        map.insert(p1,p2);
                    }
                    if(f0.contains(p2) && !f0.contains(p1))
                    {
                        map.insert(p2,p1);
                    }
                }
            }
            sprintf(buffer,"%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d\n",
                    1,1,1,1,0,0,0,0,8,0,elementid,f0[0],f0[1],f0[2],f0[3],map[f0[0]],map[f0[1]],map[f0[2]],map[f0[3]]);
            out.write(buffer);
            elementid++;//单元编号+1
            break;
        //根据5组面节点的顺序重构5面体
        case 5:
            map.clear();
            for (auto e:ele)
            {
                if(e.size()==3){
                    f0=e;
                    break;
                }
            }
            for(auto e:ele){
                if(e.size()==3)continue;
                for(auto order:orders)
                {
                    int p1=e.at(order[0]);//线段的端点1
                    int p2=e.at(order[1]);//线段的端点2
                    //如果端点1在f0上,端点2不在f0上
                    if(f0.contains(p1) && !f0.contains(p2))
                    {
                        map.insert(p1,p2);
                    }
                    if(f0.contains(p2) && !f0.contains(p1))
                    {
                        map.insert(p2,p1);
                    }
                }
            }
            sprintf(buffer,"%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d\n",
                    1,1,1,1,0,0,0,0,8,0,elementid,f0[0],f0[1],f0[2],f0[2],map[f0[0]],map[f0[1]],map[f0[2]],map[f0[2]]);
            out.write(buffer);
            elementid++;//单元编号+1
            break;
        case 4:
            f0.append(ele[0]);
            f0.append(ele[1]);
            f0=f0.toList().toSet().toList().toVector();
            sprintf(buffer,"%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d%9d\n",
                    1,1,1,1,0,0,0,0,8,0,elementid,f0[0],f0[1],f0[2],f0[2],f0[3],f0[3],f0[3],f0[3]);
            out.write(buffer);
            elementid++;//单元编号+1
            break;
        }
    }
}

//写出节点坐标位置数据到CDB
void FluentParser::exportNBlock(QFile &out)
{
    int nodeid=1;//节点序号
    char buffer[200];
    int node_sum=nodeData.size();
    sprintf(buffer,"NBLOCK,%d,SOLID,%10d,%10d\n",6,node_sum,node_sum);
    out.write(buffer);
    out.write("(3i9,6e21.13e3)\n");
    //    out.write(QString("NBLOCK,%1,SOLID,%2,%3").arg(6).arg(node_sum,10,10).arg(node_sum,10,10).toStdWString());
    for (auto node:nodeData) {
        sprintf(buffer,"%9d%9d%9d%21.13e%21.13e%21.13e\n",nodeid,0,0,node[0],node[1],node[2]);
        out.write(buffer);
        nodeid++;
    }
}


//输出边界信息
void FluentParser::exportBBlock(QFile &out)
{
    QList<QString> boundaryFaces=boundaryData.keys();
    int faceNum=boundaryFaces.size();
    char buffer[200];
    sprintf(buffer,"BBLOCK,%d,SOLID,%10d,%10d\n",6,faceNum,faceNum);
    out.write(buffer);
    out.write("(1i10,1i40,1i10)\n");
    int faceid=1;//面序号
    for(auto face:boundaryFaces){
        sprintf(buffer,"%10d%40s%10d\n",faceid,face.toStdString().c_str(),boundaryData[face]);
        out.write(buffer);
        faceid++;
    }

}

//输入信息验证
QString FluentParser::ValidateInputData()
{
    if(!ui->checkBox_mesh->isChecked() && !ui->checkBox_result->isChecked()) {return tr("no export type was select!");}
    if(ui->lineEdit_projectName->text().trimmed().isEmpty()){return tr("projectname is empty!");}
    if(ui->checkBox_mesh->isChecked())
    {
        if(ui->lineEdit_casFile->text().trimmed().isEmpty()){return tr("casfile is empty!");}
        if(ui->lineEdit_cdbFile->text().trimmed().isEmpty()){return tr("cdbfile is empty!");}
    }
    if(ui->checkBox_result->isChecked())
    {
        if(ui->lineEdit_txtFile->text().trimmed().isEmpty()){return tr("txtfile is empty!");}
        if(ui->lineEdit_caseName->text().trimmed().isEmpty()){return tr("casename is empty!");}
        if(ui->lineEdit_xmlFile->text().trimmed().isEmpty()){return tr("xmlfile is empty!");}
        if(this->getExportIndexs().length()==0){return tr("result type is empty!");}
    }

    return "";
}

//void FluentParser::initializeDB()
//{
//    //    QString dbfile="d:\\data.db";
//    //    db.setDatabaseName(dbfile);
//    //    QSqlQuery query(db); //以下执行相关QSL语句
//    //    query
//    //    if (db.open()) {
//    //        query.exec("create table if not exists faces ("
//    //                   "id INTEGER PRIMARY KEY AUTOINCREMENT,nodenum TINYINT, elementid INT,"
//    //                   "node0 INT,node1 INT,node2 INT,node3 INT,node4 INT,node5 INT,node6 INT,node7 INT)");
//    ////        query.exec("insert into faces values(NULL, 1,2,3,4,5,6,7,8,9,10)");
//    ////    }
//    //    query.exec("select * from faces");
//    //    while (query.next()) {
//    //        qDebug()<<query.value(0).toInt();
//    //    }
//    //    db.close();
//}

//导出结果文件
void FluentParser::exportResult(QString txt_file,QString xml_file)
{
    QFile txtfile(txt_file);
    if(!txtfile.open(QIODevice::ReadOnly) || separator.isEmpty())
    {
        return;
    }
    QVector<int> index=getExportIndexs();
    xml_document doc;
    xml_node node_declare=doc.prepend_child(node_declaration);
    node_declare.append_attribute("version")="1.0";
    node_declare.append_attribute("encoding")="UTF-8";
    xml_node node_root=doc.append_child("Results");//Results根节点
    xml_node node_project=node_root.append_child("Project");//项目节点
    node_project.append_attribute("name")=ui->lineEdit_projectName->text().toStdString().c_str();//项目节点的name属性
    xml_node node_case=node_project.append_child("Case");//工况节点
    node_case.append_attribute("id")=ui->comboBox_caseID->currentText().toInt();//工况节点的id属性
    node_case.append_attribute("name")=ui->lineEdit_caseName->text().toStdString().c_str();//工况节点的name属性
    node_case.append_attribute("property")=ui->lineEdit_caseDetail->text().toStdString().c_str();//工况节点的property属性
    txtfile.readLine();//跳过第一行表头
    int count=0;
    while(!txtfile.atEnd())
    {
        QStringList nodeDataList=this->getDataItems(txtfile.readLine().trimmed());
        if(nodeDataList.length())
        {
            count++;
            xml_node node_node=node_project.append_child("Node");//节点节点
            node_node.append_attribute("id")=nodeDataList[0].toStdString().c_str();
            for(int i:index)
            {
                node_node.append_child(results[i].toStdString().c_str()).text()=nodeDataList[i].toStdString().c_str();
            }
            if(count%100==0){this->statusBar()->showMessage(QString("Node%1").arg(count));}
        }
    }
    doc.save_file(xml_file.toStdString().c_str(),"\t",format_no_escapes,encoding_utf8);
    this->statusBar()->showMessage(tr("Result File Successfully Saved!"),1000);
}
