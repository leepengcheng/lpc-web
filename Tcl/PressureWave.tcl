package	 require msgcat
# encoding system euc-cn
namespace eval ::Customized::Util::WaveForce {

proc main {} {\

	variable parent
	variable loadpattern
	variable selobjs
	variable factor
	variable rw
	variable h1cent
	variable H
	variable Lm
	variable gravity
	variable picktype
	variable btn_pick
	############
	variable png
	variable font
	
	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	set png(pick) [file join $path images pick24x24.png]
	set png(update) [file join $path images update24x24.png]
	set png(check) [file join $path images check24x24.png]	
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(en)  {Consolas 12 normal}
	#destroy the window
	if [winfo exists .waveforce] {
	  wm deiconify .waveforce
	  raise .waveforce
	  return
	}
	set parent [toplevel .waveforce]

	wm title $parent [::msgcat::mc "Wave Force "]

	#adjust the location of locaLmat
	set x [expr {([winfo screenwidth  .]-400)/2}]
	set y [expr {([winfo screenheight .]-460)/2}]
	wm geometry .waveforce 400x460+$x+$y
	wm protocol $parent WM_DELETE_WINDOW {
            destroy .waveforce
      }


	##load pattern
	set label_loadtype  [label $parent.label_loadtype \
	-font $font(cn) \
	-text [::msgcat::mc "Load Pattern "] ]

	set combo_loadtype [euidl::combobox $parent.loadtype \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -variable [scope loadpattern] ]

	set combo_pick [euidl::combobox $parent.picktype \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 10\
         -font $font(cn) \
         -variable [scope picktype] \
		 -command [code changePickTypeEvent]]

	set btn_pick [euidl::button $parent.btn_pick \
        -command [namespace code pick_node_area]   \
     	-imagefile $png(pick) \
        -tooltip [msgcat::mc "Select Nodes "] ]

	set entry_pickinfo [entry $parent.entry_pickinfo \
		-textvariable [scope selobjs] \
        -width 15\
        -font $font(en)\
        -relief  ridge]

   	set btn_selcurrent [euidl::button $parent.btn_selcurrent \
        -command [namespace code selCurrentEvent]   \
     	-imagefile $png(check) \
        -tooltip [msgcat::mc "Select Current "] ]

    #input for water height
	set label_H [euidl::label $parent.label_H \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "Mean WaveHeight "]]

    set entry_H [entry  $parent.entry_H -width 10  \
		-textvariable [scope H] \
        -width 15\
        -font $font(en)]


	# set btn_H [euidl::button $parent.btn_H\
	# 	-command [code pick_node]   \
	# 	-imagefile $png(pick) \
	# 	-tooltip [msgcat::mc "Select Wavetop Node "] ]

