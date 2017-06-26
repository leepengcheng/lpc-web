#include "matconfigure.h"
#include "dataimport.h"


/*材料管理模块主界面逻辑 */

MatConfigure::MatConfigure(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MatConfigure)
{
    ui->setupUi(this);
    this->InitializeWindow();//初始化Tree
    this->LoadMaterial();//载入本地数据
}
MatConfigure::~MatConfigure()
{
    delete ui;
}

//检查材料名或者材料类型名是否存在
bool MatConfigure::IsMaterialExist(QString matName,QTreeWidgetItem* parent)
{
    int num=parent?parent->childCount():ui->treeWidget_Mat->topLevelItemCount();
    for (int j = 0; j < num; ++j) {
        QTreeWidgetItem* matItem=parent?parent->child(j):ui->treeWidget_Mat->topLevelItem(j);
        //添加材料时,需要判断材料类型和材料名称
        if(matName.trimmed().toUpper()==matItem->text(0).trimmed().toUpper())
            return true;
    }
    return false;
}


//数据验证
QString MatConfigure::PreValidation(QString title,QString message,QString text,QString tips)
{
    bool isOk;//是否点击了确定按钮
    QString input=QInputDialog::getText(this,title,message,QLineEdit::Normal,text,&isOk).trimmed();
    //按下取消按钮
    if(!isOk)
    {
        return "";
    }
    //输入为空
    if (input.isEmpty() ||input.contains(" ")){
        QMessageBox::information(this,tr("Error"),tips);
        return "";
    }
    return input;
}

//显示隐藏同名称的控件
void MatConfigure::ShowPropertyControls(bool isShow)
{
    for(auto key:keyList)
    {
        ui->frame_Mat->findChild<QLineEdit*>("lineEdit_"+key)->setVisible(isShow);
        ui->frame_Mat->findChild<QCheckBox*>("checkBox_"+key)->setVisible(isShow);
        ui->frame_Mat->findChild<QLabel*>("label_"+key)->setVisible(isShow);
    }
}

//初始化窗体
void MatConfigure::InitializeWindow()
{
    this->setFixedSize(this->width(),this->height());//固定窗口大小
    this->setWindowIcon(QIcon(":/ANSYS.ico"));//设置图标
    this->ui->label_ICON->setVisible(false);//隐藏图标 以免扯皮
    QToolBar* bar=addToolBar(tr("File"));
    bar->addAction(ui->actionLoad);
    bar->addAction(ui->actionSave);
    bar->addAction(ui->actionImport);
    bar->addAction(ui->actionExport);
    keyList=QStringList({"EX","PRXY","DENS","YS","TM"});
    for(auto key:keyList)
    {
        QLineEdit* lineEdit=ui->frame_Mat->findChild<QLineEdit*>("lineEdit_"+key);
        lineEdit->setValidator(new QDoubleValidator());//输入验证
        QCheckBox* checkBox=ui->frame_Mat->findChild<QCheckBox*>("checkBox_"+key);
        connect(checkBox,SIGNAL(toggled(bool)),lineEdit,SLOT(setEnabled(bool)));
        lineEditMap.insert(key,lineEdit);
        checkBoxMap.insert(key,checkBox);
    }
    ui->treeWidget_Mat->setColumnCount(2);//设置为2列
    ui->treeWidget_Mat->setHeaderLabels(QStringList({tr("Material"),tr("Property")}));//设置标题
    ui->treeWidget_Mat->hideColumn(1);//隐藏第2列
    ui->treeWidget_Mat->setContextMenuPolicy(Qt::CustomContextMenu);//启用右键菜单
    //////////////////////////////////事件//////////////////////////////////////////////////////////////////////////////
    //点击载入材料属性
    connect(ui->treeWidget_Mat,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(LoadSelectItem(QTreeWidgetItem*)));
    //提交修改材料
    connect(ui->pushButton_EDIT,SIGNAL(clicked(bool)),this,SLOT(EditMaterial()));
    //右键菜单事件
    connect(ui->treeWidget_Mat,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowContextMenu()));
    //加载本地材料
    connect(ui->actionLoad,SIGNAL(triggered(bool)),this,SLOT(LoadMaterial()));
    //保存本地材料
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(SaveMaterial()));
    //导入外部材料
    connect(ui->actionImport,SIGNAL(triggered(bool)),this,SLOT(ImportMaterial()));
    //保存本地材料
    connect(ui->actionExport,SIGNAL(triggered(bool)),this,SLOT(ExportMaterial()));
    //点击item时的statusbar状态提示
    connect(ui->treeWidget_Mat,&QTreeWidget::itemClicked,this,[this](QTreeWidgetItem* item){
        if(!item->parent())
        {
            return;
        }
        QString tips;
        QStringList list=item->text(1).split(" ");
        for (int i = 0; i < list.length()/2; ++i) {
            tips.append(QString("<%1 %2> ").arg(list[2*i],list[2*i+1]));
        }
        this->statusBar()->showMessage(tr("Material Property ").prepend(item->text(0)).append(tips),1000);});
}

