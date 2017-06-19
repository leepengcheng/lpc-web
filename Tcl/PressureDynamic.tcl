package	 require msgcat
# encoding system euc-cn
namespace eval ::Customized::Util::DynamicPressure {

proc main {} {\

	variable parent
	variable png
	variable font
	variable plotshow
	#####################
	variable underface
	variable sideface
	variable ori_gravity
	variable ori_water
	variable loadpattern
	variable filename
	variable factor
	variable rw
	variable btn_under_check
	variable btn_side_check

	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	set png_unpick [file join $path images unpick24x24.png]
	set png(pick) [file join $path images pick24x24.png]
	set png(hide) [file join $path images hide24x24.png]
	set png(config) [file join $path images config24x24.png]
	set png(import) [file join $path images import24x24.png]
	set png(chart) [file join $path images chart24x24.png]
	set png(check) [file join $path images check24x24.png]
	set plotshow    [file join $path "AnsysPlugin.exe"]
	#default error message is null
	# set err {}
	#destroy the window
	if [winfo exists .dynamicpressure] {
		wm deiconify .dynamicpressure
	  	raise .dynamicpressure
	  	return
	}
	set parent [toplevel .dynamicpressure]
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(en)  {Consolas 12 normal}
	wm title $parent [::msgcat::mc "Dynamic Pressure "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-380)/2}]
	set y [expr {([winfo screenheight .]-500)/2}]
	wm geometry .dynamicpressure 380x500+$x+$y
	wm protocol $parent WM_DELETE_WINDOW {
            if {[winfo exists .torlence]} {
             destroy .torlence  
        }
            destroy .dynamicpressure
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

	#input data file
	set labeltips [label $parent.labeltips \
		-text [::msgcat::mc "Please Select Data File: "] \
		-width 20 -font $font(cn)]
	set frame_file [frame $parent.frame_file]
	set textfile [entry $parent.textfile\
		-textvariable [scope filename] \
		-width 25\
		-font $font(cn)]

	set btn_import [euidl::button $parent.importbtn \
		-tooltip [::msgcat::mc "Import File "] \
		-imagefile $png(import) \
		-command [code selectFile_Click]]

	set btn_chart [euidl::button $parent.chartbtn \
		-tooltip [::msgcat::mc "Show Data "] \
		-imagefile $png(chart) \
		-command [code btn_chart_click]]

	set label_side [euidl::label $parent.label_side \
		    -height 2 \
		    -font $font(cn) \
		    -text [msgcat::mc "Select Sideface: "]]

    set entry_side [entry  $parent.entry_side  \
    		-textvariable [scope sideface] \
	        -width 15\
	        -relief ridge\
	        -font $font(en)]

	set btn_side [euidl::button $parent.btn_side \
        -command [list [namespace code pick_area] "side"]   \
        -imagefile $png(pick) \
        -tooltip [msgcat::mc "Select Sideface: "] ]

   	set btn_side_check [euidl::button $parent.btn_side_check \
        -command [list [code select_current] side]   \
     	-imagefile $png(check) \
        -tooltip [msgcat::mc "Select Current "] ]

	set label_under [euidl::label $parent.label_under \
		    -height 2 \
		    -font $font(cn) \
		    -text [msgcat::mc "Select Underface: "]]

    set entry_under [entry  $parent.entry_under  \
    		-textvariable [scope underface] \
	        -width 15\
	        -relief ridge\
	        -font $font(en)]

	set btn_under [euidl::button $parent.btn_under \
        -command [list [namespace code pick_area] "under"]   \
        -imagefile $png(pick) \
        -tooltip [msgcat::mc "Select Underface: "] ]

   	set btn_under_check [euidl::button $parent.btn_under_check \
        -command [list [code select_current] under]   \
     	-imagefile $png(check) \
        -tooltip [msgcat::mc "Select Current "] ]

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

    
       # gravity direction
   	set label_direction_G [euidl::label $parent.label_g_d \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "GravityDirection: "]]
  
	set combo_direction_G [euidl::combobox $parent.ori_gravity \
         -exportselection 1 \
         -listheight 100 \
         -width 13\
         -style dropdown \
         -font $font(en) \
         -tooltip [msgcat::mc "GravityDirection: "] \
         -variable [scope ori_gravity] ]
	# water flow direction
    set label_direction_W [euidl::label $parent.label_w_d \
			-height 2 \
			-font $font(cn) \
			-text [msgcat::mc "WaterFlowDirection: "]]
  
	set combo_direction_W [euidl::combobox $parent.ori_water \
         -exportselection 1 \
          -style dropdown \
         -listheight 100 \
         -width 13\
         -tooltip [msgcat::mc "WaterFlowDirection: "] \
         -font $font(en) \
         -variable [scope ori_water] ]

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
		-width 8\
		-command [namespace code applyLoad] ]
 
	set btn_cancel [button $parent.btn_cancel  \
		-text [msgcat::mc "Cancel "] \
		-font $font(cn) \
		-width 8\
		-command { destroy .dynamicpressure}]


	grid $labeltips -row 0 -column 0 -columnspan 2 -sticky w  -padx 2m  -pady 2m
	grid $btn_import -row 0 -column 2 -sticky w 
	grid $btn_chart  -row 0 -column 3 -sticky e -padx 2m

	grid $textfile -row 1 -column 0 -columnspan 3 -sticky ew -padx 3m -pady 2m

	grid $label_loadtype  -row 2 -column 0 -sticky w -padx 2m -pady 2m
	grid $combo_loadtype  -row 2 -column 1 -sticky w -padx 2m  -pady 2m

	grid $label_direction_G -row 3 -column 0 -sticky w  -padx 2m -pady 2m
	grid $combo_direction_G -row 3 -column 1 -sticky w  -padx 2m -pady 2m

	grid $label_under -row 4 -column 0 -sticky w  -padx 2m 
	grid $entry_under -row 4 -column 1 -sticky w  -padx 2m 
	grid $btn_under -row 4 -column 2 -sticky w   
	grid $btn_under_check -row 4 -column 3 -sticky e   -padx 2m


	grid $label_direction_W -row 5 -column 0 -sticky w  -padx 2m 
	grid $combo_direction_W -row 5 -column 1 -sticky w   -padx 2m 

	grid $label_side -row 6 -column 0 -sticky w  -padx 2m 
	grid $entry_side -row 6 -column 1 -sticky w  -padx 2m 
	grid $btn_side -row 6 -column 2 -sticky w 
	grid $btn_side_check -row 6 -column 3 -sticky e  -padx 2m

	grid $label_rw -row 7 -column 0 -sticky w -padx 2m 
	grid $entry_rw -row 7 -column 1 -sticky w -padx 2m 

	grid $label_factor -row 8 -column 0 -sticky w -padx 2m 
	grid $entry_factor -row 8 -column 1 -sticky w -padx 2m

	pack  $btn_ok  $btn_cancel -in $frame_oncancel -side left -fill both -padx 12m -pady 4m
	grid $frame_oncancel  -row 9 -column 0   -columnspan 4   -sticky nsew

	
	#default set X
	$combo_direction_G insert list end [list X X -X -X  Y  Y -Y -Y  Z  Z  -Z -Z]
	$combo_direction_W insert list end [list X X -X -X  Y  Y -Y -Y  Z  Z  -Z -Z]
	$combo_loadtype  insert list end [list [msgcat::mc "Replace "] "Replace" [msgcat::mc "Append " ] "Append"]
	#initialize the param	
	::Customized::Util::DynamicPressure::initialize
}

