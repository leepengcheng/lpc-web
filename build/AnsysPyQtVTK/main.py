# coding:utf-8
import os
import sys
import vtk
from util import *
from domain import *
from config import *
from ConfigParser import ConfigParser
from collections import defaultdict,Counter
from dataparser import DataParser
from model import Model
from section import Section
#界面部分
from PyQt5 import QtCore, QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow,QFileDialog,QAction
from ui.ui_main import Ui_mainWindow

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        QtWidgets.QMainWindow.__init__(self, parent)
        self.ui = Ui_mainWindow()
        self.ui.setupUi(self)
        self.ren = vtk.vtkRenderer()
        self.ren.SetBackground(0.1, 0.2, 0.4)  #背景色
        self.ui.vtkWidget.GetRenderWindow().AddRenderer(self.ren)
        self.iren = self.ui.vtkWidget.GetRenderWindow().GetInteractor()
        #设置鼠标事件
        interactorStyle = vtk.vtkInteractorStyleTrackballCamera()
        self.iren.SetInteractorStyle(interactorStyle)  #交互类型
        self.iren.AddObserver('LeftButtonPressEvent', self.leftButtonClick,
                              1.0)  #交互事件
        self.sel = Selection()  #拾取对象初始化
        self.cmd = CMD.SHOW_NODEINFO  #全局命令参数,默认为显示节点信息
        self.initializeSignals()  #初始化信号槽链接
        ###圆心 起点 终点输入框
        self.actors = defaultdict(list)  #维护的全局actors字典,用于actor的动态显示/隐藏等操作
        self.edits = {
            "center": (self.ui.edit_center_x, self.ui.edit_center_y,
                       self.ui.edit_center_z),
            "start": (self.ui.edit_start_x, self.ui.edit_start_y,
                      self.ui.edit_start_z),
            "end": (self.ui.edit_end_x, self.ui.edit_end_y,
                    self.ui.edit_end_z),
            "p123":(self.ui.edit_p1,self.ui.edit_p2,self.ui.edit_p3),
            "cdb":(self.ui.edit_cdbfile,),
            "xml":(self.ui.edit_xmlfile,),
            "xspace": (self.ui.edit_xspace, ),
            "yspace": (self.ui.edit_yspace, )
        }
        self.loadConfig()#加载配置

    def leftButtonClick(self, obj, event):
        '''
        左键交互事件
        '''
        clickPos = self.iren.GetEventPosition()  #获得点击位置
        picker = vtk.vtkPropPicker()
        picker.Pick(clickPos[0], clickPos[1], 0, self.ren)
        self.sel.current = picker.GetActor()  #获得选择的对象
        # print self.ren.GetWorldPoint()
        if self.sel.current:
            color = self.sel.getColor()
            if color == YELLOW:  #节点
                self.executeUserCommand()  #执行节点命令
                self.sel.cancelhightLight()  #取消高亮上个对象
                self.sel.hightLight()  #高亮当前对象
                self.sel.update()  #将选择的对象加入previous
            elif color == GREEN:  #单元面
                pass

    def initializeSignals(self):
        '''
        初始化信号槽
        '''
        self.ui.check_section.clicked.connect(self.showSection)
        self.ui.check_model.clicked.connect(self.showModel)
        self.ui.check_polar.clicked.connect(self.showPolarLine)
        self.ui.check_boundary.clicked.connect(self.showBoundary)
        self.ui.check_polarsys.clicked.connect(lambda arg:self.showCoordinate(arg,"cylinder_sys"))
        self.ui.check_localsys.clicked.connect(lambda arg:self.showCoordinate(arg,"local_sys"))
        self.ui.check_worldsys.clicked.connect(lambda arg:self.showCoordinate(arg,"world_sys"))
        self.ui.btn_saveconf.clicked.connect(self.saveConfig)
        self.ui.btn_cdbfile.clicked.connect(lambda:self.getOpenFilePath("cdb"))
        self.ui.btn_xmlfile.clicked.connect(lambda:self.getOpenFilePath("xml"))
        #选择圆心 起点 终点
        self.ui.btn_center.clicked.connect(
            lambda: self.setCommand(CMD.GET_CENTER))
        self.ui.btn_start.clicked.connect(
            lambda: self.setCommand(CMD.GET_START))
        self.ui.btn_end.clicked.connect(lambda: self.setCommand(CMD.GET_END))
        self.ui.btn_selnode.clicked.connect(lambda: self.setCommand(CMD.SHOW_NODEINFO))
        self.ui.combo_infosys.currentTextChanged.connect(self.executeUserCommand)
        self.ui.combo_infotype.currentTextChanged.connect(self.executeUserCommand)
    
    def executeUserCommand(self):
        #执行命令
        if self.sel.current is None:
            return
        wxyz = self.sel.getCenter()
        if self.cmd == CMD.SHOW_NODEINFO:
            sys=self.ui.combo_infosys.currentText()
            display=self.ui.combo_infotype.currentText()
            info=self.section.getNodeData(wxyz,sys,display)
            self.ui.edit_result.setPlainText(info)
        elif self.cmd == CMD.HIDE_NODE:
            self.sel.current.SetVisibility(False)  #隐藏
        elif self.cmd == CMD.GET_CENTER:
            setLineEidtText(self, wxyz, "center")
        elif self.cmd == CMD.GET_START:
            setLineEidtText(self, wxyz, "start")
        elif self.cmd == CMD.GET_END:
            setLineEidtText(self, wxyz, "end")

    def showPolarLine(self,isshow):
        "创建柱坐标数值输出线"
        self.loadImportData("section")
        pts=[]
        # self.section.getSectionOuterLines()
        # lines=self.section.secOuterlines
        # for l in lines:
        #     pts.extend(l)
        # counter=Counter(pts)
        



        
        if isshow:
            if not "section" in self.actors.keys():
                self.showSection(True)
            actors=[]
            params=self.getSectionInputData()
            if not params is None:
                pt_size=float(self.ui.edit_ptsize.text())
                sec_actor = self.section.createPolarLineActor(*params,r=pt_size)
                actors.extend(sec_actor)                              
                self.showActors("polar", actors)
        else:
            self.hideActors("polar")

    def showCoordinate(self,isshow,sysname="world_sys"):
        if isshow:
            
            if sysname=="local_sys":
                p123 = getLineEidtData(self,"p123",int).tolist()
                params=getNodesNDXYZ(self.section,*p123)
            elif sysname=="cylinder_sys":
                params=self.getSectionInputData()[:3]
            else:
                params=[np.array((0,0,0)),np.array((1,0,0)),np.array((0,1,0))]
            actors=[]
            syssize=int(self.ui.edit_syssize.text())
            axis_actor=createAxisActor(*params,scale=syssize) #创建坐标系
            actors.append(axis_actor)
            self.showActors(sysname, actors)
        else:
            self.hideActors(sysname)


    def showSection(self,isshow):
        '''
        创建并显示截面
        '''
        self.loadImportData("section")
        if isshow:
            p123 = getLineEidtData(self,"p123",int)
            #如果没有输入截面节点
            if p123 is None:
                showMessage(self,CMD.GET_SEC)
                self.ui.check_section.setChecked(False)
                return
            p1,p2,p3=p123.tolist()
            actors=[]
            sec_actor=self.section.createSectionActor(p1, p2, p3, showLine=True)
            secnode_actor=self.section.createSectionNodeActors()
            actors.append(sec_actor)    #创建截面
            actors.extend(secnode_actor)  #创建交互节点
            self.showActors("section", actors, True)  #添加并显示
        else:
            self.hideActors("section")

    def showModel(self,isshow):
        '''
        创建并显示网格模型
        '''
        self.loadImportData("model")
        if isshow:
            showLine=True if self.ui.combo_patternmodel.currentText()==u"线框模式" else False
            display=self.ui.combo_result.currentText()
            actors=self.model.createMeshActor(display,showLine)  #显示网格模型
            self.showActors("model", actors, True)
        else:
            self.hideActors("model")
    
    def showBoundary(self,isshow):
        '''创建并显示边界'''
        self.loadImportData("model")
        showLine=True if self.ui.combo_patternboundary.currentText()==u"线框模式" else False
        flagid=self.ui.combo_boundaryid.currentText()
        if isshow:
            col=QtWidgets.QColorDialog.getColor()
            if col.isValid():
                self.ui.combo_boundaryid.setStyleSheet('QWidget {background-color: %s}' % col.name())
                color=col.getRgb()[:3]
            else:
                color=(0,0,1)
            bounadry_actor=self.model.createBoundaryActor(flagid,showLine,color)
            if bounadry_actor is None:
                showMessage(self,u"模型中未包含边界信息!")
                self.ui.check_boundary.setChecked(False)
                return
            actors=[bounadry_actor]  #显示边界
            self.showActors("boundary", actors, True)
        else:
            self.hideActors("boundary")

    def showActors(self, actorname, actors, resetCamera=False):
        '''
        创建并刷新对象
        '''
        sel_actors = self.actors[actorname]
        for actor in sel_actors:
            self.ren.GetActors().RemoveItem(actor)
        for actor in actors:
            if actor is None:        #None检测
                continue
            sel_actors.append(actor) 
            if isinstance(actor, vtk.vtkScalarBarActor):
                self.ren.AddActor2D(actor)
            else:
                self.ren.AddActor(actor)
        self.ren.ResetCamera() if resetCamera else None  #重置相机
        # self.ui.vtkWidget.repaint()
        self.ui.vtkWidget.update()  #更新窗体

    def hideActors(self,module):
        "隐藏对象"
        for actor in self.actors[module]:
            actor.SetVisibility(False)
        # self.ui.vtkWidget.repaint()
        self.ui.vtkWidget.update()  #更新窗体

    def setCommand(self, cmd):
        "设置命令"
        showMessage(self,cmd)
        self.cmd = cmd

    def loadImportData(self,dtype,update=False):
        "加载数据"
        cdb=getLineEidtData(self,"cdb",str)
        xml=getLineEidtData(self,"xml",str)
        if not "data" in vars(self) or update:
            self.data = DataParser(cdb, xml)
        if not "points" in vars(self) or update:
            self.points = self.data.parsePoints()
        if not "elements" in vars(self) or update:    
            self.elements = self.data.parseElements()
        if not "results" in vars(self) or update:
            self.results = self.data.parseResults()
        if not "boundary" in vars(self) or update:
            self.boundary = self.data.parseBounaryCells()
        if "model" in dtype and not "model" in vars(self) or update:
            self.model = Model(self.points, self.elements, self.results, self.boundary)
        if "section" in dtype and not "section" in vars(self) or update:
            self.section = Section(self.points, self.elements, self.results)


    def getOpenFilePath(self,dtype):
        "选择打开的文件"
        if dtype=="cdb":
            f=QFileDialog.getOpenFileName(self,u"选择cdb网格文件",".","cdb files(*.cdb)")[0].strip()
            if f=="":
                return
            else:
                self.ui.edit_cdbfile.setText(f)
        elif dtype=="xml":
            f=QFileDialog.getOpenFileName(self,u"选择xml结果文件",".","xml files(*.xml)")[0].strip()
            if f=="":
                return
            else:
                self.ui.edit_xmlfile.setText(f)
    
    def loadConfig(self):
        "加载配置"
        fcf=os.path.join(os.path.dirname(__file__),"db.conf")
        conf=ConfigParser()
        conf.read(fcf)
        self.ui.edit_cdbfile.setText(conf.get("file","cdb"))
        self.ui.edit_xmlfile.setText(conf.get("file","xml"))
        self.ui.edit_xspace.setText(conf.get("section","xspace"))
        self.ui.edit_yspace.setText(conf.get("section","yspace"))
        self.ui.edit_ptsize.setText(conf.get("section","ptsize"))
        self.ui.combo_infosys.setCurrentIndex(conf.getint("section","infosys"))
        self.ui.combo_infotype.setCurrentIndex(conf.getint("section","infotype"))
        setLineEidtText(self,conf.get("section","center").split(","),"center")
        setLineEidtText(self,conf.get("section","start").split(","),"start")
        setLineEidtText(self,conf.get("section","end").split(","),"end")
        setLineEidtText(self,conf.get("section","p123").split(","),"p123")

    def saveConfig(self):
        "保存配置"
        fcf=os.path.join(os.path.dirname(__file__),"db.conf")
        conf=ConfigParser()
        conf.read(fcf)
        conf.set("file","cdb",self.ui.edit_cdbfile.text())
        conf.set("file","xml",self.ui.edit_xmlfile.text())
        conf.set("section","ptsize",self.ui.edit_ptsize.text())
        conf.set("section","xspace",self.ui.edit_xspace.text())
        conf.set("section","yspace",self.ui.edit_yspace.text())
        conf.set("section","center","%s,%s,%s"%a2T(getLineEidtData(self,"center")))
        conf.set("section","start","%s,%s,%s"%a2T(getLineEidtData(self,"start")))
        conf.set("section","end","%s,%s,%s"%a2T(getLineEidtData(self,"end")))
        conf.set("section","p123","%s,%s,%s"%a2T(getLineEidtData(self,"p123",int)))
        conf.set("section","infosys",self.ui.combo_infosys.currentIndex())
        conf.set("section","infotype",self.ui.combo_infotype.currentIndex())
        conf.write(open(fcf,"w"))
        showMessage(self,u"配置保存完毕")
    
    def getSectionInputData(self):
        center = getLineEidtData(self, "center")  #圆心
        start = getLineEidtData(self, "start")  #起点
        end = getLineEidtData(self, "end")  #终点
        xspace = getLineEidtData(self, "xspace")  #X向/径向间距
        yspace = getLineEidtData(self, "yspace")  #Y向/环向间距
        params=(center, start, end, xspace,yspace)
        for param in params:
            if param is None:
                showMessage(self,u"请输入扫描线的圆心-起点-终点-径向和环向间距!")
                self.ui.check_polar.setChecked(False)
                return None
        return center, start, end, xspace,yspace



if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    window.iren.Initialize()
    sys.exit(app.exec_())


