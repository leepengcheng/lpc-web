#coding:utf-8
'''
工具方法
'''
from time import clock
from functools import wraps
import vtk
import numpy as np
from config import *

def costTime(f):
    "计时函数"
    @wraps(f)
    def wrapper(*arg, **kargs):
        start = clock()
        result = f(*arg, **kargs)
        print("%20s():Cost %.4f s" % (f.func_name, clock() - start))
        return result
    return wrapper


def getTMatrix(p1, p2, p3=(0,0,0),vector_z=None,isNormal=True):
    """ 获得截面的局部坐标系转换矩阵
        @p1:起点
        @p2:终点
        @p3:切割线的拉伸方向定位点
        @vector_z:面的法向矢量
        @isNormal:当提供vector_z时,直线p1p2是否位于垂直与vec_z的平面内
    """
    #已知面的法向量vector_z 和 p1 p2点来创建矩阵
    if vector_z is not None:
        #p1p2位于vector_z的垂直面上
        if isNormal:
            vector_x = p2-p1
            vector_y = np.cross(vector_z, vector_x)  # 环向Y矢量
        #p1p2不位于vector_z的垂直面上
        else:
            vector = p2 - p1  # p1p2矢量，不为X轴
            vector_y = np.cross(vector_z, vector)  # 环向Y矢量
            vector_x = np.cross(vector_y, vector_z)
    #由 3个点创建矩阵
    else:
        vector_x = p2 - p1  # 切面x向方向矢量
        vector_t = p3 - p1
        vector_z = np.cross(vector_x, vector_t)  # 切面y向方向矢量
        vector_y = np.cross(vector_z, vector_x)  # 切面z向方向矢量

    rotate_matrix = np.array([vector_x, vector_y, vector_z])
    rotate_norm = np.linalg.norm(rotate_matrix, axis=1).reshape([3,1])   # 每列的模
    rotate_matrix = rotate_matrix / rotate_norm  # 余弦矩阵
    transform_matrix = np.eye(4)  #初始化
    transform_matrix[:3,:3]=rotate_matrix #旋转量
    p=rotate_matrix.dot(p1)               #平移量
    transform_matrix[:3,3] = -p           #平移矩阵
    return transform_matrix  

def getRotMatrixFromZAxis(angle,size=4):
    "求绕Z轴旋转angle后的矩阵"
    cosa=np.cos(np.deg2rad(angle))
    sina=np.sin(np.deg2rad(angle))
    rmatrix=np.array([[cosa,sina,0],[-sina,cosa,0],[0,0,1]])
    if size==4:
       tmatrix=np.eye(4)
       tmatrix[:3,:3]=rmatrix
       return tmatrix
    return rmatrix
    
def getAngleFrom2Vector(vec1,vec2):
    "求两个矢量的夹角角度"
    norm1=np.sqrt(np.sum(np.power(vec1,2)))
    norm2=np.sqrt(np.sum(np.power(vec2,2)))
    cosa=abs(vec1.dot(vec2)/(norm1*norm2))
    angle=np.rad2deg(np.arccos(cosa))
    return angle


def getColorTable(num): 
    "获得颜色条的颜色映射表"
    colorTable = vtk.vtkLookupTable()
    colorTable.SetNumberOfColors(num)
    colorTable.Build()
    # 蓝色-绿色-黄色-红色
    # (0, 0, 1), (0, 1, 1), (0, 1, 0), (1, 1, 0), (1, 0, 0)
    # 当seg=1时,分母为0,此时令值大于4.0即可
    binds = [4.0 / (num - 1) * i for i in range(0, num)] \
        if num > 1 else [4.0]
    binds_int = [int(x) for x in binds]
    binds_dec = [binds[x] - binds_int[x] for x in range(0, num)]
    for i in range(0, num):
        if binds_int[i] == 0:
            colorTable.SetTableValue(i, 0, binds_dec[i], 1, 1.0)
        elif binds_int[i] == 1:
            colorTable.SetTableValue(i, 0, 1, 1 - binds_dec[i], 1.0)
        elif binds_int[i] == 2:
            colorTable.SetTableValue(i, binds_dec[i], 1, 0, 1.0)
        elif binds_int[i] == 3:
            colorTable.SetTableValue(i, 1, 1 - binds_dec[i], 0, 1.0)
        else:
            colorTable.SetTableValue(i, 1, 0, 0, 1.0)  # 红色
    return colorTable


def getLineEidtData(self, sel="center",dtype=float):
    "获得界面输入框的值并转换为float"
    sel_edits = self.edits[sel]
    xyz = []
    num = len(sel_edits)
    for x in xrange(num):
        sel_text = sel_edits[x].text().strip()
        if len(sel_text) == 0:
            return None
        if num == 1:
            return dtype(sel_text)
        else:
            xyz.append(float(sel_text))
    return np.array(xyz,dtype=dtype)

def setLineEidtText(self, data, sel="center"):
    "设置界面输入框的文本值"
    sel_edits = self.edits[sel]
    for i in xrange(len(sel_edits)):
        sel_edits[i].setText(str(data[i]))
    self.ui.statusbar.clearMessage()

