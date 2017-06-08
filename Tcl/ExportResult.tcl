package	 require msgcat
# encoding system euc-cn
namespace eval ::euidl::IO::ExportResult {} {

proc main {} \
{
	variable parent
	variable apdlfile
	variable optionallist
	variable selectedlist
	variable caseid
	variable casename
	variable caseproperty
	variable projectname
	variable projectproperty
	variable projectunits
	variable xmlfile
	variable cdbcheck
	variable xmlcheck
	variable cdbfile
	variable btnok 
	#####control to show and hide ######
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
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-550)/2}]
	set y [expr {([winfo screenheight .]-650)/2}]
	wm geometry .exportwindow 550x650+$x+$y

	#########Import Cdb File ###############
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
	#Create the frame and button control
	set xmlfileCtrls(frame_Optional)   [labelframe $parent.frame_Optional \
		-text [msgcat::mc "Optional Result "] \
		-font $font_cn]

	set xmlfileCtrls(frame_Selected)   [labelframe $parent.frame_Selected \
		-text [msgcat::mc "Selected Result "] \
		-font $font_cn]
	set frame_AddDel [frame $parent.frame_AddDel ]
	set frame_OkCancel [frame $parent.frame_OkCancel]
	#Create scrollBar for the listbox
	set scrollBar_Optional [scrollbar $parent.scrollBar_Optional \
		-orient vertical \
		-width 1\
		-command {.exportwindow.listbox_optional yview}]

	set scrollBar_Selected [scrollbar $parent.scrollBar_Selected\
		-orient vertical \
		-width 1\
		-command {.exportwindow.listbox_selected yview}]

	#create ListBox
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

	#
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

	set label_projectunits [label $parent.label_projectunits \
		-text "[msgcat::mc "Model Units "]:" \
		-font $font_cn]

  	set combo_projectunits [euidl::combobox $parent.combo_projectunits \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font_cn \
         -variable [scope projectunits]]

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

	#Create Button Ok 
	set btnok [button $parent.btn_ok\
		-text [msgcat::mc "Ok "] \
		-font $font_cn  \
		-width 6\
		-command [namespace code onButtonOkClick]]

	#Create Button Cancel
	set btncancel [button $parent.btn_cancel \
		-text [msgcat::mc "Cancel "] \
		-font $font_cn  \
		-width 6\
		-command {destroy .exportwindow}]

	$xmlfileCtrls(combo_caseid) insert list end [list 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9]
	$combo_projectunits insert list end [list "" "" "m-Pa" "m-Pa" "m-Kpa" "m-Kpa"  "m-Mpa" "m-Mpa" "mm-Mpa" "mm-Mpa"]
	#Start arrange these controls
	#Arrange the label tips

	grid $label_projectunits  -row 4 -column 0 -sticky w -pady 2m
	grid $combo_projectunits  -row 4 -column 0 -sticky e -pady 2m

	grid $xmlfileCtrls(label_caseid)  -row 4 -column 2 -sticky w -pady 2m
	grid $xmlfileCtrls(combo_caseid)  -row 4 -column 2 -sticky e -pady 2m


	grid $xmlfileCtrls(label_casename)  -row 5 -column 0 -sticky w -pady 2m
	grid $xmlfileCtrls(entry_casename)  -row 5 -column 0 -sticky e -pady 2m

	grid $xmlfileCtrls(label_caseproperty)  -row 5 -column 2 -sticky w -pady 2m
	grid $xmlfileCtrls(entry_caseproperty)   -row 5 -column 2 -sticky e -pady 2m


	pack  $xmlfileCtrls(listbox_Optional) $scrollBar_Optional \
	-in $xmlfileCtrls(frame_Optional) -side left -fill both -padx 2m -pady 4m 
	grid $xmlfileCtrls(frame_Optional) -row 6 -column 0 -sticky nsew

	#Arrange the frame_AddDel 
	pack $xmlfileCtrls(btnadd) $xmlfileCtrls(btndel)  -in $frame_AddDel -side top -pady 5m
	grid $frame_AddDel    -row 6 -column 1  -padx 1m

	#Arrange the frame_Selected
	pack $xmlfileCtrls(listbox_Selected)  $scrollBar_Selected \
	-in $xmlfileCtrls(frame_Selected) -fill y -side left -padx 2m -pady 4m 
	grid $xmlfileCtrls(frame_Selected) -row 6 -column 2 -sticky nsew 

	# pack $label_xmltips  -side top -anchor center -fill x 
	grid $xmlfileCtrls(label_xmltips)  -row 7 -column 0 -columnspan 2 -sticky nw -pady 2m
	grid $xmlfileCtrls(btn_xmlfile) -row 7 -column 2 -pady 2m 

	grid $xmlfileCtrls(entry_xmlfile) -row 8 -column 0 -columnspan 3 -sticky nsew 

	#packthe Ok and cacel button into frame
	pack $btnok  $btncancel -in $frame_OkCancel -padx 25m  -side left -fill x
	grid $frame_OkCancel  -row 9 -column 0 -columnspan 3 -sticky nsew  -pady 4m

	#initialize the window
	::euidl::IO::ExportResult::initialize
}