//写出材料配置文件
void MatConfigure::WriteMaterial(QString xml_file, QString msg_file)
{
    QFile msgfile(msg_file);
    msgfile.open(QIODevice::WriteOnly);
    msgfile.write("::msgcat::mcmset zh_CN {\n");
    xml_document doc;
    xml_node node_declare=doc.prepend_child(node_declaration);
    node_declare.append_attribute("version")="1.0";
    node_declare.append_attribute("encoding")="UTF-8";

    xml_node node_root=doc.append_child("Root");
    node_root.append_attribute("units")="SI";

    for (int i = 0; i < ui->treeWidget_Mat->topLevelItemCount(); ++i) {
        QTreeWidgetItem* typeItem=ui->treeWidget_Mat->topLevelItem(i);
        xml_node typeNode=node_root.append_child("Type");
        QString type_id=QString("%1root").arg(i+1);
        typeNode.append_attribute("id")=type_id.toStdString().c_str();
        msgfile.write(QString("\"%1\"       \"%2\"\n").arg(type_id).arg(typeItem->text(0)).toStdString().c_str());
        for (int j = 0; j < typeItem->childCount(); ++j) {
            QTreeWidgetItem* matItem=typeItem->child(j);
            xml_node matNode=typeNode.append_child("Mat");
            QString mat_id=QString("%1n%2").arg(i+1).arg(j+1);
            matNode.append_attribute("id")=mat_id.toStdString().c_str();
            matNode.text()=matItem->text(1).toStdString().c_str();
            msgfile.write(QString("\"%1\"       \"%2\"\n").arg(mat_id).arg(matItem->text(0)).toStdString().c_str());
        }
    }
    msgfile.write("}");
    msgfile.close();
    doc.save_file(xml_file.toStdString().c_str());

}

//选中材料后载入材料属性
void MatConfigure::LoadSelectItem(QTreeWidgetItem* selItem)
{
    QTreeWidgetItem *parentItem=selItem->parent();
    ui->label_ICON->setPixmap(selItem->icon(0).pixmap(QSize(64,64)));
    ShowPropertyControls(parentItem);//显示属性页
    if (parentItem)
    {
        ui->lineEdit_TYPE->setText(parentItem->text(0));
        ui->lineEdit_NAME->setText(selItem->text(0));
        ui->lineEdit_NAME->setVisible(true);
        ui->label_NAME->setVisible(true);
        ui->pushButton_EDIT->setVisible(true);
        QStringList properties=selItem->text(1).split(" ");
        QStringList transproperties;
        for_each(properties.begin(),properties.end(),[&](QString str){if(str!="")transproperties.append(str);});
        for(QString key:keyList)
        {
            int keyIndex=transproperties.indexOf(key);
            bool hasKey=keyIndex!=-1;
            QString value=hasKey?transproperties[keyIndex+1]:"";
            checkBoxMap[key]->setChecked(hasKey);
            lineEditMap[key]->setText(value);
        }
    }
    else
    {
        ui->lineEdit_TYPE->setText(selItem->text(0));
        ui->lineEdit_NAME->setVisible(false);
        ui->label_NAME->setVisible(false);
        ui->pushButton_EDIT->setVisible(false);
    }
}

