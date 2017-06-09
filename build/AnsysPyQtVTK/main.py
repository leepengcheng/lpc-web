# coding:utf-8
import os
import sys
import vtk
from util import *
from domain import *
from config import *
from collections import defaultdict
from dataparser import DataParser
from model import Model
from section import Section
#界面部分
from PyQt5 import QtCore, QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow
from ui.ui_main import Ui_mainWindow

path = os.path.dirname(__file__)
cdb, xml = [os.path.join(path, "data", f) for f in ("block.cdb", "block.xml")]
data = DataParser(cdb, xml)
#解析数据
points = data.parsePoints()
elements = data.parseElements()
results = data.parseResults()
boundary = data.parseBounaryCells()


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
        self.cmd = CMD.WORLD_XYZ  #全局命令参数,默认为显示总体坐标值
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
            "xspace": (self.ui.edit_xspace, ),
            "yspace": (self.ui.edit_yspace, )
        }

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
                self.executeUserCommand(sender="Node")  #执行节点命令
                # self.showNodeData(node_wxyz,"Sx")
                self.sel.cancelhightLight()  #取消高亮上个对象
                self.sel.hightLight()  #高亮当前对象
                self.sel.update()  #将选择的对象加入previous
            elif color == GREEN:  #单元面
                pass

    def initializeSignals(self):
        '''
        初始化信号槽
        '''
        self.ui.btn_section.clicked.connect(self.showSection)
        self.ui.btn_model.clicked.connect(self.showModel)
        self.ui.btn_cylinder.clicked.connect(self.showPolarLine)
        self.ui.btn_descarte.clicked.connect(self.hideActors)
        #选择圆心 起点 终点
        self.ui.btn_center.clicked.connect(
            lambda: self.setCommand(CMD.GET_CENTER))
        self.ui.btn_start.clicked.connect(
            lambda: self.setCommand(CMD.GET_START))
        self.ui.btn_end.clicked.connect(lambda: self.setCommand(CMD.GET_END))

    def executeUserCommand(self, sender):
        #执行命令
        wxyz = self.sel.getCenter()
        if sender == "Node":
            if self.cmd == CMD.WORLD_XYZ:
                print wxyz
            elif self.cmd == CMD.LOCAL_STRESS:
                print self.showNodeData(wxyz)
            elif self.cmd == CMD.HIDE_THENODE:
                self.sel.current.SetVisibility(False)  #隐藏
            elif self.cmd == CMD.GET_CENTER:
                setLineEidtText(self, wxyz, "center")
            elif self.cmd == CMD.GET_START:
                setLineEidtText(self, wxyz, "start")
            elif self.cmd == CMD.GET_END:
                setLineEidtText(self, wxyz, "end")

    def showPolarLine(self):
        "创建柱坐标数值输出线"
        actors=[]
        center = getLineEidtData(self, "center")  #圆心
        start = getLineEidtData(self, "start")  #起点
        end = getLineEidtData(self, "end")  #终点
        xspace = getLineEidtData(self, "xspace")  #X向/径向间距
        yspace = getLineEidtData(self, "yspace")  #Y向/环向间距
        if None in (center, start, end, xspace, yspace):
            self.ui.statusbar.showMessage(u"请输入柱坐标系的圆心 起点 终点位置 X间距 Y间距", 2000)
            return
        axis_actor=createAxisActor(center, start, end) #创建坐标系
        sec_actor = self.secplot.createPolarLineActor(center, start, end, xspace,
                                                   yspace)
        actors.append(axis_actor)
        actors.extend(sec_actor)                              
        self.showActors("polar", actors)

    def showSection(self):
        '''
        创建并显示截面
        '''
        p1, p2, p3 = 265, 258, 95
        actors = []
        self.secplot = Section(points, elements, results)  #初始化
        actors.append(
            self.secplot.createSectionActor(p1, p2, p3, showLine=True))  #创建截面
        actors.extend(self.secplot.createSectionNodeActors())  #创建交互节点
        self.showActors("section", actors, True)  #添加并显示

    def showModel(self):
        '''
        创建并显示网格模型
        '''
        actors = []
        self.model = Model(points, elements, results, boundary)
        actors.append(self.model.createMeshActor(showLine=True))  #显示网格模型
        actors.append(self.model.createBoundaryActor("10"))  #显示边界
        self.showActors("model", actors, True)
            
        

    def showActors(self, actorname, actors, resetCamera=False):
        '''
        显示并刷新对象
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

    def hideActors(self):
        "隐藏对象"
        for actor in self.actors['polar']:
            actor.SetVisibility(False)
        # self.ui.vtkWidget.repaint()
        self.ui.vtkWidget.update()  #更新窗体

    def setCommand(self, cmd):
        "设置命令"
        self.ui.statusbar.showMessage(cmd)
        self.cmd = cmd


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    window.iren.Initialize()
    sys.exit(app.exec_())
