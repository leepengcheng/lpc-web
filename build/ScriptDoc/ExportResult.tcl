package	 require msgcat
namespace eval ::euidl::IO::ExportResult {} {


#创建主界面并进行初始化
proc main {} \
{
	#变量区域
	variable parent
	variable apdlfile
	variable optionallist
	variable selectedlist
	variable caseid
	variable caseunits
	variable casename
	variable caseproperty
	variable projectname
	variable projectproperty
	variable xmlfile
	variable cdbcheck
	variable xmlcheck
	variable cdbfile
	variable btnok 
	variable projectCtrls
	variable cdbfileCtrls
	variable xmlfileCtrls

	set path [file dirname [info script]]
	msgcat::mcload  [file join $path locale]
	set apdlfile [file join [file dirname $path] apdl  ExportResult.apdl]
	set png_import [file join $path images import24x24.png]
	if [winfo exists .exportwindow] {
		  wm deiconify .exportwindow
		  raise .exportwindow
		  return
	}
	set parent [toplevel .exportwindow]
	set font_cn  {"Microsoft YaHei" 12 normal}

	wm title $parent [msgcat::mc "Export Result "]
	#调整窗口大小和位置
	set x [expr {([winfo screenwidth  .]-550)/2}]
	set y [expr {([winfo screenheight .]-650)/2}]
	wm geometry .exportwindow 550x650+$x+$y

	#创建导出CDB网格控件
	set projectCtrls(label_name) [label $parent.label_projectname \
		-text "[msgcat::mc "Project Name "]:" \
		-font $font_cn]

	set projectCtrls(entry_name) [entry $parent.entry_projectname \
		-textvariable [scope projectname] \
		-width 15\
		-font $font_cn]

	set projectCtrls(label_property) [label $parent.label_projectProperty \
		-text "[msgcat::mc "Project Property "]:" \
		-font $font_cn]

	set projectCtrls(entry_property) [entry $parent.entry_projectProperty \
		-textvariable [scope projectproperty] \
		-width 15\
		-font $font_cn]

	set cdbfileCtrls(label_tips) [label $parent.label_cdbtips \
		-text "[msgcat::mc "Please Select Cdb File "]:" \
		-font $font_cn]

	set cdbfileCtrls(entry_file) [entry $parent.entry_cdbfile  \
		-font $font_cn  \
		-textvariable [scope cdbfile]]

	set cdbfileCtrls(btn_file) [euidl::button $parent.btn_cdbfile \
		-tooltip [msgcat::mc "Please Select Cdb File "] \
		-font $font_cn \
		-imagefile $png_import \
		-command [list [code selectfile] cdb]]


	set frameExport [labelframe $parent.frameExport \
		-font $font_cn \
		-text [msgcat::mc "Export Data Type "]]

	set checkbtn_cdb [euidl::checkbutton $parent.checkbtn_cdb \
		-font $font_cn\
		-command [code ChangeExportType]\
		-text [msgcat::mc "Export Cdb File "]\
		-variable [scope cdbcheck]]

	set checkbtn_xml [euidl::checkbutton $parent.checkbtn_xml \
		-font $font_cn\
		-text [msgcat::mc "Export LoadCase File "]\
		-variable [scope xmlcheck]\
	    -command [code ChangeExportType]]	
	

	#控件布局
	pack $checkbtn_cdb $checkbtn_xml -in  $frameExport -side left -padx 12m -pady 1m
	grid $frameExport -row 0 -column 0  -columnspan 3 -sticky nsew 

	grid $projectCtrls(label_name)  -row 1 -column 0 -sticky w -pady 2m
	grid $projectCtrls(entry_name)  -row 1 -column 0 -sticky e -pady 2m
	grid $projectCtrls(label_property)  -row 1 -column 2 -sticky w -pady 2m
	grid $projectCtrls(entry_property)  -row 1 -column 2 -sticky e -pady 2m

	grid $cdbfileCtrls(label_tips) -row 2 -column 0 -columnspan 2 -sticky nw -pady 2m
	grid $cdbfileCtrls(btn_file) -row 2 -column 2 -pady 2m 
	grid $cdbfileCtrls(entry_file) -row 3 -column 0 -columnspan 3 -sticky nsew 
	

	#######################################################################
	#创建导出工况结果控件
	set xmlfileCtrls(frame_Optional)   [labelframe $parent.frame_Optional \
		-text [msgcat::mc "Optional Result "] \
		-font $font_cn]

	set xmlfileCtrls(frame_Selected)   [labelframe $parent.frame_Selected \
		-text [msgcat::mc "Selected Result "] \
		-font $font_cn]
	set frame_AddDel [frame $parent.frame_AddDel ]
	set frame_OkCancel [frame $parent.frame_OkCancel]

	#创建滚动条
	set scrollBar_Optional [scrollbar $parent.scrollBar_Optional \
		-orient vertical \
		-width 1\
		-command {.exportwindow.listbox_optional yview}]

	set scrollBar_Selected [scrollbar $parent.scrollBar_Selected\
		-orient vertical \
		-width 1\
		-command {.exportwindow.listbox_selected yview}]

	set xmlfileCtrls(listbox_Optional) [listbox $parent.listbox_optional \
		-yscroll "$scrollBar_Optional set" \
		-listvariable [scope optionallist]\
		-font $font_cn \
		-width 19 \
		-height 8 \
		-selectmode extended]

	set xmlfileCtrls(listbox_Selected) [listbox $parent.listbox_selected\
		-yscroll "$scrollBar_Selected set" \
		-listvariable [scope selectedlist] \
		-font $font_cn \
		-width 19 \
		-height 8 \
		-selectmode extended]

	set xmlfileCtrls(label_caseid) [label $parent.label_caseid \
		-text "[msgcat::mc "Case ID "]:" \
		-font $font_cn]

  	set xmlfileCtrls(combo_caseid) [euidl::combobox $parent.combo_caseid \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -command [code autoCompleteData]\
         -width 13\
         -font $font_cn \
         -variable [scope caseid]]

	set xmlfileCtrls(label_caseunits) [label $parent.label_caseunits \
		-text "[msgcat::mc "Case Units "]:" \
		-font $font_cn]

  	set xmlfileCtrls(combo_caseunits) [euidl::combobox $parent.combo_caseunits \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font_cn \
         -variable [scope caseunits]]

	set xmlfileCtrls(label_casename) [label $parent.label_casename \
		-text "[msgcat::mc "Case Name "]:" \
		-font $font_cn]

	set xmlfileCtrls(entry_casename) [entry $parent.entry_casename \
		-textvariable [scope casename] \
		-width 15\
		-font $font_cn]

	set frame_CaseProperty [frame $parent.frame_caseproperty]
	set xmlfileCtrls(label_caseproperty) [label $parent.label_caseproperty \
		-text "[msgcat::mc "Property "]:" \
		-font $font_cn]

	set xmlfileCtrls(entry_caseproperty) [entry $parent.entry_caseproperty \
		-textvariable [scope caseproperty] \
		-width 15\
		-font $font_cn]

	set xmlfileCtrls(btnadd) [button $parent.btn_add \
		-text "[msgcat::mc "Add "]>>" \
		-width 7 \
		-font $font_cn \
		-command [namespace code button_add_click]]

	set xmlfileCtrls(btndel) [button $parent.btn_del \
		-text "[msgcat::mc "Del "]<<" \
		-width 7\
		-font $font_cn \
		-command [namespace code button_del_click]]

	set xmlfileCtrls(label_xmltips) [label $parent.label_xmltips \
		-text "[msgcat::mc "Please Select LoadCase File "]:" \
		-font $font_cn]

	set xmlfileCtrls(entry_xmlfile) [entry $parent.entry_xmlfile  \
		-font $font_cn  \
		-textvariable [scope xmlfile]]

	set xmlfileCtrls(btn_xmlfile) [euidl::button $parent.btn_xmlfile \
		-tooltip [msgcat::mc "Please Select LoadCase File "] \
		-font $font_cn \
		-imagefile $png_import \
		-command [list [code selectfile] xml]]

	#创建确定按钮
	set btnok [button $parent.btn_ok\
		-text [msgcat::mc "Ok "] \
		-font $font_cn  \
		-width 6\
		-command [namespace code onButtonOkClick]]

	#确定取消按钮
	set btncancel [button $parent.btn_cancel \
		-text [msgcat::mc "Cancel "] \
		-font $font_cn  \
		-width 6\
		-command {destroy .exportwindow}]

	
	$xmlfileCtrls(combo_caseid) insert list end [list 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9]
	$xmlfileCtrls(combo_caseunits) insert list end [list "" "" "m-Pa" "SI" "m-Kpa" "MK"  "m-Mpa" "MM" "mm-Mpa" "MPA"]

	#控件布局
	grid $xmlfileCtrls(label_caseunits)  -row 4 -column 0 -sticky w -pady 2m
	grid $xmlfileCtrls(combo_caseunits)  -row 4 -column 0 -sticky e -pady 2m

	grid $xmlfileCtrls(label_caseid)  -row 4 -column 2 -sticky w -pady 2m
	grid $xmlfileCtrls(combo_caseid)  -row 4 -column 2 -sticky e -pady 2m


	grid $xmlfileCtrls(label_casename)  -row 5 -column 0 -sticky w -pady 2m
	grid $xmlfileCtrls(entry_casename)  -row 5 -column 0 -sticky e -pady 2m

	grid $xmlfileCtrls(label_caseproperty)  -row 5 -column 2 -sticky w -pady 2m
	grid $xmlfileCtrls(entry_caseproperty)   -row 5 -column 2 -sticky e -pady 2m


	pack  $xmlfileCtrls(listbox_Optional) $scrollBar_Optional \
	-in $xmlfileCtrls(frame_Optional) -side left -fill both -padx 2m -pady 4m 
	grid $xmlfileCtrls(frame_Optional) -row 6 -column 0 -sticky nsew

	
	pack $xmlfileCtrls(btnadd) $xmlfileCtrls(btndel)  -in $frame_AddDel -side top -pady 5m
	grid $frame_AddDel    -row 6 -column 1  -padx 1m

	
	pack $xmlfileCtrls(listbox_Selected)  $scrollBar_Selected \
	-in $xmlfileCtrls(frame_Selected) -fill y -side left -padx 2m -pady 4m 
	grid $xmlfileCtrls(frame_Selected) -row 6 -column 2 -sticky nsew 


	grid $xmlfileCtrls(label_xmltips)  -row 7 -column 0 -columnspan 2 -sticky nw -pady 2m
	grid $xmlfileCtrls(btn_xmlfile) -row 7 -column 2 -pady 2m 

	grid $xmlfileCtrls(entry_xmlfile) -row 8 -column 0 -columnspan 3 -sticky nsew 


	pack $btnok  $btncancel -in $frame_OkCancel -padx 25m  -side left -fill x
	grid $frame_OkCancel  -row 9 -column 0 -columnspan 3 -sticky nsew  -pady 4m

	#初始化参数
	::euidl::IO::ExportResult::initialize
}



## 选择文件的保存路径
proc selectfile {filetype} \
{
	variable cdbfile
	variable xmlfile	
	variable caseid
	variable projectname

	set projectname [string trim $projectname]
	set xmlfilename [format "%s-case-%s.xml" $projectname $caseid]
	set cdbfilename [format "%s.cdb" $projectname]
	if {$filetype=="xml"} {
		set xmlfile [tk_getSaveFile -initialfile $xmlfilename -filetypes {{xml .xml}}]
	} else {
		set cdbfile [tk_getSaveFile -initialfile $cdbfilename -filetypes {{cdb .cdb}}]
	}

}
#当改变导出结果类型时enable/disable对应控件
proc ChangeExportType {} \
{
	#全局变量
	variable xmlcheck
	variable cdbcheck
	variable projectCtrls
	variable xmlfileCtrls
	variable cdbfileCtrls

	set state_xml [expr $xmlcheck==1?"normal":"disabled"]
	set state_cdb [expr $cdbcheck==1?"normal":"disabled"]

	foreach name [array names projectCtrls] {
		if {[string match "*property" $name]} {
			$projectCtrls($name) configure -state $state_cdb
		}
	}

	foreach name [array names cdbfileCtrls] {
		$cdbfileCtrls($name) configure -state $state_cdb
	}


	foreach name [array names xmlfileCtrls] {
		if {[string match "frame*" $name]} {
			$xmlfileCtrls($name) configure -foreground [expr $xmlcheck==1?"black":"gray"]
		} else {
			$xmlfileCtrls($name) configure -state $state_xml
		}
		
	}

}


# 点击确定开始导出网格和结果文件
proc onButtonOkClick {} \
{
	#变量区域
	variable parent
	variable btnok
	variable cdbcheck
	variable xmlcheck
	variable projectname
	variable projectproperty
	variable caseid
	variable casename
	variable caseproperty
	variable cdb_export
	variable xml_export

	#设置初始值
    set cdb_export 0
	set xml_export 0
	##############################
    # 验证输入信息的正确性
    if {[exportValidation]==1} {
		#按下按钮
		$btnok configure -relief sunken
		$btnok configure -state  disabled
    	#选择所有
		catch {ans_sendcommand "ALLSEL,ALL"} err
		#将当前项目名称/属性 保存到变量 _PROJECT_NAME&_PROJECT_PROPERTY
		catch {ans_sendcommand "*set,_PROJECT_NAME,'$projectname'"} err
		catch {ans_sendcommand "*set,_PROJECT_PROPERTY,'$projectproperty'"} err
		set casename_index     [format "_LOADCASE_%s_NAME"     $caseid]
		set caseproperty_index [format "_LOADCASE_%s_PROPERTY" $caseid]
		catch {ans_sendcommand "*set,$casename_index,'$casename'"} err
		catch {ans_sendcommand "*set,$caseproperty_index,'$caseproperty'"} err
    	if {$cdb_export==1} {exportCdb}
    	if {$xml_export==1} {exportXml}
    } else {
    	$btnok configure -relief raised
    	$btnok configure -state  normal
    	return
    }
	#恢复按钮状态
	$btnok configure -relief raised
	$btnok configure -state  normal
	destroy $parent
	set msg [tk_messageBox -icon info \
		-title [msgcat::mc "Tips "] \
		-message  [msgcat::mc "Export Is Done! "]]
}

#验证输入信息格式是否正确
proc exportValidation {} \
{
	variable parent
	variable cdbcheck
	variable xmlcheck
    variable xmlfile
	variable cdbfile
	variable projectname
	variable casename
	variable caseunits
	variable selectedlist
	#导出网格和结果信息的标识符
	variable cdb_export
	variable xml_export

	###########导出cdb网格文件的数据验证##################
    #当“导出CDB”和“导出XML”都没有选中时
    if {$cdbcheck==0 && $xmlcheck==0} {
		tk_messageBox  -parent $parent \
			-icon info \
			-title [msgcat::mc "Tips "] \
			-message  [msgcat::mc "No Result Was Select! "]
		return  0
	}
    #当没有输入项目名称时
	if {[string trim $projectname]==""} {
		tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
		-message  [msgcat::mc "Project Name Can Not Be Empty! "]
		return  0
	}
   	if {$cdbcheck==1} {
		#当cdb文件路径为空时
		if {[string trim $cdbfile]==""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "Wrong File Path Of CDB! "]
			return 0
		}
	    #当cdb按文件路径包含空格和中文时
		if {[regexp -all -inline -- {[\u4e00-\u9fa5]|\s+}  $cdbfile]!=""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "CDB File Path Contains Space or Chinese! "]
			return 0
		}
		set cdb_export 1
	###########导出xml结果文件的的数据验证#####################
    } 
    if {$xmlcheck==1} { 
	    #如果没有选择导出单位制
		if {[string trim $caseunits]==""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "Please Select Current Units! "]
			return 0
		}

		#如果xml结果文件的路径为空
		if {[string trim $xmlfile]==""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "Wrong File Path Of XML! "]
			return 0
		}
		#如果xml文件路径包含空格和中文
		if {[regexp -all -inline -- {[\u4e00-\u9fa5]|\s+}  $xmlfile]!=""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "XML File Path Contains Space or Chinese! "]
			return 0
		}

		#如果没有输入工况名称
		if {[string trim $casename]==""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "Case Name Can Not Be Empty! "]
			return 0
		}
		#如果没有选择导出结果类型
		# if {[llength $selectedlist]==0} {
		# 	tk_messageBox  -parent $parent \
		# 	-icon warning \
		# 	-title [msgcat::mc "Failed Export Result "] \
		# 	-message  [msgcat::mc "No Result type is Selected! "]
		# 	return 0
		# }

		#如果sx sy sz sxy sxz syz s1 中任意1个结果没有导出
		#后续配筋计算必须至少需要这7个结果类型
		set essentials {Sx Sy Sz Sxy Sxz Syz S1}
		foreach var $essentials {
			if {[lsearch $selectedlist $var]<0} {
			tk_messageBox  -parent $parent \
			-icon warning \
			-title [msgcat::mc "Failed Export Result "] \
			-message  "[msgcat::mc "Output Results Must Contain: "]$var\n[msgcat::mc "Else Can Not Solve! "]"
			return 0
			}
		}

		set xml_export 1
	} 
	return  1
}

