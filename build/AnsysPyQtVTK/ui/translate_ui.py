#coding:utf-8
'''
用于将UI文件转换为Python 的界面文件
'''
import os
import glob
import re
 
script1="from PyQt5 import QtCore, QtGui, QtWidgets"
script2="from vtk.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor"
script3="self.vtkWidget = QtWidgets.QWidget(self.centerWindow)"
script4="self.vtkWidget = QVTKRenderWindowInteractor(self.centerWindow)"
index=1 

path=os.path.dirname(__file__)
uis=glob.glob(os.path.join(path,"*.ui"))
for ui in uis:
    global out
    name=os.path.basename(ui).split(".")[0]
    out=os.path.join(path,name+".py")
    while os.path.exists(out):
        out=os.path.join(path,"%s-%s.py"%(name,index))
        index+=1
    command="pyuic5 %s -o %s"%(ui,out)
    os.system(command)

content=None
with open(out,'rt') as f:
    content=f.read()
    content=content.replace(script1,script1+"\n"+script2)
    content=content.replace(script3,script4)
with open(out,'wt') as f:
    f.write(content)