package  require msgcat
namespace eval ::euidl::Seismic::TimeHistory {

proc main {} {

  variable parent
  variable tabset
  # variable btnback
  # variable btnnext
  variable png
  variable font
  variable plotshow

  set path [file dirname [info script]]
  ::msgcat::mcload  [file join $path locale seismic]
  set png(check) [file join $path images check24x24.png]
  set png(import) [file join $path images import24x24.png]
  set png(chart) [file join $path images chart24x24.png]
  set font(cn)   {"Microsoft YaHei" 12 normal}
  set font(en)   {Consolas 12 normal}
  set plotshow    [file join $path "AnsysPlugin.exe"]

  if [winfo exists .timehistory] { 
       wm deiconify .timehistory
      raise .timehistory
      return
  }

  set parent [toplevel  .timehistory]

  wm title $parent [msgcat::mc "TimeHistory Analysis "]
  set x [expr {([winfo screenwidth  .]-600)/2}]
  set y [expr {([winfo screenheight .]-360)/2}]
  wm geometry .timehistory 600x360+$x+$y
  wm protocol $parent WM_DELETE_WINDOW {
            if {[winfo exists .exphelp]} {
             destroy .exphelp}
            destroy .timehistory
      }

  # set footer [frame $parent.footer]
  # set btncancel [button $parent.cancel \
  #       -text [::msgcat::mc "Cancel "] \
  #       -width 8\
  #       -font $font(cn)\
  #       -command {destroy .timehistory}]
  # set btnback [button $parent.previous \
  #       -text [::msgcat::mc "Back "] \
  #       -width 8\
  #       -font $font(cn) \
  #       -command [list [code tabSwitch] "Back"]]
  # set btnnext [button $parent.next \
  #       -text [::msgcat::mc "Next "] \
  #       -font $font(cn)\
  #       -width 8\
  #       -command [list [code tabSwitch] "Next"]]

  set tabset [::euidl::tabnotebook $parent.tabset]
  $tabset configure -equaltabs no
  $tabset configure -raiseselect true
  $tabset configure -tabpos n
  $tabset configure -height 3.5i
  $tabset configure -width 6.0i

  grid $tabset -row 1 -column 0 -sticky nsew
  # pack $btnback $btnnext  $btncancel -in $footer -side left -padx 15m
  # grid $footer -row 2 -column 0 -pady 10

  #add tab to the window
  # addTabModal
  # addTabSpectrum
  addTabTimeHistory 
    
  #initialize the data
  ::euidl::Seismic::TimeHistory::initialize

}


proc addTabTimeHistory {} \
{
  variable parent
  variable png
  variable font
  ###############
  variable tabset
  variable parentglo
  variable datafile
  variable datatype
  variable unittype
  variable dataspace
  variable gravity
  variable gxyz
  ####output########
  variable label_dataSpace
  variable entry_dataSpace
  variable label_dataGravity
  variable entry_dataGravity

  set page [$tabset add]
  $tabset pageconfigure 0  \
    -label [msgcat::mc "TimeHistory Analysis "] \
    -font $font(cn) 
    # -command [code changeButtonState]

  set tab0 [frame $page.hold]


  set label_dataType [label $parent.label_dataType \
        -font $font(cn) \
        -text [::msgcat::mc "Input Data Type "] ]

  set combo_dataType [euidl::combobox $parent.combo_dataType \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -command [code changeDataTpye]\
         -variable [scope datatype]]

  set label_unitType [label $parent.label_unitType \
        -font $font(cn) \
        -text [::msgcat::mc "Data Unit Type "] ]

  set combo_unitType [euidl::combobox $parent.combo_unitType \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -command [code changeUnitTpye]\
         -variable [scope unittype]]


  set label_dataSpace [label $parent.label_dataSpace \
        -font $font(cn) \
        -state disabled\
        -text [::msgcat::mc "Input Data Space "] ]

  set entry_dataSpace [euidl::entry  $parent.entry_dataSpace \
        -variable [scope dataspace] \
        -validate focusout\
        -width 15\
        -state disabled\
        -font $font(en)\
        -validatedata real]

  set label_dataGravity [label $parent.label_dataGravity \
        -font $font(cn) \
        -text "[::msgcat::mc "Gravity "]G:" ]

  set entry_dataGravity [euidl::entry  $parent.entry_dataGravity \
        -variable [scope gravity] \
        -validate focusout\
        -width 15\
        -font $font(en)\
        -validatedata real]


  set label_xyzGravity [label $parent.label_xyzGravity \
        -font $font(cn) \
        -text [::msgcat::mc "Load Direction "] ]

  set combo_xyzGravity [euidl::combobox $parent.combo_xyzGravity \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -variable [scope gxyz]]

  set label_importData [euidl::label $parent.label_importData \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Please Select The TimeHistory Data File: "]]

  set btn_importData [euidl::button $parent.btn_importData \
      -command [code dataImport]   \
      -imagefile $png(import) \
      -tooltip [msgcat::mc "Import TimeHistory Data "] ]

  set entry_importData [entry  $parent.entry_importData \
        -width 10  \
        -textvariable [scope datafile] \
        -font $font(cn)]


  set btn_updateTimeChart [euidl::button $parent.btn_updateTimeChart \
      -command [code updateTimeHistoryChart]   \
      -imagefile $png(chart) \
      -tooltip [msgcat::mc "TimeHistory Input Data "] ]

  set btn_solveTime [euidl::button $parent.btn_solveTime \
      -command [code timeSolve]   \
      -imagefile $png(check) \
      -tooltip [msgcat::mc "Solve TimeHistory "] ]


  grid $label_dataType  -in $tab0 -row 0 -column 0  -sticky w
  grid $combo_dataType    -in $tab0 -row 0 -column 1 -sticky w

  grid $label_dataSpace  -in $tab0 -row 0 -column 2  -sticky w
  grid $entry_dataSpace   -in $tab0 -row 0 -column 3 -sticky w

  grid $label_unitType -in $tab0 -row 1 -column 0  -pady 5m -sticky w
  grid $combo_unitType   -in $tab0 -row 1 -column 1 -sticky w

  grid $label_dataGravity  -in $tab0 -row 1 -column 2 -pady 5m -sticky w
  grid $entry_dataGravity   -in $tab0 -row 1 -column 3 -sticky w

  grid $label_xyzGravity  -in $tab0 -row 2 -column 0  -sticky w -sticky w
  grid $combo_xyzGravity   -in $tab0 -row 2 -column 1 -sticky w


  grid $label_importData  -in $tab0 -row 3 -column 0 -columnspan 2  -sticky w
  grid $btn_importData    -in $tab0 -row 3 -column 2  
  grid $btn_updateTimeChart  -in $tab0 -row 3 -column 3  
  grid $entry_importData  -in $tab0 -row 4 -column 0  -columnspan 4 -sticky nsew
  grid $btn_solveTime  -in $tab0 -row 5 -column 0  -columnspan 4 -sticky nsew -pady 5m


  for {set i 0} {$i < 4} {incr i} {
    grid columnconfigure $tab0 $i -weight 1
  }

  pack $tab0 -side top -anchor nw -fill both -expand 1 -padx 5 -pady 5

  $combo_dataType insert list end [list\
   [msgcat::mc "Time-Acceleration "] "T-G" \
   [msgcat::mc "Space-Acceleration " ] "S-G"]

  $combo_unitType insert list end [list\
   [msgcat::mc "g/s2 "]  "GS2" \
   [msgcat::mc "m/s2 " ] "MS2"]  

  $combo_xyzGravity insert list end [list X  X Y Y Z Z]  
}
proc changeDataTpye {} \
{
  variable label_dataSpace
  variable entry_dataSpace
  variable datatype
  if {$datatype=="S-G"} {
    $label_dataSpace  configure -state normal
    $entry_dataSpace   configure -state normal
  } else {
    $label_dataSpace  configure -state disabled
    $entry_dataSpace   configure -state disabled    
  }

}
proc changeUnitTpye {} \
{
  variable label_dataGravity
  variable entry_dataGravity
  variable unittype
  variable gravity

  if {$unittype=="GS2"} {
    $label_dataGravity  configure -state normal
    $entry_dataGravity   configure -state normal
    set gravity 9.8  
  } else {
    $label_dataGravity  configure -state disabled
    $entry_dataGravity   configure -state disabled
    set gravity 1  
  }
  
}
#switch the tab item
proc tabSwitch {arg} \
{

  variable tabset

  set index [$tabset view]
  switch -exact -- $arg {
    Back {
      incr index -1
      $tabset view $index
    }
    Next {
      incr index
      $tabset view $index    
    }
  }
  changeButtonState
}

#change Back\Next Button State
proc changeButtonState {} \
{
    variable tabset
    variable btnback
    variable btnnext
    set pageindex [$tabset view]
    set pageend [$tabset index end]
    $btnnext configure -state [expr $pageindex==$pageend?"disabled":"normal"]
    $btnback configure -state [expr $pageindex==0?"disabled":"normal"]
}

#initialize the form
proc initialize {} \
{
  variable tabset
  # variable btnback
  variable gravity
  variable gxyz
  variable datafile
  variable datatype
  variable dataspace
  variable unittype

  set datafile {}
  set datatype "T-G"
  set gravity 9.8
  set dataspace 0.05
  set unittype "GS2"
  set gxyz Z
  #set the first tab as the default
  $tabset view 0
  # $btnback configure -state "disabled"


}


proc dataImport {} \
{

    variable datafile
    variable datatype
    variable dataspace
    variable parent
    variable Ts
    variable Gs
    variable num

    set filename [tk_getOpenFile \
      -parent $parent\
      -initialfile *.txt \
      -filetypes {{Txt .txt}}]

    if {![file exists $filename] || [string trim $filename]==""} {
      return
    }
    set datafile $filename
    set fl [open $datafile r]
    set data [read $fl]
    #close data file
    close $fl
    set Ts {}
    set Gs {}
    set num 0
    if {$datatype=="S-G"} {
      foreach g $data {
        lappend Ts [expr $num*$dataspace]
        lappend Gs $g
        incr num
      }
    } else {
      foreach {t g} $data {
        lappend Ts $t
        lappend Gs $g
        incr num
      }
    }
    # updateTimeHistoryChart
}


proc updateTimeHistoryChart {} \
{
  variable parent
  variable Gs
  variable Ts
  variable plotshow
  variable datafile
  variable num


  if {![file exists $datafile] || $num==0 } {
    tk_messageBox  \
      -parent $parent \
      -icon warning \
      -title [::msgcat::mc "Tips "] \
      -message  [::msgcat::mc "Can Not Get TimeHistory Data! "]
    return
  }

  set title  [::msgcat::mc "TimeHistory Input Data "]
  set xlabel [::msgcat::mc "Time(s) "]
  set ylabel "[::msgcat::mc "Acceleration "]g/s^2"
  set geometry    [wm geometry $parent]


  set data  "\"<root  title='$title' geometry='$geometry' xlabel='$xlabel' ylabel='$ylabel'>
            <graphs>
            <graph id='1'>
            <xlist>$Ts</xlist>
            <ylist>$Gs</ylist>
            <style legend='$title' line='black blue 0.2' point='blue red 1' />
            </graph>
            </graphs>
            </root>\""

  exec $plotshow $data
  raise $parent
}



proc timeSolve {} \
{

  variable parent
  variable datafile
  variable gravity
  variable gxyz
  variable Ts
  variable Gs
  variable num 


  if {![file exists $datafile] || $num==0} {
    tk_messageBox  \
      -parent $parent \
      -icon warning \
      -title [::msgcat::mc "Tips "] \
      -message  [::msgcat::mc "Can Not Get TimeHistory Data! "]
      return      
  }
  if {[checkEnvironment]} {
    return
  }
  catch {ans_sendcommand "/UIS,MSGPOP,3"} err
  #create the background line
    for {set i 0} {$i < $num} {incr i} {
      set t    [lindex $Ts $i]
      set g    [lindex $Gs $i]
      switch -exact -- $gxyz {
        X { catch {ans_sendcommand "ACEL,$g*$gravity,0,0"} err}
        Y { catch {ans_sendcommand "ACEL,0,$g*$gravity,0"} err}
        Z { catch {ans_sendcommand "ACEL,0,0,$g*$gravity"} err}
      }
      # catch {ans_sendcommand "ACEL,$g*$gravity,0,0"}
      catch {ans_sendcommand "TIME,$t"} err
      catch {ans_sendcommand "OUTERS,ALL,ALL"} err
      catch {ans_sendcommand "SOLVE"} err

  }  

  #logging
  puts "#########TimeHistory Analysis APDL Start ############"
  puts "*DO,i,1,$num,1"
  switch -exact -- $gxyz {
    X {puts "ACEL,g,0,0"}
    Y {puts "ACEL,0,g,0"}
    Z {puts "ACEL,0,0,g"}
  }
  puts  "TIME,time"
  puts  "*ENDDO"
  puts  "OUTERS,ALL,ALL"
  puts  "SOLVE"
  puts "#########TimeHistory Analysis APDL Finish ############"

  # #Enable  error message pop-up dialog box
  catch {ans_sendcommand "/UIS,MSGPOP,DEFA"} err
  
  # catch {ans_sendcommand "/FINISH"} err
  if [winfo exists .timehistory] {
      raise .timehistory
      return
  }
  
}
proc checkEnvironment {} \
{
    variable parent
    # array set env_dict {17 PREP7 21 SOLU 31 POST1}
    # 0 = Begin level,  17 = PREP7 21 = SOLUTION, 31 = POST1, 36 = POST26, 
    set env [ans_getvalue "ACTIVE,0,ROUT"]
    if {$env!=21} {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -parent $parent\
        -message "[msgcat::mc "This Operation Need Environment: "] \n [msgcat::mc "SOLU"]" \
        -icon warning 
        return 1
    } 
    return 0
}

}

::euidl::Seismic::TimeHistory::main