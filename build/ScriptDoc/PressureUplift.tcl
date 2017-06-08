encoding system euc-cn
package	 require msgcat
namespace eval ::Customized::Util::UpLiftPressure {

proc main {} {\

	variable parent
	variable btn_pick
	#####################
	variable nodes
	variable factor
	variable scale
	variable expression
	variable loadpattern
	variable btn_check

	variable png
	variable font
	
	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	set png(select) [file join $path images pick24x24.png]
	set png(question) [file join $path images question24x24.png]
	set png(check) [file join $path images check24x24.png]
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(small) {"Microsoft YaHei" 10 normal} 
	set font(en)  {Consolas 12 normal}

	#destroy the window
	if [winfo exists .upliftpressure] {
	  wm deiconify .upliftpressure
	  raise .upliftpressure
	  return
	}
	set parent [toplevel .upliftpressure]
	wm title $parent [::msgcat::mc "UpLift Pressure "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-350)/2}]
	set y [expr {([winfo screenheight .]-400)/2}]
	wm geometry .upliftpressure 350x320+$x+$y
	wm protocol $parent WM_DELETE_WINDOW {
            if {[winfo exists .exphelp]} {
             destroy .exphelp  
        }
            destroy .upliftpressure
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
	    -text [msgcat::mc "Selected Nodes: "]]

	set btn_pick [euidl::button $parent.btn_pick \
        -command [code pick_node]   \
     	-imagefile $png(select) \
        -tooltip [msgcat::mc "Select Nodes "] ]

	set entry_pickinfo [entry $parent.entry_pickinfo \
		-textvariable [scope nodes] \
        -width 15\
        -font $font(en)\
        -relief  ridge]

   	set btn_check [euidl::button $parent.btn_check \
        -command [namespace code select_current]   \
     	-imagefile $png(check) \
        -tooltip [msgcat::mc "Select Current "] ]

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

	set label_scale [euidl::label $parent.labelscale \
		-height 2 \
		-font $font(cn) \
		-text [msgcat::mc "Scale: "]]

	set entry_scale [euidl::entry  $parent.entryscale -width 10  \
		-variable [scope scale] \
		-validate focusout\
		-width 15\
		-font $font(en)\
		-validatedata ansreal]

	set label_exp [euidl::label $parent.label_exp \
		-height 2 \
		-font $font(cn) \
		-text "[msgcat::mc "Expression: "]:"]

	set entry_exp [euidl::entry  $parent.entry_exp -width 10  \
		-variable [scope expression] \
		-validate focusout\
		-width 15\
		-font $font(en)]

	set btn_question [euidl::button $parent.btn_question \
        -command [code showExpHelp]   \
     	-imagefile $png(question) \
        -tooltip [msgcat::mc "Show Expression "] ]

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
		-command { destroy .upliftpressure}]


	grid $label_loadtype  -row 1 -column 0 -sticky w -pady 2m
	grid $combo_loadtype  -row 1 -column 1 -sticky w -pady 2m

	grid $label_pick -row 2 -column 0  -sticky w
	grid $entry_pickinfo -row 2 -column 1 -sticky w 
	grid $btn_pick -row 2 -column 2  
	grid $btn_check -row 2 -column 3 

	grid $label_factor -row 3 -column 0 -sticky w
	grid $entry_factor -row 3 -column 1 -sticky w


	grid $label_scale -row 4 -column 0 -sticky w
	grid $entry_scale -row 4 -column 1 -sticky w

	grid $label_exp -row 5 -column 0  -sticky w
	grid $entry_exp -row 5 -column 1 -sticky w 
	grid $btn_question -row 5 -column 2 

	pack  $btn_ok  $btn_cancel -in $frame_oncancel -side left -fill both -padx 12m -pady 5m
	grid $frame_oncancel  -row 6 -column 0   -columnspan 3   -sticky nsew

	#default set X
	$combo_loadtype  insert list end [list [msgcat::mc "Replace "] "Replace" [msgcat::mc "Append " ] "Append"]

	#initialize the param
	::Customized::Util::UpLiftPressure::initialize
}

proc select_current {} \
{
	variable btn_check

	get_SelectedNodes
	
	$btn_check configure -relief sunken
	
}
proc initialize {} \
{
	variable factor
	variable scale
	variable nodes
	variable loadpattern
	variable expression

	set loadpattern "Replace"
	set factor 1.2
	set scale  0.01
	set nodes {}
	set expression {}



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
   after 25 [raise .upliftpressure]
}