#导出CDB网格文件
proc exportCdb {} \
{
	variable cdbfile
	catch {ans_sendcommand "CDWRITE,ALL,'$cdbfile'"} err
}
#导出Xml结果文件
proc exportXml {} \
{
	variable apdlfile
	variable parent
	variable selectedlist	
	variable xmlfile
	variable caseid
	variable casename
	variable caseproperty
	variable projectname
	variable caseunits	

	#屏蔽ANSYS警告信息
	catch {ans_sendcommand "/UIS,MSGPOP,3"} err
	#vwrite 每次只能导出19个结果类型
	set resultlen [llength $selectedlist]
	set results {}
	#mm-Mpa单位制,默认单位制，所有系数均为1
    set MMMpa  {Ux 1 Uy 1 Uz 1 Usum 1 \
		Sx 1 Sy 1 Sz 1 S1 1  S2 1 S3 1 \
		Sxy 1 Syz 1 Sxz 1 Seqv 1}
	#m-pa 到 mm-Mpa 单位制转换
    set MMpa_MMMpa {Ux 1000 Uy 1000 Uz 1000 Usum 1000 \
		Sx 1 Sy 1 Sz 1 S1 1  S2 1 S3 1 \
		Sxy 1 Syz 1 Sxz 1 Seqv 1}
	set MPa_MMMpa  {Ux 1000 Uy 1000 Uz 1000 Usum 1000 \
		Sx 1.0e-6 Sy 1.0e-6 Sz 1.0e-6 S1 1.0e-6 S2 1.0e-6 S3 1.0e-6 Sxy \
		1.0e-6 Syz 1.0e-6 Sxz 1.0e-6 Seqv 1.0e-6}
    ##m-Kpa 到mm-Mpa 单位制转换
    set MKpa_MMMpa  {Ux 1000 Uy 1000 Uz 1000 Usum 1000 \
		Sx 1.0e-3 Sy 1.0e-3 Sz 1.0e-3 S1 1.0e-3 S2 1.0e-3 S3 1.0e-3 Sxy \
		1.0e-3 Syz 1.0e-3 Sxz 1.0e-3 Seqv 1.0e-3}

    if {$caseunits=="MK"} {
    	array set theunitList $MKpa_MMMpa
    } elseif {$caseunits=="SI"} {
    	array set theunitList $MPa_MMMpa
    } elseif {$caseunits=="MM"} {
        array set theunitList $MMpa_MMMpa
    } else {
        array set theunitList $MMMpa
    }
	
	#如果输出结果类型个数>=12,分成2行输出
	if {$resultlen>=12} {
		set len [expr $resultlen/2]
		incr len -1
		lappend results [lrange $selectedlist 0 $len]
		incr len 
		lappend results [lrange $selectedlist $len end]
	} else {
		lappend results $selectedlist
	}
	set fl [open $apdlfile w]
	#####开始写出APDL############
	puts $fl "*cfopen,'$xmlfile'"
	#xml根节点元素
	puts $fl {*vwrite}
	puts $fl {('<?xml version="1.0" encoding="gb2312"?>')}
	puts $fl {*vwrite}
	puts $fl {('<Results>')}
	#xml工况节点元素
	puts $fl {*vwrite}
	puts $fl [format {('<Project name="%s">')} $projectname]
	puts $fl {*vwrite}
	puts $fl [format {('<Case id="%s" name="%s" property="%s">')} $caseid $casename $caseproperty]
	#主APDL
	puts $fl {*get,_count,node,,count}
	puts $fl {*set,_num,0}

	puts $fl {*do,i,1,_count}
	puts $fl {_num=ndnext(_num)}
	puts $fl {*if,_num,GT,0,then}

	#类似于 >>>"*get,_Ux,node,_num,U, x"
	foreach var $selectedlist  {
		set args [parseLabel $var]
		puts $fl "*get,_$var,node,_num,$args"
	}
	#根据选择的单位制将输出值转化为mm-mpa
	foreach var $selectedlist  {
		if {[array get theunitList $var ]!=""} {
			puts $fl "*set,_$var,_$var*$theunitList($var)"
		}
	}
	#格式化输出
	puts $fl  { *vwrite,chrval(_num) }
	puts $fl  {('<Node id="',A8,'">')}
	foreach result $results {
			#输出节点结果数据，例如 >>>"*vwrite,_ux,_uy,_uz
			set  line [join $result ",_"]
			puts $fl  "*vwrite,_$line"
			#输出节点结果数据，例如 >>> ('<Ux>',F10.3,'</Ux>')
			set line {}
			foreach var $result {
				lappend line [format "'<%s>',E13.6,'</%s>'" $var $var]
			}
			set  line [join $line ",/"]
			puts $fl "($line)"
	}

	puts $fl {*vwrite}
	puts $fl {('</Node>')}
	#do嵌套
	puts $fl {*endif}
	puts $fl {*enddo}

	puts $fl {*vwrite}
	puts $fl {('</Case>')}


	#######Solid65单元破坏信息导出########
	#检查当前定义的的solid65单元类型
	set etlist {}  
	foreach {mat id} [ans_getlist TYPE] {
		if {[lindex $mat 1]=="SOLID65"} {
			lappend etlist $id
		}
	}
	if {$etlist != {}} {
		set pts {53 60 67 74 81 88 95 102}

		#选择所有类型为solid65的单元
		set et0  [lindex  $etlist 0]
		puts $fl  "ESEL,S,TYPE,,$et0"
		for {set i 1} {$i < [llength $etlist]} {incr i} {
			set  et   [lindex  $etlist $i]
			puts $fl  "ESEL,A,TYPE,,$et"
		}
		
		#开始写出单元信息
		puts $fl {*vwrite}
		puts $fl {('<Elements>')}

		puts $fl  {*GET,_ecount,ELEM,,COUNT}
		puts $fl  {*SET,_enum,0}
		puts $fl  {*DO,I,1,_ecount}
		puts $fl  {_enum=ELNEXT(_enum)}
		puts $fl  {*IF,_enum,NE,0,THEN}
		for {set i 0 } {$i < [llength $pts]} {incr i} {
			set num [expr $i+1]
			set index [lindex $pts $i]
			puts $fl "*GET,_c$num,ELEM,_enum,NMISC,$index"
		}
		puts $fl {*vwrite,chrval(_enum),_c1,_c2,_c3,c_4,_c5,_6,_c7,_c8}
		puts $fl {('<Element id="',A8,'">',8F5.1,'</Element>')}
		puts $fl {*ENDIF}
		puts $fl {*ENDDO}
		puts $fl {*vwrite}
		puts $fl {('</Elements>')}
	} 
	#######完成破坏性信息导出#######


	puts $fl {*vwrite}
	puts $fl {('</Project>')}
	puts $fl {*vwrite}
	puts $fl {('</Results>')}
	puts $fl {*cfclos}

	##关闭文件
	catch {close $fl} err 
	#############################
	#读入APDL文件并执行
	catch {ans_sendcommand "/INPUT,$apdlfile"} err
	catch {ans_sendcommand "/UIS,MSGPOP,DEFA"} err
}

