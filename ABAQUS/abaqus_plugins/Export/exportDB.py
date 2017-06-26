# coding:utf-8
import os
from abaqusConstants import *
from abaqusGui import *
import glob

#输出类型
ALL_TYPES = ('Ux', 'Uy', 'Uz', 'Usum', 'Seqv', 'Sx', 'Sy', 'Sz',
    'Sxy', 'Sxz', 'Syz', 'S1', 'S2', 'S3')
#单位制
UNITS=("m-Mpa","mm-Mpa","m-pa","mm-pa","m-Kpa")

class ExportDB(AFXDataDialog):
    [ID_ADD,ID_DEL] = range(AFXDataDialog.ID_LAST, AFXDataDialog.ID_LAST + 2)
    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def __init__(self, form):
        self.form = form
        AFXDataDialog.__init__(self, form, u'数据导出'.encode('gbk'),
                               self.OK | self.CANCEL, DIALOG_ACTIONS_SEPARATOR)


        ###导出类型 CDB/XML分组控件
        GroupBox_ExportType = FXGroupBox(
            p=self,
            text=u'数据导出类型'.encode('gbk'),
            opts=FRAME_GROOVE | LAYOUT_FILL_X)
        ###水平框架控件
        HFrame_ExportType = FXHorizontalFrame(
            p=GroupBox_ExportType,
            opts=LAYOUT_FILL_X | LAYOUT_FILL_Y,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=60,
            pr=10,
            pt=0,
            pb=0)
        #导出CDB checkButton控件
        checkBox_Cdb = FXCheckButton(
            p=HFrame_ExportType, text=u'导出网格'.encode('gbk'),tgt=form.cdbcheckKw)
        #水平填充空白
        AFXVerticalAligner(
            p=HFrame_ExportType,
            opts=0,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=0,
            pr=80,
            pt=0,
            pb=0)
        #导出XML checkButton控件
        checkBox_Xml = FXCheckButton(
            p=HFrame_ExportType, text=u'导出结果'.encode('gbk'),tgt=form.xmlcheckKw)
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        # 提示导出CBD路径的Label控件
        Label_setTips = FXLabel(
            p=self, text=u'请选择odb结果输入文件'.encode('gbk'), opts=JUSTIFY_LEFT)
        Label_setTips.setFont(getAFXFont(FONT_BOLD))
        
        odbFileHandler = ExportDBFileHandler(form, 'odbFile',
                                             'odb files (*.odb)')
        ###水平框架控件
        HFrame_OdbFile = FXHorizontalFrame(
            p=self,
            opts=0,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=0,
            pr=0,
            pt=0,
            pb=0,
            hs=DEFAULT_SPACING,
            vs=DEFAULT_SPACING)
        ###abaques odb文件路径TextField控件
        Text_odbFile = AFXTextField(
            p=HFrame_OdbFile,
            ncols=50,
            labelText=u'odb文件路径:'.encode('gbk'),
            tgt=form.odbFileKw,
            sel=0,
            opts=AFXTEXTFIELD_STRING | LAYOUT_CENTER_Y)
        icon = afxGetIcon('fileOpen', AFX_ICON_SMALL)
        ###选择odb文件的Button控件
        Button_odbFile = FXButton(
            p=HFrame_OdbFile,
            text='',
            ic=icon,
            tgt=odbFileHandler,
            sel=AFXMode.ID_ACTIVATE,
            opts=BUTTON_NORMAL | LAYOUT_CENTER_Y,
            x=0,
            y=0,
            w=5,
            h=0,
            pl=1,
            pr=1,
            pt=1,
            pb=1)
        self.setdefault_odbfile()  # set default odb_file
        # Separator
        if isinstance(self, FXHorizontalFrame):
            FXVerticalSeparator(
                p=self, x=0, y=0, w=0, h=0, pl=2, pr=2, pt=2, pb=2)
        else:
            FXHorizontalSeparator(
                p=self, x=0, y=0, w=0, h=0, pl=2, pr=2, pt=2, pb=2)
        ###水平布局控件
        HFrame_Project = FXHorizontalFrame(
            p=self, opts=0, x=0, y=0, w=0, h=0, pl=0, pr=0, pt=0, pb=0)
        # 项目描述TextField控件
        Text_projectName = AFXTextField(
            p=HFrame_Project,
            ncols=20,
            labelText=u'项目名称:'.encode('gbk'),
            tgt=form.projectNameKw,
            sel=0)
        AFXVerticalAligner(p=HFrame_Project, pl=20)
        
        #项目单位的ComboBox控件
        ComboBox_projectUnit = AFXComboBox(
            p=HFrame_Project,
            ncols=18,
            nvis=1,
            text=u'项目单位:'.encode('gbk'),
            tgt=form.projectUnitKw,
            sel=0)
        ComboBox_projectUnit.setMaxVisible(5)
        ComboBox_projectUnit.appendItem("")
        for x in UNITS:
            ComboBox_projectUnit.appendItem(x)
        ###项目描述的TextField控件
        Text_projectProperty = AFXTextField(p=self,ncols=53,labelText=u'项目描述:'.encode('gbk'),tgt=form.projectPropertyKw,sel=0)
        ###指定cdb文件路径Label控件
        Label_projectTips = FXLabel(p=self, text=u'请选择网格文件输出路径:'.encode('gbk'), opts=JUSTIFY_LEFT)
        Label_projectTips.setFont(getAFXFont(FONT_BOLD))
        # 指定cdb文件路径按钮事件
        cdbFileHandler = ExportDBFileHandler(form, 'cdbFile','cdb files (*.cdb)')
        ###水平Frame 控件
        HFrame_CdbFile = FXHorizontalFrame(
            p=self,
            opts=0,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=0,
            pr=0,
            pt=0,
            pb=0,
            hs=DEFAULT_SPACING,
            vs=DEFAULT_SPACING)
        ###cdb文件路径 TextFiled控件
        Text_cdbFile = AFXTextField(
            p=HFrame_CdbFile,
            ncols=50,
            labelText=u'cdb文件路径:'.encode('gbk'),
            tgt=form.cdbFileKw,
            sel=0,
            opts=AFXTEXTFIELD_STRING | LAYOUT_CENTER_Y)
        #打开文件的按钮图标
        icon = afxGetIcon('fileOpen', AFX_ICON_SMALL)
        ###打开文件的Button控件
        Button_cdbFile = FXButton(
            p=HFrame_CdbFile,
            text='	Select File\nFrom Dialog',
            ic=icon,
            tgt=cdbFileHandler,
            sel=AFXMode.ID_ACTIVATE,
            opts=BUTTON_NORMAL | LAYOUT_CENTER_Y,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=1,
            pr=1,
            pt=1,
            pb=1)
        ###垂直分隔控件
        AFXVerticalAligner(
            p=self, opts=0, x=0, y=0, w=0, h=0, pl=0, pr=0, pt=0, pb=10)
        # 分割线
        if isinstance(self, FXHorizontalFrame):
            FXVerticalSeparator(
                p=self, x=0, y=0, w=0, h=0, pl=2, pr=2, pt=2, pb=2)
        else:
            FXHorizontalSeparator(
                p=self, x=0, y=0, w=0, h=0, pl=2, pr=2, pt=2, pb=2)
        # 加载工况信息
        AFXVerticalAligner(
            p=self, opts=0, x=0, y=0, w=0, h=0, pl=0, pr=0, pt=0, pb=10)
        HFrame_Case = FXHorizontalFrame(
            p=self, opts=0, x=0, y=0, w=0, h=0, pl=0, pr=0, pt=0, pb=0)
        ###工况ID ComboBox控件
        ComboBox_caseID = AFXComboBox(
            p=HFrame_Case,
            ncols=18,
            nvis=1,
            text=u'工况编号:'.encode('gbk'),
            tgt=form.caseIDKw,
            sel=0)
        ComboBox_caseID.setMaxVisible(10)
        for x in range(1, 11):
            ComboBox_caseID.appendItem(str(x))

        AFXVerticalAligner(
            p=HFrame_Case, opts=0, x=0, y=0, w=0, h=0, pl=0, pr=20, pt=0, pb=0)
        Text_caseName = AFXTextField(
            p=HFrame_Case,
            ncols=20,
            labelText=u'工况名称:'.encode('gbk'),
            tgt=form.caseNameKw,
            sel=0)
        Text_caseProperty = AFXTextField(
            p=self,
            ncols=53,
            labelText=u'工况描述:'.encode('gbk'),
            tgt=form.casePropertyKw,
            sel=0)
        # 可选/已选结果类型
        HFrame_ResultList = FXHorizontalFrame(
            p=self,
            opts=LAYOUT_FILL_X,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=0,
            pr=0,
            pt=0,
            pb=0)
        GroupBox_Optional = FXGroupBox(
            p=HFrame_ResultList,
            text=u'可选输出类型'.encode('gbk'),
            opts=FRAME_GROOVE | LAYOUT_FILL_X)
        VFrame_Optional = FXVerticalFrame(
            p=GroupBox_Optional,
            opts=FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=0,
            pr=0,
            pt=0,
            pb=0)
        # VFrame_Optional.setSelector(99)
        self.List_Optional = AFXList(
            p=VFrame_Optional,
            nvis=6,
            tgt=form.optionalListKw,
            sel=0,
            opts=HSCROLLING_OFF | LIST_SINGLESELECT | LAYOUT_FILL_X)
        for item in ALL_TYPES[11:]:
            self.List_Optional.appendItem(item)
        # 添加/删除按钮
        VFrame_AddDel = FXVerticalFrame(
            p=HFrame_ResultList, pl=5, pr=5, pt=30, pb=20)
        Button_Add = FXButton(
            p=VFrame_AddDel,
            text=u'添加>>'.encode('gbk'),
            w=30,
            ic=None,
            sel=self.ID_ADD)
        AFXVerticalAligner(p=VFrame_AddDel, pt=20, pb=20)
        Button_Del = FXButton(
            p=VFrame_AddDel,
            text=u'删除<<'.encode('gbk'),
            w=30,
            ic=None,
            sel=self.ID_DEL)
        GroupBox_Selected = FXGroupBox(
            p=HFrame_ResultList,
            text=u'已选输出类型'.encode('gbk'),
            opts=FRAME_GROOVE | LAYOUT_FILL_X)
        VFrame_Selected = FXVerticalFrame(
            p=GroupBox_Selected,
            opts=FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=0,
            pr=0,
            pt=0,
            pb=0)
        self.List_Selected = AFXList(
            p=VFrame_Selected,
            nvis=6,
            tgt=form.selectedListKw,
            sel=0,
            opts=HSCROLLING_OFF | LIST_SINGLESELECT | LAYOUT_FILL_X)
        for item in ALL_TYPES[:11]:
            self.List_Selected.appendItem(item)
        Label_caseTips = FXLabel(
            p=self, text=u'请选择xml结果输出文件路径:'.encode('gbk'), opts=JUSTIFY_LEFT)
        Label_caseTips.setFont(getAFXFont(FONT_BOLD))
        xmlFileHandler = ExportDBFileHandler(form, 'xmlFile',
                                             'xml file (*.xml)')
        HFrame_CdbFile = FXHorizontalFrame(
            p=self,
            opts=0,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=0,
            pr=0,
            pt=0,
            pb=0,
            hs=DEFAULT_SPACING,
            vs=DEFAULT_SPACING)

        Text_XmlFile = AFXTextField(
            p=HFrame_CdbFile,
            ncols=50,
            labelText=u'xml文件路径:'.encode('gbk'),
            tgt=form.xmlFileKw,
            sel=0,
            opts=AFXTEXTFIELD_STRING | LAYOUT_CENTER_Y)
        Button_XmlFile = FXButton(
            p=HFrame_CdbFile,
            text='	Select File\nFrom Dialog',
            ic=icon,
            tgt=xmlFileHandler,
            sel=AFXMode.ID_ACTIVATE,
            opts=BUTTON_NORMAL | LAYOUT_CENTER_Y,
            x=0,
            y=0,
            w=0,
            h=0,
            pl=1,
            pr=1,
            pt=1,
            pb=1)
        ########控件布局结束##########################################
        #添加按钮的事件映射
        FXMAPFUNC(self, SEL_COMMAND, self.ID_ADD, ExportDB.add_item)
        FXMAPFUNC(self, SEL_COMMAND, self.ID_DEL, ExportDB.remove_item)


    #获得Odb的默认文件路径
    def setdefault_odbfile(self):
        odbfiles = glob.glob(os.path.join(os.getcwd(), '*.odb'))
        if len(odbfiles):
            self.form.odbFileKw.setValue(odbfiles[0])


    
    def add_item(self, sender, sel, ptr):
        optional=self.List_Optional
        selected=self.List_Selected
        if operator == "Add":
            op_text = self.List_Optional.getValue()
            op_index = self.List_Optional.findItem(op_text)
            if op_index != -1:
                self.List_Optional.removeItem(op_index)
                self.List_Selected.appendItem(op_text)
        return 1

    def remove_item(self, sender, sel, ptr):
        self.List_Selected.appendItem("st_text")
        st_text = self.List_Selected.getValue()
        st_index = self.List_Selected.findItem(st_text)
        if st_index != -1:
            self.List_Selected.removeItem(st_index)
            self.List_Optional.appendItem(st_text)
        return 1


