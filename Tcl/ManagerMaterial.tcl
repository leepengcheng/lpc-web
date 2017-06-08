encoding system euc-cn
package require msgcat
package require tdom
set path [file dirname [info script]]
set librayfile [file join $path mat.xml]
::msgcat::mcload  [file join $path locale]
::msgcat::mcload  [file join $path locale material]
set imagePath [file join $env(EUIDL_DIR) gui $::euidl::euidlArray(language) images]
image create photo mtlclose -file [file join $imagePath mtlclose16x16.png]
image create photo mtlopen  -file [file join $imagePath mtlopen16x16.png]
image create photo mtlicon  -file [file join $imagePath mtlicon16x16.png]

namespace eval ::euidl::material::MaterialLibrary {} {

#Create and initialize 
proc main {} {

      variable parent
      variable table
      variable treeMat
      variable tabledata

      variable font
      variable menu      
      variable theunit
      variable btn_del
      
      ##check current Environment
      if {[chekcEnvironment]} {
         return
      }

      if {[winfo exists .materiallibrary]} {
         wm deiconify .materiallibrary
         raise .materiallibrary
         return
      }

      set font(content)  {"Microsoft YaHei" 11 normal}
      set font(title)  {"Microsoft YaHei" 12 normal}
      ####################################
      set parent [toplevel .materiallibrary]
      wm title $parent [msgcat::mc "Define Material Model Behavior "]
      set x [expr {([winfo screenwidth  .]-730)/2}]
      set y [expr {([winfo screenheight .]-350)/2}]
      wm geometry $parent  730x350+$x+$y
      #delete all the window
      wm protocol $parent WM_DELETE_WINDOW {
            if {[winfo exists .tableitem]} {
             destroy .tableitem  
           }
            destroy .materiallibrary
            catch {ans_sendcommand "SAVE"} err
      }

      # #Create the menubar
      option add *tearOff 0
      set menuBar [menu $parent.menubar]
      set menu(units) [menu $menuBar.units -tearoff 0]
      $menuBar add cascade \
          -label [msgcat::mc "Units "]\
          -menu $menu(units)


      # $Edit add separator
      $menu(units) add radio \
        -label [msgcat::mc "SI :m-kg-pa "] \
        -variable [scope theunit]  \
        -value "SI"\
        -command [code setModelUnit]

      $menu(units) add radio \
        -label [msgcat::mc "MPA:mm-t-mpa "] \
        -variable [scope theunit] \
        -value "MPA"\
        -command [code setModelUnit]
        

      #add the top menubar
      $parent config -menu $menuBar 

      #Left frame
      set left [euidl::labeledframe $parent.left \
          -font $font(content)  \
          -text [msgcat::mc "Material Available "]]
      grid $left -row 1 -column 0  -sticky news
      set left [$left childsite]
      #center frame
      set center [frame $parent.center]
      grid $center  -row 1 -column 1  -sticky news
      #Right frame
      set right [euidl::labeledframe $parent.right \
          -font $font(content)\
          -text [msgcat::mc "Material Defined "]]
      grid $right -row 1 -column 2  -sticky news
      set right [$right childsite]
      grid  columnconfigure $parent 0 -weight 0
      grid  columnconfigure $parent 2 -weight 1
      grid  rowconfigure $parent 1 -weight 1

      #center button
      set btn_add [button $center.add -text "[msgcat::mc "Add "]>>" \
        -width 8 -font $font(content)\
        -command [namespace code addLocalMat] ]
      set btn_del [button $center.del -text "[msgcat::mc "Del "]<<" \
        -width 8 -font $font(content) \
        -command [namespace code deleteLocalMat] ]
      pack $btn_add  -side top -fill x -pady 90
      pack $btn_del  -side top -fill x 

      #Tree for the left side
      set table [euidl::colhead $right.table \
        -variable [namespace which -variable tabledata] \
        -selectbackground #09f\
        -borderwidth 0 \
        -hscrollmode none \
        -contextmenu 1 \
      ]

      #sortmethod 
      $table tag configure ID -sortmethod integer
      $table tag configure {Name} -sortmethod dictionary
      $table tag configure {Type} -sortmethod dictionary


      $table tag configure title -anchor center
      $table tag configure title -font $font(title)
      $table tag configure row -font $font(content)
      $table tag configure row -anchor center

      #insert head
      $table insert cols ID end
      $table insert cols {Name} end
      $table insert cols {Type} end
     
      ################################################
      # $table tag configure {EX} -sortmethod real
      # $table insert cols {EX} end

      #cofigure the width of the header
      [$table component table] width [$table tag coltag {ID}] 10
      [$table component table] width [$table tag coltag {Name}] 30
      [$table component table] width [$table tag coltag {Type}] 30
      #the rows height
      [$table component table] configure -rowheight 2
      bind $table <Double-1> [code showTableItem]
      # puts [[$table component table] configure]
      #pack the table
      pack $table -fill both -expand 1
      #ContextMenu
      $table deleteContextMenu 0 end 
      set cmindex 0
      $table insertContextMenu $cmindex command \
            -label [::msgcat::mc "Edit "] \
            -underline 0 \
            -font $font(content) \
            -command [code showTableItem]
      incr cmindex
      $table insertContextMenu $cmindex command \
      -label [::msgcat::mc "Delete "] \
      -font $font(content) \
      -underline 0 \
      -command [namespace code deleteLocalMat]

      #Tree for the right side
      set treeMat [euidl::hierarchy $left.modelControl \
            -querycommand [list [namespace code queryXmlNode] %n] \
            -execcommand [list [namespace code  selectTreeNode] %n %s]\
            -selectcommand [list [namespace code selectTreeNode] %n %s ] \
            -alwaysquery 1 \
            -closedicon mtlclose \
            -openicon mtlopen \
            -nodeicon mtlicon \
            -style single \
            -width 200 \
            -height 350 \
            -contextmenu 1 \
            -font $font(content)
      ]
      $treeMat deleteContextMenu 0 end
      set cmindex 0
      $treeMat insertContextMenu $cmindex command \
            -label [::msgcat::mc "Add "] \
            -underline 0 \
            -font $font(content) \
            -command [namespace code addLocalMat]
      incr cmindex
      $treeMat insertContextMenu $cmindex command \
            -label [::msgcat::mc "Check "] \
            -underline 0 \
            -font $font(content) \
            -command [code showTreeItem]

      grid $treeMat -sticky news
      grid  columnconfigure $right 0 -weight 1
      grid  columnconfigure $left 0 -weight 1
      grid  rowconfigure $right 0 -weight 1
      grid  rowconfigure $left 0 -weight 1
      update idletasks
      #initialize the value of material data
      ::euidl::material::MaterialLibrary::initialize
  }


#read data from local material libray file
proc loadXml {} \
{
    global librayfile
    set fl [open $librayfile r]
    set data [read $fl]
    if {[catch {close $fl} err]} {
        ans_senderror 1 "Read Material Data Failed: $err "
    }
    # set data [encoding convertto utf-8 $data]
    return $data
}

#parse and get the mat.xml
proc queryXmlNode {node} \
{
  variable localtable
  variable nodeproperty
  variable doc

  set xml_info [loadXml]
  set dom [dom parse $xml_info]
  set doc [$dom documentElement]
  set localmats(root) [ list ]
  set nodes [$doc selectNodes {/Root/Type}]

  foreach thenode $nodes { \
    #ex: 1root
    set typeid  [$thenode getAttribute id]
    #ex:Structural
    set typename [::msgcat::mc $typeid]
    #such as "1root Structural"
    lappend localmats(root)  "$typeid $typename"
    set subnodes [$thenode childNodes]
    set localmats($typeid) [list]
    foreach subnode $subnodes {
      set subid [$subnode getAttribute id]
      set subname [::msgcat::mc $subid]
      lappend localmats($typeid) "$subid $subname"
    }
  } 
      #the root node 
  if {$node == "" } {
    set node root
  }
  #if no node exists
  if {[string match {} [array names localmats $node]]} {
    return ""
  }
  return $localmats($node)
}

#return name-type-property or ""
#what the fuck!! 
proc queryXmlProperty {thenode} \
{
  variable doc
  variable theunit
  #what is the fuck???????
  # set thenode [$doc selectNode {/Root/Type/Mat[@id=$thenode]}]
  # set thenode [$doc getAttribute $thenode]
  set matNodes [$doc selectNodes {/Root/Type/Mat}]
  foreach node $matNodes {
    set nodeid [$node getAttribute id]
    if {$nodeid==$thenode} {
        set name "_NAME [::msgcat::mc $nodeid]"
        set parentid [[$node parentNode] getAttribute id]
        set type "_TYPE [::msgcat::mc $parentid]"
        set property [$node text]
        #translate the property with array
        array set propertyarray $property
        #translate the property with theunit
        switch -regexp -- $theunit {
          MPA* {
              if {[lsearch $property EX]>=0} {
                #pa-Mpa
                set propertyarray(EX) [format "%G" [expr $propertyarray(EX)*1.0e-6]]
              }
              if {[lsearch $property TM]>=0} {
                #pa-Mpa
                set propertyarray(TM) [format "%G" [expr $propertyarray(TM)*1.0e-6]]
              }
              if {[lsearch $property YS]>=0} {
                #pa-Mpa
                set propertyarray(YS) [format "%G" [expr $propertyarray(YS)*1.0e-6]]
              }
              if {[lsearch $property DENS]>=0} {
                #kg/m^3-t/mm^3
                set propertyarray(DENS) [format "%G" [expr $propertyarray(DENS)*1.0e-12]]
              }
          }
          default {}
        }
        set property [array get propertyarray]
        return [concat $name $type $property ]
    }
  }
  return ""

}

#Get The Material Type By the Material Property
proc GetAnsysMatData { matid } \
  {
    set resultlist [list]
      set propertylist {EX EY EZ NUXY NUYZ NUXZ PRXY PRYZ PRXZ GXY GYZ GXZ DENS BKIN}
      foreach index $propertylist {
        set flag [ans_getvalue $index,$matid,EXIS]
        if {$flag==1} {
          if {$index=="BKIN"} {
            ##Yield Stress && Tang Mods
            lappend resultlist "YS" [format "%G" [ans_getvalue $index,$matid,TEMP,0,CONST,1]]
            lappend resultlist "TM" [format "%G" [ans_getvalue $index,$matid,TEMP,0,CONST,2]]
          } else {
            #get the key-value from ansys
            lappend resultlist $index [format "%G" [ans_getvalue $index,$matid,TEMP,0]]
          }

        }
      }
      return $resultlist
  }



#get ansys material list numbers  
proc GetAnsysMatNumberList {} \
{
  #get the max material number
  set maxMatlNum [ans_evalexpr mpinqr(0,0,14)]
  #get the number of definded material
  set maxMatlDef [ans_evalexpr mpinqr(0,0,12)]
  set matidlocal 1
  if {$maxMatlDef == 0 } {
     return
  }
  set matdef [list]
  set matidlocal [ans_evalexpr mpnext(0)]
  if {$maxMatlNum > 0} {
    while {$matidlocal > 0} {
      #get the material info by the material id
      lappend matdef $matidlocal
      set matidlocal [ans_evalexpr mpnext($matidlocal)]
    }
  }
     return $matdef
}

#match material name and type
#method 1: ansys DB 
#method 2: xml property
#method 3: return None None
proc getProperty {id key} \
{
  variable doc 
  set index  [format "_mat_%s_%s" $id $key]
  set flag [ans_getvalue parm,$index,type]
  if {$flag!=-1} {
    set data [ans_getvalue parm,$index,value]
    #retun value contain some space
    return [string trim $data]
  }
  #ansys db property
  set ansProperty [GetAnsysMatData $id]
  #xml property
  set matNodes [$doc selectNodes {/Root/Type/Mat}]
  foreach node $matNodes {
      set xmlProperty [$node text]
      if {[lsort $xmlProperty]==[lsort $ansProperty]} {
          if {$key=="type"} {
            return [[$node parentNode] getAttribute name]
          } elseif {$key=="name"} {
            return [$node getAttribute name]
          }
      }
  }
  return "None"
}


#initialize the table data
proc initialize {} \
{ 
    variable menu
    variable table
    variable tabledata
    variable btn_del
    variable selTreeNode
    variable theunit
    variable lastunit

    set selTreeNode "root"
    set index 0
    foreach id [GetAnsysMatNumberList] {\
       set name [getProperty $id name]
       set type [getProperty $id type]
       # set unit [getProperty $id unit]

       $table insert row end
       set tabledata($index,-1) {}
       set tabledata($index,0)  $id
       set tabledata($index,1)  $name
       set tabledata($index,2)  $type
       incr index
    }
    #disabled the delete button and contextmenu
    if { [$table index end row]==-1} {
           $btn_del configure -state disabled
           [$table getContextMenu] entryconfigure 0 -state disabled
           [$table getContextMenu] entryconfigure 1 -state disabled
    }
    #theunit specified by /theunit command: \
    #0 = USER, 1 = SI, 6 = MPA
    set unitID [ans_getvalue ACTIVE,,UNITS]
    set unitID  [string trim $unitID]
    if {$unitID==-1 || $unitID==1} {
      set theunit  "SI"
      set lastunit "SI"
    } else {
      set theunit  "MPA"
      set lastunit "MPA"
    }
}

#exit the main form
  proc exitmain {} {
     variable parent
     variable localTable
     # wm withdraw $parent
     if [winfo exists $parent] {
        destroy $parent
     }
     if {[winfo exists $localTable]} {
         destroy $localTable
    }
  }

