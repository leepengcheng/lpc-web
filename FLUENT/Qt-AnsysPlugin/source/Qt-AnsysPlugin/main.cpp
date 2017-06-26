#include <QApplication>
#include "ansysdataplot.h"
#include "matconfigure.h"
#include "fluentparser.h"
using namespace std;
/*主程序入口  启动函数 根据传入的变量启动不同的模块 */
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QTranslator translator;//国际化
    translator.load(":/cn.qm");
    a.installTranslator(&translator);
    //直接点击EXE 无效
    if (argc == 1){return 1;}
    //启动参数
    const char* moduleName=argv[1];
    //启动fluent转换模块
    if (strcmp(moduleName,"fluent")==0)
    {
        FluentParser* fluentParser=new FluentParser();
        fluentParser->show();//显示fluent转换界面
    }
    //启动材料管理模块
    else if(strcmp(moduleName,"mat")==0)
    {
        MatConfigure* matConfig=new MatConfigure();
        matConfig->show();//显示Mat材料配置界面
    }
    else
    {
        //启动曲线绘图模块，传入的参数为xml格式的参数
        AnsysDataPlot *w = new AnsysDataPlot();
        if (w->parseXYData(argv[1])!= 0) {
            //显示地震曲线
            w->getEarthQuakePlot();
            w->show();
        }
    }
    return a.exec();
}