#初始化参数，检查当前模型的的结果类型，并添加到可选结果类型/已选结果类型列表中
proc initialize {} \
{
 	# variable allresults 
 	# set allresults {Ux Uy Uz Usum Sxy Syz Sxz S1 S2 S3 Seqv EPTOx EPTOy EPTOz EPTOxy EPTOyz EPTOxz EPTO1 EPTO2 EPTO3 EPTOeqv}
 	variable optionallist
	variable selectedlist
	variable projectname
	variable caseid
	variable xmlcheck
	variable cdbcheck
	variable tipsFlag
	variable caseunits


	set index  "_PROJECT_NAME"
	set flag [ans_getvalue PARM,$index,type]
	# set unitID [ans_getvalue ACTIVE,,UNITS]
	#Units specified by /UNITS command: \
	#0 = USER, 1 = SI, 2 = CGS, 3 = BFT, \
	#4 = BIN, 5 = MKS, 6 = MPA, 7 = uMKS.
	# set unitID  [string trim $unitID]
	# if {$unitID==-1 || $unitID==1} {
	# 	set caseunits "SI"
	# } else {
	# 	set caseunits "MPA"
	# }
	#set default case units
	set caseunits ""
	if {[string trim $flag]!=-1} {
		#set the projectname to variable "_PROJECT_NAME"
		set projectname [string trim [ans_getvalue parm,$index,value]]
	} else {
		#set the projectname to jobname
		set projectname [string trim [ans_getvalue "ACTIVE,0,JOBNAM"]]
	}
	set optionallist {}
	set selectedlist {}
	set caseid 1
	set cdbcheck 1
	set xmlcheck 1
	# Tips Flag
	set tipsFlag 0


	set list1  {EPTOx EPTOy EPTOz EPTOxy EPTOyz EPTOxz EPTO1 EPTO2 EPTO3 EPTOeqv}
	set list2  {Ux Uy Uz Usum Sx Sy Sz Sxy Syz Sxz  S1 S2 S3 Seqv}
	set firstnode [ans_evalexpr "ndnext(0)"]
	#Suppresses error message pop-up dialog box
	catch {ans_sendcommand "/UIS,MSGPOP,4"} err
	foreach result $list1 {
		set args [parseLabel $result]
		if {[catch {set _tmp_ [ans_getvalue "node,$firstnode,$args"]} err]} {
			} else {
				lappend optionallist $result
			}
	}
	foreach result $list2 {
		set args [parseLabel $result]
		if {[catch {set _tmp_ [ans_getvalue "node,$firstnode,$args"]} err]} {
			puts $ckerr
		} else {
			lappend  selectedlist $result
		}
	}
	# #Enable  error message pop-up dialog box
	catch {ans_sendcommand "/UIS,MSGPOP,DEFA"} err

	#show and hide the controls
	ChangeExportType
}

