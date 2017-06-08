package require msgcat

namespace eval ::Customized::Manager::Component {} {

proc main {} {

      variable table
      variable tabledata
      variable cmplistbox
      variable cmplistvar
      variable parent
      variable font
      variable png

      set path [file dirname [info script]]
      ::msgcat::mcload  [file join $path locale]
      set font(content)  {"Microsoft YaHei" 11 normal}
      set font(title)  {"Microsoft YaHei" 12 normal}

      set png(node) [file join $path images node24x24.png]
      set png(element) [file join $path images element24x24.png]
      set png(create) [file join $path images new24x24.png]
      set png(delete) [file join $path images delete24x24.png]
      set png(display) [file join $path images display24x24.png]
      set png(plot) [file join $path images plot24x24.png]
      set png(info) [file join $path images info24x24.png]

      if {[winfo exists .componentmanager]} {
             wm deiconify .componentmanager
             raise .componentmanager
             return
      }
      set parent [toplevel .componentmanager]
      wm title $parent [msgcat::mc "Component Manager "]
      set x [expr {([winfo screenwidth  .]-700)/2}]
      set y [expr {([winfo screenheight .]-450)/2}]
      wm geometry $parent  700x450+$x+$y
      #delete all the window
      wm protocol $parent WM_DELETE_WINDOW {
          if {[winfo exists .cmpapdl]} {
            destroy .cmpapdl
          }
          if {[winfo exists .cmpinfo]} {
            destroy .cmpinfo
          }
          if {[winfo exists .cmpnew]} {
            destroy .cmpnew
          }
          destroy .componentmanager
      }


      #Left frame
      set left [euidl::labeledframe $parent.left -font $font(content)  -text [msgcat::mc "Defined Components "]]
      grid $left -row 1 -column 0  -sticky news
      set left [$left childsite]

      #Header frame
      set header [euidl::labeledframe $parent.bottom -font $font(content) -text [msgcat::mc "Toolbar "]]

      #Right frame
      set right [euidl::labeledframe $parent.right -font $font(content) -text [msgcat::mc "Selected Components "]]
      grid $right -row 1 -column 1  -sticky news
      set right [$right childsite]

      #footer frame
      set footer [frame $parent.footer]
      grid $footer  -row 2 -column 0  -columnspan 2 


      grid  columnconfigure $parent 0 -weight 2
      # grid  columnconfigure $parent 1 -weight 1
      grid  columnconfigure $parent 1 -weight 1
      grid  rowconfigure $parent 1 -weight 1
      grid  rowconfigure $parent 2 -weight 1

      #footer button
      # set btncreate [button $parent.create -text [msgcat::mc "Create "]  -width 8 -font $font(content) -command [code createComponent] ]
      set btnok [button $parent.ok -text [msgcat::mc "Ok "]  -width 8 -font $font(content) -command [code execAPDL] ]
      set btncancel [button $parent.cancel -text [msgcat::mc "Cancel "] -width 8 -font $font(content) -command {destroy .componentmanager} ]
      # pack $btncreate $btnok  $btncancel -in $footer -side left -padx 10m -pady 1m
      pack  $btnok  $btncancel -in $footer -side left -padx 15m -pady 4m

      #Tree for the left side
      set table [euidl::colhead $left.table \
      -variable [scope tabledata] \
      -selectbackground #09f\
      -borderwidth 0 \
      -hscrollmode none \
      -contextmenu 1 \
      ]


      #insert head
      $table insert cols {Name} end
      $table insert cols {Type} end
      $table insert cols {Property} end


      #sortmethod 
      $table tag configure {Name} -sortmethod dictionary
      $table tag configure {Type} -sortmethod dictionary
      $table tag configure {Property} -sortmethod dictionary


      $table tag configure title -anchor center
      $table tag configure title -font $font(title)
      $table tag configure row -font $font(content)
      $table tag configure row -anchor center
     

      #cofigure the width of the header
      [$table component table] width [$table tag coltag {Name}] 30
      [$table component table] width [$table tag coltag {Type}] 15
      [$table component table] width [$table tag coltag {Property}] 30
      #the rows height
      [$table component table] configure -rowheight 2
      bind $table <Double-1> [code showComponent]
      # puts [[$table component table] configure]
      #pack the table
      pack $table -fill both -expand 1
      #ContextMenu
      $table deleteContextMenu 0 end 
      set cmindex 0
      $table insertContextMenu $cmindex command \
      -label [msgcat::mc "Sel "] \
      -font $font(content) \
      -underline 0 \
      -command [list [code handleComponent] "S"]
      incr cmindex
      $table insertContextMenu $cmindex command \
      -label [msgcat::mc "UnSel "] \
      -font $font(content) \
      -underline 0 \
      -command [list [code handleComponent] "U"]
      incr cmindex
      $table insertContextMenu $cmindex command \
      -label [msgcat::mc "ReSel "] \
      -font $font(content) \
      -underline 0 \
      -command [list [code handleComponent] "R"]
      incr cmindex
      $table insertContextMenu $cmindex command \
      -label [msgcat::mc "AlsoSel "] \
      -font $font(content) \
      -underline 0 \
      -command [list [code handleComponent] "A"]
      incr cmindex
      $table insertContextMenu $cmindex command \
            -label [::msgcat::mc "Display Component "] \
            -font $font(content) \
            -command [ code Display]
      incr cmindex
      $table insertContextMenu $cmindex command \
            -label [::msgcat::mc "Plot Component InContext "] \
            -font $font(content) \
            -command [ code PlotInContext]
      incr cmindex
      $table insertContextMenu $cmindex command \
      -label [msgcat::mc "Delete Component "] \
      -font $font(content) \
      -command [namespace code delComponent]


      #Tree for the right side
     set cmplistbox [euidl::scrolledlistbox $right.cmplistbox \
          -listvariable [scope cmplistvar]\
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
      $cmplistbox deleteContextMenu 0 end
      set cmindex 0
      $cmplistbox insertContextMenu $cmindex command \
            -label [msgcat::mc "MoveUp "] \
            -underline 0 \
            -state disabled\
            -font $font(content) \
            -command [list [namespace code cmplistMove] "up"]
      incr cmindex
      $cmplistbox insertContextMenu $cmindex command \
            -label [msgcat::mc "MoveDown "] \
            -underline 0 \
            -state disabled\
            -font $font(content) \
            -command [list [namespace code cmplistMove] "down"]
      incr cmindex
      $cmplistbox insertContextMenu $cmindex command \
            -label [msgcat::mc "Del "] \
            -underline 0 \
            -state disabled\
            -font $font(content) \
            -command [namespace code delAPDL]
      
      grid $cmplistbox -sticky news
      grid  columnconfigure $right 0 -weight 1
      grid  columnconfigure $left 0 -weight 1
      grid  rowconfigure $right 0 -weight 1
      grid  rowconfigure $left 0 -weight 1
  ###header frame icon
      set btn_node [euidl::button $parent.btn_node \
          -command [namespace code ShowNodesFilter] \
          -imagefile $png(node) \
          -tooltip [msgcat::mc "Nodes Filter "]]

      set btn_element [euidl::button $parent.btn_element \
          -command [namespace code ShowElementsFilter] \
          -imagefile $png(element) \
          -tooltip [msgcat::mc "Elements Filter "] ]

      set btn_create [euidl::button $parent.btn_create \
          -command [code createComponent] \
          -imagefile $png(create) \
          -tooltip [msgcat::mc "Create Component "] ]

      set btn_delete [euidl::button $parent.btn_delete \
          -command [code delComponent]\
          -imagefile $png(delete) \
          -tooltip [msgcat::mc "Delete Component "] ]

      set btn_display [euidl::button $parent.btn_display \
          -command [code Display]\
          -imagefile $png(display) \
          -tooltip [msgcat::mc "Display Component "] ]

      set btn_plot [euidl::button $parent.btn_plot \
          -command [code PlotInContext]\
          -imagefile $png(plot) \
          -tooltip [msgcat::mc "Plot Component InContext "] ]

      set btn_list [euidl::button $parent.btn_list \
          -command [code ListWithEntities]\
          -imagefile $png(info) \
          -tooltip [msgcat::mc "Show Component Detail "] ]

      grid $header -row 0 -column 0 -sticky news -columnspan 2
      set header [$header childsite]
      pack $btn_create $btn_delete $btn_node $btn_element\
      $btn_display $btn_plot $btn_list\
      -in $header -side left -padx 1m
      #################################### 
     update idletasks

    #initialize the value of material data
    ::Customized::Manager::Component::initialize

  }

proc ShowNodesFilter {} \
{
    variable font
    variable  nodeseltype
    variable  nodeselmethod
    variable  nodeselitem
    variable  minnodeloc
    variable  maxnodeloc
    #output controls
    variable label_nodeselitems
    variable combo_nodeselitems
    variable label_location
    variable entry_minlocation
    variable entry_maxlocation


    if {[winfo exists .nodefilter]} {
        destroy .nodefilter
    }
    set nodefilter [toplevel .nodefilter]
    wm title $nodefilter [msgcat::mc "Nodes Filter "]
    set x [expr {([winfo screenwidth  .]-300)/2}]
    set y [expr {([winfo screenheight .]-280)/2}]
    wm geometry $nodefilter  300x280+$x+$y

  set label_seltype  [label $nodefilter.label_seltype \
          -font $font(content) \
          -text [::msgcat::mc "Select Type "] ]

  set combo_seltype  [euidl::combobox $nodefilter.combo_seltype  \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 12\
         -font $font(content) \
         -variable [scope nodeseltype ] ]  

  set label_selmethod  [label $nodefilter.label_selmethod \
          -font $font(content) \
          -text [::msgcat::mc "Select Method "] ]

  set combo_selmethod  [euidl::combobox $nodefilter.combo_selmethod  \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 12\
         -command [code changeNodeMethod]\
         -font $font(content) \
         -variable [scope nodeselmethod] ]

  set label_nodeselitems  [label $nodefilter.label_nodeselitems \
          -font $font(content) \
          -text [::msgcat::mc "Select Location "] ]

  set combo_nodeselitems  [euidl::combobox $nodefilter.combo_nodeselitems  \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 12\
         -font $font(content) \
         -variable [scope nodeselitem] ]

  set label_location  [label $nodefilter.label_location \
          -font $font(content) \
          -text [::msgcat::mc "Location Scope "] ]

  set entry_minlocation [entry  $nodefilter.entry_minlocation  \
          -textvariable [scope minnodeloc] \
          -width 6\
          -font $font(content)]

  set entry_maxlocation [entry  $nodefilter.entry_maxlocation  \
          -textvariable [scope maxnodeloc] \
          -width 6\
          -font $font(content)]

  set btn_ok  [button $nodefilter.btnok \
          -font $font(content) \
          -width 8\
          -command [code ok_nodefilter_click]\
          -text [::msgcat::mc "Ok "] ]

  set btn_cancel [button $nodefilter.btncancel \
          -font $font(content) \
          -width 8\
          -command {destroy .nodefilter}\
          -text [::msgcat::mc "Cancel "] ]

  grid $label_selmethod -row 0 -column 0 -sticky w -padx 2m -pady 2m
  grid $combo_selmethod -row 0 -column 1

  grid $label_nodeselitems -row 1 -column 0 -sticky w -padx 2m -pady 2m
  grid $combo_nodeselitems -row 1 -column 1

  grid $label_location -row 2 -column 0 -sticky w -padx 2m -pady 2m
  grid $entry_minlocation -row 2 -column 1 -sticky w 
  grid $entry_maxlocation -row 2 -column 1 -sticky e

  grid $label_seltype -row 3 -column 0 -sticky w -padx 2m -pady 2m
  grid $combo_seltype -row 3 -column 1 

  grid $btn_ok -row 4 -column 0 -pady 4m
  grid $btn_cancel -row 4 -column 1

  $combo_selmethod insert list end [list [msgcat::mc "By Location "] "LOC" [msgcat::mc "Attached To "]  "1"]
  $combo_nodeselitems insert list end [list "X" "X" "Y" "Y" "Z" "Z"]
  $combo_seltype insert list end [list\
    [msgcat::mc "Sel "] S\
    [msgcat::mc "ReSel "] R\
    [msgcat::mc "AlsoSel "] A \
    [msgcat::mc "UnSel "] U]
  #initialize
  set nodeselmethod "LOC"
  set nodeselitem   "X"
  set nodeseltype   "R"
}

proc changeNodeMethod {} \
{
  variable nodeselmethod
  variable nodeselitem
  ###input control
  variable label_nodeselitems
  variable combo_nodeselitems
  variable label_location
  variable entry_minlocation
  variable entry_maxlocation

  $combo_nodeselitems delete list 0 end
  switch -exact -- $nodeselmethod {
    LOC {
        grid $label_location  $entry_minlocation $entry_maxlocation
        $label_nodeselitems configure -text [msgcat::mc  "Select Location "]
        $combo_nodeselitems insert list end [list "X" "X" "Y" "Y" "Z" "Z"]
        set nodeselitem "X"
    }
    1  {
        grid remove $label_location  $entry_minlocation $entry_maxlocation
        $label_nodeselitems configure -text [msgcat::mc  "Attached To "]
        $combo_nodeselitems insert list end [list "Elements" "E" "Areas" "A" "Lines" "L" "Volumes" "V"]
        set nodeselitem "A"
    }
  }
  
}
proc ok_nodefilter_click {} \
{
    variable cmplistbox
    variable nodeselmethod
    variable nodeselitem
    variable nodeseltype
    variable minnodeloc
    variable maxnodeloc
    
    switch -exact -- $nodeselmethod {
    LOC {
      set command "NSEL,$nodeseltype,LOC,$nodeselitem,$minnodeloc,$maxnodeloc"
    }
    1 {
      set command "NSL$nodeselitem,$nodeseltype,1"
    }
  }
  $cmplistbox insert end [list $command $command]
  if { [$cmplistbox size]>0} {
      for {set i 0} {$i < 3} {incr i} {
        [$cmplistbox getContextMenu] entryconfigure $i -state normal
      }
  } 
}
proc ShowElementsFilter {} \
{
    variable font 
    variable  elementseltype
    variable  elementselmethod
    variable  elementselitem
    #output controls
    variable label_elementselitems
    variable combo_elementselitems


    if {[winfo exists .elementfilter]} {
             destroy .elementfilter
    }
    set elementfilter [toplevel .elementfilter]
    wm title $elementfilter [msgcat::mc "Elements Filter "]
    set x [expr {([winfo screenwidth  .]-300)/2}]
    set y [expr {([winfo screenheight .]-280)/2}]
    wm geometry $elementfilter  300x280+$x+$y

  set label_seltype  [label $elementfilter.label_seltype \
          -font $font(content) \
          -text [::msgcat::mc "Select Type "] ]

  set combo_seltype  [euidl::combobox $elementfilter.combo_seltype  \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 12\
         -font $font(content) \
         -variable [scope elementseltype ] ]  

  set label_eleselmethod  [label $elementfilter.label_eleselmethod \
          -font $font(content) \
          -text [::msgcat::mc "Select Method "] ]

  set combo_eleselmethod  [euidl::combobox $elementfilter.combo_eleselmethod  \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 12\
         -command [code changeElementMethod]\
         -font $font(content) \
         -variable [scope elementselmethod] ]

  set label_elementselitems  [label $elementfilter.label_elementselitems \
          -font $font(content) \
          -text [::msgcat::mc "Selected Category "] ]

  set combo_elementselitems  [euidl::combobox $elementfilter.combo_elementselitems  \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 12\
         -font $font(content) \
         -variable [scope elementselitem] ]


  set btn_ok  [button $elementfilter.btnok \
          -font $font(content) \
          -width 8\
          -command [code ok_elementfilter_click]\
          -text [::msgcat::mc "Ok "] ]

  set btn_cancel [button $elementfilter.btncancel \
          -font $font(content) \
          -width 8\
          -command {destroy .elementfilter}\
          -text [::msgcat::mc "Cancel "] ]

  grid $label_eleselmethod -row 0 -column 0 -sticky w -padx 2m -pady 6m
  grid $combo_eleselmethod -row 0 -column 1

  grid $label_elementselitems -row 1 -column 0 -sticky w -padx 2m -pady 4m
  grid $combo_elementselitems -row 1 -column 1


  grid $label_seltype -row 3 -column 0 -sticky w -padx 2m -pady 4m
  grid $combo_seltype -row 3 -column 1 

  grid $btn_ok -row 4 -column 0 -pady 10m
  grid $btn_cancel -row 4 -column 1

  $combo_eleselmethod insert list end [list \
  [::msgcat::mc "Mat "] "Mat" \
  [::msgcat::mc "Type "]  "Type"\
  [::msgcat::mc "Real "] "Real"]
  $combo_elementselitems insert list end [GetAnsysData MAT] 
  $combo_seltype insert list end [list\
    [msgcat::mc "Sel "] S\
    [msgcat::mc "ReSel "] R\
    [msgcat::mc "AlsoSel "] A \
    [msgcat::mc "UnSel "] U]
  #initialize
  set elementselmethod "Mat"
  set elementseltype   "R"
}

proc changeElementMethod {} \
{
  
  variable elementselmethod
  ###input control
  variable label_elementselitems
  variable combo_elementselitems
  variable elementselitem


  $combo_elementselitems delete list 0 end
  $label_elementselitems configure -text "[::msgcat::mc "$elementselmethod "]:"
  set listdata [GetAnsysData $elementselmethod]
  $combo_elementselitems insert list end $listdata
  set elementselitem [lindex [lindex $listdata 0] 0]
  
}
proc ok_elementfilter_click {} \
{
    variable cmplistbox
    variable elementselmethod
    variable elementselitem
    variable elementseltype
    
    if {[string trim $elementselitem]==""} {
      tk_messageBox  \
          -parent .elementfilter \
          -icon warning \
          -title [::msgcat::mc "Tips "] \
          -message  "[::msgcat::mc "$elementselmethod "] [::msgcat::mc "UnDefined!"]"
    } else {
      set command "ESEL,$elementseltype,$elementselmethod,,$elementselitem"
      $cmplistbox insert end [list $command $command]
      if { [$cmplistbox size]>0} {
        for {set i 0} {$i < 3} {incr i} {
        [$cmplistbox getContextMenu] entryconfigure $i -state normal
        }
      } 
    }

}
proc GetAnsysData {category} \
{

set datalist {}
foreach {name id} [ans_getlist $category] {
  if {[string match "None defined" $name]} {
      continue
  }
  set id [string trim $id]
  lappend datalist $id $id
}
#append empty key and value
lappend datalist "" ""
return $datalist
}

#initialize the table data
proc initialize {} \
{ 
    variable table
    variable tabledata
    variable cmplistvar
    variable cmpcategory


    set cmplistvar {}
    set cmpcategory(1)  "Nodes"
    set cmpcategory(2)  "Elements"
    set cmpcategory(6)   "KeyPoints"
    set cmpcategory(7)   "Lines"
    set cmpcategory(8)   "Areas"
    set cmpcategory(9)   "Volumes"
    set cmpcategory(11)   "Subcomponents L1"
    set cmpcategory(12)   "Subcomponents L2"
    set cmpcategory(13)   "Subcomponents L3"
    set cmpcategory(14)   "Subcomponents L4"
    set cmpcategory(15)   "Subcomponents L5"

    set num [ans_getvalue "COMP,0,NCOMP"]
    for {set i 0} {$i < $num} {incr i} {
      set id [expr $i+1]
      set name [string trim [ans_getvalue "COMP,$id,NAME"]]
      #do not show system component
      if {[string index $name 0]=="_"} {
        continue
      }
      set type [string trim [ans_getvalue "COMP,$name,TYPE"]]
      set index  "_property_$name"
      set flag [ans_getvalue parm,$index,type]
      if {$flag!=-1} {
        set property [string trim [ans_getvalue parm,$index,value]]
      } else {
        set property "UnDefined"
      }

      $table insert row end
      set tabledata($i,-1) {}
      set tabledata($i,0)  $name
      set tabledata($i,1)  $cmpcategory($type)
      set tabledata($i,2)  $property
    }
  #disabled the delete button and contextmenu
  if { [$table index end row]==-1} {
      for {set i 0} {$i < 7} {incr i} {
        [$table getContextMenu] entryconfigure $i -state disabled
      }
    }

}

  #delete table row,delete the selected material
  proc handleComponent {operator} \
  {
    variable table
    variable tabledata
    variable cmplistbox
    set selection [lsort -integer  [$table curselection]]

      # If there is no selection then return
      if { [string match {} $selection] } {
         return
      }

      foreach sel $selection {
        set cmpname $tabledata($sel,0)
        set command "CMSEL,$operator,$cmpname"
        $cmplistbox insert end [list $command $command]
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
      if { [$cmplistbox size]>0} {
          for {set i 0} {$i < 3} {incr i} {
            [$cmplistbox getContextMenu] entryconfigure $i -state normal
          }
      } 
}
#show the local material information
  proc showComponent {} \
{
        variable table
        variable tabledata
        variable font
        variable cmpname
        variable cmptype
        variable cmpproperty
        variable subform
        variable png

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

        set cmpname $tabledata($sel,0)
        set cmptype $tabledata($sel,1)
        set cmpproperty $tabledata($sel,2)

        set subtitle "[msgcat::mc "Components "]:$cmpname"
        set thestate normal
        #create the top level
        set subform [toplevel .cmpinfo]
        grab set $subform
        #set title for the window 
        wm title .cmpinfo $subtitle
        #adjust the location of subwindow window
        set x [expr {([winfo screenwidth .]-300)/2}]
        set y [expr {([winfo screenheight .]-250)/2}]
        wm geometry $subform  300x250+$x+$y
        wm transient $subform

        set btn_infolist [euidl::button $subform.btn_infolist \
          -command [code ListWithEntities]   \
          -imagefile $png(info) \
          -width 250\
          -tooltip [msgcat::mc "Show Component Detail "] ]
        #Components ID and Components name
        set labelName [label $subform.labelName \
          -text [msgcat::mc "ComponentName "]\
          -font $font(content) \
          -width 8]
        set entryName [entry $subform.entryName\
          -textvariable [scope cmpname] \
          -width 10 \
          -font $font(content) \
          -width 15]
        set labelType [label $subform.labelType\
          -text [msgcat::mc "ComponentType "] \
          -font $font(content) \
          -width 8]
        set entryType [entry $subform.entryType \
          -textvariable [scope cmptype] \
          -width 10 -font $font(content)\
          -state disabled\
          -width 15 ]
        set labelProperty [label $subform.labelId \
          -text [msgcat::mc "ComponentProperty "] \
          -font $font(content) \
          -width 8]
        set entryProperty [entry $subform.entryId \
          -textvariable [scope cmpproperty] \
          -width 10 \
          -font $font(content) \
          -width 15]
        set frameokcancel [label $subform.frameokcancel]
        set btnok [button $subform.btnok \
          -text [msgcat::mc "Ok "]\
          -font $font(content) \
          -width 8\
          -command [namespace code ok_info_click]]
        set btnexit [button $subform.btnexit\
          -text [msgcat::mc "Exit "]\
          -font $font(content) \
          -width 8\
          -command { destroy .cmpinfo }]

        ###########create and arrange the control#################
        

        grid $labelName -row 0  -column 0  -sticky w 
        grid $entryName -row 0  -column 1  -padx 6m -pady 2m
        grid $labelType -row 1  -column 0 -sticky w 
        grid $entryType -row 1   -column 1 -padx 6m  -pady 2m
        grid $labelProperty -row 2  -column 0 -sticky w 
        grid $entryProperty -row 2  -column 1 -padx 6m  -pady 2m

        # grid $btn_infolist  -row 3  -column 0  -columnspan 2

        pack $btnok $btnexit -in $frameokcancel -side left -padx 5m 
        grid $frameokcancel  -row 4 -column 0  -columnspan 2 -pady 4m
       
        update idletasks
  }
#move up or down
proc cmplistMove {direction} \
{  
  variable cmplistvar
  variable cmplistbox

  set len [$cmplistbox size]
  set sel [lindex [$cmplistbox curselection] 0]
      
  if {$direction=="up"} {
     set nextsel [expr $sel==0?$len-1:$sel-1] 
  } else {
     set nextsel [expr $sel==$len-1?0:$sel+1]
  }
  set data [$cmplistbox get $sel]
  set nextdata [$cmplistbox get $nextsel]
  lset cmplistvar $nextsel $data
  lset cmplistvar $sel $nextdata
  $cmplistbox selection clear 0 end
  $cmplistbox selection set  $nextsel
  $cmplistbox activate  $nextsel


}
  proc editAPDL {} \
{
      
        variable font
        variable cmplistbox
        variable cmpcategory
        variable selname
        variable seltype
        variable selmethod
        variable selproperty
        variable apdlform
        variable ts


        if {[winfo exists .cmpapdl]} {
               destroy .cmpapdl]
        }
        set selection [lsort -integer  [$cmplistbox curselection]]
        # If there is no selection then return
        if { [string match {} $selection] } {
           return
        }
        # #if multi select,set the first item
        set sel [lindex $selection 0]
        $cmplistbox selection clear 0 end
        $cmplistbox selection set $sel

        set item [$cmplistbox get $sel]
        set apdl [split $item ,]
        ##########################################
        set categories {NSEL ESEL NSLA NSLE NSLL}
        set selcategory [string trim [lindex $apdl 0]]


        if {[lsearch $categories $selcategory]>=0} {
            puts [lsearch $categories $selcategory]
            return
        }
        set selmethod [string trim [lindex $apdl 1]]
        set selname [string trim [lindex $apdl 2]]
        set seltype $cmpcategory([ans_getvalue "COMP,$selname,TYPE"])


        set index  "_property_$selname"
        set flag [ans_getvalue parm,$index,type]
        if {$flag!=-1} {
          set selproperty [string trim [ans_getvalue parm,$index,value]]
        } else {
          set selproperty "UnDefined"
        }

        set subtitle "[msgcat::mc "Components "]:$selname"
        set sublabeltips  ""
        set thestate normal
        #create the top level
        set apdlform [toplevel .cmpapdl]
        grab set $apdlform
        #set title for the window 
        wm title .cmpapdl $subtitle
        #adjust the location of subwindow window
        set x [expr {([winfo screenwidth .]-300)/2}]
        set y [expr {([winfo screenheight .]-250)/2}]
        wm geometry $apdlform  300x250+$x+$y
        wm transient $apdlform

        set labelTips       [label $apdlform.labelTips -text $sublabeltips -font $font(content)]
        #Components ID and Components name
        set labelName [label $apdlform.labelName \
          -text [msgcat::mc "ComponentName "]\
          -font $font(content) \
          -width 8]
        set entryName [entry $apdlform.entryName\
          -textvariable [scope selname] \
          -state disabled\
          -width 10 \
          -font $font(content) \
          -width 15]

        set labelType [label $apdlform.labelType \
          -text [msgcat::mc "ComponentType "]\
          -font $font(content) \
          -width 8]
        set entryType [entry $apdlform.entryType\
          -textvariable [scope seltype] \
          -state disabled\
          -width 10 \
          -font $font(content) \
          -width 15]

        set labelSelMehod [label $apdlform.labelSelMehod\
          -text [msgcat::mc "Select Type "] \
          -font $font(content) \
          -width 8]
        set comboSelMehod [euidl::combobox $apdlform.comboSelMehod \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(content) \
         -variable [scope selmethod]]

        set labelProperty [label $apdlform.labelId \
          -text [msgcat::mc "ComponentProperty "] \
          -font $font(content) \
          -width 8]
        set entryProperty [entry $apdlform.entryId \
          -textvariable [scope selproperty] \
          -width 10 \
          -state disabled\
          -font $font(content) \
          -width 15]
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
          -command { destroy .cmpapdl }]

        ###########create and arrange the control#################

        grid $labelName -row 0  -column 0  -sticky w 
        grid $entryName -row 0  -column 1  -padx 6m -pady 2m
        grid $labelType -row 1  -column 0  -sticky w 
        grid $entryType -row 1  -column 1  -padx 6m -pady 2m        
        grid $labelProperty -row 2  -column 0 -sticky w 
        grid $entryProperty -row 2  -column 1 -padx 6m  -pady 2m
        grid $labelSelMehod -row 3  -column 0 -sticky w 
        grid $comboSelMehod -row 3   -column 1 -padx 6m  -pady 2m
        pack $btnok $btnexit -in $frameokcancel -side left -padx 5m 
        grid $frameokcancel  -row 4 -column 0  -columnspan 2 -pady 4m

        $comboSelMehod insert list end [list\
            [msgcat::mc "Sel "] S\
            [msgcat::mc "ReSel "] R\
            [msgcat::mc "AlsoSel "] A \
            [msgcat::mc "UnSel "] U]
        update idletasks
  }
  #delete the selected apdl
  proc delAPDL {} \
  {
    variable cmplistbox

    set selection [lsort -integer -decreasing [$cmplistbox  curselection]]
    foreach sel $selection {
      $cmplistbox delete $sel
    }

    set thestate [expr [$cmplistbox size]==0?"disabled":"normal" ]
    for {set i 0} {$i < 3} {incr i} {
      [$cmplistbox getContextMenu] entryconfigure $i -state $thestate
    }

  }


proc ok_info_click {} \
{
  variable subform
  variable table
  variable tabledata
  variable cmpname
  variable cmptype
  variable cmpproperty

  set name_reg [regexp -all -inline -- {^[A-Za-z][A-Za-z0-9_]+$}  $cmpname]
  if {$name_reg!=$cmpname || [string trim $name_reg]==""} {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -message [msgcat::mc "Wrong Component Name Format! "] \
        -icon error 
    return
  }
  set sel [$table curselection]
  #translate into upper case
  set cmpname [string toupper $cmpname]
  set previousname $tabledata($sel,0)
  set previousproperty $tabledata($sel,2)
  if {$previousname!=$cmpname} {
    #save the current node component to recover
    catch {ans_sendcommand )CM,_temp_node,NODE} err
    catch {ans_sendcommand )CM,_temp_element,ELEMENT} err
    catch {ans_sendcommand )CM,_temp_area,AREA} err
    
    ans_sendcommand "CMSEL,S,$previousname"
    ans_sendcommand "CM,$cmpname,$cmptype"
    ans_sendcommand "CMDELE,$previousname"
    #reshow the temp component
    catch {ans_sendcommand )CMSEL,S,_temp_area} err     
    catch {ans_sendcommand )CMDEL,_temp_area} err
    catch {ans_sendcommand )CMSEL,R,_temp_element} err     
    catch {ans_sendcommand )CMDEL,_temp_element} err
    catch {ans_sendcommand )CMSEL,R,_temp_node} err     
    catch {ans_sendcommand )CMDEL,_temp_node} err          
    catch {ans_sendcommand )/GO} err
  }
  if {$previousproperty!=$cmpproperty} {
    ans_sendcommand "*set,_property_$cmpname,'$cmpproperty'"
  }
  set tabledata($sel,-1) {}
  set tabledata($sel,0)  $cmpname
  set tabledata($sel,1)  $cmptype
  set tabledata($sel,2)  $cmpproperty
  $table selection set $sel,-1  
  destroy $subform
}
proc ok_apdl_click {} \
{
  variable apdlform
  variable selname
  variable selmethod
  variable cmplistvar
  variable cmplistbox

  set sel [$cmplistbox curselection]
  lset cmplistvar $sel "CMSEL,$selmethod,$selname"
  destroy $apdlform

}
proc delComponent {} \
{
    variable table
    variable tabledata

    set selection [lsort -integer -decreasing  [$table curselection]]
    # If there is no selection then return
    if { [string match {} $selection] } {
      return
    }

    foreach sel $selection {
      set name $tabledata($sel,0)
      set property $tabledata($sel,2)
      ans_sendcommand "CMDEL,$name"
      ans_sendcommand "*set,_property_$name"
      $table delete rows $sel
    }
      # # If there are no more rows in the column heading, then disabled the
      # # delete button.
     if { [$table index end row]==-1} {
          for {set i 0} {$i < 7} {incr i} {
            [$table getContextMenu] entryconfigure $i -state disabled
          }
        }
     

}
proc createComponent {} \
{
        variable table
        variable tabledata
        variable font
        variable newname
        variable newtype
        variable newproperty
        variable newcmpform


        #initialize the window
        set newname {}
        set newproperty {}
        set newtype Nodes
        if {[winfo exists .cmpnew]} {
               destroy .cmpnew]
        }
        #if multi select,set the first item

        set subtitle [msgcat::mc "Create Component "]
        set thestate normal
        #create the top level
        set newcmpform [toplevel .cmpnew]
        #set title for the window 
        wm title .cmpnew $subtitle
        #adjust the location of subwindow window
        set x [expr {([winfo screenwidth .]-300)/2}]
        set y [expr {([winfo screenheight .]-250)/2}]
        wm geometry $newcmpform  300x250+$x+$y

        #Components ID and Components name
        set labelName [label $newcmpform.labelName \
          -text [msgcat::mc "ComponentName "]\
          -font $font(content) \
          -width 8]
        set entryName [entry $newcmpform.entryName\
          -textvariable [scope newname] \
          -font $font(content) \
          -width 15]
        set labelType [label $newcmpform.labelType\
          -text [msgcat::mc "ComponentType "] \
          -font $font(content) \
          -width 8]
        set comboType [euidl::combobox $newcmpform.comboType \
                 -exportselection 1 \
                 -style dropdown \
                 -listheight 100 \
                 -width 13\
                 -font $font(content) \
                 -variable [scope newtype]]
        set labelProperty [label $newcmpform.labelId \
          -text [msgcat::mc "ComponentProperty "] \
          -font $font(content) \
          -width 8]
        set entryProperty [entry $newcmpform.entryId \
          -textvariable [scope newproperty] \
          -font $font(content) \
          -width 15]
        set frameokcancel [label $newcmpform.frameokcancel]
        set btnok [button $newcmpform.btnok \
          -text [msgcat::mc "Ok "]\
          -font $font(content) \
          -width 8\
          -command [code ok_new_cmp_click]]
        set btnexit [button $newcmpform.btnexit\
          -text [msgcat::mc "Exit "]\
          -font $font(content) \
          -width 8\
          -command { destroy .cmpnew }]

        ###########create and arrange the control#################
        grid $labelName -row 1  -column 0  -sticky w 
        grid $entryName -row 1  -column 1  -padx 6m -pady 2m
        grid $labelType -row 2  -column 0 -sticky w 
        grid $comboType -row 2   -column 1 -padx 6m  -pady 2m
        grid $labelProperty -row 3  -column 0 -sticky w 
        grid $entryProperty -row 3  -column 1 -padx 6m  -pady 2m

        pack $btnok $btnexit -in $frameokcancel -side left -padx 5m 
        grid $frameokcancel  -row 4 -column 0  -columnspan 2 -pady 4m
        $comboType insert list end [list \
            "Nodes" Nodes\
            "Elements" Elements\
            "Areas" Areas \
            "Volumes" Volumes]

        update idletasks
}

