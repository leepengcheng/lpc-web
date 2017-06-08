encoding system euc-cn
package require msgcat
namespace eval ::euidl::Manager::LoadCase {} {

proc main {} {

      variable table
      variable tabledata
      variable lclistbox
      variable lclistvar
      variable parent
      variable font
      variable png

      set path [file dirname [info script]]
      ::msgcat::mcload  [file join $path locale]
      set png(import) [file join $path images import24x24.png]
      set png(pick) [file join $path images pick24x24.png]
      set png(new) [file join $path images new24x24.png]
      set png(del) [file join $path images delete24x24.png]
      set png(erase) [file join $path images erase24x24.png]
      set png(read) [file join $path images read24x24.png]
      set png(write) [file join $path images write24x24.png]
      set png(add) [file join $path images add24x24.png]
      set png(sub) [file join $path images sub24x24.png]
      set png(square) [file join $path images square24x24.png]
      set png(min) [file join $path images min24x24.png]
      set png(absmin) [file join $path images absmin24x24.png]
      set png(max) [file join $path images max24x24.png]
      set png(absmax) [file join $path images absmax24x24.png]
      set png(sqrt) [file join $path images sqrt24x24.png]
      set png(srss) [file join $path images srss24x24.png]
      set font(content)  {"Microsoft YaHei" 11 normal}
      set font(title)  {"Microsoft YaHei" 12 normal}

      #chekc Environment
      if {[chekcEnvironment]} {
        return
      }
      if {[winfo exists .loadcasemanager]} {
             wm deiconify .loadcasemanager
             raise .loadcasemanager
             return
      }
      set parent [toplevel .loadcasemanager]
      wm title $parent [msgcat::mc "LoadCases Manager "]
      set x [expr {([winfo screenwidth  .]-700)/2}]
      set y [expr {([winfo screenheight .]-450)/2}]
      wm geometry $parent  700x450+$x+$y
      #delete all the window
      wm protocol $parent WM_DELETE_WINDOW {
          if {[winfo exists .lcapdl]} {
            destroy .lcapdl
          }
          if {[winfo exists .cmpinfo]} {
            destroy .cmpinfo
          }
          if {[winfo exists .lcnew]} {
            destroy .lcnew
          }
          destroy .loadcasemanager
        
      }

      #-------------------
      set header [euidl::labeledframe $parent.header \
          -font $font(title) -text [msgcat::mc "Toolbar "]]
      set btn_create [euidl::button $parent.btn_create \
          -command [namespace code createLoadCase] \
          -imagefile $png(new) \
          -tooltip [msgcat::mc "Create LoadCase "]]

      set btn_delete [euidl::button $parent.btn_delete \
          -command [list [code deleteLoadCase] "SEL"] \
          -imagefile $png(del) \
          -tooltip [msgcat::mc "Delete Loadcase "] ]

      set btn_write [euidl::button $parent.btn_write \
          -command [list [code handleLoadCase] "WRITE"]\
          -imagefile $png(write) \
          -tooltip [msgcat::mc "Write loadcase"] ]

      set btn_erase [euidl::button $parent.btn_erase \
          -command [list [code deleteLoadCase] "ALL"] \
          -imagefile $png(erase) \
          -tooltip [msgcat::mc "Erase loadcase"] ]

      set btn_read [euidl::button $parent.btn_read \
          -command [list [code handleLoadCase] "READ"]\
          -imagefile $png(read) \
          -tooltip [msgcat::mc "Read loadcase"] ]

      set btn_add [euidl::button $parent.btn_add \
        -command [namespace code intersectComponents] \
        -imagefile $png(add) \
        -command [list [code handleLoadCase] "ADD"]\
        -tooltip [msgcat::mc "Add loadcase"]]

      set btn_sub [euidl::button $parent.btn_sub \
        -command [namespace code intersectComponents] \
        -imagefile $png(sub) \
        -command [list [code handleLoadCase] "SUB"]\
        -tooltip [msgcat::mc "Sub loadcase"]]

      set btn_min [euidl::button $parent.btn_min \
        -command [namespace code intersectComponents] \
        -text " Min "\
        -imagefile $png(min) \
        -command [list [code handleLoadCase] "MIN"]\
        -tooltip [msgcat::mc "Min loadcase"]]

      set btn_absmin [euidl::button $parent.btn_absmin \
        -command [namespace code intersectComponents] \
        -imagefile $png(absmin) \
        -command [list [code handleLoadCase] "ABMIN"]\
        -tooltip [msgcat::mc "Absolute min loadcase"]]

      set btn_max [euidl::button $parent.btn_max \
        -command [namespace code intersectComponents] \
        -imagefile $png(max) \
        -command [list [code handleLoadCase] "MAX"]\
        -tooltip [msgcat::mc "Max loadcase"]]

      set btn_absmax [euidl::button $parent.btn_absmax \
        -command [namespace code intersectComponents] \
        -imagefile $png(absmax) \
        -command [list [code handleLoadCase] "ABMAX"]\
        -tooltip [msgcat::mc "Absolute max loadcase"]]

      set btn_squa [euidl::button $parent.btn_squa \
        -command [namespace code intersectComponents] \
        -imagefile $png(square) \
        -command [list [code handleLoadCase] "SQUA"]\
        -tooltip [msgcat::mc "Square loadcase "]]

      set btn_sqrt [euidl::button $parent.btn_sqrt \
        -command [namespace code intersectComponents] \
        -imagefile $png(sqrt) \
        -command [list [code handleLoadCase] "SQRT"]\
        -tooltip [msgcat::mc "Square root loadcase "]]

      set btn_srss [euidl::button $parent.btn_srss \
        -command [namespace code intersectComponents] \
        -imagefile $png(srss) \
        -command [list [code handleLoadCase] "SRSS"]\
        -tooltip [msgcat::mc "Srss loadcase "]]


      grid $header -row 0 -column 0 -sticky news -columnspan 2
      set header [$header childsite]
      pack $btn_create $btn_delete $btn_write $btn_erase $btn_read $btn_add \
      $btn_sub $btn_min $btn_absmin $btn_max $btn_absmax \
      $btn_squa $btn_sqrt $btn_srss -in $header -side left -padx 1m
      ####################################
      #Left frame
      set left [euidl::labeledframe $parent.left -font $font(title) \
         -text [msgcat::mc "Defined LoadCases "]]
      grid $left -row 1 -column 0  -sticky news
      set left [$left childsite]
      #footer frame
      set footer [frame $parent.footer]
      grid $footer  -row 2 -column 0  -columnspan 2 
      #Right frame
      set right [euidl::labeledframe $parent.right -font $font(title) \
          -text [msgcat::mc "Selected LoadCases "]]
      grid $right -row 1 -column 1  -sticky news
      set right [$right childsite]
      grid  columnconfigure $parent 0 -weight 2
      # grid  columnconfigure $parent 1 -weight 1
      grid  columnconfigure $parent 1 -weight 1
      grid  rowconfigure $parent 1 -weight 3
      grid  rowconfigure $parent 2 -weight 1

      #footer button
      set btnplot [button $parent.create -text [msgcat::mc "Plot Loadcase "]  -width 8 -font $font(content) -command [list [code plotLoadCase] "ALL" "Eqv"] ]
      set btnok [button $parent.ok -text [msgcat::mc "Ok "]  -width 8 -font $font(content) -command [code execAPDL] ]
      set btncancel [button $parent.cancel -text [msgcat::mc "Cancel "] -width 8 -font $font(content) -command {destroy .loadcasemanager} ]
      pack $btnplot $btnok  $btncancel -in $footer -side left -padx 10m -pady 4m

      #Tree for the left side
      set table [euidl::colhead $left.table \
      -variable [scope tabledata] \
      -selectbackground #09f\
      -borderwidth 0 \
      -hscrollmode none \
      -contextmenu 1 ]


      #insert head
      $table insert cols {ID} end
      $table insert cols {Factor} end
      $table insert cols {ABS} end
      $table insert cols {Property} end


      #sortmethod 
      $table tag configure {ID} -sortmethod integer
      $table tag configure {Factor} -sortmethod real
      $table tag configure {ABS} -sortmethod dictionary
      $table tag configure {Property} -sortmethod dictionary



      $table tag configure title -anchor center
      $table tag configure title -font $font(title)
      $table tag configure row -font $font(content)
      $table tag configure row -anchor center
     

      #cofigure the width of the header
      [$table component table] width [$table tag coltag {ID}] 10
      [$table component table] width [$table tag coltag {Factor}] 10
      [$table component table] width [$table tag coltag {ABS}] 10
      [$table component table] width [$table tag coltag {Property}] 36
      #the rows height
      [$table component table] configure -rowheight 2
      bind $table <Double-1> [code showLoadCase]
      # puts [[$table component table] configure]
      #pack the table
      pack $table -fill both -expand 1
      #ContextMenu
      $table deleteContextMenu 0 end 
      set cmindex 0
      # $table insertContextMenu $cmindex command \
      # -label [msgcat::mc "Show Displace "] \
      # -font $font(content) \
      # -command [list [code plotLoadCase] "Usum"]
      # incr cmindex
      $table insertContextMenu $cmindex command \
      -label [msgcat::mc "Plot Sel LoadCase Eqv "] \
      -font $font(content) \
      -command [list [code plotLoadCase] "Sel" "Eqv"]
      incr cmindex
     $table insertContextMenu $cmindex command \
      -label [msgcat::mc "Delete LoadCase "] \
      -font $font(content) \
      -command [list [code deleteLoadCase] "SEL"]


      #Tree for the right side
     set lclistbox [euidl::scrolledlistbox $right.lclistbox \
          -listvariable [scope lclistvar]\
          -exportselection false \
          -height 250 \
          -selectmode extended\
          -hscrollmode none \
          -textfont  $font(content) \
          -contextmenu 1 \
          -selectbackground #09f\
          -dblclickcommand [namespace code editAPDL] \
          -relief  flat  
      ]
      $lclistbox deleteContextMenu 0 end
      set cmindex 0
      $lclistbox insertContextMenu $cmindex command \
            -label [msgcat::mc "MoveUp "] \
            -underline 0 \
            -state disabled\
            -font $font(content) \
            -command [list [namespace code cmplistMove] "up"]
      incr cmindex
      $lclistbox insertContextMenu $cmindex command \
            -label [msgcat::mc "MoveDown "] \
            -underline 0 \
            -state disabled\
            -font $font(content) \
            -command [list [namespace code cmplistMove] "down"]
      incr cmindex
      $lclistbox insertContextMenu $cmindex command \
            -label [msgcat::mc "Del "] \
            -underline 0 \
            -state disabled\
            -font $font(content) \
            -command [namespace code delAPDL]
      # puts [$lclistbox configure]
      
      grid $lclistbox -sticky news
      grid  columnconfigure $right 0 -weight 1
      grid  columnconfigure $left 0 -weight 1
      grid  rowconfigure $right 0 -weight 1
      grid  rowconfigure $left 0 -weight 1
      update idletasks
      #initialize the value of material data
      ::euidl::Manager::LoadCase::initialize
  }



#initialize the table data
proc initialize {} \
{ 
    variable table
    variable tabledata
    variable casesinfo
    variable casenum
    variable lclistvar


    #erase  the apdl listbox 
    set lclistvar {}
    #load the loadcase data
    update_LoadCaseList
    set i 0
    foreach caseid [array names casesinfo] {
        #$fac $abs $property $step $substep $file
        set caserow $casesinfo($caseid)
        $table insert row end
        set tabledata($i,-1) {}
        #case id
        set tabledata($i,0)  $caseid
        #case factor
        set tabledata($i,1)  [lindex $caserow 0]
        #case abs
        set tabledata($i,2)  [lindex $caserow 1]
        
        #get the case property
        set index  "_lc_pro_$caseid"
        set flag [ans_getvalue parm,$index,type]
        if {$flag!=-1} {
          set property [string trim [ans_getvalue parm,$index,value]]
        } else {
          set property "UnDefined"
        }
        #case property
        set tabledata($i,3)  $property
        #case step
        set tabledata($i,4)  [lindex $caserow 2]
        #case substep
        set tabledata($i,5)  [lindex $caserow 3]
        #case file
        set tabledata($i,6)  [lindex $caserow 4]
        incr i
    }
        

  #disabled the delete button and contextmenu
  if { [$table index end row]==-1} {
         [$table getContextMenu] entryconfigure 0 -state disabled
         [$table getContextMenu] entryconfigure 1 -state disabled
      }

}

proc update_LoadCaseList { } \
{
  #output variable
  variable casesinfo
  variable casenum

  if {[info exists casesinfo]} {
      unset casesinfo
  }
  #change to output information to lclist.log
  catch {ans_sendcommand "/GO"} err
  catch {ans_sendcommand "/OUTPUT,lclist,log"} err
  catch {ans_sendcommand "LCDEF,STAT,ALL"} err
  #reset the output information to ansys output window
  catch {ans_sendcommand "/OUTPUT"} err
  #open the lclist.log file to get the loadcase information
  set f [open lclist.log r]
  set lclist [read $f]
  close $f
  #reg match the loadcase data
  set case_id [regexp -all -inline -- {LOAD CASE=\s*([0-9]*[0-9]+)\s*}  $lclist]
  # set case_sel [regexp -all -inline -- {SELECT=\s*([0-9]*[0-9]+)\s*}  $lclist]
  set case_abs [regexp -all -inline -- {ABS KEY=\s*([0-9]*[0-9]+)\s*}  $lclist]
  # set case_fac [regexp -all -inline -- {FACTOR=\s*([0-9]*[0-9]+)\s*}  $lclist]
  set case_fac [regexp -all -inline -- {FACTOR=\s*([\d.E+]+)\s*}  $lclist]
  set case_step [regexp -all -inline -- {LOAD STEP=\s*([0-9]*[0-9]+)\s*}  $lclist]
  set case_substep [regexp -all -inline -- {SUBSTEP=\s*([0-9]*[0-9]+)\s*}  $lclist]
  set case_file [regexp -all -inline -- {\s*FILE=(\w+\.\w+)\s*}  $lclist]
  # puts ">>>>$case_file"
  set casenum [expr [llength $case_id]/2]
  #insert the data to the table
  for {set i 0} {$i < $casenum} {incr i} {
    set index [expr 2*$i+1]
    set id [lindex $case_id  $index]
    set abs [lindex $case_abs  $index]
    #translate 0.1E+03 to 100
    set fac [expr [lindex $case_fac  $index]]
    set step [lindex $case_step  $index]
    set substep [lindex $case_substep  $index]
    set file [lindex $case_file  $index]
    set casesinfo($id) "$fac $abs $step $substep $file"
  }

}