proc select_current {arg} \
{
	variable btn_under_check
	variable btn_side_check
	variable activate_combo

	set activate_combo $arg
	get_SelectedAreas
	if {$arg=="side"} {
		$btn_side_check configure -relief sunken
	} else {
		$btn_under_check configure -relief sunken
	}
	
}
proc initialize {} \
{
	variable loadpattern
	variable underface
	variable sideface
	variable ori_gravity
	variable ori_water
	variable filename
	variable factor
	variable rw

	# set filename {}
	set loadpattern "Replace"
	set underface {}
	set sideface {}
	set factor 1.0
	set rw 10000

	#check the gravity of ansys db
	set acelX [ans_getvalue common,,acelcm,,real,22]
	set acelY [ans_getvalue common,,acelcm,,real,23]
	set acelZ [ans_getvalue common,,acelcm,,real,24]
	set  ori_gravity {}
	set  ori_water {}
	if {$acelX>0} {set ori_gravity X }
	if {$acelX<0} {set ori_gravity -X }
	if {$acelY>0} {set ori_gravity Y }
	if {$acelY<0} {set ori_gravity -Y }
	if {$acelZ>0} {set ori_gravity Z }
	if {$acelZ<0} {set ori_gravity -Z }

	

}
#Select area
proc pick_area {combo_name} \
{
	variable ori_gravity
	variable ori_water
	variable parent
	#output
	variable activate_combo

	set err {}
	if {$combo_name=="under" && $ori_gravity==""} {
		set err "No Underface Direction!"
	} 
	if {$combo_name=="side" && $ori_water==""} {
		set err "No Sideface Direction!"
	} 

	if {$err!=""} {
		tk_messageBox  -parent $parent \
			-icon warning \
			-title [::msgcat::mc "Tips: "] \
			-message  [::msgcat::mc $err]
		return
	}

	set activate_combo $combo_name

	catch {ans_sendcommand )/NOPR} err
	#save the current areas component to recover
	catch {ans_sendcommand )CM,_temp_area,AREA} err
	#start the area pick window
	::uidl::callFnc Fnc_ASEL_S [namespace code pickarea_callback]
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
  after 25 [raise .dynamicpressure]
}

