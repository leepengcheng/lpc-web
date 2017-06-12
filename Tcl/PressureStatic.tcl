package	 require msgcat
# encoding system euc-cn
namespace eval ::Customized::Util::StaticPressure {

proc main {} {\
	
	variable parent
	variable loadpattern
	variable selobjs
	variable toploc
	variable picktype
	variable btn_pick
	variable gravity
	variable rw	
	variable factor
	variable stepsize
	variable font
	variable png


	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	set png(pick) [file join $path images pick24x24.png]
	set png(update) [file join $path images update24x24.png]
	set png(check) [file join $path images check24x24.png]
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(en)  {Consolas 12 normal}

	if [winfo exists .staticpressure] {
	  wm deiconify .staticpressure
	  raise .staticpressure
	  return
	}
	set parent [toplevel .staticpressure]
	wm title $parent [::msgcat::mc "Hydrostatic Pressure "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-360)/2}]
	set y [expr {([winfo screenheight .]-420)/2}]
	wm geometry .staticpressure 360x420+$x+$y
	wm protocol $parent WM_DELETE_WINDOW {
            destroy .staticpressure
      }

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

    #input for pressure
    set label_top [euidl::label $parent.label_top \
		    -height 2 \
		    -font $font(cn) \
		    -text [msgcat::mc "TopPosition: "]]

    set entry_top [euidl::entry  $parent.entry_top -width 10  \
    		-variable [scope toploc] \
	        -validate focusout\
	        -width 15\
	        -font $font(en)\
	        -validatedata ansreal]

	set btn_update_top [euidl::button $parent.btn_update_top\
		-command [list [code autoCalculate] "WaterTop"]    \
		-imagefile $png(update) \
		-tooltip [msgcat::mc "Auto Calculate "]]

	set btn_top [euidl::button $parent.btn_top \
        -command [code ::uidl::callFnc Fnc_NSEL_S [namespace code pick_top_node]]   \
        -imagefile $png(pick) \
        -tooltip [msgcat::mc "Select Top Node "] ]



       #gravity
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
     ##button ok and Cancel
    set frame_oncancel [frame $parent.frame_oncancel]
	set btn_ok [button $parent.btn_ok  \
		-text [msgcat::mc "Ok "] \
		-font $font(cn) \
		-width 5\
		-command [namespace code apply_load] ]
     #input for factor
    set label_factor [euidl::label $parent.label_p \
	    -height 2 \
	    -font $font(cn) \
	    -text [msgcat::mc "Partial Factor: "]]

    set entry_factor [euidl::entry  $parent.entryfactor -width 10  \
		-variable [scope factor] \
        -validate focusout\
        -width 15\
        -font $font(en)\
        -validatedata ansreal]

	set btn_cancel [button $parent.btn_cancel  \
		-text [msgcat::mc "Cancel "] \
		-font $font(cn) \
		-width 5\
		-command { destroy .staticpressure}]

    set label_stepsize [euidl::label $parent.label_stepsize \
		    -height 2 \
		    -font $font(cn) \
		    -text [msgcat::mc "Water Stepsize "]]

    set entry_stepsize [euidl::entry  $parent.entry_stepsize -width 10  \
    		-variable [scope stepsize] \
	        -validate focusout\
	        -width 15\
	        -font $font(en)\
	        -validatedata ansreal]

	set btn_update_stepsize [euidl::button $parent.btn_update_stepsize\
		-command [list [code autoCalculate] "Stepsize"]    \
		-imagefile $png(update) \
		-tooltip [msgcat::mc "Auto Calculate "]]

    #input for pressure
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

	grid $label_loadtype  -row 0 -column 0 -sticky w -padx 2m -pady 2m
	grid $combo_loadtype  -row 0 -column 1 -sticky w -pady 2m


	grid $label_gravity -row 1 -column 0 -sticky w -padx 2m
	grid $combo_gravity -row 1 -column 1 -sticky w 

	grid $combo_pick -row 2 -column 0  -sticky w -padx 2m
	grid $entry_pickinfo -row 2 -column 1 -sticky w 
	grid $btn_pick -row 2 -column 2 
	grid $btn_selcurrent -row 2 -column 3 -sticky w 
	



	grid $label_top -row 3 -column 0 -sticky w -padx 2m
	grid $entry_top -row 3 -column 1 -sticky w
	grid $btn_update_top -row 3 -column 2
	grid $btn_top -row 3 -column 3 -sticky w
	
	grid $label_rw -row 5 -column 0 -sticky w -padx 2m
	grid $entry_rw -row 5 -column 1 -sticky w

	grid $label_factor -row 6 -column 0 -sticky w -padx 2m
	grid $entry_factor -row 6 -column 1 -sticky w

	grid $label_stepsize -row 7 -column 0 -sticky w -padx 2m
	grid $entry_stepsize -row 7 -column 1 -sticky w
	grid $btn_update_stepsize -row 7 -column 2 -padx 2m

	pack  $btn_ok  $btn_cancel -in $frame_oncancel -side left -fill both -padx 12m -pady 2m
	grid $frame_oncancel  -row 8 -column 0   -columnspan 3   -sticky nsew

	#default set X
	$combo_pick insert list end [list [msgcat::mc "Selected Areas: "] "AREAS" [msgcat::mc "Selected Nodes: "] "NODES"]
	$combo_gravity insert list end [list X X -X -X  Y  Y -Y -Y  Z  Z  -Z -Z]
	$combo_loadtype  insert list end [list [msgcat::mc "Replace "] "Replace" [msgcat::mc "Append " ] "Append"]
	#initialize the window
	::Customized::Util::StaticPressure::initialize
}
proc initialize {} \
{

	variable loadpattern
	variable factor
	variable rw
	variable toploc
	variable gravity
	variable selobjs
	variable picktype
	variable last_picktype
	variable stepsize


	set loadpattern "Replace"
	set picktype    "NODES"
	set last_picktype  "NODES"
	set toploc {}
	set selobjs {}
	set stepsize {}
	set factor 1.0
	set rw 10.0

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
   after 25 [raise .staticpressure]
}