class ExportDBFileHandler(FXObject):

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def __init__(self, form, keyword, patterns='*'):

        self.form = form
        self.patterns = patterns
        self.patternTgt = AFXIntTarget(0)
        exec ('self.fileNameKw = form.%sKw' % keyword)
        self.readOnlyKw = AFXBoolKeyword(None, 'readOnly',
                                         AFXBoolKeyword.TRUE_FALSE)
        FXObject.__init__(self)
        FXMAPFUNC(self, SEL_COMMAND, AFXMode.ID_ACTIVATE,
                  ExportDBFileHandler.activate)

    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    def activate(self, sender, sel, ptr):

        fileDb = AFXFileSelectorDialog(
            getAFXApp().getAFXMainWindow(), 'Select a File', self.fileNameKw,
            self.readOnlyKw, AFXSELECTFILE_ANY, self.patterns, self.patternTgt)
        fileDb.setReadOnlyPatterns('*.odb')
        fileDb.create()
        fileDb.showModal()


#     #enable/disable控件的状态
#     def change_state(self, region):
#         ischecked="disable()"
#         if self.controls[region].getCheck():
#             ischecked = "enable()"
#         for key in self.controls:
#             if key.endswith("_%s" % region):
#                 exec("self.controls[%s].%s" % (key,ischecked))
