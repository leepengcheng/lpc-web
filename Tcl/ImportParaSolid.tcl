package	 require msgcat	
namespace eval  ImportParaSolid {
proc main {} \
{
	variable parent
	variable msg
	variable font_cn
	variable progressimport
	variable filename
	variable  buttonok

	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	::msgcat::mcload  [file join $path locale import]
	set png_import [file join $path images import24x24.png]

	if [winfo exists .importxtwindow] {
		wm deiconify .importxtwindow
		raise .importxtwindow
		return
	}
	set parent [toplevel .importxtwindow]
	wm title $parent [::msgcat::mc "Import ParaSolid Model "]
	wm iconname $parent "ImportParaSolid"
	set x [expr {([winfo screenwidth  .]-360)/2}]
	set y [expr {([winfo screenheight .]-160)/2}]
	set font_cn  {"Microsoft YaHei" 12 normal}
	wm geometry .importxtwindow  360x160+$x+$y

	set labeltips [label $parent.labeltips \
		-text "[::msgcat::mc "Please Select ParaSolid File "]:" \
		-font $font_cn]
	set textfile [entry $parent.textfile \
		-textvariable [scope filename] \
		-font $font_cn]

	set buttonselect [euidl::button $parent.selectbtn \
		-tooltip [::msgcat::mc "Please Select ParaSolid File "] \
		-font $font_cn \
		-imagefile $png_import \
		-command [code btn_select_click]]

	set frame_okcancel [frame $parent.frame_okcancel]

	set buttonok [button $parent.ok \
		-text [::msgcat::mc "Ok "] \
		-width 8 \
		-font $font_cn \
		-command [code btn_ok_click] ]

	set buttoncancel   [button $parent.cancel \
		-text [::msgcat::mc "Cancel "] \
		-width 8 \
		-font $font_cn \
		-command {destroy .importxtwindow}]
	# set progressimport [label  $parent.progress  -type  normal -width  40 -height  20  -variable msg -maximum  100]
	# set progressimport [label  $parent.progress  -width  3  -height  1  -textvariable [scope msg] -bg white -font $font_cn]

	grid  $labeltips    -row 0 -column 0   -pady 4m
	grid  $buttonselect -row 0 -column 1 
	grid  $textfile     -row 1 -column 0  -columnspan 2  -sticky ew 
	grid $frame_okcancel -row 2 -column 0 -columnspan 2
	pack $buttonok $buttoncancel -in $frame_okcancel -side left -padx 8m -pady 6m
	# grid  $progressimport  -row 3 -column 0 -columnspan 2 -sticky w
	#initialize the param
	ImportParaSolid::initialize
}

proc btn_select_click {} \
{
	variable filename
	set filename [tk_getOpenFile -initialfile *.x_t -filetypes {{ParaSolid .x_t}}]
}
proc btn_ok_click {} \
{
	variable filename
	variable parent
	variable  buttonok
	variable msg

    # if file path contains space or chinese
	if {[regexp -all -inline -- {[\u4e00-\u9fa5]|\s+}  $filename]!=""} {
		tk_messageBox  -parent $parent -icon warning -title [msgcat::mc "Tips "] \
		-message  [msgcat::mc "File Path Contains Space or Chinese! "]
		return 
	}

	if {![file exists $filename]} {
		tk_messageBox  -parent $parent -icon warning -title [::msgcat::mc "Failed import model "] \
		-message  [::msgcat::mc "File not exists,Please check the path! "]
		return
	}
	set filedirname [file dirname $filename]
	set name_postfix [split [file tail $filename] .]
	set name [lindex $name_postfix 0]
	set postfix [lindex $name_postfix 1]
	set filesize [file size $filename]
	$buttonok configure -relief sunken
	$buttonok configure -state disabled
	#######################################
	ans_sendcommand  "/NOPR"
	ans_sendcommand  "~PARAIN,'$name','$postfix','$filedirname',ALL,0,0   "
	ans_sendcommand  "/FACET,NORML"
	ans_sendcommand  "/REPLOT"
	ans_sendcommand   "VPLOT"
	ans_sendcommand   "/GOPR"
	# after 1000 set msg 100
	#########################################
	$buttonok configure -relief raised
	$buttonok configure -state normal
	destroy .importxtwindow
}
# proc start {filesize} \
# {
# 	variable msg
# 	variable progressimport

# 	##file size >>>progressbar length
# 	set index [expr $filesize/1024/20]

# 	# $progressimport configure -maximum $index
#     for {set i 1} {$i < 10} {incr i} {
# 	    after 100  set msg "$i%"
# 	    $progressimport configure -width $i
# 	    vwait msg
#     }
# }
proc initialize {} \
{
	variable filename
	variable msg
	variable buttonok

	set filename {}
	set msg { }
	$buttonok configure -relief raised
	$buttonok configure -state normal
}

}

ImportParaSolid::main