  #delete table row,delete the selected material
  proc handleLoadCase {operator} \
  {
    variable parent
    variable table
    variable tabledata
    variable lclistbox
 
    if {$operator=="WRITE"} {
      if {[$lclistbox size]==0} {
      tk_messageBox -title [msgcat::mc "Tips "] \
        -parent $parent\
        -message [msgcat::mc "No LoadCase Was Read! "] \
        -icon warning 
        return
      }
      set lcfile [tk_getSaveFile  -filetypes {{loadcase .l*}}]
      set dirname [file dirname $lcfile]
      set filename [file tail $lcfile]

      set command "LCWRITE,11,'$filename','','$dirname'"
      # LCWRITE,12,'asdasdasd',' ','D:\Robomongo\'
      puts $command
      catch {ans_sendcommand $command} err
      tk_messageBox -title [msgcat::mc "Tips "] \
        -parent $parent\
        -message [msgcat::mc "Export Is Done! "] \
        -icon info
      return
    }

    set selection [lsort -integer  [$table curselection]]    
    # If there is no selection then return
    if { [string match {} $selection] } {
      return
    }
    foreach sel $selection {
      set caseid $tabledata($sel,0)
      if {[$lclistbox size]==0} {
        set command "LCASE,$caseid"

      } else {
        # set command "LCOPER,$operator,$caseid"
        # LCOPER,SQUA 
        switch -exact -- $operator {
          READ {
            set command "LCASE,$caseid"
          }
          ADD {
            set command "LCOPER,ADD,$caseid"
          }
          SUB {
            set command "LCOPER,SUB,$caseid"
          }
          SQUA {
            set command "LCOPER,SQUA "
          }
          MIN {
            set command "LCOPER,MIN,$caseid"
          }
          ABMIN {
            set command "LCOPER,ABMN,$caseid"
          }
          MAX {
            set command "LCOPER,MAX,$caseid"
          }
          ABMAX {
            set command "LCOPER,ABMX,$caseid"
          }
          SQRT {
            set command "LCOPER,SQRT"
          }
          SRSS {
            set command "LCOPER,SRSS,$caseid"
          }
          default {}
        }
      }
      $lclistbox insert end [list $command $command]
    }

    # Update the selection after the deletion is complete.
     if { [llength $selection] > 1 } {
        # If there were multiple rows selected, force selection to row 0
        set rowNum 0
     } else {
        # Only one item was selected
        set rowNum [$table index end row]
        if { $sel < $rowNum } {
           set rowNum $sel
        }
     }
    # # If there are no more rows in the column heading, then disabled the
    # # delete button.
    if { [$lclistbox size]>0} {
        for {set i 0} {$i < 3} {incr i} {
          [$lclistbox getContextMenu] entryconfigure $i -state normal
        }
    } 
}
#show the local material information
  proc showLoadCase {} \
{
        variable table
        variable tabledata
        variable font
        variable subform
        #info window variable
        variable the_fac
        variable the_abs
        variable the_pro
        variable entrystep

        if {[winfo exists .cmpinfo]} {
               destroy .cmpinfo]
        }
        set selection [lsort -integer  [$table curselection]]
        # If there is no selection then return
        if { [string match {} $selection] } {
           return
        }
        #if multi select,set the first item
        set sel [lindex $selection 0]
        $table selection clear all
        $table selection set $sel,-1
        set the_fac  $tabledata($sel,1)
        set the_abs  $tabledata($sel,2)
        set the_pro  $tabledata($sel,3)