  #select the tree node
  proc selectTreeNode { node args } \
  {
      variable treeMat
      variable selTreeNode
      $treeMat selection clear
      $treeMat selection add $node
      set selTreeNode $node
  }

  #click the mat node
  proc addLocalMat {}\
  {
    variable table
    variable property
    variable selTreeNode
    variable tabledata
    variable btn_del


    # If selection is root
    if { [regexp {\d+root} $selTreeNode] } {
        puts "please select no-root node!"
        return
    }

    if {[info exists property]} {
      unset property
    }

    set data  [queryXmlProperty $selTreeNode]
    array set property $data
    # # Get The Max Numer Id Of Ansys Material Model
    set id [ans_evalexpr mpinqr(0,0,14)]
    #increment the maxMatlNum
    incr id
    #send apdl command to ansys
    catch {ans_sendcommand "MPTEMP,1,0"} err

    foreach key [array names property]  {
      switch -exact -- $key {
        _NAME {set name    $property($key)}
        _TYPE {set type    $property($key)}
         TM   {set bkin_tm $property($key)}
         YS   {set bkin_ys $property($key)}
        default {catch {ans_sendcommand "MPDATA,$key,$id,,$property($key)"} err}
      }
    }

    ### BKIN Material Define
    if {[info exists bkin_tm] && [info exists bkin_ys]} {
          catch {ans_sendcommand "TB,BKIN,$id,1,2,1 "} err
          catch {ans_sendcommand "TBTEMP,0"} err
          catch {ans_sendcommand "TBDATA,,$bkin_ys,$bkin_tm"} err
    }
     #write the name and type into ansys DB
     catch {ans_sendcommand [format "*set,_mat_%s_name,'%s'" $id $name]} err
     catch {ans_sendcommand [format "*set,_mat_%s_type,'%s'" $id $type]} err

     set index [$table index end row]
     incr index
     $table insert row end
     set tabledata($index,-1) {}
     set tabledata($index,0)  $id
     set tabledata($index,1)  $name
     set tabledata($index,2)  $type
     # set tabledata($index,3)  $ex
     $table selection set $index,-1
     #enabled the delete command
     $btn_del configure -state normal
     [$table getContextMenu] entryconfigure 0 -state normal
     [$table getContextMenu] entryconfigure 1 -state normal
  }