	set label_Lm [euidl::label $parent.label_Lm \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "Mean WaveLength "]]

    set entry_Lm [entry  $parent.entry_Lm -width 10  \
		-textvariable [scope Lm] \
        -width 15\
        -font $font(en)]

	#input for h1%
    set label_h1cent [euidl::label $parent.label_h1cent  \
		    -height 2 \
		    -font $font(cn) \
		    -text [msgcat::mc "Height 1% "]]

    set entry_h1cent  [euidl::entry  $parent.entry_h1cent  -width 10  \
    		-variable [scope h1cent] \
	        -validate focusout\
	        -width 15\
	        -font $font(en)\
	        -validatedata ansreal]   	

    #Gravity direction
   	set label_gravity [euidl::label $parent.label_d \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "GravityDirection: "]]

	set combo_gravity [euidl::combobox $parent.gravity \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(en) \
         -variable [scope gravity] ]
	
    #input for water weight
    set label_rw [euidl::label $parent.label_rw \
		    -height 2 \
		    -font $font(cn) \
		    -text [msgcat::mc "Water Weight "]]

    set entry_rw [euidl::entry  $parent.entry_rw -width 10  \
    		-variable [scope rw] \
	        -validate focusout\
	        -width 15\
	        -font $font(en)\
	        -validatedata ansreal]   	
			      
    #input for factor
    set label_factor [euidl::label $parent.label_factor \
	    -height 2 \
	    -font $font(cn) \
	    -text [msgcat::mc "Partial Factor: "]]

    set entry_factor [euidl::entry  $parent.entryfactor -width 10  \
		-variable [scope factor] \
        -validate focusout\
        -width 15\
        -font $font(cn)\
        -validatedata ansreal]

     ##button ok and Cancel
    set frame_oncancel [frame $parent.frame_oncancel]
	set btn_next [button $parent.btn_next  \
		-text [msgcat::mc "Next "] \
		-font $font(cn) \
		-width 8\
		-command [namespace code buttonNextEvent] ]
 
	set btn_cancel [button $parent.btn_cancel  \
		-text [msgcat::mc "Cancel "] \
		-font $font(cn) \
		-width 8\
		-command { destroy .waveforce}]

	grid $label_loadtype  -row 0 -column 0 -sticky w -padx 2m -pady 2m
	grid $combo_loadtype  -row 0 -column 1 -sticky w -pady 2m


	grid $label_gravity -row 1 -column 0 -sticky w -padx 2m
	grid $combo_gravity -row 1 -column 1 -sticky w 

	grid $combo_pick -row 2 -column 0  -sticky w -padx 2m
	grid $entry_pickinfo -row 2 -column 1 -sticky w 
	grid $btn_pick -row 2 -column 2 
	grid $btn_selcurrent -row 2 -column 3  -sticky w 


	grid $label_H -row 4 -column 0 -sticky w -padx 2m
	grid $entry_H -row 4 -column 1 -sticky w  
	# grid $btn_H -row 4 -column 2 


	grid $label_Lm -row 5 -column 0 -sticky w -padx 2m
	grid $entry_Lm -row 5 -column 1 -sticky w	

	grid $label_h1cent -row 6 -column 0 -sticky w -padx 2m
	grid $entry_h1cent  -row 6 -column 1 -sticky w	

	grid $label_rw -row 7 -column 0 -sticky w -padx 2m
	grid $entry_rw -row 7 -column 1 -sticky w	 

	grid $label_factor -row 8 -column 0 -sticky w -padx 2m
	grid $entry_factor -row 8 -column 1 -sticky w

	# grid $label_step -row 9 -column 0 -sticky w -padx 2m
	# grid $entry_step -row 9 -column 1 -sticky w
	# grid $btn_update_ss -row 9 -column 2  -padx 2m


	pack  $btn_next  $btn_cancel -in $frame_oncancel -side left -fill both  -padx 12m -pady 2m
	grid  $frame_oncancel  -row 9 -column 0   -columnspan 3   -sticky nsew

	#default set X
	$combo_pick insert list end [list [msgcat::mc "Selected Areas: "] "AREAS" [msgcat::mc "Selected Nodes: "] "NODES"]
	$combo_gravity insert list end [list X X -X -X  Y  Y -Y -Y  Z  Z  -Z -Z]
	$combo_loadtype  insert list end [list [msgcat::mc "Replace "] "Replace" [msgcat::mc "Append " ] "Append"]
	Customized::Util::WaveForce::initialize
}

proc initialize {} \
{
	#main variable
	variable loadpattern
	variable picktype
	variable last_picktype
	variable H
	variable Lm 
	variable gravity
	variable selobjs
	variable factor
	variable rw
	#case variable
	variable slopei
	variable ki
	variable pwk
	variable Hp1f

	set loadpattern "Replace"
	set picktype    "NODES"
	set last_picktype  "NODES"
	set factor 1.2
	set rw 10
	set pwk {}
	set H  {}
	set Lm  {}
	set Hp1f 0
	set slopei 1.61
	set ki 1.61
	set selobjs {}

	#check the gravity of ansys db
	set acelX [ans_getvalue common,,acelcm,,real,22]
	set acelY [ans_getvalue common,,acelcm,,real,23]
	set acelZ [ans_getvalue common,,acelcm,,real,24]
	# default gravity is Z
	set  gravity "-Z"
	if {$acelX>0} {set gravity X }
	if {$acelX<0} {set gravity -X }
	if {$acelY>0} {set gravity Y }
	if {$acelY<0} {set gravity -Y }
	if {$acelZ>0} {set gravity Z }
	if {$acelZ<0} {set gravity -Z }


}

proc pick_node_area {} \
{	
	variable picktype
	catch {ans_sendcommand )/NOPR} err
	#save the current areas/nodes component to recover
	catch {ans_sendcommand )CM,_temp_area,AREA} err
	catch {ans_sendcommand )CM,_temp_node,NODE} err

	if {$picktype=="NODES"} {
		::uidl::callFnc Fnc_NSEL_S [namespace code picknode_callback]
	} else {
		::uidl::callFnc Fnc_ASEL_S [namespace code pickarea_callback]
	}
}