        #load case id
        set id $tabledata($sel,0)
        set subtitle "[msgcat::mc "ComponentID "]:$id"
        set sublabeltips  [msgcat::mc "LoadCase Property "]
        set thestate normal
        #create the top level
        set subform [toplevel .cmpinfo]
        grab set $subform
        #set title for the window 
        wm title .cmpinfo $subtitle
        #adjust the location of subwindow window
        set x [expr {([winfo screenwidth .]-360)/2}]
        set y [expr {([winfo screenheight .]-360)/2}]
        wm geometry $subform  360x360+$x+$y
        wm transient $subform
        #Components ID and Components name
        set lableid [label $subform.lableid \
          -text "[msgcat::mc "ComponentID "](ID):          "\
          -font $font(content) \
          -width 14]
        set entryid [entry $subform.entryid\
          -textvariable [scope tabledata($sel,0)] \
          -width 10 \
          -font $font(content) \
          -state disabled\
          -width 15]
        #factor
        set labelfactor [label $subform.labelfactor\
          -text "[msgcat::mc "Factor "](Factor):  " \
          -font $font(content) \
          -width 14]
        set entryfactor [entry $subform.entryfactor \
          -textvariable [scope the_fac] \
          -width 10 -font $font(content)\
          -width 15 ]
        #abs
        set labelabs [label $subform.labelabs \
          -text "[msgcat::mc "ABS "](ABS):      "\
          -font $font(content) \
          -width 14]
        set comboabs [euidl::combobox $subform.comboabs \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(content) \
         -variable [scope the_abs]]
        