//编辑材料
void MatConfigure::EditMaterial()
{
    QTreeWidgetItem *item=ui->treeWidget_Mat->currentItem();
    if (!item)
    {
        QMessageBox::information(this,tr("Tips"),tr("Please Select Mat First"));
        return;
    }
    QString matType=ui->lineEdit_TYPE->text().trimmed();
    QString matName=ui->lineEdit_NAME->text().trimmed();
    if(matType.isEmpty() || matName.isEmpty())
    {
        return;
    }
    QStringList dataList;
    int count=0;
    for(QString key:keyList)
    {
        bool isChecked=checkBoxMap[key]->isChecked();
        QString value=lineEditMap[key]->text().trimmed();
        if (isChecked && !value.isEmpty())
        {
            dataList.append(key);
            dataList.append(value);
            count++;
        }
    }
    if(count==0)
    {
        QMessageBox::information(this,tr("Tips"),tr("At Least 1 Property Shoule Be input!"));
        return;
    }
    QString data(dataList.join(" "));
    item->setText(1,data);//设置属性
    item->setToolTip(0,data);//属性提示
    this->statusBar()->showMessage(tr("Successfully Update!").prepend(matName+" "),1000);
    this->isSaved=false;
}


//右键菜单
void MatConfigure::ShowContextMenu()
{
    QTreeWidgetItem* selItem=ui->treeWidget_Mat->currentItem();
    QString selText;
    int selRow = -1;
    QTreeWidgetItem* selParent = NULL;
    if (selItem)
    {
        selText = selItem->text(0).trimmed();//选择项的名称
        selRow = ui->treeWidget_Mat->currentIndex().row();//选择项的行数
        selParent = selItem->parent();//父节点
    }

    /////action函数
    //添加材料
    auto funcaddMat=[&](){
        QString mat_name=PreValidation(tr("Create Material"),tr("Please Input Material Name:"),
                                       tr("Material").append("1"),tr("Material Name Can Not Be Empty!"));
        if(!mat_name.isEmpty())
        {
            QTreeWidgetItem *parent=selParent?selParent:selItem;
            //            int prow=ui->treeWidget_Mat->indexOfTopLevelItem(parent);//所在材料类型的row
            int row=selParent?selRow+1:parent->childCount();//当前材料的子行数
            QString type_name=parent->text(0);
            //材料名称已存在
            if (IsMaterialExist(mat_name,parent)) {
                QMessageBox::information(this,tr("Error"),QString("%1-%2 %3").arg(type_name,mat_name,tr("Is Exist!")));
                return;
            }
            else
            {
                QString data("EX 2.06E5 PRXY 0.3");//默认属性
                QTreeWidgetItem* newMatItem=new QTreeWidgetItem(QStringList({mat_name,data}));
                newMatItem->setToolTip(0,data);
                //                newMatItem->setIcon(0,treeData->getSubPixMap(prow,parent->childCount()));//不设置图标 省的扯皮
                parent->insertChild(row,newMatItem);
                ui->treeWidget_Mat->setCurrentItem(newMatItem);
                ui->treeWidget_Mat->itemClicked(newMatItem,0);
                this->statusBar()->showMessage(QString("<%1-%2> ").arg(type_name).arg(mat_name).append(tr("Succeccfully Create!")),1000);
            }
        }};
    //添加材料类型
    auto funcaddMatType=[&](){
        QString mattype=PreValidation(tr("Create Material Type"),tr("Please Input Material Type Name:"),
                                      tr("Material Type").append("1"),tr("Material Type Name Can Not Be Empty!"));
        if(!mattype.isEmpty())
        {
            if (IsMaterialExist(mattype)) {
                QMessageBox::information(this, tr("Error"), mattype.append(tr("Is Exist!")));
                return;
            }
            else
            {
                QTreeWidgetItem* newMatTypeItem = new QTreeWidgetItem(QStringList({ mattype, "" }));
                //                newMatTypeItem->setIcon(0, treeData->getRootPixMap(ui->treeWidget_Mat->topLevelItemCount())); //不设置图标 省的扯皮
                ui->treeWidget_Mat->insertTopLevelItem(selRow + 1, newMatTypeItem);
                this->statusBar()->showMessage(QString("<%1> ").arg(mattype).append(tr("Succeccfully Create!")), 1000);
            }
        }};

    //删除材料
    auto funcDelMat=[&](){
        delete selParent->takeChild(selRow);
        this->statusBar()->showMessage(QString("%1<%2> %3").arg(tr("Material")).arg(selText).arg(tr("Is Delete")),1000);
    };
    //删除材料类型
    auto funcDelMatType=[&](){
        delete ui->treeWidget_Mat->takeTopLevelItem(selRow);
        this->statusBar()->showMessage(QString("%1<%2> %3").arg(tr("Material Type")).arg(selText).arg(tr("Is Delete")),1000);
    };

    //重命名材料
    auto funcRenameMat=[&](){
        QString newmatname=PreValidation(tr("Rename Material"),tr("Please Input New Material Name:"),
                                         selText,tr("Material Name Can Not Be Empty!"));
        if(!newmatname.isEmpty())
        {
            if(selText.toUpper()!=newmatname.toUpper())
            {
                if(!IsMaterialExist(newmatname,selParent))
                {
                    selItem->setText(0,newmatname);
                    this->statusBar()->showMessage(QString("%1<%2> %3 <%4>").arg(tr("Material")).arg(selText).arg(tr("Rename to")).arg(newmatname),1000);
                    LoadSelectItem(selItem);
                }
                else
                {
                    QMessageBox::information(this,tr("Error"),QString("%1-%2 %3").arg(selParent->text(0),newmatname,tr("Is Exist!")));
                    return;
                }

            }


        }
    };

    //重命名材料类型
    auto funcRenameMatType=[&](){
        QString newtypename=PreValidation(tr("Rename Material Type"),tr("Please Input New Material Type Name:"),
                                          selText,tr("Material Type Name Can Not Be Empty!"));
        if(!newtypename.isEmpty())
        {
            if(selText.toUpper()!=newtypename.toUpper())
            {
                if(!IsMaterialExist(newtypename))
                {
                    selItem->setText(0,newtypename);
                    LoadSelectItem(selItem);
                    this->statusBar()->showMessage(QString("%1<%2> %3 <%4>").arg(tr("Material Type")).arg(selText).arg(tr("Rename to")).arg(newtypename));
                }
                else
                {
                    QMessageBox::information(this,tr("Error"),newtypename.append(tr("Is Exist!")));
                    return;
                }

            }
        }
    };

    QMenu *menu = new QMenu();//右键菜单
    //如果选中了节点，且该节点有父节点，则为材料节点
    if(selItem && selParent)
    {
        menu->addAction(QIcon(":/addmat.png"),tr("Add Mat"),funcaddMat);
        menu->addAction(QIcon(":/delmat.png"),tr("Del Mat"),funcDelMat);
        menu->addAction(QIcon(":/rename.png"),tr("Rename"),funcRenameMat);
    }
    //如果选中了节点，且该节点没有父节点，则为材料类型节点
    else if(selItem && !selParent)
    {
        menu->addAction(QIcon(":/addtype.png"),tr("Add MatType"),funcaddMatType);
        menu->addAction(QIcon(":/deltype.png"), tr("Del MatType"), funcDelMatType);
        menu->addAction(QIcon(":/addmat.png"), tr("Add Mat"), funcaddMat);
        menu->addAction(QIcon(":/rename.png"), tr("Rename"), funcRenameMatType);
    }
    //如果没有选择节点
    else
    {
        menu->addAction(QIcon(":/addtype.png"), tr("Add MatType"), funcaddMatType);
    }
    menu->exec(QCursor::pos());
}

