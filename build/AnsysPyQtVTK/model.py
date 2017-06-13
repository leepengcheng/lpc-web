#coding:utf-8
import vtk
import numpy as np
from util import costTime, getTMatrix, getColorTable
from domain import *


class Model(object):
    """模型网格"""

    def __init__(self, nodes, elements, results, boundary):
        """
        初始化数据
        """
        super(Model, self).__init__()
        self.nodeset = nodes  #节点数据
        self.elements = elements  #单元信息
        self.cells = {}  #单元面字典
        self.boundary = boundary  #边界信息
        self.results = results  #结果信息

    # @costTime
    def getNodes(self):
        """
        设置节点
        """
        nodes = self.nodeset.values()  #节点
        n_nodes = len(nodes)  #节点个数
        vtkpts = vtk.vtkPoints()
        vtkpts.SetNumberOfPoints(n_nodes + 1)  #增加0号店
        vtkpts.SetPoint(0, 0, 0, 0)  #第0个点必须设置
        for node in nodes:
            vtkpts.SetPoint(node.nid, *node.nloc)
        return vtkpts

    # @costTime
    def getPolys(self):
        """
        设置拓扑单元面
        """
        polys = vtk.vtkCellArray()
        for e in self.elements:
            for cell in e.cells:
                self.cells.update({cell.flag: cell})
                polys.InsertNextCell(cell.nnodes, cell.nodes)
        return polys

    # @costTime
    def getOuterCells(self):
        """
        提取外部单元面
        @cells:要过滤的单元面列表
        """
        faces = defaultdict(list)
        for cell in self.cells:
            key = cell.flag
            faces[key].append(cell)
        #如果面的个数为1,则为外表面
        ##面个数为1有可能不为外表面
        return [face[0] for face in faces.values() if len(face) == 1]

    # @costTime
    def getScalars(self,display):
        """设置节点结果"""
        scalars = vtk.vtkDoubleArray()
        scalars.InsertTuple1(0, 0)
        for nid, value in self.results.items():
            scalars.InsertTuple1(nid, value[display])
        return scalars

    def createBoundaryActor(self, flag, color=(0, 0, 1)):
        '''显示边界
        @flag:边界类型标识符
        #4-pressure-inlet, inlet-vent, intake-fan
        #5-pressure-outlet, exhaust-fan, outlet-vent
        #10-velocity-inlet
        #20-mass-flow-inlet
        '''
        #如果边界单元数目为0 则表示无边界信息
        if not len(self.boundary):
            return None
        cells = [self.cells[Key] for Key in self.boundary[flag]]
        boundary_polys = vtk.vtkCellArray()
        for cell in cells:
            boundary_polys.InsertNextCell(cell.nnodes, cell.nodes)
        boundaryData = vtk.vtkPolyData()
        boundaryData.SetPoints(self.vtkpts)
        boundaryData.SetPolys(boundary_polys)
        boundaryMapper = vtk.vtkPolyDataMapper()
        boundaryMapper.SetInputData(boundaryData)
        # Actor
        boundaryActor = vtk.vtkActor()
        boundaryActor.SetMapper(boundaryMapper)
        boundaryActor.GetProperty().SetColor(color)
        return boundaryActor

    # @costTime
    def getOuterLines(self, cells):
        linecelldict = defaultdict(list)
        for cell in cells:
            tempcell = cell.nodes + [cell.nodes[0]]
            for x in xrange(cell.nnodes):
                nodes = tempcell[x:x + 2]
                args = (cell.eid, cell.etype, cell.eref)
                line = Cell(nodes, *args)
                key = line.flag
                linecelldict[key].append(line)
        outerlines = []
        for linecells in linecelldict.values():
            nlc = len(linecells)
            con1 = nlc == 1 and "shell" in linecells[0].etype
            con21 = all(["solid" in x.etype for x in linecells])
            con22 = all(["shell" in x.etype for x in linecells])
            con23 = all(self.nodeset[p].ref_line
                        for l in linecells for p in l.nodes)
            con2 = nlc == 2 and con21 or (con22 and con23)
            if con1 or con2:
                # if con23:
                outerlines.append(linecells[0])
        return outerlines

    # @costTime
    def createMeshActor(self,
                 display="Seqv",
                 bindNum=10,
                 showRegion="all",
                 showLine=False):
        """
        绘制显示模型
        @showLine:显示线模型
        @display:是否显示云图
        @showRegion:显示区域 全部/外表面/外轮廓
        @bindNum:等值带数目
        """
        ##显示区域 全部/外表面/外轮廓
        # if "outerfaces" in showRegion:
        #     self.cells = self.getOuterCells(self.cells)
        # elif "outerlines" in showRegion:
        #     self.cells = self.getOuterLines(self.cells)
        # 设置 Points Polys Mapper Actor
        meshData = vtk.vtkPolyData()
        meshData.SetPoints(self.getNodes())  #设置节点
        polys=self.getPolys()  #获得单元
        if "outerlines" in showRegion:  #显示外轮廓
            meshData.SetLines(polys)
        else:
            meshData.SetPolys(polys)
        meshMapper = vtk.vtkPolyDataMapper()
        #显示结果云图
        # if display in RESULTS.keys():
        #     scalars=self.getScalars(display)
        #     min_val,max_val=scalars.GetValueRange()
        #     meshData.GetPointData().SetScalars(scalars)
        #     # Bindedfilter
        #     bandedFilter = vtk.vtkBandedPolyDataContourFilter()
        #     bandedFilter.SetInputData(meshData)
        #     bandedFilter.GenerateValues(10, min_val, max_val)
        #     colorTable = getColorTable(bindNum)
        #     meshMapper.SetLookupTable(colorTable)
        #     meshMapper.SetInputConnection(bandedFilter.GetOutputPort())
        #     # meshMapper.SetScalarRange(0,bindNum)
        #     meshMapper.SetScalarRange(min_val, max_val)
        #     # meshMapper.SetScalarModeToUseCellData()
        #     meshMapper.SetScalarModeToUsePointData()
        #     # meshMapper.SetColorModeToMapScalars()
        #     # meshMapper.SetScalarModeToDefault()

        #     # Add a scalar bar.
        #     scalarBar = vtk.vtkScalarBarActor()
        #     scalarBar.SetWidth(0.8)  #颜色条的宽度
        #     scalarBar.SetHeight(0.1)  #颜色条的高度
        #     scalarBar.SetPosition(0.1, 0.05)  #颜色条的位置
        #     scalarBar.SetLabelFormat("%.2f")  #颜色条数值格式
        #     scalarBar.SetOrientationToHorizontal()
        #     # scalarBar.SetOrientationToVertical() #颜色条数值方位
        #     # scalarBar.SetMaximumWidthInPixels(100)
        #     # scalarBar.SetMaximumHeightInPixels(600)
        #     scalarBar.SetLookupTable(colorTable)  ##颜色条颜色映射表
        #     scalarBar.SetTitle("display")  ##颜色条标题栏
        #     self.actors.append(scalarBar)  ###添加颜色条
        # else:
        meshMapper.SetInputData(meshData)
        # Actor
        meshActor = vtk.vtkActor()
        meshActor.SetMapper(meshMapper)
        if showLine:
            meshActor.GetProperty().SetRepresentationToWireframe()
        return  meshActor