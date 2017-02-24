#!/home/jiaqi/myers/bin/xatacs -f

#xATACS (VER5.0) C code:Chris Myers; Tcl/Tk code:Jeff Cuthbert

# Initialize some variables
set filename {}
set tempfilename {}
set component {}
set helpfunction {}
set defaultpath {}
set oripath [pwd]

# Set title and allow window resizing.
wm title . {LEMA (version 1.0)}

# Create menubar
frame .menubar -relief raised \
	-borderwidth 2

# File menu
menubutton .menubar.file \
	-text "File" \
	-underline 0 \
	-menu .menubar.file.menu 

pack .menubar.file \
	-side left

menu .menubar.file.menu 

.menubar.file.menu add cascade \
	-label "Load" \
	-underline 0 \
	-menu .menubar.file.menu.cas 

menu .menubar.file.menu.cas

.menubar.file.menu.cas add command \
	-label "VHDL-AMS" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+V" \
	-command {loadvhdlams "*.vhd"}
bind all {<Control-l> <Control-v>} {loadvhdlams "*.vhd"}

.menubar.file.menu.cas add command \
	-label "LHPN" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+L" \
	-command {load ll "*.g"}
bind all {<Control-l> <Control-l>} {load ll "*.g"}

.menubar.file.menu.cas add command \
	-label "SPICE Data" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+D" \
	-command {load ld "*.dat"}
bind all {<Control-l> <Control-d>} {load ld "*.dat"}

.menubar.file.menu add cascade \
	-label "Store" \
	-underline 0 \
	-menu .menubar.file.menu.cas2 

menu .menubar.file.menu.cas2

.menubar.file.menu.cas2 add command \
	-label "LHPN" \
	-underline 0 \
	-accelerator "  Ctrl+S Ctrl+L" \
	-command {process sl}
bind all {<Control-s> <Control-l>} {process sl}

.menubar.file.menu add cascade \
	-label "Display" \
	-underline 0 \
	-menu .menubar.file.menu.cas3 

menu .menubar.file.menu.cas3
	 
.menubar.file.menu.cas3 add command \
	-label "LHPN" \
	-underline 1 \
	-accelerator "  Ctrl+D Ctrl+L" \
	-command {process dl}
bind all {<Control-d> <Control-l>} {process dl}

.menubar.file.menu.cas3 add command \
	-label "State Graph" \
	-underline 0 \
	-accelerator "  Ctrl+D Ctrl+S" \
	-command {process ds}
bind all {<Control-d> <Control-s>} {process ds}

.menubar.file.menu add cascade \
	-label "Print" \
	-underline 0 \
	-menu .menubar.file.menu.cas6

menu .menubar.file.menu.cas6

.menubar.file.menu.cas6 add command \
	-label "LHPN" \
	-underline 0 \
	-accelerator "  Ctrl+P Ctrl-L" \
	-command {process pl}
bind all {<Control-p> <Control-l>} {process pl}

.menubar.file.menu.cas6 add command \
	-label "State Graph" \
	-underline 0 \
	-accelerator "  Ctrl+P Ctrl-S" \
	-command {process ps}
bind all {<Control-p> <Control-s>} {process ps}

.menubar.file.menu add command \
	-label "Quit" \
	-underline 0 \
	-accelerator "  Ctrl+Q" \
	-command { exit }
bind all <Control-q> exit 

# Verify menu
menubutton .menubar.tools \
	-text "Tools" \
	-underline 0 \
	-menu .menubar.tools.menu
pack .menubar.tools \
	-side left

menu .menubar.tools.menu

.menubar.tools.menu add command \
	-label "Verify" \
	-underline 0 \
	-accelerator "  Ctrl+T Ctrl+V" \
        -command {process va}
bind all {<Control-t> <Control-v>} {process va}

# Help menu
menubutton .menubar.help \
	-text "Help" \
	-underline 0 \
	-menu .menubar.help.menu 
pack .menubar.help \
	-side left

menu .menubar.help.menu 

.menubar.help.menu add cascade \
	-label "File" \
	-underline 0 \
	-menu .menubar.help.menu.cas1