## save file in local disk
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
proc ChangeExportType {} \
{
	variable xmlcheck
	variable cdbcheck
	#project controls
	variable projectCtrls
	#xml controls
	variable xmlfileCtrls
	#cdb controls
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


# start export xml and  cdb
proc onButtonOkClick {} \
{
	variable parent
	variable btnok
	variable cdbcheck
	variable xmlcheck
	variable projectname
	variable projectproperty
	variable projectunits
	variable caseid
	variable casename
	variable caseproperty
	#export flag
	variable cdb_export
	variable xml_export

    set cdb_export 0
	set xml_export 0
	##############################
    # validate input  and export data
    if {[exportValidation]==1} {
    	#push down the button
		$btnok configure -relief sunken
		$btnok configure -state  disabled
    	#Select all
		catch {ans_sendcommand "ALLSEL,ALL"} err
		#set model units
		catch {ans_sendcommand "*set,_PROJECT_UNITS,'$projectunits'"} err
    	if {$cdb_export==1} {
				#save current project name property to _PROJECT_NAME&_PROJECT_PROPERTY
				catch {ans_sendcommand "*set,_PROJECT_NAME,'$projectname'"} err
				catch {ans_sendcommand "*set,_PROJECT_PROPERTY,'$projectproperty'"} err
				exportCdb
			}
    	if {$xml_export==1} {
				#save loadcase name property
				set casename_index     [format "_LOADCASE_%s_NAME"     $caseid]
				set caseproperty_index [format "_LOADCASE_%s_PROPERTY" $caseid]
				catch {ans_sendcommand "*set,$casename_index,'$casename'"} err
				catch {ans_sendcommand "*set,$caseproperty_index,'$caseproperty'"} err
				exportXml
			}
    } else {
    	$btnok configure -relief raised
    	$btnok configure -state  normal
    	return
    }
	# radise the button
	$btnok configure -relief raised
	$btnok configure -state  normal
	destroy $parent
	set msg [tk_messageBox -icon info \
		-title [msgcat::mc "Tips "] \
		-message  [msgcat::mc "Export Is Done! "]]
}

#validate the input data
proc exportValidation {} \
{
	variable parent
	variable cdbcheck
	variable xmlcheck
    variable xmlfile
	variable cdbfile
	variable projectname
	variable casename
	variable projectunits
	variable selectedlist
	#export flag
	variable cdb_export
	variable xml_export

    ###########cdb  Validate#####################
    if {$cdbcheck==0 && $xmlcheck==0} {
		tk_messageBox  -parent $parent \
			-icon info \
			-title [msgcat::mc "Tips "] \
			-message  [msgcat::mc "No Result Was Select! "]
		return  0
	}
    #if no project  name
	if {[string trim $projectname]==""} {
		tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
		-message  [msgcat::mc "Project Name Can Not Be Empty! "]
		return  0
	}

	#if no units
	if {[string trim $projectunits]==""} {
		tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
		-message  [msgcat::mc "Please Select Current Units! "]
		return 0
	}
   	if {$cdbcheck==1} {
		#if cdbfile path is Empty
		if {[string trim $cdbfile]==""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "Wrong File Path Of CDB! "]
			return 0
		}
	    # if file path contains space or chinese
		if {[regexp -all -inline -- {[\u4e00-\u9fa5]|\s+}  $cdbfile]!=""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "CDB File Path Contains Space or Chinese! "]
			return 0
		}
		set cdb_export 1
	###########xml  Validate#####################
    } 
    if {$xmlcheck==1} { 

		#if xmlfile path is Empty
		if {[string trim $xmlfile]==""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "Wrong File Path Of XML! "]
			return 0
		}
		# if file path contains space or chinese
		if {[regexp -all -inline -- {[\u4e00-\u9fa5]|\s+}  $xmlfile]!=""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "XML File Path Contains Space or Chinese! "]
			return 0
		}

		#if no case name
		if {[string trim $casename]==""} {
			tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "Case Name Can Not Be Empty! "]
			return 0
		}
		#if no result type was select
		if {[llength $selectedlist]==0} {
			tk_messageBox  -parent $parent \
			-icon warning \
			-title [msgcat::mc "Failed Export Result "] \
			-message  [msgcat::mc "No Result type is Selected! "]
			return 0
		}

		# sx sy sz sxy sxz syz s1 must export!!
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

