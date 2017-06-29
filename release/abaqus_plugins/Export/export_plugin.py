#coding:utf-8
from abaqusGui import *
from abaqusConstants import *
import os
import exportDB

###########################################################################
# Class definition
###########################################################################


class Export_plugin(AFXForm):

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def __init__(self, owner):

        # Construct the base class.
        #
        AFXForm.__init__(self, owner)
        self.cmd = AFXGuiCommand(
            mode=self,
            method='export',
            objectName='export_logic',
            registerQuery=False)
        ####################################################
        self.projectNameKw = AFXStringKeyword(self.cmd, 'projectName', True,'')
        self.projectUnitKw = AFXStringKeyword(self.cmd, 'projectUnit', True,'')
        self.projectPropertyKw = AFXStringKeyword(self.cmd, 'projectProperty',True, '')
        self.odbFileKw = AFXStringKeyword(self.cmd, 'odbFile', True, '')
        self.cdbFileKw = AFXStringKeyword(self.cmd, 'cdbFile', True, '')
        self.xmlFileKw = AFXStringKeyword(self.cmd, 'xmlFile', True, '')
        self.caseIDKw = AFXStringKeyword(self.cmd, 'caseID', True, '1')
        self.caseNameKw = AFXStringKeyword(self.cmd, 'caseName', True, '')
        
        self.casePropertyKw = AFXStringKeyword(self.cmd, 'caseProperty', True,'')
        #倒数第2个参数为False时不会传递参数到逻辑代码中
        self.optionalListKw = AFXStringKeyword(self.cmd, 'optionalList', False,'')
        self.selectedListKw = AFXStringKeyword(self.cmd, 'selectedList', False,'')
        self.cdbcheckKw = AFXBoolKeyword(self.cmd, 'cdbcheck',AFXBoolKeyword.TRUE_FALSE, True, True)
        self.xmlcheckKw = AFXBoolKeyword(self.cmd, 'xmlcheck',AFXBoolKeyword.TRUE_FALSE, True, True)
        self.outputKw = AFXStringKeyword(self.cmd, 'output', True,'')

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def getFirstDialog(self):
        self.form=exportDB.ExportDB(self)
        return self.form



    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def doCustomChecks(self):
        self.xmlcheck=self.xmlcheckKw.getValue()
        self.cdbcheck=self.cdbcheckKw.getValue()
        projectname=self.projectNameKw.getValue().strip()
        projectunit=self.projectUnitKw.getValue().strip()
        odbfile=self.odbFileKw.getValue().strip()
        cdbfile=self.cdbFileKw.getValue().strip()
        xmlfile=self.xmlFileKw.getValue().strip()
        casename=self.caseNameKw.getValue().strip()
        num=self.form.List_Selected.getNumItems()
        errs=[]
        if not os.path.exists(odbfile):
            errs.append(u"请指定正确的odb计算结果文件路径!".encode('gbk'))
        if projectname=="":
            errs.append(u"请指定项目名称!".encode('gbk'))
        if projectunit=="":
            errs.append(u"请指定模型单位制!".encode('gbk'))
        if self.cdbcheck and cdbfile=="":
            errs.append(u"请指定网格文件路径!".encode('gbk'))
        if self.xmlcheck:
            if casename=="":
                errs.append(u"请指定工况名称!".encode('gbk'))
            if xmlfile=="":
                errs.append(u"请指定结果文件路径!".encode('gbk'))
            if num==0:
                errs.append(u"请选择输出结果类型!".encode('gbk'))   
        if len(errs)>0:
            showAFXErrorDialog(getAFXApp().getAFXMainWindow(),"\n".join(errs))
            return False
        output_types=[]
        for x in range(num):
            output_types.append(self.form.List_Selected.getItemText(x))
        #更新输出选择项
        self.outputKw.setValue(" ".join(output_types))
        return True

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def okToCancel(self):
        # No need to close the dialog when a file operation (such
        # as New or Open) or model change is executed.
        #
        # msg=[]
        # if self.xmlcheck:
        #     msg.append(u"cdb结果文件导出完毕!".encode('gbk'))
        # if self.cdbcheck:
        #     msg.append(u"xml结果文件导出完毕!".encode('gbk'))
        # if len(msg):
        #     showAFXInformationDialog(getAFXApp().getAFXMainWindow(),"\n".join(msg))
        return False


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Register the plug-in
#
thisPath = os.path.abspath(__file__)
thisDir = os.path.dirname(thisPath)

toolset = getAFXApp().getAFXMainWindow().getPluginToolset()
toolset.registerGuiMenuButton(
    buttonText='Export Data',
    object=Export_plugin(toolset),
    messageId=AFXMode.ID_ACTIVATE,
    icon=None,
    kernelInitString='import export_logic',
    applicableModules=ALL,
    version='N/A',
    author='N/A',
    description='N/A',
    helpUrl='N/A')