//保存至本地材料库
void MatConfigure::SaveMaterial()
{
    if(!ui->treeWidget_Mat->topLevelItemCount())
    {
        QMessageBox::information(this,tr("Tips"),tr("No Material Was Defined!"));
        return;
    }
    QFileInfo xmlinfo(ansysXmlfile);
    QFileInfo msginfo(ansysMsgfile);
    if(!xmlinfo.dir().exists())
    {
        xmlinfo.dir().mkpath(xmlinfo.absolutePath());
    }
    if(!msginfo.dir().exists())
    {
        msginfo.dir().mkpath(msginfo.absolutePath());
    }
    int qu=QMessageBox::question(this,tr("Tips"),tr("Really Override?"));
    if (qu==QMessageBox::No)
    {
        return;
    }
    this->WriteMaterial(ansysXmlfile,ansysMsgfile);
    this->isSaved=true;//保存标示位
    QMessageBox::information(this,tr("Tips"),tr("Successfully Saved!"));
}

//加载本地材料
void MatConfigure::LoadMaterial()
{
    QFile xmlfile(ansysXmlfile);
    QFile msgfile(ansysMsgfile);
    if(xmlfile.exists() && msgfile.exists())
    {
        this->PopulateTreeData(ansysXmlfile, ansysMsgfile);//载入本地数据
    }
    else
    {
        this->statusBar()->showMessage(tr("Load Local Material Failed!"), 5000);
    }
    this->isSaved = true;//载入本地数据

}
//导出本地材料
void MatConfigure::ExportMaterial()
{
    QString filepath = QFileDialog::getExistingDirectory(this,tr("Select Mat Save Dir"),"C:\\ProgramData\\AnsysPlugin");
    if(!filepath.isEmpty())
    {
        this->WriteMaterial(filepath+"//mat.xml",filepath+"//zh_cn.msg");
        QMessageBox::information(this,tr("Tips"),tr("Successfully Export!"));
    }
}