#将结果类型字符串分解，例如Sxy分解为S和xy
proc parseLabel {var} \
{
	set arg1 [string index $var 0]
	if {$arg1=="U" ||$arg1=="S"} {
	set arg2 [string range $var 1 end]
	} else {
	set arg1 "EPTO"
	set arg2 [string range $var 4 end]
	}
	return $arg1,$arg2
}

#将可选结果类型列表中的选项添加到已选结果类型列表中
proc button_add_click {} \
{
	variable parent
	variable optionallist
	variable selectedlist
	variable xmlfileCtrls
	variable tipsFlag

	#if no result
	

	set templist $optionallist
	set sellist [$xmlfileCtrls(listbox_Optional) curselection]
	if {$sellist==""} {
		return
	}

	foreach sel $sellist {
		lset templist $sel  {}
		lappend selectedlist [$xmlfileCtrls(listbox_Optional) get $sel]
	}

	set optionallist {}
	foreach var $templist {
		if {![string match {} $var]} {
			lappend  optionallist $var
		}
	}
	if {[llength $selectedlist]>8 && $tipsFlag==0} {
		tk_messageBox  -parent $parent \
		-icon info \
		-title [msgcat::mc "Tips "] \
		-message  [msgcat::mc "Too Many Result is Selected! "]
		set tipsFlag 1
	}		
}