proc get_SelectedAreas {} \
{
	#input
	variable sideface
	variable underface
	variable activate_combo

	set area_nums [ans_getvalue "area,,count"]
	set num 0
	set areas {}
	for {set i 0} {$i < $area_nums} {incr i} {
		set num [ans_evalexpr arnext($num)]
		if {$num!=0} {
			lappend areas $num
		}
	}
	if {$activate_combo=="side"} {
		set sideface $areas
	} else {
		set underface $areas
	}
}

#Select the node
proc pick_node {loc_name} \
{
	variable parent
	variable ori_water
	variable activate_entry
   

	if {$ori_water==""} {
		tk_messageBox  -parent $parent \
		-icon warning \
		-title [::msgcat::mc "Tips: "] \
		-message  [::msgcat::mc "You Must Select the Water Direction First! "]
		return
	}
	set activate_entry $loc_name

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
 	after 25 [raise .dynamicpressure]
}

proc get_SelectedNodes {} \
{
	variable parent
	variable ori_water
	variable activate_entry

	set node_nums [ans_getvalue "node,,count"]
	#Only 1 node can be select
	if {$node_nums>1} {
		tk_messageBox  -parent $parent \
		-icon warning \
		-title [::msgcat::mc "Tips: "] \
		-message  [::msgcat::mc "Only 1 node can be select!"]
		return
	}  
	set direction_flag [get_direction $ori_water]
	set direction [lindex $direction_flag 0]
	set num [ans_evalexpr "ndnext(0)"]
	set loc [ans_evalexpr [format "N%s(%s)" $direction $num]]
	if {$activate_entry=="start"} {
		set startloc $loc
	} else {
		set endloc  $loc
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
#button Selected click event
proc selectFile_Click {} \
{
	variable filename
	variable parent
	set tempfile [tk_getOpenFile -initialfile *.txt -filetypes {{Txt .txt}}]
	if {[file exists $tempfile]} {
		set filename $tempfile
		loadDataFromTxt
	}
}
#load the data 
proc loadDataFromTxt {} \
{
	#input
    variable filename
    #output
    variable inputdata

    #erase the previous data
    if {[info exists inputdata]} {
		unset inputdata
	}
	#open input data file
    set fl [open $filename r]
    set data [read $fl]
    #close data file
	close $fl
	array set inputdata $data
}

#get the Element-Node dictionary
proc getElementNodeMap {arealist} \
{
	#input
	variable parent
	variable ori_water
	#output
	variable elements

	#select None area
	catch {ans_sendcommand "ASEL,NONE"} err 
	foreach area $arealist {
		catch {ans_sendcommand "ASEL,A,,,$area"} err 
	}

	# #select all nodes attached the area
	if {[catch {ans_sendcommand "NSLA,S,1"} err]} {
		tk_messageBox  -parent $parent \
			-icon warning \
			-title [::msgcat::mc "Tips: "] \
			-message  [::msgcat::mc $err]
		return
    }


	#erase the elements array
	if {[info exists elements]} {
		unset elements
	}
	set count [ans_getvalue "node,,count"]
	set ndnum  0
	for {set i 0} {$i < $count} {incr i} {
		set  ndnum [ans_evalexpr "NDNEXT($ndnum)"]
		#adjent elements num 
		for {set j 1} {$j < 20} {incr j} {
			set enum  [ans_evalexpr "ENEXTN($ndnum,$j)"]
			if {$enum!=0} {
				lappend elements($enum) $ndnum 
			} else {
				break
			}
		}
	}

}

#Interpolation calculate
proc getNodeValue {loc} \
{
	#input 
	variable inputdata

	set keys [array names inputdata]
	if {[lsearch $keys $loc]>=0} {
		return $inputdata($loc)
	} else {
		#interpolation calculate
		lappend keys $loc
		set keys [lsort -real $keys]
		set index [lsearch $keys $loc]
		set len   [expr [llength $keys]-1]
		#if the node value small than First X label
		if {$index==0} {
			set x1 [lindex $keys 1]
			# return $inputdata($x1)
			return 0
		#if the node value grater than Last X label
		} elseif {$index==$len} {
			set x1 [lindex $keys [expr $len-1]]
			# return $inputdata($x1)
			return 0
		} else {
			set x1 [lindex $keys [expr $index-1]]
			set x2 [lindex $keys [expr $index+1]]
			set y1  $inputdata($x1)
			set y2  $inputdata($x2)
			return [expr $y1+($loc-$x1)*1.0*($y2-$y1)/($x2-$x1)]
		}

	}
	
}
proc btn_chart_click {} \
{
	variable filename
	variable inputdata
	variable plotshow
	variable parent


	if {![file exists $filename]} {return}
	set xdata [array names inputdata]
	if {[llength $xdata]==0} {return}
	set ydata {}
	foreach x $xdata {
		lappend ydata $inputdata($x)
	}
  set title  [::msgcat::mc "Dynamic Pressure "]
  set xlabel [::msgcat::mc "Distance (m) "]
  set ylabel [::msgcat::mc "Water Height(m) "]
  set geometry    [wm geometry $parent]


  set data  "\"<root  title='$title' geometry='$geometry' xlabel='$xlabel' ylabel='$ylabel'>
            <graphs>
            <graph id='1'>
            <xlist>$xdata</xlist>
            <ylist>$ydata</ylist>
            <style legend='$title' line='black blue 0.6' point='black white 8' />
            </graph>
            </graphs>
            </root>\""

  exec $plotshow $data
  raise $parent
}

proc applyLoad {} \
{
	variable parent
	variable loadpattern
	###################
	variable ori_gravity
	variable ori_water
	variable underface
	variable sideface
	variable filename

	#file is not exists
    if {![file exists $filename]} {
		tk_messageBox  -parent $parent -icon warning \
		-title [::msgcat::mc "Failed import Data "] \
		-message  [::msgcat::mc "File not exists,Please check the path! "]
		return
	}

	loadDataFromTxt

	#save the current node component to recover
	catch {ans_sendcommand )CM,_temp_node,NODE} err
	catch {ans_sendcommand )CM,_temp_element,ELEMENT} err
	catch {ans_sendcommand )CM,_temp_area,AREA} err	


	
	
	#load pattern Replace/add
	if {$loadpattern=="Replace"} {
		catch {ans_sendcommand "SFCUM,PRES,REPL,1,1,"} err
	} else {
		catch {ans_sendcommand "SFCUM,PRES,ADD,1,1,"} err 
	}

	set  err  {}
	#if no areas was selected
	if {[llength $underface]==0} {
		set err "No Underface Was Select!"
	} else {
		applyUnderLoad
	}
	if {[llength $sideface]==0} {
		set err "No Sideface Was Select!"
	} else {
		applySideLoad
	}


	#show the apply load result
	catch {ans_sendcommand "/PSF,PRES,NORM,2,0,1"} err   
	catch {ans_sendcommand "/REPLO"} err   


	#reshow the temp component
	catch {ans_sendcommand )CMSEL,S,_temp_area} err     
    catch {ans_sendcommand )CMDEL,_temp_area} err
	catch {ans_sendcommand )CMSEL,S,_temp_element} err     
    catch {ans_sendcommand )CMDEL,_temp_element} err
    catch {ans_sendcommand )CMSEL,S,_temp_node} err     
    catch {ans_sendcommand )CMDEL,_temp_node} err          
    catch {ans_sendcommand )/GO} err
	after 500 {destroy .dynamicpressure}
}

