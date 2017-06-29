package  require msgcat
namespace eval ::euidl::LoadToolKit {

proc main {} {

	variable parent
    # variable png
    variable font
    variable btn
    variable path

    set font(cn)  {"Microsoft YaHei" 12 normal}
    set font(en)  {Consolas 12 normal}
	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]

	# for {set i 1} {$i <= 6} {incr i} {
	# 	set "png($i)" [file join $path images toolkit  $i.png]
	# }
	if [winfo exists .loadtoolkit] {
      wm deiconify .loadtoolkit
	  raise .loadtoolkit
	  return
	}
	set parent [toplevel .loadtoolkit]
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(en)  {Consolas 12 normal}
	wm title $parent [::msgcat::mc "Load Tool Kit "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-300)/2}]
	set y [expr {([winfo screenheight .]-160)/2}]
	wm geometry .loadtoolkit 280x160+$x+$y
	wm protocol $parent WM_DELETE_WINDOW {
            destroy .loadtoolkit
      }

	##staticPressure
	set btn(staticPressure) [euidl::button $parent.btn(staticPressure) \
        -command [list [code showLoadTool] "PressureStatic"]  \
     	-width 10\
        -font $font(cn)\
        -height 1\
        -text [msgcat::mc "Hydrostatic Pressure "]\
        -tooltip [msgcat::mc "Hydrostatic Pressure "] ]

	set btn(dynamicPressure) [euidl::button $parent.btn(dynamicPressure) \
        -command [list [code showLoadTool] "PressureDynamic"]  \
        -width 10\
        -font $font(cn)\
        -height 1\
        -text [msgcat::mc "Dynamic Pressure " ]\
        -tooltip [msgcat::mc "Dynamic Pressure " ] ]


	set btn(ConcentratedForce) [euidl::button $parent.btn(ConcentratedForce) \
        -command [list [code showLoadTool] "PressureConcentrated"]  \
        -width 10\
        -font $font(cn)\
        -height 1\
        -text [msgcat::mc "Concentrated Force "]\
        -tooltip [msgcat::mc "Concentrated Force "] ]

	set btn(PressureUplift) [euidl::button $parent.pressureUplift \
        -command [list [code showLoadTool] "PressureUplift"]  \
        -width 10\
        -font $font(cn)\
        -height 1\
        -text [msgcat::mc "UpLift Pressure "]\
        -tooltip [msgcat::mc "UpLift Pressure "] ]

	set btn(PressureWave) [euidl::button $parent.pressureWave \
        -command [list [code showLoadTool] "PressureWave"]  \
        -width 10\
        -font $font(cn)\
        -height 1\
        -text [msgcat::mc "Wave Force "]\
        -tooltip [msgcat::mc "Wave Force "] ]

	set btn(ComponentManager) [euidl::button $parent.componentManager \
        -command [list [code showLoadTool] "ManagerComponent"]  \
        -width 10\
        -font $font(cn)\
        -height 1\
        -text [msgcat::mc "Component Manager "]\
        -tooltip [msgcat::mc "Component Manager "]]

    grid $btn(ComponentManager)   -row 0 -column 0 -padx 2m -pady 2m  
    grid $btn(staticPressure)     -row 0 -column 1  -padx 2m -pady 2m
    grid $btn(dynamicPressure)    -row 1 -column 0 -padx 2m -pady 2m  
    grid $btn(ConcentratedForce)  -row 1 -column 1 -padx 2m -pady 2m  
    grid $btn(PressureUplift)     -row 2 -column 0 -padx 2m -pady 2m  
    grid $btn(PressureWave)       -row 2 -column 1 -padx 2m -pady 2m  
    foreach name [array names btn] {
        bind $btn($name) <Enter> [list [code  btn_enter] %W]
        bind $btn($name) <Leave> [list [code  btn_leave] %W]    
    }

}

proc showLoadTool {modulename} \
{
	variable parent
    variable path
    
    if {[chekcEnvironment]} {return}
	# lower $parent
	ans_sendcommand $modulename
    destroy $parent
    # eval [exec [file join $path "AnsysScript.exe" ] $modulename $path]
}
proc btn_enter {control} \
{
    $control configure -background systemScrollbar
}

proc btn_leave {control} \
{
    $control configure -background systemWindow
}

proc chekcEnvironment {} \
{
    variable parent

    # array set env_dict {17 PREP7 21 SOLU 31 POST1}
    # 0 = Begin level,  17 = PREP7 21 = SOLUTION, 31 = POST1, 36 = POST26, 
    set env [ans_getvalue "ACTIVE,0,ROUT"]
    if {$env!=17 && $env!=21 } {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -parent $parent\
        -message "[msgcat::mc "This Operation Need Environment: "] \n [msgcat::mc "PREP7"]\n [msgcat::mc "SOLU"]" \
        -icon warning 
        return 1
    } 
    return 0
}

}

::euidl::LoadToolKit::main