#callback function of pick_node()
proc picknode_callback {uidlRtn} \
{
	switch -- $uidlRtn {
	     0 { # OK
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
   after 25 [raise .waveforce]
}

proc get_SelectedNodes {} \
{
	variable selobjs

	set node_nums [ans_getvalue "node,,count"]
	set num 0
	set selobjs {}
	for {set i 0} {$i < $node_nums} {incr i} {
		set num [ans_evalexpr ndnext($num)]
		if {$num!=0} {
			lappend selobjs $num
		}
	}
}

proc pickarea_callback {uidlRtn} \
{
	switch -- $uidlRtn {
     0 { # OK
		  get_SelectedAreas
       }
     1 { # Cancel
       }
     2 { # Apply
     	  get_SelectedAreas
     	  #disposet the pick window
          catch {ans_sendcommand Fnc_RemoveDialogs} err
       }
   }
  after 25 [raise .waveforce]
}

proc get_SelectedAreas {} \
{
	variable selobjs

	set area_nums [ans_getvalue "area,,count"]
	set num 0
	set selobjs {}
	for {set i 0} {$i < $area_nums} {incr i} {
		set num [ans_evalexpr arnext($num)]
		if {$num!=0} {
			lappend selobjs $num
		}
	}
}

proc getNodesAttachToArea {}\
{
	variable selobjs
	#initialize the nodes
	set selnodes {}
	ans_sendcommand "ASEL,NONE"

	foreach area $selobjs {
		ans_sendcommand "ASEL,A,,,$area"
	}
	# #select all nodes attached the area
	catch {ans_sendcommand "NSLA,S,1"} err]

	#get the nodes num
	set node_nums [ans_getvalue "node,,count"]
	set num 0
	for {set i 0} {$i < $node_nums} {incr i} {
		set num [ans_evalexpr ndnext($num)]
		if {$num!=0} {
			lappend selnodes $num
		}
	}
	return $selnodes
}


proc buttonNextEvent {} {
	variable parent
	variable picktype
	variable selobjs
	variable loadpattern
	variable H
	variable Lm
	variable h1cent
	variable rw
	#output
	variable nodes
	variable hz
	variable Hcr
	variable caseindex
	variable pi

	set pi 3.141592653
	set err {}
	#####check input data####
	#if no areas was selected
	if {[llength $selobjs]==0} {
		set err "No Object Was Select! "
	} 
	
	if {$picktype=="AREAS"} {
		set nodes [getNodesAttachToArea]
	} else {
		set nodes $selobjs
	}
	#if no nodes was selected
	if {[llength $nodes]==0} {
		set err "No Nodes Was Select!"
	}
   
	#validate  Hcr temp value
	if {[catch {set t1 [expr ($Lm+2*$pi*$h1cent)/($Lm-2*$pi*$h1cent)]} error]} {
		set err "Hcr Can Not Be Calculated! "
	}  else {
		if {$t1<=0} {
			set err "Hcr Can Not Be Calculated! "
		}
	}

	#if empty input
	foreach var [list $H $Lm $h1cent $rw] {
			if {[string trim $var]==""} {
				set err "Empty Input Data!"
				break
		}
	}

	#show the err message
	if {![string match {} $err]} {
		tk_messageBox  -parent $parent \
			-icon warning \
			-title [::msgcat::mc "Tips "] \
			-message  [::msgcat::mc $err]
		return
	} 
	

	#calculate Hcr
	set Hcr   [expr 0.25*$Lm*log($t1)/$pi]
	#calculate hz
	set t2  [expr 2*$pi*$H/$Lm]
	#cthx=(e^x+e^-x)/(e^x-e^-x)
	set et2  [expr (exp($t2)+exp(-$t2))/(exp($t2)-exp(-$t2))]
	set hz   [expr $pi*$h1cent*$h1cent*$et2/$Lm]


	############switch case##################################
	#case 1:H>=Hcr && h>=Lm/2
	if {$H>=$Hcr && $H>=[expr 0.5*$Lm]} {
		set caseindex 1
		showSubCaseWindow
	#case 2:H>=Hcr && h<Lm/2 ->sechx=1/chx=2/[e^(x)+e^(-x)]
	} elseif {$H>=$Hcr && $H<[expr 0.5*$Lm]} {
		set caseindex 2
		showSubCaseWindow
	#case 3:H<Hcr 
	} else {
		set caseindex 3
		showSubCaseWindow
	}
	
}


proc apply_load {} \
{
	variable parent
	variable loadpattern
	variable gravity
	variable factor
	variable nodes
	variable pwk
	##for logging###
	variable Hcr
	variable hz



	set direction_flag [get_direction $gravity]
	set direction [lindex $direction_flag 0]
	set flag [lindex $direction_flag 1]
	set nodedata {}
	foreach node $nodes {
		lappend nodedata [ans_evalexpr [format "N%s(%s)" $direction $node]]
	}
	set nodedata [lsort -real $nodedata]
	set min_val [lindex $nodedata 0]
	set max_val [lindex $nodedata end]
	set height [expr $max_val-$min_val]
	
	####stepsize##################################
	# set stepsize [expr -$flag*$factor*$pwk/$height]
	set stepsize [expr -$flag*$factor*$pwk]

	if {$loadpattern=="Replace"} {
		set cmd_sfcum "SFCUM,PRES,REPL,1,1,"
	} else {

		set cmd_sfcum "SFCUM,PRES,ADD,1,1," 
	}
	##main apdl
	catch {ans_sendcommand $cmd_sfcum} err
	catch {ans_sendcommand "SFGRAD,PRES,0,$direction,$max_val,$stepsize"} err
	catch {ans_sendcommand "SF,ALL,PRES,0"} err
	catch {ans_sendcommand "/PSF,PRES,NORM,2,0,1"} err
	catch {ans_sendcommand "/REP,FAST"} err

	#logging
	puts "######## Wave Force APDL Start ############"
	puts ">>>Hcr:$Hcr   hz:$hz"
	puts ">>>> $cmd_sfcum"
	puts ">>>> SFGRAD,PRES,0,$direction,$max_val,$stepsize"
	puts ">>>> SF,ALL,PRES,0"
	puts "######## Wave Force APDL Finish ############"
	#reshow the temp component
	catch {ans_sendcommand )CMSEL,S,_temp_area} err     
    catch {ans_sendcommand )CMDEL,_temp_area} err
	catch {ans_sendcommand )CMSEL,S,_temp_element} err     
    catch {ans_sendcommand )CMDEL,_temp_element} err
    catch {ans_sendcommand )CMSEL,S,_temp_node} err     
    catch {ans_sendcommand )CMDEL,_temp_node} err          
    catch {ans_sendcommand )/GO} err
	after 500 {destroy .waveforce}
}


