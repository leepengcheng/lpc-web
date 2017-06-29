# coding:utf-8
import os
from abaqus import *
from abaqusConstants import ELEMENT_NODAL
from xml.etree import ElementTree
from collections import defaultdict
from time import clock

start = clock()
#输出类型
ALL_TYPES = ('Ux', 'Uy', 'Uz', 'Usum', 'Seqv', 'Sx', 'Sy', 'Sz', 'Sxy', 'Sxz',
             'Syz', 'S1', 'S2', 'S3')


class Node(object):
    def __init__(self):
        for x in ALL_TYPES:
            object.__setattr__(self, x, None if x.startswith("U") else [])

    def __getattr__(self, name):
        value = self.__dict__.get(name, None)
        #求应力平均值
        if isinstance(value, list):
            return str(sum(value) / len(value))
        return str(value)

    def __setattr__(self, name, value):
        if isinstance(self.__dict__.get(name, 0), list):
            self.__dict__[name].append(value)
        else:
            self.__dict__[name] = value

    def __call__(self, name):
        return self.__getattr__(name)


#开始导出文件
def export(**kargs):
    start = clock()
    output = kargs['output']  #输出结果类型
    cdbcheck = kargs['cdbcheck']  #选择导出CDB
    xmlcheck = kargs['xmlcheck']  #选择导出XML
    odbfile = kargs['odbFile']  #ODB文件路径
    cdbfile = kargs['cdbFile']  #CDB文件路径
    xmlfile = kargs['xmlFile']  #XML文件路径
    projectname = kargs['projectName']  #项目名称
    projectProperty = kargs['projectProperty']  #项目名称
    caseid = kargs['caseID']  #工况名称
    casename = kargs['caseName']  #工况名称
    caseProperty = kargs['caseProperty']  #工况名称
    unit = kargs['projectUnit']  #模型单位
    #####################################
    msg = []  #消息
    myodb = session.openOdb(name=odbfile)
    args_cdb = (cdbfile, projectname, projectProperty, unit)
    args_xml = (xmlfile, projectname,casename, caseProperty, caseid, output, unit)
    #导出CDB
    if cdbcheck:
        export_cdb(myodb, args_cdb)
        msg.append(u"cdb网格模型文件导出完毕!\n保存路径为: %s".encode('gbk') % cdbfile)
    #导出XML
    if xmlcheck:
        export_xml(myodb, args_xml)
        msg.append(u"xml计算结果文件导出完毕!\n保存路径为: %s".encode('gbk') % xmlfile)
    myodb.close()  #关闭odb文件
    ###打印消息
    if len(msg):
        print "\n".join(msg)
    print u"共耗时: %s s".encode("gbk") % (clock() - start)


def export_cdb(myodb, args):
    """
    导出cdb模型文件
    """
    cdbfile, projectname, projectProperty, unit = args
    instances = myodb.rootAssembly.instances
    materials = [myodb.materials[name] for name in myodb.materials.keys()]
    for matid, material in enumerate(materials, 1):
        matname = material.name
        # elastic = material.elastic.table[0]
        # telastic = material.elastic.type ##ISOTROPIC
        # density = material.density.table[0]
    with open(cdbfile, 'wt') as f:
        f.write("*SET,_PROJECT_NAME,'%s'\n" % projectname)  #项目名称
        f.write("*SET,_PROJECT_PROPERTY,'%s'\n" % projectProperty)  #项目描述
        f.write("*SET,_PROJECT_UNITS,'%s'\n" % unit)  #项目单位
        for key in instances.keys():
            nodes = instances[key].nodes
            elements = instances[key].elements
            nds, nes = len(nodes), len(elements)
            f.write("NBLOCK,6,SOLID,%s,%s\n" % (nds, nds))
            f.write("(3i9,6e21.13e3)\n")
            for node in nodes:
                nlabel = node.label
                coordinates = node.coordinates.tolist()
                f.write(("%9d" * 3 + "%21.13e" * 3 + "\n") %
                        tuple([nlabel, 0, 0] + coordinates))

            f.write("EBLOCK,19,SOLID,%s,%s\n" % (nes, nes))
            f.write("(19i9)\n")
            for element in elements:
                elabel = element.label
                connectivity = element.connectivity
                matname = element.sectionCategory.name  # solid < MATERIAL-1 >
                properties = (1, 1, 1, 1, 0, 0, 0, 0, 8, 0)
                f.write(("%9d" * 19 + "\n") % (properties +
                                               (elabel, ) + connectivity))


def export_xml(myodb, args):
    """
    导出xml结果文件
    """
    xmlfile, projectname,casename, caseProperty, caseid, output, unit = args
    output_types = output.split()
    unit_u, unit_s = unit.split("-")  #模型单位
    #长度换算比例
    uscale = 1000 if unit_u == "m" else 1
    #应力换算比例
    sscale = 1e-6 if unit_s == "pa" else 1e-3 if unit_s == "Kpa" else 1
    # session.viewports['Viewport:1'].setValues(displayedObject=obj)
    nsets = myodb.rootAssembly.nodeSets
    nset = nsets[' ALL NODES']  # nset = nsets[nsets.keys()[-1]]
    theframe = myodb.steps[myodb.steps.keys()[-1]].frames[-1]
    s_outs = theframe.fieldOutputs['S']  #应力输出
    u_outs = theframe.fieldOutputs['U']  #位移输出

    #应力输出:节点应力
    s_vals = s_outs.getSubset(region=nset, position=ELEMENT_NODAL).values
    #应力输出:节点位移
    u_vals = u_outs.getSubset(region=nset).values

    # 输出字典
    nodes = defaultdict(Node)
    for s in s_vals:
        nid = str(s.nodeLabel)
        sdata = s.data
        nodes[nid].S1 = s.maxPrincipal * sscale
        nodes[nid].S2 = s.midPrincipal * sscale
        nodes[nid].S3 = s.minPrincipal * sscale
        nodes[nid].Seqv = s.mises * sscale
        nodes[nid].Sx = sdata[0] * sscale
        nodes[nid].Sy = sdata[1] * sscale
        nodes[nid].Sz = sdata[2] * sscale
        nodes[nid].Sxy = sdata[3] * sscale
        nodes[nid].Sxz = sdata[4] * sscale
        nodes[nid].Syz = sdata[5] * sscale

    for u in u_vals:
        nid = str(u.nodeLabel)
        udata = u.data
        nodes[nid].Usum = u.magnitude * uscale
        nodes[nid].Ux = udata[0] * uscale
        nodes[nid].Uy = udata[1] * uscale
        nodes[nid].Uz = udata[2] * uscale
    # xml节点：Results
    results = ElementTree.Element("Results")
    # XML节点:Project
    project = ElementTree.SubElement(results, "Project")
    project.attrib['name'] = projectname
    # xml节点：Case
    case = ElementTree.SubElement(project, "Case")
    case.attrib['id'] = caseid  #工况ID
    case.attrib['name'] = casename  #工况名称
    case.attrib['property'] = caseProperty  #工况属性
    for key in nodes.keys():
        # xml节点：Node
        xmlNode = ElementTree.SubElement(case, "Node")
        xmlNode.attrib['id'] = key
        node = nodes[key]
        for output_type in output_types:
            ElementTree.SubElement(xmlNode,
                                   output_type).text = node(output_type)
    tree = ElementTree.ElementTree(results)
    #保存XML
    tree.write(xmlfile, xml_declaration=True, encoding='utf-8')