proc exportCdb {} \
{
	variable cdbfile
	catch {ans_sendcommand "CDWRITE,ALL,'$cdbfile'"} err
}
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
	variable projectunits	

	catch {ans_sendcommand "/UIS,MSGPOP,3"} err
	#vwrite can only write 19 paras one time
	set resultlen [llength $selectedlist]
	set results {}
	#base units
    set MMMpa  {Ux 1 Uy 1 Uz 1 Usum 1 \
		Sx 1 Sy 1 Sz 1 S1 1  S2 1 S3 1 \
		Sxy 1 Syz 1 Sxz 1 Seqv 1}
	#translate projectunits m-pa to mm-Mpa
    set MMpa_MMMpa {Ux 1000 Uy 1000 Uz 1000 Usum 1000 \
		Sx 1 Sy 1 Sz 1 S1 1  S2 1 S3 1 \
		Sxy 1 Syz 1 Sxz 1 Seqv 1}
	set MPa_MMMpa  {Ux 1000 Uy 1000 Uz 1000 Usum 1000 \
		Sx 1.0e-6 Sy 1.0e-6 Sz 1.0e-6 S1 1.0e-6 S2 1.0e-6 S3 1.0e-6 Sxy \
		1.0e-6 Syz 1.0e-6 Sxz 1.0e-6 Seqv 1.0e-6}
    ##translate projectunits m-Kpa to mm-Mpa
    set MKpa_MMMpa  {Ux 1000 Uy 1000 Uz 1000 Usum 1000 \
		Sx 1.0e-3 Sy 1.0e-3 Sz 1.0e-3 S1 1.0e-3 S2 1.0e-3 S3 1.0e-3 Sxy \
		1.0e-3 Syz 1.0e-3 Sxz 1.0e-3 Seqv 1.0e-3}

    if {$projectunits=="m-Kpa"} {
    	array set theunitList $MKpa_MMMpa
    } elseif {$projectunits=="m-Pa"} {
    	array set theunitList $MPa_MMMpa
    } elseif {$projectunits=="m-Mpa"} {
        array set theunitList $MMpa_MMMpa
    } else {
        array set theunitList $MMMpa
    }
	
	#if the result nums >=12,then divide into to parts
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
	###start write apdl
	# puts $fl "*cfopen,$name,$postfix,$filedirname"
	#header
	puts $fl "*cfopen,'$xmlfile'"
	puts $fl {*vwrite}
	puts $fl {('<?xml version="1.0" encoding="gb2312"?>')}
	puts $fl {*vwrite}
	puts $fl {('<Results>')}
	#load case id
	puts $fl {*vwrite}
	puts $fl [format {('<Project name="%s">')} $projectname]
	puts $fl {*vwrite}
	puts $fl [format {('<Case id="%s" name="%s" property="%s">')} $caseid $casename $caseproperty]
	#apdl
	puts $fl {*get,_count,node,,count}
	puts $fl {*set,_num,0}

	puts $fl {*do,i,1,_count}
	puts $fl {_num=ndnext(_num)}
	puts $fl {*if,_num,GT,0,then}

	#such as >>>"*get,_Ux,node,_num,U, x"
	foreach var $selectedlist  {
		set args [parseLabel $var]
		# such as >>>*get,_ux,node,i,u,x
		puts $fl "*get,_$var,node,_num,$args"
	}
	## translate result projectunits
	foreach var $selectedlist  {
		if {[array get theunitList $var ]!=""} {
			puts $fl "*set,_$var,_$var*$theunitList($var)"
		}
	}
	#output format
	puts $fl  { *vwrite,chrval(_num) }
	puts $fl  {('<Node id="',A8,'">')}
	foreach result $results {
			#node result data,such as >>>"*vwrite,_ux,_uy,_uz
			set  line [join $result ",_"]
			puts $fl  "*vwrite,_$line"
			#node result format,such as ('<Ux>',F10.3,'</Ux>')
			set line {}
			foreach var $result {
				lappend line [format "'<%s>',E13.6,'</%s>'" $var $var]
			}
			set  line [join $line ",/"]
			puts $fl "($line)"
	}

	puts $fl {*vwrite}
	puts $fl {('</Node>')}
	#nest body
	puts $fl {*endif}
	puts $fl {*enddo}

	puts $fl {*vwrite}
	puts $fl {('</Case>')}


	#######Solid Element Data start########
	#element type list of Solid65
	set etlist {}  
	foreach {mat id} [ans_getlist TYPE] {
		if {[lindex $mat 1]=="SOLID65"} {
			lappend etlist $id
		}
	}
	if {$etlist != {}} {
		set pts {53 60 67 74 81 88 95 102}

		#select all element of Solid65
		set et0  [lindex  $etlist 0]
		puts $fl  "ESEL,S,TYPE,,$et0"
		for {set i 1} {$i < [llength $etlist]} {incr i} {
			set  et   [lindex  $etlist $i]
			puts $fl  "ESEL,A,TYPE,,$et"
		}
		
		#begin write data to apdl
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
	#######Solid Element Data end #######


	puts $fl {*vwrite}
	puts $fl {('</Project>')}
	#close vwrite
	puts $fl {*vwrite}
	puts $fl {('</Results>')}
	puts $fl {*cfclos}

	##close flie
	catch {close $fl} err 
	#############################
	#read the apdl
	catch {ans_sendcommand "/INPUT,$apdlfile"} err
	catch {ans_sendcommand "/UIS,MSGPOP,DEFA"} err
}

#initialize the listbox and check the result is/not exis
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
	variable projectunits


	set index  "_PROJECT_NAME"
	set flag [ans_getvalue PARM,$index,type]
	# set unitID [ans_getvalue ACTIVE,,UNITS]
	#Units specified by /UNITS command: \
	#0 = USER, 1 = SI, 2 = CGS, 3 = BFT, \
	#4 = BIN, 5 = MKS, 6 = MPA, 7 = uMKS.
	# set unitID  [string trim $unitID]
	# if {$unitID==-1 || $unitID==1} {
	# 	set projectunits "SI"
	# } else {
	# 	set projectunits "MPA"
	# }
	#set default case units
	set projectunits ""
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

#tranlate Sxy to  S,xy
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