        #load step
        set labelstep [label $subform.labelstep \
          -text "[msgcat::mc "Step "](Step):          " \
          -font $font(content) \
          -width 14]
        set entrystep [entry $subform.entrystep \
          -textvariable [scope tabledata($sel,4)] \
          -width 10 \
          -state disabled\
          -font $font(content) \
          -width 15]
        #substep
        set labelsubstep [label $subform.labelsubstep \
          -text "[msgcat::mc "Substep "](Substep):" \
          -font $font(content) \
          -width 14]
        set entrysubstep [entry $subform.entrysubstep \
          -textvariable [scope tabledata($sel,5)] \
          -width 10 \
          -state disabled\
          -font $font(content) \
          -width 15]
        #file 
        set labelfile [label $subform.labelfile \
          -text "[msgcat::mc "LCFile "](CaseFile):"\
          -font $font(content) \
          -width 14]
        set entryfile [entry $subform.entryfile \
          -textvariable [scope tabledata($sel,6)] \
          -width 10 \
          -state disabled\
          -font $font(content) \
          -width 15]
        #property
        set labelproperty [label $subform.labelId \
          -text "[msgcat::mc "Property "](Property):  " \
          -font $font(content) \
          -width 16]
        set entryproperty [entry $subform.entryId \
          -textvariable [scope the_pro] \
          -width 10 \
          -font $font(content) \
          -width 15]