proc get_SelectedNodes {} \
{
	variable parent
	variable nodes

	set node_nums [ans_getvalue "node,,count"]
	set nodes {}
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
	variable factor
	variable scale
	variable nodes
	variable loadpattern
	variable expression


	#save the current node component to recover
	catch {ans_sendcommand )CM,_temp_node,NODE} err
	catch {ans_sendcommand )CM,_temp_element,ELEMENT} err
	catch {ans_sendcommand )CM,_temp_area,AREA} err

	set  err  {}

	if {[llength $nodes]==0} {
		set err "No Nodes Was Select!"
	}
	if {$expression==""} {
		set err "Expression is None "
	}

	set firstnode [ans_evalexpr "ndnext(0)"]
	set theExp [ParseExpression $firstnode]


	if {[catch {ans_evalexpr $theExp}]} {
		set err "Expression is Wrong! "
	}
	#check the err
	if {![string match {} $err]} {
		tk_messageBox  -parent $parent \
			-icon warning \
			-title [::msgcat::mc "Tips "] \
			-message  [::msgcat::mc $err]
		return
	}

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


	#get the max node number
	set NMAX [lindex [lsort -real $nodes] end]

	ans_sendcommand   $cmd_sfcum
	ans_sendcommand  "*dim,_PRES,array,$NMAX"
	foreach node $nodes {
		# ans_sendcommand [format "_PRES($node)=$factor*$scale*($peak-N%s($node))" $direction]
		ans_sendcommand "_PRES($node)=$factor*$scale*([ParseExpression $node])"
	}



	ans_sendcommand "SFFUN,PRES,_PRES(1)"
	#!in node i ,the pressure is 0 + _PRES(i)
	ans_sendcommand "SF,ALL,PRES,0"
	#delete the temp array
	ans_sendcommand "*SET,_PRES"
	#plot the force
	ans_sendcommand "/PSF,PRES,NORM,2,0,1"
	ans_sendcommand "/REP,FAST"


	#logging
	puts "######## UpLift Pressure APDL Start ############"
	puts ">>>> *dim,_PRES,array,$NMAX"
	puts ">>>> _PRES(i)=$factor*$scale*([ParseExpression i])"
	puts ">>>> $cmd_sfcum"
	puts ">>>> SFFUN,PRES,_PRES(1)"
	puts ">>>> SF,ALL,PRES,0"
	puts ">>>> *SET,_PRES"
	puts "######## UpLift Pressure APDL Finish ############"
	#reshow the temp component
	catch {ans_sendcommand )CMSEL,S,_temp_area} err     
    catch {ans_sendcommand )CMDEL,_temp_area} err
	catch {ans_sendcommand )CMSEL,S,_temp_element} err     
    catch {ans_sendcommand )CMDEL,_temp_element} err
    catch {ans_sendcommand )CMSEL,S,_temp_node} err     
    catch {ans_sendcommand )CMDEL,_temp_node} err          
    catch {ans_sendcommand )/GO} err
	after 500 {destroy .upliftpressure}
}

#reg match
proc ParseExpression {node} \
{
	variable expression

	regsub -all -nocase {NX\(.*\)|NX|X}  $expression NX($node)  regx
	regsub -all -nocase {NY\(.*\)|NY|Y}  $regx       NY($node)  regy
	regsub -all -nocase {NZ\(.*\)|NZ|Z}  $regy       NZ($node)  regz
	return $regz
}
proc showExpHelp {} \
{

	variable font
	variable factor
	variable scale
	variable utimateExp


	if [winfo exists .exphelp] {
	  destroy .exphelp
	}
	set utimateExp {}
	set help [toplevel .exphelp]

	wm title $help [::msgcat::mc "Show Expression "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-500)/2}]
	set y [expr {([winfo screenheight .]-220)/2}]
	wm geometry .exphelp 500x220+$x+$y
	set label_tips [label $help.label_tips \
		-text [::msgcat::mc "Utimate Expression:"]\
		-font $font(cn)]
	set entry_exp [entry $help.entry_exp \
		-font $font(cn)\
		-relief ridge\
		-state readonly\
		-textvariable [scope utimateExp]]
	set tips1 [label $help.tips1 \
		-text [::msgcat::mc "Expression Is NoCase"]\
		-font $font(small)]
	set tips2 [label $help.tips2 \
		-text [::msgcat::mc "X->NX(id),Y->NY(id),Z->NZ(id)"]\
		-font $font(small)]	
	set tips3 [label $help.tips3 \
		-text [::msgcat::mc "Exp:2*X+Y-Z+100=2*NX+NY()-NZ()"]\
		-font $font(small)]
	set tips4 [label $help.tips4 \
		-text [::msgcat::mc "Utimate Result"]\
		-font $font(small)]	
	set btnok [button $help.btnok \
		-text  [::msgcat::mc "Ok"]\
		-font $font(small)\
		-width 8\
		-command {destroy .exphelp}]

	grid $tips1 -row 0 -column 0 -sticky w
	grid $tips2 -row 1 -column 0 -sticky w
	grid $tips3 -row 2 -column 0 -sticky w
	grid $tips4 -row 3 -column 0 -sticky w

	grid $btnok -row 6 -column 0 -pady 3m

	set parsedexp [ParseExpression "id"]
	if {$parsedexp!=""} {
		grid $label_tips -row 4 -column 0 -sticky w
		grid $entry_exp -row 5 -column 0 -sticky ew
		set utimateExp "$factor*$scale*($parsedexp)"
	} 

	
}

}

::Customized::Util::UpLiftPressure::main
