#coding:utf-8
from abaqus import *
from time import clock


def importParasolid(**kargs):
    start = clock()
    file_xt=kargs['fileName']
    mdb.models[mdb.models.keys()[0]].rootAssembly.importParasolidFile(filename=file_xt, ids=())
    print u"导入Parasolid模型共耗时: %s s".encode("gbk") % (clock() - start)