#coding:utf-8
from xml.etree.ElementTree import ElementTree
from collections import defaultdict
from domain import *
from config import *
import numpy as np

class DataParser(object):
    """数据类"""

    def __init__(self, fcdb, fxml):
        super(DataParser, self).__init__()
        self.fcdb = fcdb
        self.loadCDB()
        self.fxml = fxml
        self.loadXML()

    #@costTime
    def loadXML(self):
        tree = ElementTree()
        tree.parse(self.fxml)
        root = tree.getroot()
        self.results = root.findall("Project/Case/Node")

    #@costTime
    def loadCDB(self):
        '''单元和节点数据定位'''
        with open(self.fcdb, 'r') as f:
            self.data = f.readlines()
        self.bb = None
        for x in xrange(len(self.data)):
            if self.data[x].startswith('NBLOCK'):
                self.nb = x + 2
                self.nbnum = int(self.data[x].split(',')[-1].strip())
            elif self.data[x].startswith('EBLOCK'):
                self.eb = x + 2
                self.ebnum = int(self.data[x].split(',')[-1].strip())
            elif self.data[x].startswith('BBLOCK'):
                self.bb = x + 2
                self.bbnum = int(self.data[x].split(',')[-1].strip())

    #@costTime
    def parseElements(self):
        '''单元数据'''
        start = self.eb
        end = self.eb + self.ebnum
        return [Element(line) for line in self.data[start:end]]

    #@costTime
    def parseResults(self, resultType):
        '''返回指定的结果'''

        return dict([(int(x.attrib['id']), float(x.find(resultType).text))
                for x in self.results])


    def parseResults(self):
        '''
        返回节点的应力和位移矩阵
        '''
        results = {}
        for node in self.results:
            nid = int(node.attrib['id'])
            result={}
            for x in RESULTS.keys():
                result[x]=float(node.find(x).text)
            results[nid]=NodeResult(result)
        return results


    #@costTime
    def parsePoints(self):
        '''节点数据'''
        points = {}
        start = self.nb
        end = self.nb + self.nbnum
        for line in self.data[start:end]:
            n = int(line[:9])
            ref = line[9:18].strip()
            x = float(line[27:48])
            y = 0.0 if line[48:69].strip() == "" else float(line[48:69])
            z = 0.0 if line[69:90].strip() == "" else float(line[69:90])
            points[n] = Point(n, ref, (x, y, z))
        return points

    def parseBounaryCells(self):
        '''边界数据'''
        boundaryCells = defaultdict(list)
        if self.bb is None:
            return boundaryCells
        start = self.bb
        end = self.bb + self.bbnum
        for line in self.data[start:end]:
            data = line.split()
            boundaryCells[data[2]].append(data[1])
        return boundaryCells