        set frameokcancel [label $subform.frameokcancel]
        set btnok [button $subform.btnok \
          -text [msgcat::mc "Ok "]\
          -font $font(content) \
          -width 10\
          -command [namespace code ok_case_info_click]]
        set btnexit [button $subform.btnexit\
          -text [msgcat::mc "Exit "]\
          -font $font(content) \
          -width 10\
          -command { destroy .cmpinfo }]

        ###########create and arrange the control#################
        # grid $labelTips  -row 0  -column 0  -columnspan 2  -pady 2m

        #id
        grid $lableid -row 1  -column 0  -sticky w 
        grid $entryid -row 1  -column 1  -padx 6m -pady 2m
        #factor
        grid $labelfactor -row 2  -column 0 -sticky w 
        grid $entryfactor -row 2   -column 1 -padx 6m  -pady 2m
        #abs
        grid $labelabs -row 3  -column 0 -sticky w 
        grid $comboabs -row 3   -column 1 -padx 6m  -pady 2m
        #abs
        grid $labelproperty -row 4  -column 0 -sticky w 
        grid $entryproperty -row 4   -column 1 -padx 6m  -pady 2m
        #step
        grid $labelstep -row 5  -column 0 -sticky w 
        grid $entrystep -row 5   -column 1 -padx 6m  -pady 2m
        #substep
        grid $labelsubstep -row 6  -column 0 -sticky w 
        grid $entrysubstep -row 6  -column 1 -padx 6m  -pady 2m
        #file
        grid $labelfile -row 7  -column 0 -sticky w 
        grid $entryfile -row 7  -column 1 -padx 6m  -pady 2m

        pack $btnok $btnexit -in $frameokcancel -side left -padx 5m 
        grid $frameokcancel  -row 8 -column 0  -columnspan 2 -pady 4m

        $comboabs insert list end  [list YES 1 NO 0]
        update idletasks
  }
#move up or down
proc cmplistMove {direction} \
{  
  variable lclistvar
  variable lclistbox

  set len [$lclistbox size]
  set sel [lindex [$lclistbox curselection] 0]
      
  if {$direction=="up"} {
     set nextsel [expr $sel==0?$len-1:$sel-1] 
  } else {
     set nextsel [expr $sel==$len-1?0:$sel+1]
  }
  set data [$lclistbox get $sel]
  set nextdata [$lclistbox get $nextsel]
  lset lclistvar $nextsel $data
  lset lclistvar $sel $nextdata
  $lclistbox selection clear 0 end
  $lclistbox selection set  $nextsel
  $lclistbox activate  $nextsel


}
  proc editAPDL {} \
{
      
        variable font
        #get the Defined loadcase ids
        variable casesinfo
        variable lclistbox
        variable selid
        variable seloper
        variable selproperty
        variable apdlform


        if {[winfo exists .lcapdl]} {
               destroy .lcapdl]
        }
        set selection [lsort -integer  [$lclistbox curselection]]
        # If there is no selection then return
        if { [string match {} $selection] } {
           return
        }
        # #if multi select,set the first item
        set sel [lindex $selection 0]
        $lclistbox selection clear 0 end
        $lclistbox selection set $sel

        set item [$lclistbox get $sel]
        set apdl [split $item ,]
        #LCCASE||LCOPER
        set seltype [string trim [lindex $apdl 0]]
        if {$seltype=="LCASE"} {
          set seloper "INIT"
          set selid [string trim [lindex $apdl 1]]
        } else {
          #add sub sqrt and soso
          set seloper [string trim [lindex $apdl 1]]
          #LCOPER,SUB,1
          if {[llength $apdl]==3} {
              set selid [string trim [lindex $apdl 2]]
          #LCOPER,SQRT
          } else {
              set selid {}
          }

        }
        set index  "_lc_pro_$selid"
        set flag [ans_getvalue parm,$index,type]
        if {$flag!=-1} {
          set selproperty [string trim [ans_getvalue parm,$index,value]]
        } else {
          set selproperty "UnDefined"
        }

        set subtitle "[msgcat::mc "LoadCase "]:$selid"
        set sublabeltips  [::msgcat::mc "LoadCase Property "]
        set thestate normal
        #create the top level
        set apdlform [toplevel .lcapdl]
        grab set $apdlform
        #set title for the window 
        wm title .lcapdl $subtitle
        #adjust the location of subwindow window
        set x [expr {([winfo screenwidth .]-300)/2}]
        set y [expr {([winfo screenheight .]-250)/2}]
        wm geometry $apdlform  300x250+$x+$y
        wm transient $apdlform

        set labelTips       [label $apdlform.labelTips -text $sublabeltips -font $font(content)]
        #Components ID and Components name
        set labelName [label $apdlform.labelName \
          -text "[msgcat::mc "Case ID "]:"\
          -font $font(content) \
          -width 8]
        set comboID [euidl::combobox $apdlform.comboID\
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 18\
         -font $font(content) \
         -variable [scope selid]]

        set labelType [label $apdlform.labelType\
          -text "[msgcat::mc "OperateType "]:" \
          -font $font(content) \
          -width 8]
        set comboOper [euidl::combobox $apdlform.comboOper \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 18\
         -font $font(content) \
         -variable [scope seloper]]

        set labelProperty [label $apdlform.labelId \
          -text "[msgcat::mc "Property "]:" \
          -font $font(content) \
          -width 8]
        set entryProperty [entry $apdlform.entryId \
          -textvariable [scope selproperty] \
          -width 10 \
          -state disabled\
          -font $font(content) \
          -width 20]
        set frameokcancel [label $apdlform.frameokcancel]
        set btnok [button $apdlform.btnok \
          -text [msgcat::mc "Ok "]\
          -font $font(content) \
          -width 8\
          -command [namespace code ok_apdl_click]]
        set btnexit [button $apdlform.btnexit\
          -text [msgcat::mc "Exit "]\
          -font $font(content) \
          -width 8\
          -command { destroy .lcapdl }]

        ###########create and arrange the control#################
        grid $labelTips  -row 0  -column 0  -columnspan 2  -pady 2m

        grid $labelName -row 1  -column 0  -sticky w 
        grid $comboID -row 1  -column 1  -padx 6m -pady 2m
        grid $labelType -row 2  -column 0 -sticky w 
        grid $comboOper -row 2   -column 1 -padx 6m  -pady 2m
        grid $labelProperty -row 3  -column 0 -sticky w 
        grid $entryProperty -row 3  -column 1 -padx 6m  -pady 2m

        pack $btnok $btnexit -in $frameokcancel -side left -padx 5m 
        grid $frameokcancel  -row 4 -column 0  -columnspan 2 -pady 4m

        $comboOper insert list end [list \
        [msgcat::mc "INIT "] INIT\
        [msgcat::mc "ADD "] ADD \
        [msgcat::mc "SUB "] SUB \
        [msgcat::mc "MIN "] MIN \
        [msgcat::mc "MAX "] MAX\
        [msgcat::mc "ABMN "] ABMN\
        [msgcat::mc "ABMX "] ABMX\
        [msgcat::mc "SQRT "] SQRT\
        [msgcat::mc "SQUA "] SQUA\
        [msgcat::mc "SRSS "] SRSS\
        ]
        #empty value 
        set ids {"" ""}
        foreach var [array names casesinfo] {
          lappend ids $var $var
        } 
        $comboID insert list end $ids
        update idletasks
  }
  #delete the selected apdl
  proc delAPDL {} \
  {
    variable lclistbox

    set selection [lsort -integer -decreasing [$lclistbox  curselection]]
    foreach sel $selection {
      $lclistbox delete $sel
    }
    if { [$lclistbox size]==0} {
          for {set i 0} {$i < 3} {incr i} {
            [$lclistbox getContextMenu] entryconfigure $i -state disabled
          }

    } else {
          for {set i 0} {$i < 3} {incr i} {
            [$lclistbox getContextMenu] entryconfigure $i -state normal
          }
    }

  }