  #delete table row,delete the selected material
  proc deleteLocalMat {} \
  {
    variable table
    variable tabledata
    variable btn_del
    set selection [lsort -integer -decreasing [$table curselection]]

      # If there is no selection then return
      if { [string match {} $selection] } {
         return
      }

      foreach sel $selection {
        set matid $tabledata($sel,0)
        catch {ans_sendcommand [format "*set,_mat_%s_name" $matid]} err
        catch {ans_sendcommand [format "*set,_mat_%s_type"  $matid]} err
        catch {ans_sendcommand "MPDE,ALL,$matid"} err 
        catch {ans_sendcommand "TBDE,ALL,$matid"} err
      }
      foreach sel $selection {
        $table delete rows $sel
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
       if { [$table index end row]==-1} {
             $btn_del configure -state disabled
             [$table getContextMenu] entryconfigure 0 -state disabled
             [$table getContextMenu] entryconfigure 1 -state disabled
          }
       $table selection set $rowNum,0
}
#show the local material information
  proc showTableItem {} \
{
        variable parent
        variable table
        variable tabledata
        variable localTable
        variable font
        variable theunit
        variable tableitemarray
        variable tableitemarraycopy


        if {[winfo exists .tableitem]} {
               destroy .tableitem]
        }
        set selection [lsort -integer -decreasing [$table curselection]]
        # If there is no selection then return
        if { [string match {} $selection] } {
           return
        }
        #if multi select,set the first item
        set sel [lindex $selection end]
        $table selection clear all
        $table selection set $sel,-1

        #clear the array
        if {[info exists tableitemarray]} {
          unset tableitemarray
        }
        if {[info exists tableitemarraycopy]} {
          unset tableitemarraycopy
        }        
        set data [GetAnsysMatData $tabledata($sel,0)]
        set id   "_ID    $tabledata($sel,0)"
        set name "_NAME  $tabledata($sel,1)"
        set type "_TYPE  $tabledata($sel,2)"
        set tableitemdatalist [concat $id $name $type $data]
        array set tableitemarray $tableitemdatalist
        array set tableitemarraycopy $tableitemdatalist
        #create the top level
        set localTable [toplevel .tableitem]
        #modal control
        grab set $localTable
        #set title for the window
        wm title $localTable  [format "%s:%s  %s:%s" \
            [::msgcat::mc "Material ID "] $tableitemarray(_ID) \
            [::msgcat::mc "Material Name "] $tableitemarray(_NAME)]
        # #adjust the location of subwindow window
        # set x [expr {([winfo screenwidth .]-320)/2}]
        # set y [expr {([winfo screenheight .]-290)/2}]
        # wm geometry $localTable  320x320+$x+$y
        # wm transient $localTable
        ############################################################
        set index 0
        foreach key  [lsort -dictionary [array names tableitemarray]] {
            set keyname [::msgcat::mc $key]
            switch -exact -- $key {
              _ID   {set keyname [::msgcat::mc "Material ID "]}
              _NAME {set keyname [::msgcat::mc "Material Name "]}
              _TYPE {set keyname [::msgcat::mc "Material Type "]}
              EX   { if {$theunit=="SI"} {lappend keyname "(Pa)"}    else { lappend keyname "(Mpa)"} }
              TM   { if {$theunit=="SI"} {lappend keyname "(Pa)"}    else { lappend keyname "(Mpa)"} }
              YS   { if {$theunit=="SI"} {lappend keyname "(Pa)"}    else { lappend keyname "(Mpa)"} }
              DENS { if {$theunit=="SI"} {lappend keyname [::msgcat::mc "Kg/m2"] } else { lappend keyname [::msgcat::mc "T/mm2"]} }
            }
          set label_property [label $localTable.label_$key \
              -text $keyname  \
              -font $font(content) ]

          set entry_property [entry $localTable.entry_$key \
              -textvariable  [scope tableitemarray($key)] \
              -font $font(content) \
              -width 15]

          grid $label_property -row $index -column 0 -sticky w -padx 2m -pady 2m
          grid $entry_property -row $index -column 1 -sticky w -padx 2m -pady 2m
          incr index
        }

        set btn_ok [button $localTable.btn_ok \
            -text [::msgcat::mc "Ok "]  \
            -width 8 \
            -font $font(content) \
            -command [code ok_tableonclick]]

        set btn_cancel [button $localTable.btn_cancel \
            -text [::msgcat::mc "Cancel "]  \
            -width 8 \
            -font $font(content) \
            -command { destroy .tableitem }]

        grid $btn_ok     -row $index -column 0   -pady 6m 
        grid $btn_cancel -row $index -column 1   -pady 6m

        # #adjust the location of subwindow window
        set width [expr 55*[llength [array names tableitemarray]]]
        set x [expr {([winfo screenwidth .]-320)/2}]
        set y [expr {([winfo screenheight .]-$width)/2}]
        wm geometry $localTable  320x$width+$x+$y
        wm transient $localTable
        
  }
 

#show the local material information
  proc showTreeItem {} \
{
        variable parent
        variable selTreeNode
        variable font
        variable theunit
        variable treeproperty

        if {[info exists treeproperty]} {
          unset treeproperty
        }
        if {[winfo exists .treeitem]} {
               destroy .treeitem]
        }
        if {[string match *root $selTreeNode]} {
          return
        }
        set data  [queryXmlProperty $selTreeNode]
        array set treeproperty $data