#将已选结果类型列表中的选项删除，并添加到到可选结果类型列表中
proc button_del_click {args} \
{
	variable optionallist
	variable selectedlist
	variable xmlfileCtrls

	set templist $selectedlist
	set sellist [$xmlfileCtrls(listbox_Selected) curselection]
	if {$sellist==""} {
		return
	}
	foreach sel $sellist {
		lset templist $sel  {}
		lappend optionallist [$xmlfileCtrls(listbox_Selected) get $sel]
	}

	set selectedlist {}
	foreach var $templist {
		if {![string match {} $var]} {
			lappend  selectedlist $var
		}
	}
	
}


#根据ANSYS Db文件名称自动填充工况工况名称和属性
proc autoCompleteData {} \
{
	variable caseid
	variable casename
	variable caseproperty

	set casename_index     [format "_LOADCASE_%s_NAME"     $caseid]
	set caseproperty_index [format "_LOADCASE_%s_PROPERTY" $caseid]

	if {[ans_getvalue parm,$casename_index,type]!=-1} {
		set casename [string trim [ans_getvalue parm,$casename_index,value]]
	} 
	if {[ans_getvalue parm,$caseproperty_index,type]!=-1} {
		set caseproperty [string trim [ans_getvalue parm,$caseproperty_index,value]]
	} 	
}
}


::euidl::IO::ExportResult::main