proc showSubCaseWindow {} {
	#input
	variable parent
	variable font
	variable png
	variable caseindex
	variable H
	variable Lm
	variable h1cent
	variable rw
	variable pi
	variable hz
	#output
	variable Hp1f
	variable slopei
	variable ki
	variable pwk

	#minimize parent window
	if [winfo exists $parent] {
		lower $parent
	}
	#destroy the case1 if exists
	if [winfo exists .waveforce.case] {
	  destroy .waveforce.case
	}
	set case [toplevel .waveforce.case]
	wm title $case [::msgcat::mc "Wave Force "]

	#adjust the location of locaLmat
	set x [expr {([winfo screenwidth  .]-380)/2}]
	set y [expr {([winfo screenheight .]-200)/2}]
	wm geometry .waveforce.case 380x200+$x+$y
	wm protocol .waveforce.case WM_DELETE_WINDOW {
            destroy .waveforce.case
    }
    #Hp1f
	set label_Hp1f [euidl::label $case.label_Hp1f  \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "Surplus WaveHeight "]]

    set entry_Hp1f [entry  $case.entry_Hp1f  -width 10  \
		-textvariable [scope Hp1f] \
        -width 15\
        -font $font(en)]

	#vari
    set label_slopei [euidl::label $case.label_slopei \
	    -height 2 \
	    -font $font(cn) \
	    -text [msgcat::mc "Slope "]]

	set combo_slopei [euidl::combobox $case.combo_slopei \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(en) \
		 -variable [scope slopei]\
		 -command [code changeSlopeEvent]]
	#ki
	set label_ki [euidl::label $case.label_ki  \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "Slope Coefficient "]]

    set entry_ki [entry  $case.entry_ki  \
		-width 10  \
		-textvariable [scope ki] \
        -width 15\
        -font $font(en)]

	set label_step [euidl::label $case.label_p \
	    -height 2 \
	    -font $font(cn) \
	    -text [msgcat::mc "WaveStepSize "]]

    set entry_step [euidl::entry  $case.entry_p -width 10  \
		-variable [scope pwk] \
        -validate focusout\
        -width 15\
        -font $font(en)\
        -validatedata ansreal]

	set btn_update_ss [euidl::button $case.btn_update_ss\
		-command [code calculatePwk]   \
		-imagefile $png(update) \
		-tooltip [msgcat::mc "Auto Calculate "]]

	##button ok and Cancel
    set frame_oncancel [frame $case.frame_oncancel]
	set btn_ok [button $case.btn_ok  \
		-text [msgcat::mc "Ok "] \
		-font $font(cn) \
		-width 5\
		-command [namespace code apply_load] ]
 
	set btn_cancel [button $case.btn_cancel  \
		-text [msgcat::mc "Cancel "] \
		-font $font(cn) \
		-width 5\
		-command {destroy .waveforce.case}]
	#########case logic################
	if {$caseindex==1} {
	   set pwk [expr 0.25*$rw*$Lm*($h1cent+$hz)]
	} elseif {$caseindex==2} {
		###controls arrange
		grid $label_Hp1f -row 0 -column 0 -sticky w -padx 2m
		grid $entry_Hp1f  -row 0 -column 1 -sticky w
	} else {
		grid $label_slopei -row 1 -column 0 -sticky w -padx 2m
		grid $combo_slopei  -row 1 -column 1 -sticky w

		grid $label_ki -row 2 -column 0 -sticky w -padx 2m
		grid $entry_ki  -row 2 -column 1 -sticky w

		#set initialize data
		$combo_slopei insert list end [list \
		"1/10" 1.89\
		"1/20" 1.61\
		"1/30"  1.48\
		"1/40" 1.41\
		"1/50"  1.36 \
		"1/60" 1.33\
		"1/80" 1.29\
		"<1/100" 1.25]
	}

	grid $label_step -row 3 -column 0 -sticky w -padx 2m
	grid $entry_step -row 3 -column 1 -sticky w
	grid $btn_update_ss -row 3 -column 2  -padx 2m

	pack  $btn_ok  $btn_cancel -in $frame_oncancel -side left -fill both  -padx 12m -pady 2m
	grid  $frame_oncancel  -row 4 -column 0   -columnspan 3   -sticky nsew
	#calculate Pwk once
	calculatePwk
}

