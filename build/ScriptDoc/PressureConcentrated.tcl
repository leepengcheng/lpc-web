package	 require msgcat
# encoding system euc-cn
namespace eval ::Customized::Util::ConcentratedForce {

proc main {} {\

	variable parent
	variable btn_pick
	variable font_cn
	variable font_en
	#####################
	variable fx
	variable fy
	variable fz
	variable factor
	variable nodes

	global png_pick
	global png_check

	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	::msgcat::mcload  [file join $path locale load]
	set png_pick [file join $path images pick24x24.png]
	set png_check [file join $path images check24x24.png]

	#destroy the window
	if [winfo exists .concentratedforce] {
	  wm deiconify .concentratedforce
	  raise .concentratedforce
	  return
	}
	set parent [toplevel .concentratedforce]
	set font_cn  {"Microsoft YaHei" 12 normal}
	set font_en  {Consolas 12 normal}
	wm title $parent [::msgcat::mc "Concentrated Force "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-330)/2}]
	set y [expr {([winfo screenheight .]-320)/2}]
	wm geometry .concentratedforce 330x320+$x+$y
	

    set label_pick [euidl::label $parent.label_pick \
	    -height 2 \
	    -font $font_cn \
	    -text [msgcat::mc "Selected Nodes: "]]

	set btn_pick [euidl::button $parent.btn_pick \
        -command [namespace code pick_Node]   \
     	-imagefile $png_pick \
        -tooltip [msgcat::mc "Select Nodes "] ]

	set entry_pickinfo [entry $parent.entry_pickinfo \
		-textvariable [scope nodes] \
        -width 12\
        -font $font_en\
        -relief  ridge]

   	set btn_selall [euidl::button $parent.btn_selall \
        -command [code select_all]   \
     	-imagefile $png_check \
        -tooltip [msgcat::mc "Select Current "] ]        

    #input for height
	set label_x [euidl::label $parent.label_x \
			-height 2 \
			-font $font_cn \
			-text [msgcat::mc "X Force: "]]

    set entry_x [euidl::entry  $parent.entry_h -width 10  \
		-variable [scope fx] \
        -validate focusout\
        -width 15\
        -font $font_en\
        -validatedata ansreal]

    #input for pressure
    set label_y [euidl::label $parent.label_y \
		    -height 2 \
		    -font $font_cn \
		    -text [msgcat::mc "Y Force: "]]

    set entry_y [euidl::entry  $parent.entry_y \
            -width 10  \
    		-variable [scope fy] \
	        -validate focusout\
	        -width 15\
	        -font $font_en\
	        -validatedata ansreal]
    #input for pressure
    set label_z [euidl::label $parent.label_p \
	    -height 2 \
	    -font $font_cn \
	    -text [msgcat::mc "Z Force: "]]

    set entry_z [euidl::entry  $parent.entry_p -width 10  \
		-variable [scope fz] \
        -validate focusout\
        -width 15\
        -font $font_en\
        -validatedata ansreal]
     #input for factor

    set label_factor [euidl::label $parent.label_factor \
	    -height 2 \
	    -font $font_cn \
	    -text [msgcat::mc "Partial Factor: "]]

    set entry_factor [euidl::entry  $parent.entryfactor -width 10  \
		-variable [scope factor] \
        -validate focusout\
        -width 15\
        -font $font_en\
        -validatedata ansreal]

  	# set collapse [euidl::expandcollapse $parent.collapse \
   #      -label [::msgcat::mc "Contact Pairs"]]
	
    set frame_oncancel [frame $parent.frame_oncancel]
	set btn_ok [button $parent.btn_ok  \
		-text [msgcat::mc "Ok "] \
		-font $font_cn \
		-width 5\
		-command [namespace code apply_load] ]
 
	set btn_cancel [button $parent.btn_cancel  \
		-text [msgcat::mc "Cancel "] \
		-font $font_cn \
		-width 5\
		-command { destroy .concentratedforce}]



	grid $label_pick -row 0 -column 0  -sticky w -padx 2m
	grid $entry_pickinfo -row 0 -column 1 -sticky w 
	grid $btn_pick -row 0 -column 1 -sticky e 
	grid $btn_selall -row 0 -column 2 -sticky e

	grid $label_x -row 1 -column 0 -sticky w -padx 2m
	grid $entry_x -row 1 -column 1 -sticky w

	grid $label_y -row 2 -column 0 -sticky w -padx 2m
	grid $entry_y -row 2 -column 1 -sticky w

	grid $label_z -row 3 -column 0 -sticky w -padx 2m
	grid $entry_z -row 3 -column 1 -sticky w

	grid $label_factor -row 4 -column 0 -sticky w -padx 2m
	grid $entry_factor -row 4 -column 1 -sticky w

	pack  $btn_ok  $btn_cancel -in $frame_oncancel -side left -fill both -padx 12m -pady 5m
	grid $frame_oncancel  -row 5 -column 0   -columnspan 3   -sticky nsew
	#initialize the data
	::Customized::Util::ConcentratedForce::initialize
}

