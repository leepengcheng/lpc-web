#coding:utf-8
from config import *
import numpy as np


class Cell():
    """docstring for Cell"""

    def __init__(self, nodes, eid, etype, eref):
        self.nodes = nodes
        self.nnodes = len(nodes)
        self.eid = eid
        self.etype = etype
        self.eref = eref
        self.flag = "-".join(
            [str(x) for x in sorted(self.nodes, reverse=True)])


class Element(object):
    """docstring for Element"""

    def __init__(self, line):
        li = [int(x) for x in line.split()]
        self.id = li[10]
        self.ref = li[6]
        self.nnum = li[8]
        self.nodes = li[11:]
        self.cells = []
        if self.nnum == 8:
            if self.nodes[4] == self.nodes[5] == self.nodes[6] == self.nodes[7]:
                self.type = 'solid4'
            elif self.nodes[2] == self.nodes[3] and self.nodes[6] == self.nodes[7]:
                self.type = 'solid5'
            else:
                self.type = 'solid6'
        else:
            if self.nodes[2] == self.nodes[3]:
                self.type = 'shell3'
            else:
                self.type = 'shell4'
        data = (self.id, self.type, self.ref)
        for t in eorder[self.type]:
            nodes = [self.nodes[x] for x in t]
            self.cells.append(Cell(nodes, *data))


class Point(object):
    """docstring for Point"""

    def __init__(self, nodeID, nodeRef, nodeLoc):
        super(Point, self).__init__()
        self.nid = nodeID
        self.nref = nodeRef
        self.nloc = nodeLoc

    @property
    def ref_line(self):
        return 0 if not self.nref[-1] in ('1', '2') else self.nref[:-1]

    @property
    def ref_face(self):
        return 0 if not self.nref[-1] in ('3', ) else self.nref[:-1]

class NodeResult(object):
    def __init__(self,data):
        super(NodeResult, self).__init__()
        self.data=data


    def __getitem__(self,index):
        return self.data[index]

    def stress(self,matrix=np.identity(3),key=None):
        '''
            节点的应力矩阵
        '''
        #如果key为Seqv
        if key=="Seqv":
            return self.data["Seqv"]
        stress_matrix=np.array([[self.data['Sx'], self.data['Sxy'], self.data['Sxz']], 
                            [self.data['Sxy'], self.data['Sy'], self.data['Syz']], 
                            [self.data['Sxz'], self.data['Syz'], self.data['Sz']]], dtype=float)
        trans_stress_matrix=matrix.dot(stress_matrix).dot(matrix.transpose())
        if  key is not None:
            index=RESULTS[key]
            return trans_stress_matrix[index]
        else:
            return  trans_stress_matrix

    def displace(self,matrix=np.identity(3),key=None):
        '''
        节点位移矩阵
        '''
        #如果key为Usum
        if key=="Usum":
            return self.data["Usum"]
        displace_matrix=np.array([self.data['Ux'], self.data['Uy'], self.data['Uz']], dtype=float)
        trans_displace_matrix=matrix.dot(displace_matrix)
        if key is not None:
            index=RESULTS[key]
            return  trans_displace_matrix[index]
        else:
            return trans_displace_matrix

class Selection():
    "选择项类"
    def __init__(self):
        self.previous=[] #已选择的
        self.current=None #当前选择的
        self.center=None  #圆心
        self.start=None   #起点
        self.end=None     #终点
    
    def reset(self):
        "重置"
        self.__init__()
    
    @property
    def hasCenter(self):
        return self.center is not None

    @property
    def hasStart(self):
        return self.start is not None
    
    @property
    def hasEnd(self):
        return self.end is not None


    def cancelhightLight(self):
        "恢复上个对象的颜色"
        if len(self.previous):
            self.previous[-1].GetProperty().DeepCopy(self.current.GetProperty())

    def getColor(self):
        "获得当前选中项的颜色"
        return self.current.GetProperty().GetColor()


    def getCenter(self):
        "获得当前选中项的中心"
        return self.current.GetMapper().GetInputAlgorithm().GetCenter() 


    def hightLight(self,color=RED):
        "高亮中项"
        self.current.GetProperty().SetColor(color)


    def update(self):
        "将当前选中项加入已选列表"
        self.previous.append(self.current)


class CMD(object):
    "指令类"
    WORLD_XYZ=1 #显示世界坐标
    LOCAL_XYZ=2 #显示局部坐标
    WORLD_STRESS=3 #显示总体应力
    LOCAL_STRESS=4 #显示局部应力
    HIDE_THENODE=5 #隐藏当前节点
    GET_CENTER=u"请选择柱坐标系圆弧 <圆心>"
    GET_START=u"请选择柱坐标圆弧 <起点>"
    GET_END=u"请选择柱坐标圆弧  <终点>"