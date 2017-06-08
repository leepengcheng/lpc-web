package	 require msgcat
# encoding system euc-cn
namespace eval ::Customized::Util::WaveForce {

proc main {} {\

	variable parent
	variable combo_gravity
	variable btn_wavetop
	variable btn_pick
	#####################
	variable areas
	variable factor
	variable dens
	variable wavetop
	variable stepsize
	variable gravity
	variable loadpattern
	variable btn_check
	variable png
	variable font
	
	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	set png(pick) [file join $path images pick24x24.png]
	set png(update) [file join $path images update24x24.png]
	set png(check) [file join $path images check24x24.png]	
	#default error message is null
	# set err {}
	#destroy the window
	if [winfo exists .waveforce] {
	  wm deiconify .waveforce
	  raise .waveforce
	  return
	}
	set parent [toplevel .waveforce]
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(en)  {Consolas 12 normal}
	wm title $parent [::msgcat::mc "Wave Force "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-350)/2}]
	set y [expr {([winfo screenheight .]-420)/2}]
	wm geometry .waveforce 350x420+$x+$y
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

    set label_pick [euidl::label $parent.label_pick \
	    -height 2 \
	    -font $font(cn) \
	    -text [msgcat::mc "Selected Areas: "]]

	set btn_pick [euidl::button $parent.btn_pick \
        -command [namespace code pick_area]   \
     	-imagefile $png(pick) \
        -tooltip [msgcat::mc "Select Areas "] ]

	set entry_pickinfo [entry $parent.entry_pickinfo \
		-textvariable [scope areas] \
        -width 15\
        -font $font(en)\
        -relief  ridge]

   	set btn_check [euidl::button $parent.btn_check \
        -command [namespace code select_current]   \
     	-imagefile $png(check) \
        -tooltip [msgcat::mc "Select Current "] ]

    #input for wavetop
	set label_wavetop [euidl::label $parent.label_top \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "WaveTop "]]

    set entry_wavetop [entry  $parent.entry_top -width 10  \
		-textvariable [scope wavetop] \
        -width 15\
        -font $font(en)]


	set btn_wavetop [euidl::button $parent.btn_wavetop\
		-command [code pick_node]   \
		-imagefile $png(pick) \
		-tooltip [msgcat::mc "Select Wavetop Node "] ]

	set btn_update_wp [euidl::button $parent.btn_update_wp\
		-command [list [code autoCalculate] "Wavetop"]    \
		-imagefile $png(update) \
		-tooltip [msgcat::mc "Auto Calculate "]]

    #input for stepsize
    set label_step [euidl::label $parent.label_p \
	    -height 2 \
	    -font $font(cn) \
	    -text [msgcat::mc "WaveStepSize: "]]

    set entry_step [euidl::entry  $parent.entry_p -width 10  \
		-variable [scope stepsize] \
        -validate focusout\
        -width 15\
        -font $font(en)\
        -validatedata ansreal]

	set btn_update_ss [euidl::button $parent.btn_update_ss\
		-command [list [code autoCalculate] "Stepsize"]   \
		-imagefile $png(update) \
		-tooltip [msgcat::mc "Auto Calculate "]]

    #direction
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

    #input for pressure
    set label_dens [euidl::label $parent.label_dens \
		    -height 2 \
		    -font $font(cn) \
		    -text [msgcat::mc "Water Density "]]

    set entry_dens [euidl::entry  $parent.entry_dens -width 10  \
    		-variable [scope dens] \
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
	set btn_ok [button $parent.btn_ok  \
		-text [msgcat::mc "Ok "] \
		-font $font(cn) \
		-width 5\
		-command [namespace code apply_load] ]
 
	set btn_cancel [button $parent.btn_cancel  \
		-text [msgcat::mc "Cancel "] \
		-font $font(cn) \
		-width 5\
		-command { destroy .waveforce}]

	grid $label_loadtype  -row 0 -column 0 -sticky w -padx 2m -pady 2m
	grid $combo_loadtype  -row 0 -column 1 -sticky w -pady 2m


	grid $label_gravity -row 1 -column 0 -sticky w -padx 2m
	grid $combo_gravity -row 1 -column 1 -sticky w 

	grid $label_pick -row 2 -column 0  -sticky w -padx 2m
	grid $entry_pickinfo -row 2 -column 1 -sticky w 
	grid $btn_pick -row 2 -column 2 
	grid $btn_check -row 2 -column 3  -sticky w 


	grid $label_wavetop -row 4 -column 0 -sticky w -padx 2m
	grid $entry_wavetop -row 4 -column 1 -sticky w  
	grid $btn_update_wp -row 4 -column 2 
	grid $btn_wavetop -row 4 -column 3 -sticky w

	grid $label_dens -row 5 -column 0 -sticky w -padx 2m
	grid $entry_dens -row 5 -column 1 -sticky w	 

	grid $label_factor -row 6 -column 0 -sticky w -padx 2m
	grid $entry_factor -row 6 -column 1 -sticky w

	grid $label_step -row 7 -column 0 -sticky w -padx 2m
	grid $entry_step -row 7 -column 1 -sticky w
	grid $btn_update_ss -row 7 -column 2  -padx 2m


	pack  $btn_ok  $btn_cancel -in $frame_oncancel -side left -fill both  -padx 12m -pady 2m
	grid $frame_oncancel  -row 8 -column 0   -columnspan 3   -sticky nsew

	#default set X
	$combo_gravity insert list end [list X X -X -X  Y  Y -Y -Y  Z  Z  -Z -Z]
	$combo_loadtype  insert list end [list [msgcat::mc "Replace "] "Replace" [msgcat::mc "Append " ] "Append"]
	Customized::Util::WaveForce::initialize
}