        #create the top level
        set localTree [toplevel .treeitem]
        #modal control
        grab set $localTree
        #set title for the window 
        wm title $localTree [::msgcat::mc "Material Name "]:$treeproperty(_NAME)
        #adjust the location of subwindow window
        # set x [expr {([winfo screenwidth .]-320)/2}]
        # set y [expr {([winfo screenheight .]-290)/2}]
        # wm geometry $localTree  320x320+$x+$y
        # wm transient $localTree
        set index 0
        foreach key  [lsort -dictionary [array names treeproperty]] {
            set keyname [::msgcat::mc $key]
            switch -exact -- $key {
              _NAME {set keyname [::msgcat::mc "Material Name "]}
              _TYPE {set keyname [::msgcat::mc "Material Type "]}
              EX   { if {$theunit=="SI"} {lappend keyname "(Pa)"}    else { lappend keyname "(Mpa)"} }
              TM   { if {$theunit=="SI"} {lappend keyname "(Pa)"}    else { lappend keyname "(Mpa)"} }
              YS   { if {$theunit=="SI"} {lappend keyname "(Pa)"}    else { lappend keyname "(Mpa)"} }
              DENS { if {$theunit=="SI"} {lappend keyname [::msgcat::mc "Kg/m2"] } else { lappend keyname [::msgcat::mc "T/mm2"]} }
            }
          set label_property [label $localTree.label_$key \
              -text $keyname  \
              -font $font(content) ]

          set entry_property [entry $localTree.entry_$key \
              -textvariable  [scope treeproperty($key)] \
              -font $font(content) \
              -state disabled\
              -width 15]

          grid $label_property -row $index -column 0 -sticky w -padx 2m -pady 2m
          grid $entry_property -row $index -column 1 -sticky w -padx 2m -pady 2m
          incr index
        }

