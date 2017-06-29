#ifndef MATCONFIGURE_H
#define MATCONFIGURE_H

#include <QMessageBox>
#include "ui_matconfigure.h"
#include "algorithm"
#include "matdataparse.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QCloseEvent>
#include <QToolBar>
#include <QMenu>
namespace Ui {
class MatConfigure;
}

class MatConfigure : public QMainWindow
{
    Q_OBJECT

public:
    explicit MatConfigure(QWidget *parent = 0);
    void PopulateTreeData(const char*,const char*);
    ~MatConfigure();
private:
    Ui::MatConfigure *ui;//uiΩÁ√Ê
    QStringList keyList;
    TreeData* treeData;
    QHash<QString,QLineEdit*> lineEditMap;
    QHash<QString,QCheckBox*> checkBoxMap;
    bool isSaved=false;
    const char* ansysXmlfile="C:\\ProgramData\\AnsysPlugin\\Tcl\\mat.xml";
    const char* ansysMsgfile="C:\\ProgramData\\AnsysPlugin\\Tcl\\locale\\material\\zh_cn.msg";
    bool  IsMaterialExist(QString,QTreeWidgetItem* parent=NULL);
    QString  PreValidation(QString,QString,QString,QString);
    void  ShowPropertyControls(bool);
    void  InitializeWindow();
    void  WriteMaterial(QString,QString);
private slots:
    void LoadSelectItem(QTreeWidgetItem*);
    void EditMaterial();
    void ShowContextMenu();
    void SaveMaterial();
    void LoadMaterial();
    void ExportMaterial();
    void ImportMaterial();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MATCONFIGURE_H