proc pick_top_node {uidlRtn} {
	
	switch -- $uidlRtn {
	     0 { # OK
 			 get_TopLocation
	       }
	     1 { # Cancel
	       }
	     2 { # Apply
	     	 get_TopLocation
	     	 #disposet the pick window
          	 catch {ans_sendcommand Fnc_RemoveDialogs} err
	       }
   }
	after 25 [raise .staticpressure]
}

proc get_TopLocation {} {
	variable parent
	variable toploc
	variable gravity

	set node_nums [ans_getvalue "node,,count"]
	if {$node_nums>1} {
		tk_messageBox  -parent $parent \
		-icon warning \
		-title [::msgcat::mc "Tips "] \
		-message  [::msgcat::mc "Only 1 node can be select!"]
		return
	}  
	set direction_flag [get_direction $gravity]
	set direction [lindex $direction_flag 0]
	set num [ans_evalexpr "ndnext(0)"]
	set toploc [ans_evalexpr [format "N%s(%s)" $direction $num]]
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
  raise .staticpressure
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

#calculate toploc and pressure in smart pattern
proc autoCalculate {args} \
{

	variable parent
	variable toploc
	variable picktype
	variable stepsize
	variable gravity
	variable selobjs
	variable rw


	set direction_flag [get_direction $gravity]
	set direction [lindex $direction_flag 0]
	set flag [lindex $direction_flag 1]

	if {$picktype=="AREAS"} {
		set nodes [getNodesAttachToArea]
	} else {
		set nodes $selobjs
	}
	#if no nodes was selected then return
	if {[llength $nodes]==0} {
		return	
	}
	set nodedata {}
	foreach node $nodes {
		lappend nodedata [ans_evalexpr [format "N%s(%s)" $direction $node]]
	}
	set nodedata [lsort -real $nodedata]
	set min_val [lindex $nodedata 0]
	set max_val [lindex $nodedata end]
	set height [expr $max_val-$min_val]


	if {$args=="WaterTop"} {
		set toploc [expr $flag==-1.0?$min_val:$max_val]
	}
	####################<<step size>>###################################
	if {$args=="Stepsize"} {
		set stepsize [expr  -$flag*$rw]
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


proc apply_load {} \
{
	variable parent
	variable factor
	variable stepsize
	variable gravity
	variable toploc
	variable selobjs
	variable nodes
	variable loadpattern
	variable picktype



	set  err  {}
	#if no selobjs was selected
	if {[llength $selobjs]==0} {
		set err "No Object Was Select! "
	}

	if {$picktype=="AREAS"} {
		set nodes [getNodesAttachToArea]
	} else {
		set nodes $selobjs
	}

	if {[llength $nodes]==0} {
		set err "No Nodes Was Select!!"
	}

	foreach var [list $toploc $stepsize] {
			if {[string trim $var]==""} {
				set err "Empty Input Data!"
				break
		}
	}
	
	if {![string match {} $err]} {
		tk_messageBox  -parent $parent \
			-icon warning \
			-title [::msgcat::mc "Tips "] \
			-message  [::msgcat::mc $err]
		return
	}

	#get the direction and flag
	set direction_flag [get_direction $gravity]
	set direction [lindex $direction_flag 0]

	if {$loadpattern=="Replace"} {
		set cmd_sfcum "SFCUM,PRES,REPL,1,1,"
	} else {
		set cmd_sfcum "SFCUM,PRES,ADD,1,1," 
	}

	#main apdl
	catch {ans_sendcommand $cmd_sfcum} err
	catch {ans_sendcommand "SFGRAD,PRES,0,$direction,$toploc,[expr $factor*$stepsize]"} err
	catch {ans_sendcommand "SF,ALL,PRES,0"} err
	catch {ans_sendcommand "/PSF,PRES,NORM,2,0,1"} err
	catch {ans_sendcommand "/REP,FAST"} err
	#logging
	puts "######## Static Pressure APDL Start ############"
	puts "$cmd_sfcum"
	puts "SFGRAD,PRES,0,$direction,$toploc,$stepsize*$factor"
	puts "SF,ALL,PRES,0"
	puts "######## Static Pressure APDL Finish ############"
	#reshow the temp component
	catch {ans_sendcommand )CMSEL,S,_temp_area} err     
    catch {ans_sendcommand )CMDEL,_temp_area} err
	catch {ans_sendcommand )CMSEL,S,_temp_element} err     
    catch {ans_sendcommand )CMDEL,_temp_element} err
    catch {ans_sendcommand )CMSEL,S,_temp_node} err     
    catch {ans_sendcommand )CMDEL,_temp_node} err          
    catch {ans_sendcommand )/GO} err
	after 500 {destroy .staticpressure}
}
#translate the gravity
proc get_direction {thedirection} \
{
	if {[string length $thedirection]==1} {
		set gravity $thedirection
		set  flag  -1.0
	} else {
		set gravity [string index $thedirection 1]
		set flag 1.0
	}
	return [list $gravity $flag]

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
}
::Customized::Util::StaticPressure::main