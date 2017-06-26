# coding:utf-8
'''
将原始tcl脚本转换为c++程序
'''
import glob
import os

##########版本:A-中南院 B-艾泰克####
VERSION="A"  ####修改此处
##########################################
DATA={
    "A":[("int","char*"),("licchecker","IsLicenseValid",'libFun("")')],
    "B":[("bool",""),("Decode","CheckRegister",'libFun()?0:1')]
}
HEADER="typedef %s(*Fun)(%s);"
CODE='''
    HINSTANCE mylib = LoadLibrary(L"%s.dll");
    if (mylib == NULL)
    {
        return -1;
    }
    Fun libFun = (Fun)GetProcAddress(mylib, "%s");
    return %s;
    '''
############################################


def translate(tcl_file):
    result = ""
    with open(tcl_file, 'rt') as f:
        for line in f.readlines():
            line = line.strip()
            # 如果为空行/注释行 则跳过
            if line == "" or line.startswith("#"):
                continue
            # 传入脚本路径
            if line.startswith("set path"):
                result += 'cout << "set path "<<path << endl;\n'
                continue
            if line == "ans_sendcommand $modulename":
                line = 'eval [exec [file join $path "AnsysScript.exe" ] $modulename $path]'
            # 换行符\转换为\\
            line = line.replace("\\", "\\\\")
            # 双引号"转换为\\
            line = line.replace('"', '\\\"')
            result += 'cout<<"%s"<<endl;\n' % line
    return result



dirpath=os.path.dirname(__file__)
template_path=os.path.join(dirpath,"ansys_script.template.cpp")
ansys_dirpath=os.path.dirname(dirpath)
tcl_path=os.path.join(ansys_dirpath,"AnsysPlugin\source\AnsysPlugin\Tcl\*.tcl") #tcl源文件的地址
# 读取cpp模板
with open(template_path, 'rt') as f:
    template = f.read()
for tclfile in glob.glob(tcl_path):
    basename = os.path.basename(tclfile).split(".")[0]
    flag = "//@@%s" % basename
    if flag in template:
        funcstr = translate(tclfile)
        template = template.replace(flag, funcstr)
    else:
        print("模板中不存在<%s>模块!" % basename)

PARAM=DATA[VERSION]
HEADER,CODE=HEADER%PARAM[0],CODE%PARAM[1]
template=template.replace("//@@HEADER",HEADER)
template=template.replace("//@@DECODE",CODE)
maincpp_path=os.path.join(ansys_dirpath,"AnsysScript\AnsysScript\main.cpp") #生成的cpp的路径
with open(maincpp_path, 'wt') as f:
    f.write(template)
print u"转换完毕!!!"