proc ok_new_cmp_click {} \
{
    variable newname
    variable newtype
    variable newproperty
    variable newcmpform
    variable tabledata
    variable table
    variable cmpne

    
    #check the input component name Format
    # [regexp -all -inline -- {[\u4e00-\u9fa5]|\s+}  $newname]
    set name_reg [regexp -all -inline -- {^[A-Za-z][A-Za-z0-9_]+$}  $newname]
    if {$name_reg!=$newname ||[string trim $newname]==""} {
      tk_messageBox -title [msgcat::mc "Tips "] \
          -message [msgcat::mc "Wrong Component Name Format! "] \
          -icon error 
      return
    }
    set newname [string toupper $newname]
    set rownum [$table index end row]
    for {set i 0} {$i <= $rownum} {incr i} {
      #translate into upper case
      if {$newname==$tabledata($i,0)} {
        set answer [tk_messageBox \
          -title [msgcat::mc "Tips "] \
          -message [msgcat::mc "Really Cover The Component? "] \
          -icon question \
          -type yesno ]
        if {$answer=="yes"} {
          ans_sendcommand "CM,$newname,$newtype"
          ans_sendcommand "*set,_property_$newname,'$newproperty'"
          set tabledata([expr $rownum+1],-1) {}
          $table insert row $i
          $table delete rows $i
          set tabledata($i,-1) {}
          set tabledata($i,0)  $newname
          set tabledata($i,1)  $newtype
          set tabledata($i,2)  $newproperty
          $table  selection set  $i,-1
          destroy  $newcmpform
        } 
        return
      }
    }
    ans_sendcommand "CM,$newname,$newtype"
    ans_sendcommand "*set,_property_$newname,'$newproperty'"
    incr rownum
    $table insert row end
    set tabledata($rownum,-1) {}
    set tabledata($rownum,0)  $newname
    set tabledata($rownum,1)  $newtype
    set tabledata($rownum,2)  $newproperty
    $table  selection set  $rownum,-1
    for {set i 0} {$i < 7} {incr i} {
      [$table getContextMenu] entryconfigure $i -state normal
    }
    destroy  $newcmpform
}

