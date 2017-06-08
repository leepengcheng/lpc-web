package  require msgcat
namespace eval ::euidl::Seismic::SpectrumAnalysis {

proc main {} {

  variable parent
  variable tabset
  variable btnback
  variable btnnext
  variable png
  variable font
  variable plotshow


  set path [file dirname [info script]]
  ::msgcat::mcload  [file join $path locale seismic]
  set png(check) [file join $path images check24x24.png]
  set png(import) [file join $path images import24x24.png]
  set png(save) [file join $path images save24x24.png]
  set png(chart) [file join $path images chart24x24.png]
  set png(update) [file join $path images update24x24.png]
  set font(cn)  {"Microsoft YaHei" 12 normal}
  set font(en)  {Consolas 12 normal}
  set plotshow    [file join $path "AnsysPlugin.exe"]

  if [winfo exists .spectrum] {
      wm deiconify .spectrum
      raise .spectrum
      return
  }

  set parent [toplevel  .spectrum]
  wm title $parent [msgcat::mc "Spectrum Analysis "]
  set x [expr {([winfo screenwidth  .]-650)/2}]
  set y [expr {([winfo screenheight .]-500)/2}]
  wm geometry .spectrum 650x500+$x+$y
  wm protocol $parent WM_DELETE_WINDOW {
            if {[winfo exists .exphelp]} {
             destroy .exphelp}
            destroy .spectrum
      }

  set footer [frame $parent.footer]
  set btncancel [button $parent.cancel \
        -text [::msgcat::mc "Exit "] \
        -width 8\
        -font $font(cn)\
        -command {destroy .spectrum}]
  set btnback [button $parent.previous \
        -text [::msgcat::mc "Back "] \
        -width 8\
        -font $font(cn) \
        -command [list [code tabSwitch] "Back"]]
  set btnnext [button $parent.next \
        -text [::msgcat::mc "Next "] \
        -font $font(cn)\
        -width 8\
        -command [list [code tabSwitch] "Next"]]

  set tabset [::euidl::tabnotebook $parent.tabset]
  $tabset configure -equaltabs no
  $tabset configure -raiseselect true
  $tabset configure -tabpos n
  $tabset configure -height 4.5i
  $tabset configure -width 6i

  grid $tabset -row 1 -column 0 -sticky nsew
  pack $btnback $btnnext  $btncancel -in $footer -side left -padx 15m
  grid $footer -row 2 -column 0 -pady 10

  #add tab to the window
  addTabModal
  addTabSpectrum
  ::euidl::Seismic::SpectrumAnalysis::initialize

}

proc addTabModal {} \
{
  variable font
  variable png
  ################ 
  variable tabset
  variable parent
  variable method
  variable number
  variable table
  variable tabledata


  set page [$tabset add]
  $tabset pageconfigure 0  \
    -label [msgcat::mc "Modal Analysis "] \
    -font $font(cn) \
    -command [code changeButtonState]

  set tab1 [frame $page.hold]

  set label_modeMethod  [label $parent.label_modemethod \
        -font $font(cn) \
        -text [::msgcat::mc "Mode Method "] ]

  set combo_modeMethod [euidl::combobox $parent.combo_modemethod \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -variable [scope method]]

  set label_modeNumber [euidl::label $parent.label_modeNumber \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Mode Number "]]

  set entry_modeNumber [euidl::entry  $parent.entry_modeNumber \
        -width 10  \
        -variable [scope number] \
        -validate focusout\
        -width 15\
        -font $font(en)\
        -validatedata integer]


  set btn_solveModal [euidl::button $parent.btn_solveModal \
      -command [code modalSolve]   \
      -height 30\
      -width 50\
      -imagefile $png(check) \
      -tooltip [msgcat::mc "Solve Modal "] ]

  set btn_readModal [euidl::button $parent.btn_readModal \
      -command [code modalImport]   \
      -height 30\
      -width 50\
      -imagefile $png(import) \
      -tooltip [msgcat::mc "Import Modal "] ]

  set btn_saveModal [euidl::button $parent.btn_saveModal \
      -command [code modalExport]   \
      -height 30\
      -width 50\
      -imagefile $png(save) \
      -tooltip [msgcat::mc "Export Modal "] ]      

        #Right frame
  set modalframe [euidl::labeledframe $parent.modalframe \
      -font $font(cn) \
      -text [msgcat::mc "Defined Frequencies "]]

  grid $modalframe -in $tab1 -row 0 -column 2 -rowspan 3 -columnspan 2 -sticky news
  set modalframe [$modalframe childsite]

  #############Tree for the left side
  set table [euidl::colhead $modalframe.table \
    -variable [scope tabledata] \
    -selectbackground #09f\
    -borderwidth 0 \
    -width 280\
    -height 180\
    -hscrollmode none \
    -contextmenu 1 \
  ]
  #insert head
  $table insert cols {ID} end
  $table insert cols {Frequency} end

  #sortmethod 
  $table tag configure {ID} -sortmethod integer
  $table tag configure {Frequency} -sortmethod dictionary

  $table tag configure title -anchor center
  $table tag configure title -font $font(cn)
  $table tag configure row -font $font(cn)
  $table tag configure row -anchor center
 
  #cofigure the width of the header
  [$table component table] width [$table tag coltag {ID}] 15
  [$table component table] width [$table tag coltag {Frequency}] 35
  #height
  [$table component table] configure -rowheight 2
  pack $table -fill both -expand 1
  # puts [[$table component table] configure]
  #pack the table
  pack $table -fill both -expand 1
  #ContextMenu
  $table deleteContextMenu 0 end 
  set cmindex 0
  $table insertContextMenu $cmindex command \
    -label [msgcat::mc "Delete Frequency "] \
    -font $font(cn) \
    -underline 0 \
    -command [code deleteFrequency]
  #################################################
  grid $label_modeMethod -in $tab1 -row 0 -column 0  -sticky w 
  grid $combo_modeMethod -in $tab1 -row 0 -column 1  -sticky w -padx 2m

  grid $label_modeNumber -in $tab1 -row 1 -column 0  -sticky w
  grid $entry_modeNumber -in $tab1 -row 1 -column 1  -sticky w -padx 2m


  grid $btn_solveModal -in $tab1 -row 2 -column 0  -columnspan 2  
  grid $btn_readModal  -in $tab1 -row 3 -column 2  
  grid $btn_saveModal  -in $tab1 -row 3 -column 3  


  for {set i 0} {$i < 4} {incr i} {
    grid rowconfigure    $tab1 $i  -weight 1
    grid columnconfigure $tab1 $i -weight 1
  }

  pack $tab1 -side top -anchor nw -fill both -expand 1 -padx 5 -pady 5

  $combo_modeMethod  insert list end [list\
   [msgcat::mc "Block Lanczos "] "LANB" \
   [msgcat::mc "Subspace " ] "SUBSP"]  
}
proc modalExport {} \
{
  variable parent
  variable table
  variable tabledata
  variable projectname


  set rowsnum [$table index end row]
  if {$rowsnum==-1} {
    tk_messageBox  -parent $parent \
    -icon warning \
    -title [::msgcat::mc "Tips "] \
    -message  [::msgcat::mc "Solve Modal First! "]
    return
  }

  set modalfile [tk_getSaveFile -initialfile [format "%s_modal.txt" $projectname] -filetypes {{txt .txt}}]
  #if no file
  if {[string trim $modalfile]==""} {
    return 
  }

  set fl [open $modalfile w]
  #write the table
  for {set i 0} {$i <=$rowsnum} {incr i} {
      set num  $tabledata($i,0)  
      set fre  $tabledata($i,1)  
      puts $fl "$num $fre"
    }
  catch {close $fl} err

  set msg [tk_messageBox  -parent $parent \
    -icon info \
    -title [msgcat::mc "Tips "] \
    -message  [msgcat::mc " Save successful! "]]
}
proc importMcomResustFile {} \
{
  variable parent
  variable mcomfile
  variable projectname

  set selfile [tk_getOpenFile -initialfile [format "%s.mcom" $projectname] -filetypes {{mcom .mcom}}]
  #if no file
  if {[file exists $selfile]} {
    set mcomfile [string trim $selfile]
    readMcomResultFile
  }
}
proc addTabSpectrum {} \
{
  variable font
  variable png
 ################## 
  variable tabset
  variable parent
  variable buildtype
  variable intensity
  variable bmax
  variable tg
  variable gravity
  variable direction
  variable site
  variable basesite
  variable mcomfile

  set page [$tabset add]
  $tabset pageconfigure 1 \
      -label [msgcat::mc "Spectrum Analysis "]\
      -font $font(cn)\
      -command [code changeButtonState]

  set tab1 [frame $page.hold]

  set label_buildtype  [label $parent.label_buildtype \
        -font $font(cn) \
        -text [::msgcat::mc "Build Type "] ]

  set combo_buildtype [euidl::combobox $parent.combo_buildtype \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -command [list [code changeSpecCombobox] "buildtype"]\
         -variable [scope buildtype]]

  set label_intensity  [label $parent.label_intensity \
        -font $font(cn) \
        -text [::msgcat::mc "Earthquake Intensity "] ]

  set combo_intensity [euidl::combobox $parent.combo_intensity \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -command [list [code changeSpecCombobox] "intensity"]\
         -variable [scope intensity]]

  set label_gravity [euidl::label $parent.label_gravity \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Acceleration "]]

  set entry_gravity [euidl::entry  $parent.entry_gravity \
        -variable [scope gravity]\
        -font $font(cn)\
        -validate focusout\
        -validatedata real\
        -width 15]

  set label_basesite [euidl::label $parent.label_basesite \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Base Site Value "]]

  set combo_basesite [euidl::combobox $parent.combo_basesite \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -command [list [code changeSpecCombobox] "site"]\
         -variable [scope basesite]]

  set label_site [euidl::label $parent.label_site \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Site Class "]]

  set combo_site [euidl::combobox $parent.combo_site \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -command [list [code changeSpecCombobox] "site"]\
         -variable [scope site]]


  set label_bmax [euidl::label $parent.label_bmax \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Bmax "]]

  set entry_bmax [euidl::entry  $parent.entry_bmax \
        -width 10  \
        -variable [scope bmax] \
        -validate focusout\
        -width 15\
        -font $font(en)\
        -validatedata real]

  set label_tg [euidl::label $parent.label_tg \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Tg "]]

  set entry_tg [euidl::entry  $parent.entry_tg \
        -width 10  \
        -variable [scope tg] \
        -validate focusout\
        -width 15\
        -font $font(en)\
        -validatedata real]

  set btn_updatechart [euidl::button $parent.btn_updatechartl \
      -command [code updateSpectrumChart]   \
      -height 30\
      -width 50\
      -imagefile $png(chart) \
      -tooltip [msgcat::mc "Update Spectrum "] ]

  set label_direction [euidl::label $parent.label_direction \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "Load Direction "]]

  set combo_direction [euidl::combobox $parent.combo_direction \
         -exportselection 1 \
         -style dropdown \
         -listheight 100 \
         -width 13\
         -font $font(cn) \
         -variable [scope direction]]

  set btn_spectrumSolve [euidl::button $parent.btn_spectrumSolve \
      -command [code spectrumSolve]   \
      -height 30\
      -width 50\
      -imagefile $png(check)  \
      -tooltip [msgcat::mc "Solve Spectrum "] ]

  set label_readmcom [euidl::label $parent.label_readmcom \
        -height 2 \
        -font $font(cn) \
        -text [msgcat::mc "The McomFile "]]

  set entry_readmcom [entry  $parent.entry_readmcom \
        -textvariable [scope mcomfile] \
        -font $font(cn)]

  set btn_selectmcom [euidl::button $parent.btn_selectmcom \
        -command [code importMcomResustFile]   \
        -height 30\
        -width 40\
        -imagefile $png(import) \
        -tooltip [msgcat::mc "Select McomFile "] ]

  set btn_readmcom [euidl::button $parent.btn_readmcom \
        -command [code readMcomResultFile]   \
        -height 30\
        -width 40\
        -imagefile $png(update) \
        -tooltip [msgcat::mc "Read McomFile "] ]

 

  grid $label_buildtype -in $tab1 -row 0 -column 0  -sticky w
  grid $combo_buildtype -in $tab1 -row 0 -column 1  -sticky w  -padx 4m -pady 1m


  grid $label_direction -in $tab1 -row 0 -column 2  -sticky w  -padx 4m
  grid $combo_direction -in $tab1 -row 0 -column 3  -sticky w   -pady 1m

  grid $label_basesite -in $tab1 -row 1 -column 0  -sticky w 
  grid $combo_basesite -in $tab1 -row 1 -column 1  -sticky w  -padx 4m -pady 1m

  grid $label_site -in $tab1 -row 1 -column 2 -sticky w   -padx 4m
  grid $combo_site -in $tab1 -row 1 -column 3 -sticky w   -pady 1m

  grid $label_intensity -in $tab1 -row 2 -column 0  -sticky w  
  grid $combo_intensity -in $tab1 -row 2 -column 1  -sticky w   -padx 4m -pady 1m

  grid $label_gravity -in $tab1 -row 2 -column 2  -sticky w  -padx 4m
  grid $entry_gravity -in $tab1 -row 2 -column 3  -sticky w   -pady 1m

  grid $label_bmax -in $tab1 -row 3 -column 0  -sticky w 
  grid $entry_bmax -in $tab1 -row 3 -column 1  -sticky w -padx 4m 

  grid $label_tg -in $tab1 -row 3 -column 2  -sticky w -padx 4m
  grid $entry_tg -in $tab1 -row 3 -column 3  -sticky w 


  grid $btn_updatechart -in $tab1 -row 4 -column 0 -columnspan 2
  grid $btn_spectrumSolve -in $tab1 -row 4 -column 2  -columnspan 2

  grid $label_readmcom  -in $tab1 -row 5 -column 0  
  grid $entry_readmcom  -in $tab1 -row 5 -column 1  -columnspan 2 -sticky ew
  grid $btn_selectmcom  -in $tab1 -row 5 -column 3  -sticky w -padx 5m
  grid $btn_readmcom  -in $tab1 -row 5 -column 3  -sticky e -padx 5m
 

  $combo_buildtype  insert list end [list\
     [msgcat::mc "Earth Dam "] Earth\
     [msgcat::mc "Gravity Dam "] Gravity\
     [msgcat::mc "Arch Dam "] Arch\
     [msgcat::mc "Sluice IntaketTower "] Sluice]

  $combo_intensity  insert list end [list\
     "default (1.0g)" 1\
     "6 (0.05g)" 0.05\
     "7 (0.10g)" 0.10\
     "7 (0.15g)" 0.15\
     "8 (0.20g)" 0.20\
     "8 (0.30g)" 0.30\
     "9 (0.40g)" 0.40]


  $combo_basesite  insert list end [list\
     "0.35s" 1\
     "0.40s" 2\
     "0.45s" 3]  

  $combo_site  insert list end [list\
     "I0" 1\
     "I1" 2\
     "II" 3\
     "III" 4\
     "IV" 5]  

  $combo_direction  insert list end [list \
    "X" "1,0,0"\
    "Y" "0,1,0"\
    "Z" "0,0,1"]

  for {set i 0} {$i < 4} {incr i} {
    grid rowconfigure    $tab1 $i  -weight 1
    grid columnconfigure $tab1 $i -weight 1
  }

  pack $tab1 -side top -anchor nw -fill both -expand 1 -padx 5 -pady 5  
}


#switch the tab item
proc tabSwitch {arg} \
{

  variable tabset
  # variable table
  # variable parent

  set index [$tabset view]
  switch -exact -- $arg {
    Back {
      incr index -1
      $tabset view $index
    }
    Next {
      # set rowsnum [$table index end row]
      # if {$rowsnum==-1 && $index==0} {
      #   tk_messageBox  -parent $parent \
      #     -icon warning \
      #     -title [::msgcat::mc "Tips "] \
      #     -message  [::msgcat::mc "Solve Modal First! "]
      # return
      # }
      incr index
      $tabset view $index    
    }
  }
  changeButtonState
}

proc translateModalData {} \
{
  variable frequs
  variable values
  variable gravity


  set trfrequs  {}
  set trvalues  {}

  set datalist [GetSpecModalPoints]
  set frequs [lindex $datalist 0]
  set values [lindex $datalist 2]
  set num [llength $frequs]
  for {set i 0} {$i < $num} {incr i} {
    lappend trfrequs [expr [lindex $frequs $i]*$gravity]
    lappend trvalues [expr [lindex $values $i]*$gravity]
  }
  return [list $trfrequs $trvalues]
}
#change Back\Next Button State
proc changeButtonState {} \
{
    variable parent
    variable table
    variable tabset
    variable btnback
    variable btnnext

    set pagenow [$tabset view]
    set pageend [$tabset index end]
    set rowsnum [$table index end row]
    $btnnext configure -state [expr $pagenow==$pageend?"disabled":"normal"]
    $btnback configure -state [expr $pagenow==0?"disabled":"normal"]
}
proc changeSpecCombobox {args} \
{
  #####buildtype combobox
  if {$args=="buildtype"} {
      variable buildtype
      variable bmax
      switch -exact -- $buildtype {
        Earth {set bmax 1.60}
        Gravity {set bmax 2.0}
        Arch {set bmax 2.50}
        Sluice {set bmax 2.25}
      }
  }
  #####site &base site combobox
  if {$args=="site"} {
    variable basesite
    variable site
    variable tg
    set index "$basesite-$site" 
    switch -exact -- $index {
       1-1 {set tg  0.20}
       1-2 {set tg  0.25}
       1-3 {set tg  0.35}
       1-4 {set tg  0.45}
       1-5 {set tg  0.65}
       2-1 {set tg  0.25}
       2-2 {set tg  0.30}
       2-3 {set tg  0.40}
       2-4 {set tg  0.55}
       2-5 {set tg  0.75}
       3-1 {set tg  0.30}
       3-2 {set tg  0.35}
       3-3 {set tg  0.45}
       3-4 {set tg  0.65}
       3-5 {set tg  0.90}
     } 
    
  }
  #####intensity combobox
  if {$args=="intensity"} {
      variable intensity
      variable gravity
      set gravity [expr $intensity*9.8]
  }
}
#initialize the form
proc initialize {} \
{
  variable tabset
  variable btnback
  variable method
  variable number
  variable intensity
  variable basesite
  variable site
  variable buildtype
  variable bmax
  variable tg
  variable gravity
  variable direction
  variable projectname
  variable datafile
  variable mcomfile

  ## set the default method to subspace 
  set method "SUBSP"
  set number 10
  ##################
  set buildtype Earth
  set bmax 1.60
  ##################
  set intensity 0.10
  set basesite 1
  set site 3
  set tg 0.35
  ##################
  set gravity 9.8
  ###############
  set direction "1,0,0"
  ###################
  set datafile {}
  set projectname [string trim [ans_getvalue "ACTIVE,0,JOBNAM"]]
  set mcomfile [format "%s.mcom" $projectname]
  #set the first tab as the default
  $tabset view 0
  $btnback configure -state "disabled"
  #get the frequencies and show 
  modalShow
}

#tab0
proc modalSolve {} \
{
  variable method
  variable number
  variable parent

  if {[checkEnvironment]} {
    return
  }
  set msg ""
  set number [string trim $number]
  if {$number==""} {
    set msg "The Modal Number is Empty! "
  }
  if {$number>100} {
    set msg "The Biggest Modal Number is 100! "
  }
  if {$msg!=""} {
    tk_messageBox  -parent $parent \
    -icon warning \
    -title [::msgcat::mc "Tips "] \
    -message  $msg   
    return
  }
  catch {ans_sendcommand "ANTYPE,MODAL"} err
  catch {ans_sendcommand "MODOPT,$method,$number"} err
  catch {ans_sendcommand "MXPAND,$number, , ,1"} err
  catch {ans_sendcommand  "SOLVE"} err
  
  raise .spectrum
  after  500 [modalShow]
}
#tab0
proc modalShow {} \
{
  variable parent
  variable number
  variable table
  variable tabledata

  set rowsnum [$table index end row]
  #clear the table
  for {set i $rowsnum} {$i >=0 } {incr i -1} {
     $table delete rows $i
  }

  #fill the table
  for {set i 0} {$i < $number} {incr i} {
    set num [expr $i+1]
    set fre [ans_getvalue "MODE,$num,FREQ"]
    if {$fre!=0} {
      $table insert row end
      set tabledata($i,-1) {}
      set tabledata($i,0)  $num
      set tabledata($i,1)  $fre
    }
  }

  #enable the delete contextmenu
  if { [$table index end row]!=-1} {
        [$table getContextMenu] entryconfigure 0 -state normal
      }   
}

proc modalImport {} \
{
    variable table
    variable tabledata
    variable tabset
    variable parent
    variable projectname

    set filename [tk_getOpenFile \
      -parent $parent\
      -initialfile [format "%s_modal.txt" $projectname] \
      -filetypes {{Txt .txt}}]

    if {![file exists $filename]} {
      return
    }
    set rowsnum [$table index end row]
    #clear the table
    for {set i $rowsnum} {$i >=0 } {incr i -1} {
       $table delete rows $i
    }
    set fl [open $filename r]
    set data [read $fl]
    #close data file
    close $fl
    #insert file data
    set index 0
    foreach {id fre} $data {
        if {$fre!=0} {
        $table insert row end
        set tabledata($index,-1) {}
        set tabledata($index,0)  $id
        set tabledata($index,1)  $fre
        incr index
      }
    }
}
proc spectrumSolve {} \
{
  #input variable
  variable parent
  variable table
  variable direction
  

  #if no modal Frequency value is find
  if {[$table index end row]==-1} {
    tk_messageBox  -parent $parent \
    -icon warning \
    -title [::msgcat::mc "Tips "] \
    -message  [::msgcat::mc "Solve Modal First! "]
    return
  }
  if {[checkEnvironment]} {
    return
  }
  set transData [translateModalData]
  set frequs [lindex $transData 0]
  set values [lindex $transData 1]
  #apdl command
  # catch {ans_sendcommand "FINISH"} err
  # #Spectrum analysis
  # catch {ans_sendcommand "/SOLU"} err
  #Spectrum analysis
  catch {ans_sendcommand "ANTYPE,SPECTR"} err
  #Single point spectrum 
  catch {ans_sendcommand "SPOPT,SPRS"} err
  #Seismic acceleration response spectrum and factor
  catch {ans_sendcommand "SVTYP,2,1"} err
  #Global X-axis as spectrum direction
  catch {ans_sendcommand "SED,$direction"} err

  #clear all the input table
  catch {ans_sendcommand "FREQ" } err

  set commands(FREQ) {}
  lappend commands(FREQ) [lrange $frequs 0 8]
  lappend commands(FREQ) [lrange $frequs 9 18]
  lappend commands(FREQ) [lrange $frequs 19 20]


  set commands(SV,) {}
  lappend commands(SV,) [lrange $values 0 8]
  lappend commands(SV,) [lrange $values 9 18]
  lappend commands(SV,) [lrange $values 19 20]

  foreach name [lsort [array names commands]] {
    foreach command $commands($name) {
      if {[llength $command]!=0} {
        set command [concat $name $command]
        set command [join $command ","]
        catch {ans_sendcommand $command } err
      }
    }
  }
  ##mode combine default SRSS method
  catch {ans_sendcommand "SRSS,0.001,DISP,,STATIC"} err

  #Solve the spectrum value
  catch {ans_sendcommand "Solve"} err
  # catch {ans_sendcommand "FINISH"} err
  readMcomResultFile
}

####tab1
proc updateSpectrumChart {} \
{
  variable parent
  variable table
  variable plotshow


  set rowsnum [$table index end row]
  if {$rowsnum==-1} {
    tk_messageBox  -parent $parent \
    -icon warning \
    -title [::msgcat::mc "Tips "] \
    -message  [::msgcat::mc "Solve Modal First! "]
    return
  }
  set title  [::msgcat::mc "Design Response Spectrum "]
  set xlabel [::msgcat::mc "Time(s) "]
  set ylabel "[::msgcat::mc "Acceleration "]m/s^2"
  set geometry    [wm geometry $parent]


  set sepcCurve   [GetSpecCurvePoints]
  set specName [::msgcat::mc "Design Response Spectrum "]
  set xcurve      [lindex $sepcCurve 0]
  set ycurve      [lindex $sepcCurve 1]


  set specPonts   [GetSpecModalPoints]
  set SpecModalName [::msgcat::mc "Modal Spectrum "]
  set xmodal     [lindex $specPonts 1]
  set ymodal      [lindex $specPonts 2]


  set data  "\"<root  title='$title' geometry='$geometry' xlabel='$xlabel' ylabel='$ylabel'>
                <graphs>
                  <graph id='1'>
                    <xlist>$xcurve</xlist>
                    <ylist>$ycurve</ylist>
                    <style legend='$specName' line='black blue 0.3' />
                  </graph>
                  <graph id='2'>
                    <xlist>$xmodal</xlist>
                    <ylist>$ymodal</ylist>
                    <style legend='$SpecModalName' point='black blue 6' />
                  </graph>
                </graphs>
              </root>\""
  
  exec $plotshow $data
  after 300 [raise $parent]

}

proc GetSpecCurvePoints {} \
{

  set maxtime 3.0
  set space 0.05
  #get the space num
  set num [expr $maxtime/$space]
  set times {}
  set values {}
  #create the background line
  for {set i 0} {$i <= $num} {incr i} {
    set thetime [expr $space*$i]
    set thevalue [getModalData $thetime]
    lappend times $thetime
    lappend values $thevalue
  }
  return [list $times $values]
}


#####tab1
proc GetSpecModalPoints {} \
{
  #input
  variable table
  variable tabledata

  set num [$table index end row]
  if {$num==-1} {
    return {}
  } 
  set frequs {}
  set times  {}
  set values {}
  for {set i 0} {$i <= $num} {incr i} {
    set thefre  $tabledata($i,1)
    set thetime [expr 1.0/$thefre]
    set thevalue [getModalData $thetime]
    lappend frequs $thefre
    lappend times  $thetime
    lappend values $thevalue 
  }
  return  [list $frequs $times $values]
}

######NBT 35047-2015 page16######
proc getModalData {thetime} \
{

  variable bmax
  variable tg
  #get slope
  set slope [expr ($bmax-1.0)/0.1]
  ##calculate the acc
  if {$thetime<0.1} {
    set theval [expr $slope*$thetime+1.0]
  } elseif {$thetime>=0.1 && $thetime<=$tg} {
    set theval $bmax
  } else {
    set theval [expr $bmax*pow(($tg/$thetime),0.6)]
  }
  return $theval
}
proc readMcomResultFile {} \
{
  variable parent
  variable mcomfile

  #read the mcom file
  if {![file exists $mcomfile]} {
    tk_messageBox  \
      -parent $parent \
      -icon warning \
      -title [::msgcat::mc "Tips "] \
      -message  [::msgcat::mc "Can Not Find The mcom File! "]
    return  
  } else {
    catch {ans_sendcommand "/POST1" } err
    catch {ans_sendcommand "/INPUT,$mcomfile" } err
  }
  tk_messageBox  \
    -parent $parent \
    -icon warning \
    -title [::msgcat::mc "Tips "] \
    -message  [::msgcat::mc "McomFile Import Successfully! "]
}
proc deleteFrequency {} \
{
    variable table
    variable tabledata
    set selection [lsort -integer -decreasing  [$table curselection]]
    # If there is no selection then return
      if { [string match {} $selection] } {
      return
    }

    foreach sel $selection {
      $table delete rows $sel
    }
    # # If there are no more rows in the column heading, then disabled the
    # # delete button.
    if { [$table index end row]==-1} {
      [$table getContextMenu] entryconfigure 0 -state disabled
    }   
}
proc checkEnvironment {} \
{
    variable parent
    # array set env_dict {17 PREP7 21 SOLU 31 POST1}
    # 0 = Begin level,  17 = PREP7 21 = SOLUTION, 31 = POST1, 36 = POST26, 
    set env [ans_getvalue "ACTIVE,0,ROUT"]
    if {$env!=21 } {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -parent $parent\
        -message "[msgcat::mc "This Operation Need Environment: "] \n [msgcat::mc "SOLU"]" \
        -icon warning 
        return 1
    } 
    return 0
}


}

::euidl::Seismic::SpectrumAnalysis::main