package  require msgcat
namespace eval ::euidl::Seismic::SeismicLoadKit {

proc main {} {

	# variable png
	variable font
	variable parent
	variable btn
	variable path

	set path [file dirname [info script]]
	::msgcat::mcload  [file join $path locale]
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(en)  {Consolas 12 normal}

	# for {set i 6} {$i <= 7} {incr i} {
	# 	set "png($i)" [file join $path images toolkit  $i.png]
	# }
	if [winfo exists .seismicloadkit] {
	  wm deiconify .seismicloadkit
	  raise .seismicloadkit
	  return
	}
	set parent [toplevel .seismicloadkit]
	set font(cn)  {"Microsoft YaHei" 12 normal}
	set font(en)  {Consolas 12 normal}

	################################################
	wm title $parent [::msgcat::mc "Seismic Load Kit "]
	#adjust the location of localmat
	set x [expr {([winfo screenwidth  .]-260)/2}]
	set y [expr {([winfo screenheight .]-80)/2}]
	wm geometry .seismicloadkit 260x80+$x+$y
	wm protocol $parent WM_DELETE_WINDOW {
            destroy .seismicloadkit
      }

	##staticPressure
	set btn(Spectrum) [euidl::button $parent.btn_staticPressure \
        -command [list [code showSeismicModule] "SeismicSpectrum"]  \
     	-width 10\
     	-height 1\
     	-font $font(cn)\
     	-text [msgcat::mc "Spectrum Analysis "]\
        -tooltip [msgcat::mc "Spectrum Analysis "] ]

	set btn(TimeHistory) [euidl::button $parent.btn(TimeHistory) \
        -command [list [code showSeismicModule] "SeismicTimeHistory"]  \
     	-width 10\
     	-height 1\
     	-font $font(cn)\
     	-text [msgcat::mc "TimeHistory Analysis " ]\
        -tooltip [msgcat::mc "TimeHistory Analysis " ] ]


    grid $btn(Spectrum)     -row 0 -column 0 -padx 2m -pady 2m  
    grid $btn(TimeHistory)  -row 0 -column 1  -padx 2m -pady 2m
 
 	foreach name [array names btn] {
 	 	bind $btn($name) <Enter> [list [code  btn_enter] %W]
 		bind $btn($name) <Leave> [list [code  btn_leave] %W]	
 	}

}

proc showSeismicModule {modulename} \
{
	variable parent
	variable path
	
	if {[checkEnvironment]} {return}
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

proc checkEnvironment {} \
{
    variable parent
    # array set env_dict {17 PREP7 21 SOLU 31 POST1}
    # 0 = Begin level,  17 = PREP7 21 = SOLUTION, 31 = POST1, 36 = POST26, 
    set env [ans_getvalue "ACTIVE,0,ROUT"]
    if {$env!=17 && $env!=21} {
    tk_messageBox -title [msgcat::mc "Tips "] \
        -parent $parent\
        -message "[msgcat::mc "This Operation Need Environment: "] \n [msgcat::mc "PREP7"]\n [msgcat::mc "SOLU"]" \
        -icon warning 
        return 1
    } 
    return 0
}
}

::euidl::Seismic::SeismicLoadKit::main