proc execAPDL {} \
{
  variable cmplistvar
  foreach var $cmplistvar {
    ans_sendcommand $var
  }
  #save the component
  ans_sendcommand "SAVE"
  ans_sendcommand "/REPLOT"
  # destroy  .componentmanager
}

#Display the component
proc Display {} {
      variable table
      variable tabledata

      ::apdl::storeUserSelection ALL
      set selection [$table curselection]

      if { [string match {} $selection] } {
         ::apdl::restoreUserSelection
         return
      }

      set first 1
      foreach sel $selection {
         if { $first } {
            set first 0
            catch {ans_sendcommand \
                  CMSEL,S,$tabledata($sel,0)}
         } else {
            catch {ans_sendcommand \
                  CMSEL,A,$tabledata($sel,0)}
         }
      }
      catch {ans_sendcommand CMPLOT}
      ::apdl::restoreUserSelection
}

# #plot component in Context Mode
proc PlotInContext {} {

      variable table
      variable tabledata
      ############
      global env


      set selection [$table curselection]

      # If there is no selection then return
      if { [string match {} $selection] } {
         return
      }

      ::apdl::noprint 1
      ::apdl::storeUserSelection ALL

      # Get the settings before changing
      set sledge(3) [ans_getvalue COMMON,,D3COM,,INT,16]
      set sledge(4) [ans_getvalue COMMON,,D3COM,,REAL,173]
      set slnum [ans_getvalue COMMON,,D3COM,,INT,30]
      set pnum(type) [ans_getvalue COMMON,,D3COM,,INT,99]
      set pnum(real) [ans_getvalue COMMON,,D3COM,,INT,100]
      set psymb(esys) [ans_getvalue COMMON,,D3COM,,INT,143]

      if {[file exists [file join ~ cmpplot.mac]]} {
         file delete -force [file join ~ cmpplot.mac]
      } elseif {[file exists [file join [pwd] cmpplot.mac]]} {
         file delete -force [file join [pwd] cmpplot.mac]
      }

      ans_sendcommand *CREATE,cmpplot,mac
      ans_sendcommand /COM,
      ans_sendcommand /COM,PLOT COMPONENT(S)
      ans_sendcommand ~eui,'::apdl::noprint 1'
      ans_sendcommand ~eui,'::apdl::nooutput 1'

      catch {ans_sendcommand /GSAVE,COMPONENT,GSAV,'$env(EUIDL_DEFAULTS)',TRLC} err
      ans_sendcommand /TRLCY,ON
      ans_sendcommand /EDGE,,1,45


      if { [string match {} $selection] } {
         ::apdl::restoreUserSelection
         ::apdl::noprint 0
         return 0
      }

      # Get the type of this selection

      set sel [lindex $selection 0]
      set name $tabledata($sel,0)
      set type $tabledata($sel,1)

      switch -- $type {
         Elements {
            catch {ans_sendcommand ESEL,ALL}
         }
         Areas {
            catch {ans_sendcommand ASEL,ALL}
         }
         Volumes {
            catch {ans_sendcommand VSEL,ALL}
         }
      }
      catch {ans_sendcommand CM,ALL$type,$type}
      catch {ans_sendcommand CMSEL,S,ALL$type}
      catch {ans_sendcommand /TRLCY,CM,1.0,ALL$type}

      set selection [$table curselection]
      foreach sel $selection {
         ans_sendcommand CMSEL,A,$tabledata($sel,0)
         ans_sendcommand \
                /TRLCY,CM,0.0,$tabledata($sel,0)
      }
#      ans_sendcommand CMSEL,ALL
      ans_sendcommand CMPLOT

      ans_sendcommand CMDEL,ALL$type
      ans_sendcommand /TRLCY,DEFA
      catch {ans_sendcommand /GRESUME,COMPONENT,GSAV,'$env(EUIDL_DEFAULTS)'} err
      catch {ans_sendcommand /EDGE,,$sledge(3),$sledge(4)} err

      catch {ans_sendcommand /PSYMB,ESYS,$psymb(esys)} err
      catch {ans_sendcommand /NUM,$slnum} err
      catch {ans_sendcommand /PNUM,TYPE,$pnum(type)} err
      catch {ans_sendcommand /PNUM,REAL,$pnum(type)} err

      ans_sendcommand /mrep,cmpplot
      ans_sendcommand ~eui,'::apdl::nooutput 0'
      ans_sendcommand ~eui,'::apdl::noprint 0'
      ans_sendcommand *END

      catch {ans_sendcommand cmpplot}
      ::apdl::restoreUserSelection
      ::apdl::noprint 0
   }

 proc ListWithEntities {} {
      variable table
      variable tabledata

      ::apdl::storeUserSelection ALL
      set selection [lsort -integer  [$table curselection]]

      if { [string match {} $selection] } {
         ::apdl::restoreUserSelection
         return 0
      }
      catch {ans_sendcommand CMSEL,NONE}
      foreach sel $selection {
         catch {ans_sendcommand \
               CMSEL,A,$tabledata($sel,0)}
      }
      catch {ans_sendcommand CMLIST,,1}
      ::apdl::restoreUserSelection
   }

   # raise .componentmanager
}  

#create and show window
::Customized::Manager::Component::main