//导入外部材料
void MatConfigure::ImportMaterial()
{
    DataImport* dataimport=new DataImport(this);
    dataimport->show();
}

//清空并重新加载材料配置文件
void MatConfigure::PopulateTreeData(const char* xmlfile,const char* msgfile)
{
    treeData=new TreeData(xmlfile,msgfile);//解析XML和MSG文件
    ui->treeWidget_Mat->clear();
    ui->treeWidget_Mat->addTopLevelItems(treeData->widgetItemList);
    //点击第种材料类型的第一种子材料
    QTreeWidgetItem* firstItem=ui->treeWidget_Mat->topLevelItem(0)->child(0);
    if(firstItem)
    {
        ui->treeWidget_Mat->setCurrentItem(firstItem);
        ui->treeWidget_Mat->itemClicked(firstItem,0);
    }
    this->statusBar()->showMessage(tr("Successfully Loading Material!"),1500);
    this->isSaved=false;


}

//窗口关闭时检查是否保存
void MatConfigure::closeEvent(QCloseEvent *event)
{
    if(this->isSaved)
    {
        event->accept();
        return;
    }
    int r=QMessageBox::question(this,tr("Tips"),tr("File Not Save! Really quit?"));
    if (r==QMessageBox::Yes)
        event->accept();
    else
    {
        event->ignore();
    }


}