proc ok_case_info_click {} \
{
  variable table
  variable subform
  variable tabledata
  #input data
  variable the_fac
  variable the_abs
  variable the_pro


  set sel [$table curselection]
  set tabledata($sel,1)  $the_fac
  set tabledata($sel,2)  $the_abs
  set tabledata($sel,3)  $the_pro

 if {[string length $the_pro]>10} {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -message [msgcat::mc "Property length is too long! "] \
        -icon question 
  }

  set caseid $tabledata($sel,0)
  #factor data
  catch {ans_sendcommand "LCFACT,$caseid,$the_fac"} err
  #abs data
  catch {ans_sendcommand "LCABS,$caseid,$the_abs"} err
  #change property
  catch {ans_sendcommand "*set,_lc_pro_$caseid,'$the_pro'"} err

  destroy $subform
}

proc ok_apdl_click {} \
{
  variable apdlform
  variable selid
  variable seloper
  variable lclistvar
  variable lclistbox

  set sel [$lclistbox curselection]
  switch -exact -- $seloper {
    INIT {
      if {$selid==""} {return}
      set command "LCASE,$selid"
    }
    SQRT {
      set command "LCOPER,SQRT"
    }
    SQUA {
      set command "LCOPER,SQUA"
    }
    default {
      if {$selid==""} {return}
      set command "LCOPER,$seloper,$selid"
    }
  }
  lset lclistvar $sel $command
  destroy $apdlform

}
proc deleteLoadCase {arg} \
{
    variable table
    variable tabledata

    if {$arg=="ALL"} {
      set selection {}
      set casenum [$table index end row]
      puts $casenum
      #decreasing sort
      for {set i $casenum} {$i >=0} {incr i -1} { lappend selection $i }      
    } else {
      set selection [lsort -integer -decreasing  [$table curselection]]
    }
    # If there is no selection then return
    if { [string match {} $selection] } {
      return
    }
    
    foreach sel $selection {
      set caseid $tabledata($sel,0)
      catch {ans_sendcommand "LCDEF,$caseid,ERASE"} err
      catch {ans_sendcommand "*set,_lc_pro_$caseid"} err
      $table delete rows $sel
    }
      # # If there are no more rows in the column heading, then disabled the
      # # delete button.
     if { [$table index end row]==-1} {
           [$table getContextMenu] entryconfigure 0 -state disabled
           [$table getContextMenu] entryconfigure 1 -state disabled
        }
     

}
proc createLoadCase {} \
{ 
        variable newlcform
        variable table
        variable tabledata
        variable font
        variable png
        variable method
        variable step
        variable substep
        variable casefile
        variable newid
        variable newproperty
        
        #output
        variable labelstep
        variable entrystep
        variable labelsubstep
        variable entrysubstep
        variable labelfile
        variable entryfile
        variable btnfile


        if {[winfo exists .lcnew]} {
               destroy .lcnew]
        }
    
        set subtitle [msgcat::mc "Create LoadCase "]
        set thestate normal
        #create the top level
        set newlcform [toplevel .lcnew]
        #set title for the window 
        wm title $newlcform $subtitle
        #adjust the location of subwindow window
        set x [expr {([winfo screenwidth .]-320)/2}]
        set y [expr {([winfo screenheight .]-360)/2}]
        wm geometry $newlcform  320x360+$x+$y

        #Components ID and Components name
        set labelMethod [label $newlcform.labelmethod\
          -text "[msgcat::mc "Creation Method "]:        " \
          -font $font(content) \
          -width 10]

        set comboMethod [euidl::combobox $newlcform.combomethod \
          -exportselection 1 \
          -style dropdown \
          -listheight 100 \
          -width 14\
          -font $font(content) \
          -command [code changmethod]\
          -variable [scope method]]

        set labelID [label $newlcform.labelID \
          -text "[msgcat::mc "Case ID "]:        "\
          -font $font(content) \
          -width 10]
        set entryID [euidl::entry $newlcform.entryID\
          -variable [scope newid] \
          -validate focusout\
          -width 16 \
          -font $font(content)\
          -validatedata ansinteger]
        


        #load step
        set labelstep [label $newlcform.labelstep \
          -text "[msgcat::mc "Step ID "]:    "\
          -font $font(content) \
          -width 10]

        set entrystep [entry $newlcform.entrystep\
          -textvariable [scope step] \
          -width 16 \
          -font $font(content)]
        #load substep
        set labelsubstep [label $newlcform.labelsubstep \
          -text "[msgcat::mc "Substep ID "]:"\
          -font $font(content) \
          -width 10]

        set entrysubstep [entry $newlcform.entrysubstep\
          -textvariable [scope substep] \
          -width 16 \
          -font $font(content)]

        set labelfile [label $newlcform.labelfile \
          -text "[msgcat::mc "Please Select The LoadCase File "]:"\
          -state disabled\
          -font $font(content)]

        set framefile [label $newlcform.framefile]
        set entryfile [entry $newlcform.entryfile\
          -textvariable [scope casefile] \
          -state disabled\
          -font $font(content)]

        set btnfile [euidl::button $newlcform.btnfile\
          -state disabled\
          -tooltip [msgcat::mc "Please Select The LoadCase File "] \
          -imagefile $png(import) \
          -command [code selectfile]\
          -font $font(content) ]

        set labelProperty [label $newlcform.labelpro \
          -text "[msgcat::mc "Property "]:        " \
          -font $font(content) \
          -width 10]
        set entryProperty [entry $newlcform.entryId \
          -textvariable [scope newproperty] \
          -width 16 \
          -font $font(content)]

        set frameokcancel [label $newlcform.frameokcancel]
        set btnok [button $newlcform.btnok \
          -text [msgcat::mc "Ok "]\
          -font $font(content) \
          -width 8\
          -command [code ok_new_lc_click]]
        set btnexit [button $newlcform.btnexit\
          -text [msgcat::mc "Exit "]\
          -font $font(content) \
          -width 8\
          -command { destroy .lcnew }]

        ###########create and arrange the control#################
        grid $labelMethod -row 1  -column 0 -sticky w 
        grid $comboMethod -row 1  -column 1 -sticky w -pady 2m
        grid $labelID -row 2  -column 0  -sticky w 
        grid $entryID -row 2  -column 1  -sticky w -pady 2m
        grid $labelProperty -row 3  -column 0  -sticky w 
        grid $entryProperty -row 3  -column 1  -sticky w -pady 2m
        grid $labelProperty -row 4  -column 0 -sticky w  
        grid $entryProperty -row 4  -column 1 -sticky w -pady 2m
        grid $labelstep -row 5  -column 0  -sticky w 
        grid $entrystep -row 5  -column 1  -sticky w  -pady 2m
        grid $labelsubstep -row 6  -column 0  -sticky w 
        grid $entrysubstep -row 6  -column 1  -sticky w -pady 2m

        grid $labelfile -row 7  -column 0  -columnspan 2  -sticky w

        grid $entryfile -row 8  -column 0   -columnspan 2 -sticky w
        grid $btnfile   -row 8  -column 1    -sticky e

        pack $btnok $btnexit -in $frameokcancel -side left -padx 5m 
        grid $frameokcancel  -row 9 -column 0  -columnspan 2 -pady 2m

        $comboMethod insert list end [list \
          [msgcat::mc "From Result File "] Result \
          [msgcat::mc "From LoadCase File "] LoadCase]
        #initialize the value
        set method Result
        set newid {}
        set newproperty {}
        set step 1
        set substep 0
        set casefile {}
        update idletasks
}
proc changmethod {} \
{
    #input
    variable newlcform
    variable font
    variable method
    variable step
    variable substep
    variable casefile
    #output
    variable labelstep
    variable entrystep
    variable labelsubstep
    variable entrysubstep
    variable labelfile
    variable entryfile
    variable btnfile


    if {$method=="Result"} {
      set casefile {}
      set step 1
      set substep 0
      #normal
      $labelstep configure -state normal
      $entrystep configure -state normal
      $labelsubstep configure -state normal
      $entrysubstep configure -state normal
      #disabled
      $labelfile configure -state disabled
      $entryfile configure -state disabled
      $btnfile configure -state disabled
    } else {
      set step {}
      set substep {}
      #disabled
      $labelstep configure -state disabled
      $entrystep configure -state disabled
      $labelsubstep configure -state disabled
      $entrysubstep configure -state disabled
      #normal
      $labelfile configure -state normal
      $entryfile configure -state normal
      $btnfile configure -state normal

    }
  
}
proc ok_new_lc_click {} \
{
    #input variable
    variable method
    variable step
    variable substep
    variable casefile
    variable newid
    variable newproperty
    #tempdata
    variable newlcform
    variable tabledata
    variable table


    if {$newid>99 ||$newid<1} {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -message [msgcat::mc "the Scope Of CaseID is 0-99!"] \
        -icon error 
        return
    }

    #check if the case id is exists
    set msg {}
    set num {}
    set rownum [$table index end row]
    for {set i 0} {$i <= $rownum} {incr i} {
      if {$newid==$tabledata($i,0)} {
        set num $i
        set msg [tk_messageBox -title [msgcat::mc "Tips "] \
          -type yesno\
          -message [msgcat::mc "Really Cover The LoadCase? "] \
          -icon error]
        if {$msg=="no"} { return} 
      }
    }


    
    switch -exact -- $method {
      Result {
        set thestep [expr $step==1?"":$step]
        set thesubstep [expr $substep==0?"":$substep]
        catch {ans_sendcommand "LCDEF,$newid,$thestep,$thesubstep"} err
      }

      default {
        if {![file exists $casefile]} {
            tk_messageBox -title [msgcat::mc "Tips "] \
              -message [msgcat::mc "File not exists,Please check the path! "] \
              -icon error 
              return
            }
        set filelist [split [string trim $casefile] .]
        set prefix [lindex $filelist 0]
        set postfix [lindex $filelist 1]
        catch {ans_sendcommand "LCFILE,$newid,'$prefix','$postfix'"} err
      }
    }

    if {[string trim $newproperty]==""} {
      set newproperty "UnDefined"
    } 
     catch {ans_sendcommand "*set,_lc_pro_$newid,'$newproperty'"} err

    if {$msg=="yes"} {
        set rownum $num
    } else {
        incr rownum
        $table insert row end
    }
    set tabledata($rownum,-1) {}
    set tabledata($rownum,0)  $newid
    #default value is 1
    set tabledata($rownum,1)  1
    #default value is 0/NO
    set tabledata($rownum,2)  0
    set tabledata($rownum,3)  $newproperty
    $table  selection set  $rownum,-1

    #enable the delete button and contextmenu
    [$table getContextMenu] entryconfigure 0 -state normal
    [$table getContextMenu] entryconfigure 1 -state normal
    destroy  $newlcform


}
proc selectfile {} \
{
  variable casefile
  set casefile [tk_getOpenFile  -filetypes {{Loadcase .l*}}]

}