def showMessage(self,msg):
    "显示消息"
    self.ui.statusbar.showMessage(msg)


def isPointInPoly(pt,poly,matrix):
    "判断点是否在多边形内"
    a,b,npts=0,0,len(poly)
    poly=[getNodeTFXYZ(p,matrix,3) for p in poly] #转换为截面坐标
    poly=[p-pt for p in poly] #将平面原点移动到pt
    for i in xrange(npts):
        p0=poly[i]
        p1=poly[i+1] if i!=npts-1 else poly[0]
        #当有交点或者过端点的同时另一端的Y值大于0
        #过2个端点认为没有交点
        if p0[1]*p1[1]<0  or (p0[1]==0 and p1[1]>0) or (p0[1]>0 and p1[1]==0):
            x=p0[0]-p0[1]*(p1[0]-p0[0])/(p1[1]-p0[1]) #交点的X坐标
            #点在线上
            if x==0:
                return -1
            if x>0:
                a+=1
            else:
                b+=1
    #两侧都为奇数
    return a%2 and b%2 
    
def isPointInSection(pt,polys,matrix):
    "判断点是否在截面上"
    #将pt转换为局部坐标系坐标
    pt=getNodeTFXYZ(pt,matrix,3)
    for poly in polys:
        if isPointInPoly(pt,poly,matrix):
            return True
    return False

def createAxisActor(center,start,end,scale=5):
    '''创建坐标系
    @center:圆心
    @start:起点
    @end:终点
    @scale:坐标系比例大小
    '''
    matrix=getTMatrix(center,start,end)
    matrix[:3,:3]=matrix[:3,:3].T
    matrix[:3,3]=center
    mat=matrix.reshape([1,16])
    axesActor = vtk.vtkAxesActor()
    tmatrix = vtk.vtkMatrix4x4()
    tmatrix.DeepCopy(mat.tolist()[0])
    axesActor.SetUserMatrix(tmatrix)
    axesActor.SetTotalLength(scale, scale, scale)
    return axesActor
            
def createLineActor(start, end,color=WHITE):
    "创建线"
    #point
    points = vtk.vtkPoints()
    points.SetNumberOfPoints(2)
    points.SetPoint(0, *start)
    points.SetPoint(1, *end)
    #line cell
    linecell = vtk.vtkCellArray()
    linecell.InsertNextCell(2)
    linecell.InsertCellPoint(0)
    linecell.InsertCellPoint(1)
    linepoly = vtk.vtkPolyData()
    linepoly.SetPoints(points)
    linepoly.SetLines(linecell)
    lineMapper = vtk.vtkPolyDataMapper()
    lineMapper.SetInputData(linepoly)
    lineMapper.Update()
    lineActor = vtk.vtkActor()
    lineActor.SetMapper(lineMapper)
    lineActor.GetProperty().SetColor(color)
    return lineActor
        

def createPointActors(pts,color=YELLOW,r=0.5):
    "创建节点"
    pointActors=[]
    for pt in pts:
        source = vtk.vtkSphereSource()
        source.SetRadius(r)
        source.SetCenter(*pt)
        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputConnection(source.GetOutputPort())
        actor = vtk.vtkActor()
        actor.SetMapper(mapper)
        actor.GetProperty().SetColor(color)
        pointActors.append(actor)
    return pointActors           


def getUniformPoints(start,end,space,has_begin=True,has_end=True):
    '''
    求起点终点之间的等距分布点
    @has_start:包含起点
    @has_end:包含终点
    '''
    start, end = np.array(start), np.array(end)
    pts=[]
    vec = end - start
    dis = np.sqrt((vec**2).sum())  #距离
    if space < dis:
        steps = np.arange(0, 1, space / dis)
        for step in steps:
            pt=start + step * vec
            pts.append(pt)
    if not has_begin:
        del pts[0]
    if has_end:
        pts.append(end)
    return pts


def getNodeNDXYZ(self,pt):
    "获得节点的ndarrary类型的坐标位置"
    #如果传入的是节点号
    if isinstance(pt,int):
        pt = self.nodeset[pt].nloc
    #如果传入的不是ndarray
    if not isinstance(pt,np.ndarray):
        pt = np.array(pt)
    return pt


def getNodeTFXYZ(pt,matrix,n=None):
    "获得转换后的节点坐标"
    if not isinstance(pt,np.ndarray):
        pt=np.array(pt)
    if len(pt)==3:
        pt=np.append(pt,1)
    if n is None:
        return matrix.dot(pt)[:3]
    else:
        return matrix.dot(pt)[:3].round(n)

def getNodesNDXYZ(self,*pts):
    "获得多个节点的ndarrary类型的坐标位置"
    return [getNodeNDXYZ(self,pt) for pt in pts]

def a2T(pt,n=None):
    "ndarray 精度缩减并转换为tuple"
    if n is None:
        return tuple(pt.tolist())
    else:
        return tuple(pt.round(3).tolist())