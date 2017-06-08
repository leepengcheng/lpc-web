#ifndef TREEMODEL_H
#define TREEMODEL_H
#include "pugixml.hpp"
#include <QFile>
#include <QTreeWidgetItem>
using namespace pugi;
using namespace std;

class TreeData
{
public:
    TreeData(const char* xmlfile,const char* msgfile);
    QList<QTreeWidgetItem*>      widgetItemList;
    QPixmap getRootPixMap(int row);
    QPixmap getSubPixMap(int row,int col);
private:
    void parseMaterialMsg(const char* msgfile);
    QHash<QString, QString> matNameHashMap;
};

#endif // TREEMODEL_H