proc execAPDL {} \
{
  variable parent
  variable lclistvar
  # catch {ans_sendcommand "/POST1"} err
  foreach var $lclistvar {
     catch {ans_sendcommand $var} err
  }
   catch {ans_sendcommand "SAVE"} err
  # destroy $parent
}

proc plotLoadCase {arg rst} \
{
  
  variable table
  variable tabledata

  if {$arg=="ALL"} {
    #exec APDL
     execAPDL
  } else {
     set selection [lsort -integer  [$table curselection]]
    # If there is no selection then return
    if { [string match {} $selection] } {
       return
    }
    #if multi select,set the first item
    set sel [lindex $selection 0]
    $table selection clear all
    $table selection set $sel,-1
    set caseid $tabledata($sel,0)
    #read the loadcase
    ans_sendcommand "LCASE,$caseid"
  }
 
  switch -exact -- $rst {
    Usum {
      #??? some error
      puts "PLNSOL,U,Usum,0,1.0 "
    }
    Eqv {
      ans_sendcommand "PLNSOL,S,EQV,0,1.0"
    }
    default {}
  }
}
proc chekcEnvironment {} \
{
    # variable parent

    # array set env_dict {17 PREP7 21 SOLU 31 POST1}
    # 0 = Begin level,  17 = PREP7 21 = SOLUTION, 31 = POST1, 36 = POST26, 
    set env [ans_getvalue "ACTIVE,0,ROUT"]
    if {$env!=31 } {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -message "[msgcat::mc "This Operation Need Environment: "] \n [msgcat::mc "POST1"]" \
        -icon warning 
        return 1
    } 
    return 0
}


}  

#create window
::euidl::Manager::LoadCase::main