#underface applyLoad
proc applyUnderLoad {} \
{
	#input
	variable underface
	variable elements
	variable ori_water
	variable rw
	variable factor

	#get elements map by nodes
	getElementNodeMap  $underface

	#get water flow direction
	set water_ori_flag [get_direction $ori_water]
	set water_ori [lindex $water_ori_flag 0]
	# set flag [lindex $direction_flag 1]

	foreach name [array names elements] {
		#if element node size small than 3,continue
		if {[llength $elements($name)] <3} {
			continue
		}
		set locs_h {}
		#unSelect all nodes  
		catch {ans_sendcommand "NSEL,None"} err  
		foreach node $elements($name) {
			#get the node location of x
			lappend locs_h [ans_evalexpr [format "N%s(%s)" $water_ori $node]]
			#Select the element node 
			catch {ans_sendcommand "NSEL,A,,,$node"} err   
		}
			#sort the location of nodes
			set locs_h [lsort -real $locs_h]
			#Element min and max location
			set min_loc [lindex $locs_h 0]
			set max_loc [lindex $locs_h end]
			set element_dis [expr $max_loc-$min_loc]

			set min_height [getNodeValue $min_loc]
			set max_height [getNodeValue $max_loc]
			
			########calculate the pressure
			set min_pressure [expr $rw*$min_height*$factor]
			set max_pressure [expr $rw*$max_height*$factor]

			set stepsize [expr ($max_pressure-$min_pressure)/$element_dis]


			catch {ans_sendcommand "SFGRAD,PRES,0,$water_ori,$min_loc,$stepsize"} err 
			catch {ans_sendcommand "SF,ALL,PRES,$min_pressure"} err 
	}
}

