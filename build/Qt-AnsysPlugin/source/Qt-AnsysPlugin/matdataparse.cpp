#include "matdataparse.h"
#include <QDir>
//#include <random>
//#include <ctime>


/*材料管理模块树形控件创建及填充 */

TreeData::TreeData(const char *xmlfile, const char *msgfile)
{
    xml_document doc;
    xml_parse_result result=doc.load_file(xmlfile);
    if(result.status!=xml_parse_status::status_ok)
    {
        return;
    }
    //节点翻译字典
    this->parseMaterialMsg(msgfile);//获得材料的id 和property
    xpath_node_set type_set=doc.select_nodes("//Type");
    int row=0;
    int col=0;
    for(auto node:type_set)
    {
        xml_node type_node=node.node();
        QString type_key=type_node.attribute("id").value();
        QString type_name=matNameHashMap.value(type_key,type_key);
        QTreeWidgetItem* type_item=new QTreeWidgetItem(QStringList({type_name,""}));
//        type_item->setIcon(0,getRootPixMap(row));//不设置图标 省的扯皮
        for(auto mat_node:type_node.children())
        {
            QString mat_key=mat_node.attribute("id").value();
            QString mat_name=matNameHashMap.value(mat_key,mat_key);
            QString mat_data =mat_node.text().as_string();
            QTreeWidgetItem* mat_item=new QTreeWidgetItem(QStringList({mat_name,mat_data}));
//            mat_item->setIcon(0,getSubPixMap(row,col));//不设置图标 省的扯皮
            mat_item->setToolTip(0,mat_data);//提示
            type_item->addChild(mat_item);
            col++;
        }
        this->widgetItemList.push_back(type_item);
        row++;
    }
}

//解析MSG文件
void TreeData::parseMaterialMsg(const char* msgfile)
{
    matNameHashMap.clear();//清除字典
    QFile file(msgfile);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return;
    }
    while(!file.atEnd())
    {
        QByteArray line=file.readLine().trimmed();
        if (line.contains("{") || line.contains("}"))
        {
            continue;
        }
        else
        {
            QList<QByteArray> strList=line.split(' ');
            QString key=strList[0].replace("\"","");
            QString val=(*strList.rbegin()).replace("\"","");
            matNameHashMap.insert(key,val);
        }
    }
}

//获得材料类型下某种材料图标
QPixmap TreeData::getSubPixMap(int row, int col)
{
    QPixmap* pixMap=new QPixmap(":/sprite.png");
    int rowNum=7;
    int colNum=11;
    int r=row%rowNum;//取余
    int c=col%(colNum-1)+1;//取余
    return pixMap->copy(c*64,r*64,64,64);

}

//获得材料类型的图标
QPixmap TreeData::getRootPixMap(int row)
{
    QPixmap* pixMap=new QPixmap(":/sprite.png");
    int rowNum=7;
    int r=row%rowNum;//取余
    return pixMap->copy(0,r*64,64,64);
}