#change slope 
proc changeSlopeEvent {} {
	variable slopei
	variable ki
	set ki $slopei
}
#change pick type 
proc changePickTypeEvent {} {
	variable picktype
	variable btn_pick
	variable last_picktype
	variable selobjs

	if {$picktype==$last_picktype} {
		return
	}
	set last_picktype $picktype
	set selobjs {}
	if {$picktype=="AREAS"} {
		$btn_pick configure -tooltip [msgcat::mc "Select Areas "]
	} else {
		$btn_pick configure -tooltip [msgcat::mc "Select Nodes "]
	}

}
proc selCurrentEvent {} {
	variable picktype

	if {$picktype=="AREAS"} {
		get_SelectedAreas
	} else {
		get_SelectedNodes
	}
}

#get the pwk value
proc calculatePwk {} {
	
	#input
	variable caseindex
	variable H
	variable Lm
	variable h1cent
	variable rw
	variable pi
	variable hz
	variable Hp1f
	variable ki
	#output
	variable pwk

	if {$caseindex==1} {
	    set pwk [expr 0.25*$rw*$Lm*($h1cent+$hz)]
	} elseif {$caseindex==2} {
		set index [expr 2*$pi*$H/$Lm]
		set p1f [expr 2*$rw*$h1cent/(exp($index)+exp(-$index))]
		set pwk [expr 0.5*( ($h1cent+$hz)*($rw*$H+$p1f)+$Hp1f ) ]
	} else {
		set lambda 0.6
		if {$H > [expr 1.7*$h1cent]} {
			set lambda 0.5
		}
		set p0 [expr $ki*$rw*$h1cent]
		set pwk [expr 0.5*$p0*((1.5-0.5*$lambda)*$h1cent+(0.7+$lambda)*$H)]
	}

}

#translate the direction
proc get_direction {thedirection} \
{
	if {[string length $thedirection]==1} {
		set direction $thedirection
		set  flag  -1.0
	} else {
		set direction [string index $thedirection 1]
		set flag 1.0
	}
	return [list $direction $flag]

}

}

::Customized::Util::WaveForce::main