#sideface applyLoad
proc applySideLoad {} \
{
	variable sideface
	variable elements
	variable ori_gravity
	variable ori_water
	variable factor
	variable rw

	#get elements map by nodes
	getElementNodeMap  $sideface

	#get water flow direction
	set water_ori_flag [get_direction $ori_water]
	set water_ori [lindex $water_ori_flag 0]

	set gravity_ori_flag [get_direction $ori_gravity]
	set gravity_ori [lindex $gravity_ori_flag 0]
	set gravity_flag [lindex $gravity_ori_flag end]

	foreach name [array names elements] {
		#if element node size small than 3,continue
		if {[llength $elements($name)] <3} {
			continue
		}
		set locs_h {}
		set locs_v {}
		#unSelect all nodes  
		catch {ans_sendcommand "NSEL,None"} err

		foreach node $elements($name) {
			#get the node location of x
			lappend locs_h [ans_evalexpr [format "N%s(%s)" $water_ori $node]]
			lappend locs_v [ans_evalexpr [format "N%s(%s)" $gravity_ori $node]]
			#Select the element node 
			catch {ans_sendcommand "NSEL,A,,,$node"} err
		}
			#get the average location of horizontion
			set sum [expr [join $locs_h "+"]]
			#the average value of the element nodes
			set average [expr $sum*1.0/[llength $elements($name)]]

			set absheight  [getNodeValue $average]
			#sort the vertial
			set locs_v [lsort -real $locs_v]
			#Element min and max location 
			set min_loc_v [lindex $locs_v 0]
			set max_loc_v [lindex $locs_v end]
			set element_dis [expr $max_loc_v-$min_loc_v]

			set min_height [expr $absheight-$min_loc_v]
			set max_height [expr $absheight-$max_loc_v]
			#if the node vertical location grater than abs height,continue
			if {$min_height<0} {continue}
			##calculate the pressure
			set min_pressure [expr $rw*$min_height*$factor]
			set max_pressure [expr $rw*$max_height*$factor]

			set stepsize [expr ($max_pressure-$min_pressure)/$element_dis]

			catch {ans_sendcommand "SFGRAD,PRES,0,$direction,$min_loc_v,$stepsize"} err
			catch {ans_sendcommand "SF,ALL,PRES,$min_pressure"} err



	}
	
}
}

::Customized::Util::DynamicPressure::main