        set btn_exit [button $localTree.btn_exit \
            -text [::msgcat::mc "Exit "]  \
            -width 8 \
            -font $font(content) \
            -command { destroy .treeitem }]

        grid $btn_exit -row $index -column 0  -columnspan 2 -pady 5m

        set width [expr 55*[llength [array names treeproperty]]]
        set x [expr {([winfo screenwidth .]-320)/2}]
        set y [expr {([winfo screenheight .]-$width)/2}]
        wm geometry $localTree  320x$width+$x+$y
        wm transient $localTree

}

proc ok_tableonclick {} \
{
  variable localTable
  variable tableitemarray
  variable tableitemarraycopy
  variable table
  variable tabledata

  set id_this    $tableitemarray(_ID)
  set id_last    $tableitemarraycopy(_ID)
  set name_this $tableitemarray(_NAME)
  set type_this $tableitemarray(_TYPE)

  if {[array get tableitemarray]==[array get tableitemarraycopy]} {
      destroy $localTable
      return
  } 
  set ansysMatNumList [GetAnsysMatNumberList]
  if {$id_last!=$id_this} {
    if {[lsearch $ansysMatNumList  $id_this]>=0} {
      tk_messageBox  -parent $localTable -icon warning -title [::msgcat::mc "Wrong Material Id "] \
      -message  [::msgcat::mc "Material Id %d is Existing! " $id_this ]
      return
    } 
  }
  #delete old material
  catch {ans_sendcommand "MPDE,ALL,$id_last"} err
  catch {ans_sendcommand "TBDE,ALL,$id_last"} err
  #create new material
  catch {ans_sendcommand "MPTEMP,1,0"} err
  foreach key [array names tableitemarray]  {
    switch -exact -- $key {
      _ID   {}
      _NAME {
        #delete the old flag and create new flag
        catch {ans_sendcommand [format "*set,_mat_%s_name"  $id_last]} err
        catch {ans_sendcommand [format "*set,_mat_%s_name,'%s'"  $id_this $name_this]} err      
      }
      _TYPE {
        #delete the old flag and create new flag
        catch {ans_sendcommand [format "*set,_mat_%s_type"  $id_last]} err
        catch {ans_sendcommand [format "*set,_mat_%s_type,'%s'"  $id_this $type_this]} err 
      }
       TM   {set bkin_tm $tableitemarray($key)}
       YS   {set bkin_ys $tableitemarray($key)}
      default {
        catch {ans_sendcommand  "MPDATA,$key,$id_this,,$tableitemarray($key)"} err
      }
    }
  } 
  #Re Define BNIN
  if {[info exists bkin_tm] && [info exists bkin_ys]} {
        catch {ans_sendcommand "TB,BKIN,$id_this,1,2,1 "} err
        catch {ans_sendcommand "TBTEMP,0"} err
        catch {ans_sendcommand "TBDATA,,$bkin_ys,$bkin_tm"} err
  }
  set sel [$table curselection]
  set tabledata($sel,0)  $id_this
  set tabledata($sel,1)  $name_this
  set tabledata($sel,2)  $type_this
  destroy $localTable
}

proc setModelUnit {} \
{
  variable table
  variable parent
  variable lastunit
  variable theunit

  set rowNum [$table index end row]
  if {$rowNum>=0 && $theunit!=$lastunit} {
      tk_messageBox  -parent $parent -icon warning -title [::msgcat::mc "Tips "] \
      -message  [::msgcat::mc "Can not change the theunit of added Material! "]
      set theunit $lastunit
      return
  }
  catch {ans_sendcommand "/UNITS,$theunit"} err
  set lastunit $theunit
}

proc chekcEnvironment {} \
{
    # array set env_dict {17 PREP7 21 SOLU 31 POST1}
    # 0 = Begin level,  17 = PREP7 21 = SOLUTION, 31 = POST1, 36 = POST26, 
    set env [ans_getvalue "ACTIVE,0,ROUT"]
    if {$env!=17 && $env!=21 } {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -message "[msgcat::mc "This Operation Need Environment: "] \n [msgcat::mc "PREP7"]\n [msgcat::mc "SOLU"]" \
        -icon warning 
        return 1
    } 
    return 0
}

}  

#create window
::euidl::material::MaterialLibrary::main