menu .menubar.help.menu.cas1

.menubar.help.menu.cas1 add cascade \
	-label "Load" \
	-underline 0 \
	-menu .menubar.help.menu.cas1.cas1
menu .menubar.help.menu.cas1.cas1

.menubar.help.menu.cas1.cas1 add command \
	-label "VHDL-AMS" \
	-underline 0 \
	-command {set helpfunction loadvhdl; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "LHPN" \
	-underline 0 \
	-command {set helpfunction loadlpn; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "SPICE Data" \
	-underline 0 \
	-command {set helpfunction loaddata; help}

.menubar.help.menu.cas1 add cascade \
	-label "Store" \
	-underline 0 \
	-menu .menubar.help.menu.cas1.cas2
menu .menubar.help.menu.cas1.cas2

.menubar.help.menu.cas1.cas2 add command \
	-label "LHPN" \
	-underline 0 \
	-command {set helpfunction storelpn; help}

.menubar.help.menu.cas1 add cascade \
	-label "Display" \
	-underline 0 \
	-menu .menubar.help.menu.cas1.cas3
menu .menubar.help.menu.cas1.cas3

.menubar.help.menu.cas1.cas3 add command \
	-label "LHPN" \
	-underline 1 \
	-command {set helpfunction showlpn; help}

.menubar.help.menu.cas1.cas3 add command \
	-label "State Graph" \
	-underline 1 \
	-command {set helpfunction showrsg; help}

.menubar.help.menu.cas1 add cascade \
	-label "Print" \
	-underline 0 \
	-menu .menubar.help.menu.cas1.cas4
menu .menubar.help.menu.cas1.cas4

.menubar.help.menu.cas1.cas4 add command \
	-label "LHPN" \
	-underline 0 \
	-command {set helpfunction printlpn; help}

.menubar.help.menu.cas1.cas4 add command \
	-label "State Graph" \
	-underline 1 \
	-command {set helpfunction printrsg; help}

.menubar.help.menu.cas1 add command \
	-label "Quit" \
	-underline 0 \
	-command {set helpfunction quit; help}

.menubar.help.menu add cascade \
	-label "Tools" \
	-underline 0 \
	-menu .menubar.help.menu.cas4
menu .menubar.help.menu.cas4

.menubar.help.menu.cas4 add command \
	-label "Verify" \
	-underline 0 \
	-command {set helpfunction verify; help}

.menubar.help.menu add cascade \
        -label "Analysis engine" \
        -underline 0 \
        -menu .menubar.help.menu.cas7
menu .menubar.help.menu.cas7

.menubar.help.menu.cas7 add command \
        -label "DBMs" \
        -underline 0 \
        -command {set helpfunction lhpndbm; help}

.menubar.help.menu.cas7 add command \
        -label "BDDs" \
        -underline 0 \
        -command {set helpfunction lhpnbdd; help}

.menubar.help.menu add cascade \
	-label "Other options" \
	-underline 0 \
	-menu .menubar.help.menu.cas12
menu .menubar.help.menu.cas12

.menubar.help.menu.cas12 add command \
	-label "Dot" \
	-underline 0 \
	-command {set helpfunction dot; help}

.menubar.help.menu.cas12 add command \
	-label "Verbose" \
	-underline 0 \
	-command {set helpfunction verbose; help}

pack .menubar -side top \
	-fill x -expand false

# Create frames for filename, option buttons, and result log
frame .b1
frame .b1a
frame .b6
frame .b11
frame .b9b
frame .b7
frame .b12
pack .b1 .b6 .b7 .b12 -side top -expand 0 -fill x

# Field to display current design filename
label .b1.l -text "Current design:" -padx 0
entry .b1.cmd -width 60 -relief sunken \
	-textvariable filenamedisp
pack .b1.l .b1.cmd -side left

set but9 [button .b1.new -text "NEW" \
    -command {set filenamedisp {}; set tempfilename {}; set component {}}]
pack .b1.new -side right

# Timing method buttons
label .b7.l -text "Analysis engine:" -padx 0

radiobutton .b7.c1 -text "DBMs" \
        -variable timing_method \
        -value "Untimed" \
        -command {eval [concat X_interface tL]}

radiobutton .b7.c2 -text "BDDs" \
        -variable timing_method \
        -value "Geometric" \
        -command {eval [concat X_interface tB]}

pack .b7.l .b7.c1 .b7.c2 -side left 
.b7.c1 invoke

# Other option buttons
label .b12.l -text "Other options:" -padx 0

checkbutton .b12.c1 -text "Dot" \
	-variable Dot \
	-onvalue "x" \
	-offvalue "y" \
	-command {eval [concat X_interface od]}

checkbutton .b12.c3 -text "Verbose" \
	-variable Verbose \
	-onvalue "x" \
	-offvalue "y" \
	-command {eval [concat X_interface ov]}

pack .b12.l .b12.c1 .b12.c3 -side left

.b12.c1 invoke

# Create a text widget to log the output
message .msg -width 20c -justify left -relief raised -bd 1\
        -font -Adobe-Helvetica-Medium-R-Normal--*-120-* \
        -text "Result Log" 
pack .msg -side top -expand 0 -fill x
frame .t
set log [text .t.log -width 45 -height 18 \
	-borderwidth 2 -relief sunken -setgrid true\
	-yscrollcommand {.t.scroll set}]
scrollbar .t.scroll -command {.t.log yview}
pack .t.scroll -side right -fill y
pack .t.log -side left -fill both -expand true
pack .t -side top -fill both -expand true

# Procedures for file selection
proc getfile {} {

    global browser_struct filename component name filenamedisp

    set dir .
    set browser_struct(dir) $dir     
    set browser_struct(name) $filenamedisp
    set file [fileselect]
    set workpath [file dirname $file]
    cd $workpath
#    puts ""
#    puts "Current working directory is: [pwd]"
    set filename [file tail $file]
    set filenamedisp "[pwd]/$filename $component"
    browse $filename
}

proc browse {file} {
    set w .browser 
    global browser_struct filename
    if {[string compare $browser_struct(dir) "."] != 0} \
            {set file $browser_struct(dir)/$file}
    if [file isdirectory $file] {
        $w.list delete 0 end
        foreach i [exec ls -a $file/$browser_struct(filter)] {
           puts stdout $i
           $w.list insert end $i
        }
        set browser_struct(dir) $file
    } else {
        if [file isfile $file] {
            global $browser_struct(name)
            set filename [lindex [split $file .] 0]
            set $browser_struct(name) $file
        } else {
            puts stdout "\"$file\" isn't a directory or regular file"
        }
    }
}

proc fileselectResources {} {
    # path is used to enter the file name
    option add *Fileselect*path.relief		sunken	startup
    option add *Fileselect*path.background	lightgrey startup
    option add *Fileselect*path.foreground	black	startup
    # Text for the label on pathname/directory/component entry
    option add *Fileselect*l.text	       "File            :" startup
    option add *Fileselect*d.text		"Directory   :" startup
    option add *Fileselect*c.text		"Component:" startup
    # Text for the OK and Cancel buttons
    option add *Fileselect*ok*text		OK	startup
    option add *Fileselect*ok*underline		0	startup
    option add *Fileselect*cancel.text		Cancel	startup
    option add *Fileselect*cancel.underline 	0	startup
    # Size of the listbox
    option add *Fileselect*list.width		40	startup
    option add *Fileselect*list.height		10	startup
}

# fileselect returns the selected pathname, or {}
proc fileselect {{why ""} {default {}} {mustExist 1} } {
	global fileselect component defaultpath

	set t [toplevel .fileselect -bd 4 -class Fileselect]
	fileselectResources
    
	message $t.msg -aspect 1000 -text $why
	pack $t.msg -side top -fill x
    
	# Create a read-only entry for the current directory
        frame $t.direct
	set fileselect(dirEnt) [entry $t.direct.dir -width 15 \
		-relief flat -state disabled]
	label $t.direct.d -padx 0
	pack $t.direct -side top -fill x
	pack $t.direct.d -side left 
	pack $t.direct.dir -side right -fill x -expand true

	# Create an entry for the pathname
	# The value is kept in fileselect(path)
	frame $t.top
	label $t.top.l -padx 0
	set e [entry $t.top.path \
		-textvariable fileselect(path)]
	pack $t.top -side top -fill x
	pack $t.top.l -side left
	pack $t.top.path -side right -fill x -expand true
    
	# Create an entry for the component
	# The value is kept in component
	frame $t.comp
	label $t.comp.c -padx 0
	set ec [entry $t.comp.path \
		-textvariable component]
	pack $t.comp -side top -fill x
	pack $t.comp.c -side left
	pack $t.comp.path -side right -fill x -expand true

        # Create a listbox to hold the directory contents
	set lb [listbox $t.list \
		-yscrollcommand [list $t.scroll set]]
	scrollbar $t.scroll -command [list $lb yview]

	# Create the OK and Cancel buttons
	# The OK button has a rim to indicate it is the default
	frame $t.buttons -bd 10
	frame $t.buttons.ok -bd 2 -relief sunken
	set ok [button $t.buttons.ok.b \
		-command fileselectOK]
	set can [button $t.buttons.cancel \
		-command fileselectCancel]
	menubutton $t.buttons.ext  -relief sunken -bd 2 -text Ext \
                   -menu $t.buttons.ext.menu

	menu $t.buttons.ext.menu

        $t.buttons.ext.menu add command \
                -label VHD \
                -command {set fileselect(path) *.vhd ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label G \
                -command {set fileselect(path) *.g ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label ALL \
                -command {set fileselect(path) {} ; fileselectOK}
    
    	# Pack the list, scrollbar, and button box
	# in a horizontal stack below the upper widgets
	pack $t.list -side left -fill both -expand true
	pack $t.scroll -side left -fill y
	pack $t.buttons -side left -fill both
	pack $t.buttons.ok $t.buttons.cancel $t.buttons.ext\
		-side top -padx 10 -pady 5
	pack $t.buttons.ok.b -padx 4 -pady 4

	fileselectBindings $t $e $ec $lb $ok $can

	# Initialize variables and list the directory
	if {[string length $default] == 0} {
		set fileselect(path) $defaultpath
		set dir "[pwd]/"
	} else {
		set fileselect(path) [file tail $default]
		set dir [file dirname $default]
	}
	set fileselect(dir) {}
	set fileselect(done) 0
	set fileselect(mustExist) $mustExist

	# Wait for the listbox to be visible so
	# we can provide feedback during the listing 
	tkwait visibility .fileselect.list
	fileselectList $dir
	fileselectOK

	tkwait variable fileselect(done)
	destroy $t
	return $fileselect(path)
}

proc fileselectBindings { t e ec lb ok can } {
	# t - toplevel
	# e - name entry
	# lb - listbox
	# ok - OK button
	# can - Cancel button

	# Elimate the all binding tag because we
	# do our own focus management
	foreach w [list $e $lb $ok $can] {
	    bindtags $w [list $t [winfo class $w] $w]
	}
	# Dialog-global cancel binding
	bind $t <Control-c> fileselectCancel

	# Entry bindings
	bind $e <Return> fileselectOK
	bind $e <space> fileselectComplete

	# Entry bindings
	bind $ec <Return> fileselectOK

	# A single click, or <space>, puts the name in the entry
	# A double-click, or <Return>, selects the name
	bind $lb <space> "fileselectTake $%W ; focus $e"
	bind $lb <Button-1> \
		"fileselectClick %W %y ; focus $e"
	bind $lb <Return> "fileselectTake %W ; fileselectOK"
	bind $lb <Double-Button-1> \
		"fileselectClick %W %y ; fileselectOK"

	# Focus management.  	# <Return> or <space> selects the name.
	bind $e <Tab> "focus $lb ; $lb select set 0"
	bind $lb <Tab> "focus $e"

	# Button focus.  Extract the underlined letter
	# from the button label to use as the focus key.
	foreach but [list $ok $can] {
		set char [string tolower [string index  \
			[$but cget -text] [$but cget -underline]]]
		bind $t <Alt-$char> "focus $but ; break"
	}
	bind $ok <Tab> "focus $can"
	bind $can <Tab> "focus $ok"

	# Set up for type in
	focus $e
}

proc fileselectList { dir {files {}} } {
	global fileselect

	# Update the directory display
	set e $fileselect(dirEnt)
	$e config -state normal
	$e delete 0 end
	$e insert 0 $dir
	$e config -state disabled
	# scroll to view the tail end
	$e xview moveto 1

	.fileselect.list delete 0 end
	set fileselect(dir) $dir
	if ![file isdirectory $dir] {
	    .fileselect.list insert 0 "Bad Directory"
	    return
	}
	.fileselect.list insert 0 Listing...
	update idletasks
	.fileselect.list delete 0
	if {[string length $files] == 0} {
		# List the directory and add an
		# entry for the parent directory
		set files [glob -nocomplain $fileselect(dir)/*]
		.fileselect.list insert end ../
	}
	# Sort the directories to the front
	set dirs {}
	set others {}
	foreach f [lsort $files] {
		if [file isdirectory $f] {
			lappend dirs [file tail $f]/
		} else {
			lappend others [file tail $f]
		}
	}
	foreach f [concat $dirs $others] {
		.fileselect.list insert end $f
	}
}

proc fileselectOK {} {
    global fileselect 

    # Handle the parent directory specially
    if {[regsub {^\.\./?} $fileselect(path) {} newpath] != 0} {
	set fileselect(path) "$newpath/"
	set fileselect(dir) [file dirname $fileselect(dir)]
	fileselectOK
	return
    }
    
    set path $fileselect(dir)$fileselect(path)
    
    if [file isdirectory $path] {
	set fileselect(path) {}
	fileselectList $path
	return
    }
    if [file exists $path] {
	set fileselect(path) $path
	set fileselect(done) 1
	return
    }
    # Neither a file or a directory.
    # See if glob will find something
    if [catch {glob $path} files] {
	# No, perhaps the user typed a new
	# absolute pathname
	if [catch {glob $fileselect(path)} path] {
	    # Nothing good
	    if {$fileselect(mustExist)} {
		# Attempt completion
		fileselectComplete
	    } elseif [file isdirectory \
		    [file dirname $fileselect(path)]] {
		# Allow new name
		set fileselect(done) 1
	    }
	    return
	} else {
	    # OK - try again
	    set fileselect(dir) [file dirname $fileselect(path)]
	    set fileselect(path) [file tail $fileselect(path)]
	    fileselectOK
	    return
	}
    } else {
	# Ok - current directory is ok,
	# either select the file or list them.
	#if {[llength [split $files]] == 1} {
	#    puts "12"
	#    puts $files
	#    set fileselect(path) $files
	#    fileselectOK
	#} else {
	set fileselect(dir) "[file dirname [lindex $files 0]]/"
	lappend files /..
	fileselectList $fileselect(dir) $files
	#}
    }
}
proc fileselectCancel {} {
    global fileselect
    set fileselect(done) 1
    set fileselect(path) {}
}

proc fileselectClick { lb y } {
    # Take the item the user clicked on
    global fileselect
    set fileselect(path) [$lb get [$lb nearest $y]]
}
proc fileselectTake { lb } {
    # Take the currently selected list item
    global fileselect
    set fileselect(path) [$lb get [$lb curselection]]
}

proc fileselectComplete {} {
    global fileselect
    
    # Do file name completion
    # Nuke the space that triggered this call
    set fileselect(path) [string trim $fileselect(path) \t\ ]
    
    # Figure out what directory we are looking at
    # dir is the directory
    # tail is the partial name
    if {[string match /* $fileselect(path)]} {
	set dir "[file dirname $fileselect(path)]/"
	set tail [file tail $fileselect(path)]
    } elseif [string match ~* $fileselect(path)] {
	if [catch {file dirname $fileselect(path)} dir] {
	    return	;# Bad user
	}
	set tail [file tail $fileselect(path)]
    } else {
	set path $fileselect(dir)/$fileselect(path)
	set dir "[file dirname $path]/"
	set tail [file tail $path]
    }
    # See what files are there
    set files [glob -nocomplain $dir/$tail*]
    if {[llength [split $files]] == 1} {
	# Matched a single file
	set fileselect(dir) $dir
	set fileselect(path) [file tail $files]
    } else {
	if {[llength [split $files]] > 1} {
	    # Find the longest common prefix
	    set l [expr [string length $tail]-1]
	    set miss 0
	    # Remember that files has absolute paths
	    set file1 [file tail [lindex $files 0]]
	    while {!$miss} {
		incr l
		if {$l == [string length $file1]} {
		    # file1 is a prefix of all others
		    break
		}
		set new [string range $file1 0 $l]
		foreach f $files {
		    if ![string match $new* [file tail $f]] {
			set miss 1
			incr l -1
			break
		    }
		}
	    }
	    set fileselect(path) [string range $file1 0 $l]
	}
	lappend files /..
	fileselectList $dir $files
    }
}

# Procedure to display help
proc help {} {
    global helpfunction temp2filename errfilename
    global prsinput prslog env
    set temp1 {xterm -e man -M }
    global temp2filename tempfilename errfilename input log oripath  
    set command [append temp1 $env(ATACS) " " $helpfunction]
    if [catch {open "|$command |& cat"} prsinput] {
#		$log insert end $input\n
    } else {
	fileevent $prsinput readable ReptLog
#		$log insert end $command\n
    }
}

# Procedure to load a new design file
proc load {com filter} {
    global filename tempfilename filenamedisp pwd component defaultpath

    set defaultpath $filter
    getfile 
    if {$filename != {}} then { \
       eval [concat X_interface $com $filename $component]
       if {$component != {}} then { \
	   set tempfilename $component
       } else {
           if {$filename != {}} then { \
	       set tempfilename $filename }
       }
       More
       set filename {}; set component {}
    }
}

# Procedure to load a new design file
proc loadvhdlams {filter} {
    global filename tempfilename filenamedisp pwd component defaultpath

    set defaultpath $filter
    getfile 
    if {$filename != {}} then { \
       eval [concat X_interface lv $filename $component]
       if {$component != {}} then { \
	   set tempfilename $component
       } else {
           if {$filename != {}} then { \
	       set tempfilename $filename }
       }
       More
       set filename {}; set component {}
       eval [concat X_interface sl]
       eval [concat X_interface ll]
    }
}

# Procedure to process commands
proc process {com} {
    global filename tempfilename filenamedisp component
    if {$tempfilename == {}} then { 
	getfile 
        set tempfilename $filename
    }
    if {$component != {}} then { 
	    set tempfilename $component
    } else { 
      if {$filename != {}} then { 
	  set tempfilename $filename }
    }
    eval [concat X_interface $com $filename $component]
    More
    set filename {}; set component {}
}

# Read and log output from the program
proc Log {} {
	global input log
	if [eof $input] {
	    Stopfilename
	} else {
	    gets $input line
	    if {$line != ""} {
		$log insert end $line\n
		$log see end
	    }
	}
}

# Stop the program and fix up the button
proc Stopfilename {} {
	global input 
	catch {close $input}
}

# More the contents of the command field
proc More {} {
        set temp1 {more }
	global input log
	set command [append temp1 "atacs.log"]
	if [catch {open "|$command |& cat"} input] {
#		$log insert end $input\n
	} else {
		fileevent $input readable Log
#		$log insert end $command\n
	        
	}
}

# Read and log output from the program
proc ReptLog {} {
	global prsinput prslog
	if [eof $prsinput] {
		Stopprsfilename
	} else {
      		gets $prsinput line
	        if {$line != ""} {
		    $prslog insert end $line\n
		    $prslog see end
		}
	}
}

# Stop the program and fix up the button
proc Stopprsfilename {} {
	global prsinput 
	catch {close $prsinput}
}

bind all {<Control-c><Control-b>} {destroy .browser}