#initialize the window value
proc initialize {} \
{
	variable fx
	variable fy
	variable fz
	variable factor

	variable nodes
	set fx {}
	set fy {}
	set fz {}
	set factor 1.0
	set nodes {}

}
proc pick_Node {} \
{
	catch {ans_sendcommand )/NOPR} err
	#save the current node component to recover
	catch {ans_sendcommand )CM,_temp_node,NODE} err
	lower .concentratedforce
	::uidl::callFnc Fnc_NSEL_S [namespace code picknode_callback]
}

proc picknode_callback { uidlRtn } { 
   switch -- $uidlRtn {
     0 { # OK
     	#clear nodes list first
		set nodes {}
     		get_SelectedNodes
       }
     1 { # Cancel
       }
     2 { # Apply
     	  get_SelectedNodes
     	  #disposet the pick window
          catch {ans_sendcommand Fnc_RemoveDialogs} err
       }
   }
 	after 25 [raise .concentratedforce]

}
proc select_all {} \
{
	get_SelectedNodes
}
#get Selected nodes and append to label
proc get_SelectedNodes {} \
{
	variable nodes
	variable btn_pick
	global png_pick

	set nodes {}
	set node_nums [ans_getvalue "node,,count"]
	$btn_pick configure -relief sunken
	set num 0
	for {set i 0} {$i < $node_nums} {incr i} {
	set num [ans_evalexpr "ndnext($num)"]
		if {$num!=0} {
			lappend nodes $num
		}
	}

}



proc apply_load {} \
{
	variable fx
	variable fy
	variable fz
	variable nodes
	variable factor
	variable parent

	if {[llength $nodes]==0} {
		tk_messageBox  -parent $parent \
			-icon warning \
			-title [::msgcat::mc "Tips "] \
			-message  [::msgcat::mc "No Nodes Was Select! "]
		return
	}

	# 0 = Begin level,  17 = PREP7
	#21 = SOLUTION, 31 = POST1, 36 = POST26, 
	# set env [ans_getvalue "ACTIVE,0,ROUT"]
 #    if {$env!=21||$env!=17} {
 #      ans_sendcommand "/SOL"
 #    }
	#apply  load to nodes
	if {[string trim $fx]!=""} {
		ans_sendcommand "F,all,Fx,[expr $fx*$factor]"
	}

	if {[string trim $fy]!=""} {
		ans_sendcommand "F,all,Fy,[expr $fy*$factor]"
	}
	if {[string trim $fz]!=""} {
		ans_sendcommand "F,all,Fz,[expr $fz*$factor]"
	}

	#########startlogging############
	puts "#########Concentrated Force APDL Start ############"
	puts "NSEL,[join $nodes ,]"
	if {[string trim $fx]!=""} {puts "F,all,Fx,$fx*$factor"}
	if {[string trim $fy]!=""} {puts "F,all,Fy,$fy*$factor"}
	if {[string trim $fz]!=""} {puts "F,all,Fz,$fz*$factor"}
	puts "#########Concentrated Force APDL Finish ############"
	##########end logging############
	catch {ans_sendcommand )CMSEL,S,_temp_node} err     
    catch {ans_sendcommand )CMDEL,_temp_node} err          
    catch {ans_sendcommand )/GO} err
	#destroy window
	# after 500 {destroy .concentratedforce}
	destroy .concentratedforce
   
}

}

::Customized::Util::ConcentratedForce::main