proc select_current {} \
{
	variable btn_check
	get_SelectedAreas
	$btn_check configure -relief sunken
}
proc initialize {} \
{
	variable loadpattern
	variable wavetop
	variable stepsize
	variable gravity
	variable areas
	variable factor
	variable dens


	set loadpattern "Replace"
	set factor 1.0
	set dens 9.81
	set stepsize {}
	set wavetop  {}
	set areas {}

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

proc pick_area {} \
{	

	catch {ans_sendcommand )/NOPR} err
	#save the current areas component to recover
	catch {ans_sendcommand )CM,_temp_area,AREA} err
	#start the area pick window
	::uidl::callFnc Fnc_ASEL_S [namespace code pickarea_callback]
}

proc pick_node {} \
{

	catch {ans_sendcommand )/NOPR} err
	#save the current node component to recover
	catch {ans_sendcommand )CM,_temp_node,NODE} err
	#start the area pick window
	::uidl::callFnc Fnc_NSEL_S [namespace code picknode_callback]
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
   after 25 [raise .waveforce]
}

proc get_SelectedNodes {} \
{
	variable parent
	variable wavetop
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
	set wavetop [ans_evalexpr [format "N%s(%s)" $direction $num]]
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
	variable areas

	set area_nums [ans_getvalue "area,,count"]
	set num 0
	set areas {}
	for {set i 0} {$i < $area_nums} {incr i} {
		set num [ans_evalexpr arnext($num)]
		if {$num!=0} {
			lappend areas $num
		}
	}
}

#calculate wavevalley and stepsize in smart pattern
proc autoCalculate {args} \
{
	variable parent	
	variable wavetop
	variable stepsize
	variable gravity
	variable areas
	variable nodes
	variable factor
	variable dens

	set direction_flag [get_direction $gravity]
	set direction [lindex $direction_flag 0]
	set flag [lindex $direction_flag 1]
	#select all nodes attached the area
	selectNodesByArea
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
	#test data
	if {$args=="Wavetop"} {
		set wavetop [expr $flag==-1.0?$min_val:$max_val]
	}
	#wave force ？
	if {$args=="Stepsize"} {
		set stepsize [expr  -0.5*$max_val*$max_val*$flag*$dens*$factor/$height]
	}
	
}

proc selectNodesByArea {}\
{
	variable nodes
	variable areas
	#initialize the nodes
	set nodes {}

	ans_sendcommand "ASEL,NONE"

	foreach area $areas {

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
			lappend nodes $num
		}
	}
}

proc apply_load {} \
{
	variable parent
	variable wavetop
	variable gravity
	variable stepsize
	variable areas
	variable nodes
	variable loadpattern

	set  err  {}
	#if no areas was selected
	if {[llength $areas]==0} {
		set err "No Object Was Select! "
	}

	selectNodesByArea

	if {[llength $nodes]==0} {
		set err "No Nodes Was Select!"
	}

	foreach var [list $wavetop $stepsize] {
			if {[string trim $var]==""} {
				set err "Empty Input Data!"
				break
		}
	}
	
	#check the err
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


	# Current Environment: 
	# 0 = Begin level,  17 = PREP7, 21 = SOLUTION, 31 = POST1, 36 = POST26, 
	set env [ans_getvalue "ACTIVE,0,ROUT"]
	if {$env!=21} {
		ans_sendcommand "/SOLU"
	}

	if {$loadpattern=="Replace"} {
		set cmd_sfcum "SFCUM,PRES,REPL,1,1,"
	} else {
		set cmd_sfcum "SFCUM,PRES,ADD,1,1," 
	}
	##main apdl
	ans_sendcommand $cmd_sfcum
	ans_sendcommand "SFGRAD,PRES,0,$direction,$wavetop,$stepsize"
	ans_sendcommand "SF,ALL,PRES,0"
	ans_sendcommand "/PSF,PRES,NORM,2,0,1"
	ans_sendcommand "/REP,FAST"
	#logging
	puts "######## Wave Force APDL Start ############"
	puts ">>>> $cmd_sfcum"
	puts ">>>> SFGRAD,PRES,0,$direction,$wavetop,$stepsize"
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
