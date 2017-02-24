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
wm title . {Xatacs (version 5.0)}

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
	-label "FILE" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+F" \
	-command {set defaultpath {}; getfile}
bind all {<Control-l> <Control-f>} \
	{set defaultpath {}; getfile} 

.menubar.file.menu.cas add command \
	-label "CSP" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+C" \
	-command {load lc "*.csp"}
bind all {<Control-l> <Control-c>} {load lc "*.csp"}

.menubar.file.menu.cas add command \
	-label "HSE" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+H" \
	-command {load lh "*.hse"}
bind all {<Control-l> <Control-h>} {load lh "*.hse"}

.menubar.file.menu.cas add command \
	-label "VHDL" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+V" \
	-command {load lv "*.vhd"}
bind all {<Control-l> <Control-v>} {load lv "*.vhd"}

.menubar.file.menu.cas add command \
	-label "GRAPH" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+G" \
	-command {load lg "*.g"}
bind all {<Control-l> <Control-g>} {load lg "*.g"}

.menubar.file.menu.cas add command \
	-label "XBM" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+X" \
	-command {load lx "*.unc"}
bind all {<Control-l> <Control-x>} {load lx "*.unc"}

.menubar.file.menu.cas add command \
	-label "ER" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+E" \
	-command {load le "*.er"}
bind all {<Control-l> <Control-e>} {load le "*.er"}

.menubar.file.menu.cas add command \
	-label "TEL" \
	-underline 0 \
	-accelerator "  Ctrl+L Ctrl+T" \
	-command {load lt "*.tel"}
bind all {<Control-l> <Control-e>} {load lt "*.tel"}

.menubar.file.menu.cas add command \
	-label "RSG" \
	-underline 1 \
	-accelerator "  Ctrl+L Ctrl+S" \
	-command {load ls "*.rsg"}
bind all {<Control-l> <Control-s>} {load ls "*.rsg"}

.menubar.file.menu add cascade \
	-label "Store" \
	-underline 0 \
	-menu .menubar.file.menu.cas2 

menu .menubar.file.menu.cas2

.menubar.file.menu.cas2 add command \
	-label "GRAPH" \
	-underline 0 \
	-accelerator "  Ctrl+S Ctrl+G" \
	-command {process sg}
bind all {<Control-s> <Control-g>} {process sg}

.menubar.file.menu.cas2 add command \
	-label "ER" \
	-underline 0 \
	-accelerator "  Ctrl+S Ctrl+E" \
	-command {process se}
bind all {<Control-s> <Control-e>} {process se}

.menubar.file.menu.cas2 add command \
	-label "RSG" \
	-underline 1 \
	-accelerator "  Ctrl+S Ctrl+S" \
	-command {process ss}
bind all {<Control-s> <Control-s>} {process ss}

.menubar.file.menu.cas2 add command \
	-label "STate Graph" \
	-underline 1 \
	-accelerator "  Ctrl+S Ctrl+M" \
	-command {process st}
bind all {<Control-s> <Control-m>} {process sm}

.menubar.file.menu.cas2 add command \
	-label "PRS" \
	-underline 0 \
	-accelerator "  Ctrl+S Ctrl+P" \
	-command {process2 sp .prs}
bind all {<Control-s> <Control-p>} {store2 sp .prs}

.menubar.file.menu.cas2 add command \
	-label "NET" \
	-underline 0 \
	-accelerator "  Ctrl+S Ctrl+N" \
	-command {process2 sn .net}
bind all {<Control-s> <Control-n>} {store2 sn .net}

.menubar.file.menu.cas2 add command \
	-label "VHDL" \
	-underline 0 \
	-accelerator "  Ctrl+S Ctrl+V" \
	-command {process2 sv S.vhd}
bind all {<Control-s> <Control-v>} {store2 sv S.vhd}

.menubar.file.menu add cascade \
	-label "Display" \
	-underline 0 \
	-menu .menubar.file.menu.cas3 

menu .menubar.file.menu.cas3
	 
.menubar.file.menu.cas3 add command \
	-label "ER" \
	-underline 1 \
	-accelerator "  Ctrl+D Ctrl+E" \
	-command {process de}
bind all {<Control-d> <Control-e>} {process de}

.menubar.file.menu.cas3 add command \
	-label "RSG" \
	-underline 1 \
	-accelerator "  Ctrl+D Ctrl+S" \
	-command {process ds}
bind all {<Control-d> <Control-s>} {process ds}

.menubar.file.menu.cas3 add command \
	-label "STATz" \
	-underline 4 \
	-accelerator "  Ctrl+D Ctrl+Z" \
	-command {process dz}
bind all {<Control-d> <Control-z>} {process dz}

.menubar.file.menu.cas3 add command \
	-label "NET" \
	-underline 0 \
	-accelerator "  Ctrl+D Ctrl+N" \
	-command {process dn}
bind all {<Control-d> <Control-n>} {process dn}

.menubar.file.menu add cascade \
	-label "Print" \
	-underline 0 \
	-menu .menubar.file.menu.cas6

menu .menubar.file.menu.cas6

.menubar.file.menu.cas6 add command \
	-label "ER" \
	-underline 0 \
	-accelerator "  Ctrl+P Ctrl-E" \
	-command {process pe}
bind all {<Control-p> <Control-e>} {process pe}

.menubar.file.menu.cas6 add command \
	-label "RSG" \
	-underline 1 \
	-accelerator "  Ctrl+P Ctrl-S" \
	-command {process ps}
bind all {<Control-p> <Control-s>} {process ps}

.menubar.file.menu.cas6 add command \
	-label "STATz" \
	-underline 4 \
	-accelerator "  Ctrl+P Ctrl+Z" \
	-command {process pz}
bind all {<Control-d> <Control-z>} {process pz}

.menubar.file.menu.cas6 add command \
	-label "REG" \
	-underline 0 \
	-accelerator "  Ctrl+P Ctrl-R" \
	-command {process pr}
bind all {<Control-p> <Control-r>} {process pr}

.menubar.file.menu.cas6 add command \
	-label "NET" \
	-underline 0 \
	-accelerator "  Ctrl+P Ctrl-N" \
	-command {process pn}
bind all {<Control-p> <Control-n>} {process pn}

.menubar.file.menu add command \
	-label "Quit" \
	-underline 0 \
	-accelerator "  Ctrl+Q" \
	-command { exit }
bind all <Control-q> exit 

# Synthesis menu
menubutton .menubar.synth \
	-text "Synthesis" \
	-underline 1 \
	-menu .menubar.synth.menu 
pack .menubar.synth \
	-side left

menu .menubar.synth.menu 

.menubar.synth.menu add command \
	-label "Using Single Cube Algorithm" \
	-underline 6 \
	-accelerator "  Ctrl+Y Ctrl+S" \
	-command {process2 ys .prs}
bind all {<Control-y> <Control-s>} {process2 ys .prs}

.menubar.synth.menu add command \
	-label "Using Multicube Algorithm" \
	-underline 6 \
	-accelerator "  Ctrl+Y Ctrl+M" \
	-command {process2 ym .prs}
bind all {<Control-y> <Control-m>} {process2 ym .prs}

.menubar.synth.menu add command \
	-label "Using BDD Algorithm" \
	-underline 6 \
	-accelerator "  Ctrl+Y Ctrl+B" \
	-command {process2 yb .prs}
bind all {<Control-y> <Control-b>} {process2 yb .prs}

.menubar.synth.menu add command \
	-label "Using Direct Algorithm" \
	-underline 6 \
	-accelerator "  Ctrl+Y Ctrl+D" \
	-command {process2 yd .prs}
bind all {<Control-y> <Control-d>} {process2 yd .prs}

# Analysis menu
menubutton .menubar.timing \
	-text "Analysis" \
	-underline 0 \
	-menu .menubar.timing.menu
pack .menubar.timing \
	-side left

menu .menubar.timing.menu

.menubar.timing.menu add command \
	-label "Simulate Cycle Period" \
	-underline 0 \
	-accelerator "  Ctrl+A Ctrl+S" \
	-command {process as}
bind all {<Control-a> <Control-s>} {process as}

.menubar.timing.menu add command \
	-label "Compute Cycle Period Profile" \
	-underline 21 \
	-accelerator "  Ctrl+A Ctrl+P" \
	-command {process ap}
bind all {<Control-a> <Control-p>} {process ap}

# Tools menu
menubutton .menubar.tools \
	-text "Verify" \
	-underline 0 \
	-menu .menubar.tools.menu
pack .menubar.tools \
	-side left

menu .menubar.tools.menu

.menubar.tools.menu add command \
	-label "Using ATACS" \
	-underline 6 \
	-accelerator "  Ctrl+V Ctrl+A" \
	-command {process va}
bind all {<Control-v> <Control-a>} {process va}

.menubar.tools.menu add command \
	-label "Using ORBITS" \
	-underline 6 \
	-accelerator "  Ctrl+V Ctrl+O" \
	-command {process vo}
bind all {<Control-v> <Control-o>} {process vo}

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
	-label "FILE" \
	-underline 0 \
	-command {set helpfunction loadfile; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "CSP" \
	-underline 0 \
	-command {set helpfunction loadcsp; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "HSE" \
	-underline 0 \
	-command {set helpfunction loadhse; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "VHDL" \
	-underline 0 \
	-command {set helpfunction loadvhdl; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "GRAPH" \
	-underline 0 \
	-command {set helpfunction loadg; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "XBM" \
	-underline 0 \
	-command {set helpfunction loadxbm; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "ER" \
	-underline 0 \
	-command {set helpfunction loader; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "TEL" \
	-underline 0 \
	-command {set helpfunction loadtel; help}

.menubar.help.menu.cas1.cas1 add command \
	-label "RSG" \
	-underline 1 \
	-command {set helpfunction loadrsg; help}

.menubar.help.menu.cas1 add cascade \
	-label "Store" \
	-underline 0 \
	-menu .menubar.help.menu.cas1.cas2
menu .menubar.help.menu.cas1.cas2

.menubar.help.menu.cas1.cas2 add command \
	-label "GRAPH" \
	-underline 0 \
	-command {set helpfunction storeg; help}

.menubar.help.menu.cas1.cas2 add command \
	-label "ER" \
	-underline 0 \
	-command {set helpfunction storeer; help}

.menubar.help.menu.cas1.cas2 add command \
	-label "RSG" \
	-underline 1 \
	-command {set helpfunction storersg; help}

.menubar.help.menu.cas1.cas2 add command \
	-label "STate Graph" \
	-underline 2 \
	-command {set helpfunction storesg; help}

.menubar.help.menu.cas1.cas2 add command \
	-label "PRS" \
	-underline 0 \
	-command {set helpfunction storeprs; help}

.menubar.help.menu.cas1.cas2 add command \
	-label "NET" \
	-underline 0 \
	-command {set helpfunction storenet; help}

.menubar.help.menu.cas1.cas2 add command \
	-label "VHDL" \
	-underline 0 \
	-command {set helpfunction storevhdl; help}

.menubar.help.menu.cas1 add cascade \
	-label "Display" \
	-underline 0 \
	-menu .menubar.help.menu.cas1.cas3
menu .menubar.help.menu.cas1.cas3

.menubar.help.menu.cas1.cas3 add command \
	-label "ER" \
	-underline 1 \
	-command {set helpfunction shower; help}

.menubar.help.menu.cas1.cas3 add command \
	-label "RSG" \
	-underline 1 \
	-command {set helpfunction showrsg; help}

.menubar.help.menu.cas1.cas3 add command \
	-label "STATz" \
	-underline 4 \
	-command {set helpfunction showstat; help}

.menubar.help.menu.cas1.cas3 add command \
	-label "NET" \
	-underline 0 \
	-command {set helpfunction shownet; help}

.menubar.help.menu.cas1 add cascade \
	-label "Print" \
	-underline 0 \
	-menu .menubar.help.menu.cas1.cas4
menu .menubar.help.menu.cas1.cas4

.menubar.help.menu.cas1.cas4 add command \
	-label "ER" \
	-underline 0 \
	-command {set helpfunction printer; help}

.menubar.help.menu.cas1.cas4 add command \
	-label "RSG" \
	-underline 1 \
	-command {set helpfunction printrsg; help}

.menubar.help.menu.cas1.cas4 add command \
	-label "STATz" \
	-underline 4 \
	-command {set helpfunction printstat; help}

.menubar.help.menu.cas1.cas4 add command \
	-label "REG" \
	-underline 0 \
	-command {set helpfunction printreg; help}

.menubar.help.menu.cas1.cas4 add command \
	-label "NET" \
	-underline 0 \
	-command {set helpfunction printnet; help}

.menubar.help.menu.cas1 add command \
	-label "Quit" \
	-underline 0 \
	-command {set helpfunction quit; help}

.menubar.help.menu add cascade \
	-label "Synthesis" \
	-underline 1 \
	-menu .menubar.help.menu.cas2
menu .menubar.help.menu.cas2

.menubar.help.menu.cas2 add command \
	-label "Using Single Cube Algorithm" \
	-underline 6 \
	-command {set helpfunction single; help}

.menubar.help.menu.cas2 add command \
	-label "Using Multicube Algorithm" \
	-underline 6 \
	-command {set helpfunction multicube; help}

.menubar.help.menu.cas2 add command \
	-label "Using BDD Algorithm" \
	-underline 6 \
	-command {set helpfunction bdd; help}

.menubar.help.menu.cas2 add command \
	-label "Using Direct Algorithm" \
	-underline 6 \
	-command {set helpfunction direct; help}

.menubar.help.menu add cascade \
	-label "Verify" \
	-underline 0 \
	-menu .menubar.help.menu.cas4
menu .menubar.help.menu.cas4

.menubar.help.menu.cas4 add command \
	-label "Using ATACS" \
	-underline 6 \
	-command {set helpfunction verify; help}

.menubar.help.menu.cas4 add command \
	-label "Using ORBITS" \
	-underline 6 \
	-command {set helpfunction verorbits; help}

.menubar.help.menu add cascade \
	-label "Analysis" \
	-underline 0 \
	-menu .menubar.help.menu.cas5
menu .menubar.help.menu.cas5

.menubar.help.menu.cas5 add command \
	-label "Simulate Cycle Period" \
	-underline 0 \
	-command {set helpfunction simcyc; help}

.menubar.help.menu.cas5 add command \
	-label "Compute Cycle Period Profile" \
	-underline 0 \
	-command {set helpfunction profile; help}

.menubar.help.menu add cascade \
	-label "Timing method" \
	-underline 0 \
	-menu .menubar.help.menu.cas7
menu .menubar.help.menu.cas7

.menubar.help.menu.cas7 add command \
	-label "Untimed" \
	-underline 0 \
	-command {set helpfunction untimed; help}

.menubar.help.menu.cas7 add command \
	-label "Geometric" \
	-underline 0 \
	-command {set helpfunction geometric; help}

.menubar.help.menu.cas7 add command \
	-label "POSETs" \
	-underline 0 \
	-command {set helpfunction posets; help}

.menubar.help.menu.cas7 add command \
	-label "BAG" \
	-underline 0 \
	-command {set helpfunction bag; help}

.menubar.help.menu.cas7 add command \
	-label "BAP" \
	-underline 2 \
	-command {set helpfunction bap; help}

.menubar.help.menu.cas7 add command \
	-label "BAPTDC" \
	-underline 3 \
	-command {set helpfunction baptdc; help}

.menubar.help.menu add cascade \
	-label "Timing options" \
	-underline 0 \
	-menu .menubar.help.menu.cas8
menu .menubar.help.menu.cas8

.menubar.help.menu.cas8 add command \
	-label "Abstract" \
	-underline 0 \
	-command {set helpfunction abstract; help}

.menubar.help.menu.cas8 add command \
	-label "Partial Order" \
	-underline 0 \
	-command {set helpfunction pored; help}

.menubar.help.menu add cascade \
	-label "Technology" \
	-underline 0 \
	-menu .menubar.help.menu.cas11
menu .menubar.help.menu.cas11

.menubar.help.menu.cas11 add command \
	-label "AtomicGates" \
	-underline 0 \
	-command {set helpfunction atomic; help}

.menubar.help.menu.cas11 add command \
	-label "Generalized-C" \
	-underline 0 \
	-command {set helpfunction gc; help}

.menubar.help.menu.cas11 add command \
	-label "Standard-C" \
	-underline 0 \
	-command {set helpfunction gatelevel; help}

.menubar.help.menu.cas11 add command \
	-label "BM gC" \
	-underline 0 \
	-command {set helpfunction burstgc; help}

.menubar.help.menu.cas11 add command \
	-label "BM 2-level" \
	-underline 0 \
	-command {set helpfunction burst2l; help}

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

.menubar.help.menu.cas12 add command \
	-label "Quiet" \
	-underline 0 \
	-command {set helpfunction noparg; help}

.menubar.help.menu.cas12 add command \
	-label "NoInsert" \
	-underline 0 \
	-command {set helpfunction insertion; help}

.menubar.help.menu.cas12 add command \
	-label "MaxSize" \
	-underline 0 \
	-command {set helpfunction maxsize; help}

.menubar.help.menu.cas12 add command \
	-label "GateDelay" \
	-underline 0 \
	-command {set helpfunction gatedelay; help}

pack .menubar -side top \
	-fill x -expand false

# Create frames for filename, option buttons, and result log
frame .b1
frame .b1a
frame .b6
frame .b7
frame .b10
frame .b9
frame .b11
frame .b9b
frame .b12
pack .b1 .b6 .b7 .b10 .b9 .b11 .b9b .b12 -side top -expand 0 -fill x

# Field to display current design filename
label .b1.l -text "Current design:" -padx 0
entry .b1.cmd -width 60 -relief sunken \
	-textvariable filenamedisp
pack .b1.l .b1.cmd -side left

set but9 [button .b1.new -text "NEW" \
    -command {set filenamedisp {}; set tempfilename {}; set component {}}]
pack .b1.new -side right

# Timing method buttons
label .b7.l -text "Timing method:" -padx 0

radiobutton .b7.c1 -text "Untimed" \
	-variable timing_method \
	-value "Untimed" \
	-command {eval [concat X_interface tu]}

radiobutton .b7.c2 -text "Geometric" \
	-variable timing_method \
	-value "Geometric" \
	-command {eval [concat X_interface tg]}

radiobutton .b7.c3 -text "POSETs" \
	-variable timing_method \
	-value "POSETs" \
	-command {eval [concat X_interface ts]}

radiobutton .b7.c4 -text "BAG" \
	-variable timing_method \
	-value "BAG" \
	-command {eval [concat X_interface tb]}

radiobutton .b7.c5 -text "BAP" \
	-variable timing_method \
	-value "BAP" \
	-command {eval [concat X_interface tp]}

radiobutton .b7.c6 -text "BAPTDC" \
	-variable timing_method \
	-value "BAPTDC" \
	-command {eval [concat X_interface tt]}

pack .b7.l .b7.c1 .b7.c2 .b7.c3 .b7.c4 .b7.c5 .b7.c6 -side left
.b7.c1 invoke

# Timing option buttons
label .b10.l -text "Timing options:" -padx 0

checkbutton .b10.c1 -text "Abstract" \
	-variable Abstract \
	-onvalue "x" \
	-offvalue "y" \
	-command {eval [concat X_interface oa]}

checkbutton .b10.c2 -text "Partial Order" \
	-variable PO \
	-onvalue "x" \
	-offvalue "y" \
	-command {eval [concat X_interface op]}

pack .b10.l .b10.c1 .b10.c2 -side left

# Technology buttons
label .b9b.l -text "Technology:" -padx 0

radiobutton .b9b.c1 -text "AtomicGates" \
	-variable technology \
	-value "AtomicGates" \
	-command {eval [concat X_interface ot]}

radiobutton .b9b.c2 -text "Generalized-C" \
	-variable technology \
	-value "GencC" \
	-command {eval [concat X_interface og]}

radiobutton .b9b.c3 -text "Standard-C" \
	-variable technology \
	-value "StandardC" \
	-command {eval [concat X_interface os]}

radiobutton .b9b.c4 -text "BM gC" \
	-variable technology \
	-value "BM gC" \
	-command {eval [concat X_interface ob]}

radiobutton .b9b.c5 -text "BM 2-level" \
	-variable technology \
	-value "BM 2-level" \
	-command {eval [concat X_interface ol]}

pack .b9b.l .b9b.c1 .b9b.c2 .b9b.c3 .b9b.c4 .b9b.c5 -side left
.b9b.c2 invoke

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

checkbutton .b12.c4 -text "Quiet" \
	-variable NoParg \
	-onvalue "x" \
	-offvalue "y" \
	-command {eval [concat X_interface oq]}

checkbutton .b12.c5 -text "NoInsert" \
	-variable Insertion \
	-onvalue "x" \
	-offvalue "y" \
	-command {eval [concat X_interface oi]}

# Create a labeled entry for maxsize
entry .b12.cmd2 -width 3 -relief sunken \
	-textvariable Maxsize
set Maxsize 4

menubutton .b12.maxsize \
	-text "MaxSize" \
	-underline 0 \
	-menu .b12.maxsize.com1

menu .b12.maxsize.com1

.b12.maxsize.com1 add command \
	-label "2" \
	-command {set Maxsize 2; eval [concat X_interface M $Maxsize]}

.b12.maxsize.com1 add command \
	-label "3" \
	-command {set Maxsize 3; eval [concat X_interface M $Maxsize]}

.b12.maxsize.com1 add command \
	-label "4" \
	-command {set Maxsize 4; eval [concat X_interface M $Maxsize]}

.b12.maxsize.com1 add command \
	-label "5" \
	-command {set Maxsize 5; eval [concat X_interface M $Maxsize]}

.b12.maxsize.com1 add command \
	-label "6" \
	-command {set Maxsize 6; eval [concat X_interface M $Maxsize]}

.b12.maxsize.com1 add command \
	-label "Other" \
	-underline 0 \
	-command {eval [concat X_interface M $Maxsize]}

# Create a labeled entry for gatedelay
entry .b12.cmd3 -width 7 -relief sunken \
	-textvariable Gatedelay
set Gatedelay "0 inf"

menubutton .b12.gatedelay \
	-text "GateDelay" \
	-underline 0 \
	-menu .b12.gatedelay.com1

menu .b12.gatedelay.com1

.b12.gatedelay.com1 add command \
	-label "0 1" \
	-command {set Gatedelay "0 1"; eval [concat X_interface G $Gatedelay]}


.b12.gatedelay.com1 add command \
	-label "1 2" \
	-command {set Gatedelay "1 2"; eval [concat X_interface G $Gatedelay]}

.b12.gatedelay.com1 add command \
	-label "2 5" \
	-command {set Gatedelay "2 5"; eval [concat X_interface G $Gatedelay]}

.b12.gatedelay.com1 add command \
	-label "5 10" \
	-command {set Gatedelay "5 10"; eval [concat X_interface G $Gatedelay]}

.b12.gatedelay.com1 add command \
	-label "10 20" \
	-command {set Gatedelay "10 20";eval [concat X_interface G $Gatedelay]}

.b12.gatedelay.com1 add command \
	-label "0 inf" \
	-command {set Gatedelay "0 inf";eval [concat X_interface G $Gatedelay]}

.b12.gatedelay.com1 add command \
	-label "Other" \
	-underline 0 \
	-command {eval [concat X_interface G $Gatedelay]}

set but20 [button .b12.reset -text "Reset" -command reset]

pack .b12.l .b12.c1 .b12.c3 .b12.c4 .b12.c5 .b12.maxsize .b12.cmd2 .b12.gatedelay .b12.cmd3  \
	.b12.reset -side left

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
		-label CSP \
		-command {set fileselect(path) *.csp ; fileselectOK}

	$t.buttons.ext.menu add command \
		-label HSE \
		-command {set fileselect(path) *.hse ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label VHD \
                -command {set fileselect(path) *.vhd ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label G \
                -command {set fileselect(path) *.g ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label UNC \
                -command {set fileselect(path) *.unc ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label ER \
                -command {set fileselect(path) *.er ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label TEL \
                -command {set fileselect(path) *.tel ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label RSG \
                -command {set fileselect(path) *.rsg ; fileselectOK}

        $t.buttons.ext.menu add command \
                -label PRS \
                -command {set fileselect(path) *.prs ; fileselectOK}

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

# Procedure to reset variable options
proc reset {} {
    global Cycles Maxsize Gatedelay Linksize Tolerance
    set Maxsize 4
    eval [concat X_interface M $Maxsize]
    set Gatedelay "0 inf"
    eval [concat X_interface G $Gatedelay]
}

# Procedure to display help
proc help {} {
    global helpfunction temp2filename errfilename
    global prsinput prslog env

#      set w .helper 
#      catch {destroy $w}
#      toplevel $w
#      wm title $w $helpfunction
#      scrollbar $w.scroll -command "$w.list yview"
#      pack $w.scroll -side right -fill y
#      scrollbar $w.scroll2 -command "$w.list xview" -orient horizontal
#      pack $w.scroll2 -side bottom -fill x
#      text $w.list -xscroll "$w.scroll2 set" -yscroll "$w.scroll set" \
#  	    -relief raised -setgrid true -width 80 -height 25 
#      pack $w.list -side left -fill both -expand yes
#      set prslog $w.list
#      wm minsize $w 1 1

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

# Procedure to process commands and display the results from <filename>.ext
proc process2 {com ext} {
    global filename tempfilename temp2filename errfilename filenamedisp \
           component
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
    set temp2filename $tempfilename
    append temp2filename $ext
    set errfilename $tempfilename
    append errfilename .err
    if {[file exists $temp2filename]} {
	exec rm $temp2filename
    }
    if {[file exists $errfilename]} {
	exec rm $errfilename
    }
    eval [concat X_interface $com $filename $component]
    More2
    set filename {}; set component {}
}

# Procedure to process commands and display the results from <filename>.ext
proc findsv {com ext} {
    global filename tempfilename temp2filename errfilename filenamedisp \
           component solutions Manual
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
    set temp2filename $tempfilename
    append temp2filename $ext
    set errfilename $tempfilename
    append errfilename .err
    if {[file exists $temp2filename]} {
	exec rm $temp2filename
    }
    if {[file exists $errfilename]} {
	exec rm $errfilename
    }
    set solutions [X_findsv]

    if {$Manual=={x}} then {
	set w .best
	catch {destroy $w}
	toplevel $w
	wm minsize $w 1 1
	frame $w.f
	pack $w.f -fill both -expand 1

	frame $w.f.u
	pack $w.f.u -side top -fill both -expand 1
	scrollbar $w.f.u.scroll -command "$w.f.u.lb yview"
	listbox $w.f.u.lb -relief raised -borderwidth 2 -setgrid yes \
		-yscrollcommand "$w.f.u.scroll set" 
	pack $w.f.u.scroll -side right -fill y
	pack $w.f.u.lb -side left -expand yes -fill both -expand 1

	foreach i $solutions {
	    $w.f.u.lb insert end $i
	}
	bind $w.f.u.lb <Double-Button-1> selectVar
    }
    More2
    set filename {}; set component {}
}


proc selectVar {} { 
    set first [selection get]
    eval [concat X_findsv $first]
    catch {destroy .best} 
    More2
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

# More the contents of the command field
proc More2 {} {
        set temp1 {more }
	global temp2filename tempfilename errfilename input log oripath  
	set command [append temp1 "atacs.log"]
	if [catch {open "|$command |& cat"} input] {
#		$log insert end $input\n
	} else {
		fileevent $input readable Log
#		$log insert end $command\n
	        
	}
	if {[file exists $temp2filename]} {
	    reporter
	} else {
	    if {[file exists $errfilename]} {
		reporterr
	    }
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

# Display results
proc reporter {} {
   set w .reporter 
   global temp2filename prsinput prslog

   catch {destroy $w}

   toplevel $w
   wm title $w $temp2filename
   
   scrollbar $w.scroll -command "$w.list yview"
   pack $w.scroll -side right -fill y
   scrollbar $w.scroll2 -command "$w.list xview" -orient horizontal
   pack $w.scroll2 -side bottom -fill x
   text $w.list -xscroll "$w.scroll2 set" -yscroll "$w.scroll set" \
           -relief raised -setgrid yes -width 80 -height 25 \
           -font -Adobe-Courier-Medium-R-Normal--*-120-* 
   pack $w.list -side left -fill both -expand yes
   set prslog $w.list
   wm minsize $w 1 1

   set temp1 {more }
   set command [append temp1 $temp2filename]
   if [catch {open "|$command |& cat"} prsinput] {
#       $prslog insert end $prsinput\n
   } else {
       fileevent $prsinput readable ReptLog
#       $prslog insert end $command\n
   }
}

# Stop the program and fix up the button
proc Stopprsfilename {} {
	global prsinput 
	catch {close $prsinput}
}

# Read and log output from the program
proc ReptLogErr {} {
	global errinput errlog
	if [eof $errinput] {
		Stoperrfilename
	} else {
		gets $errinput line
	        if {$line != ""} {
		    $errlog insert end $line\n
		    $errlog see end
		}
	}
}

# Display errors
proc reporterr {} {
   set w .reporterr 
   global tempfilename errfilename errinput errlog

   catch {destroy $w}

   toplevel $w
   wm title $w $errfilename
   
   scrollbar $w.scroll -command "$w.list yview"
   pack $w.scroll -side right -fill y
   scrollbar $w.scroll2 -command "$w.list xview" -orient horizontal
   pack $w.scroll2 -side bottom -fill x
   text $w.list -xscroll "$w.scroll2 set" -yscroll "$w.scroll set" \
           -relief raised -setgrid yes -width 80 -height 25 \
           -font -Adobe-Courier-Medium-R-Normal--*-120-* 
   pack $w.list -side left -fill both -expand yes
   set errlog $w.list
   wm minsize $w 1 1

   set temp1 {more }
   set command [append temp1 $errfilename]
   if [catch {open "|$command |& cat"} errinput] {
#       $errlog insert end $errinput\n
   } else {
       fileevent $errinput readable ReptLogErr
#       $errlog insert end $command\n
   }
}

# Stop the program and fix up the button
proc Stoperrfilename {} {
	global errinput 
	catch {close $errinput}
}

# Stop the program and fix up the button
proc Stopm {} {
	global input 
	catch {close $input}
}

bind all {<Control-c><Control-b>} {destroy .browser}
bind all {<Control-c><Control-e>} {destroy .reporterr}
bind all {<Control-c><Control-r>} {destroy .reporter}
bind all {<Control-c><Control-h>} {destroy .helper}
bind all {<Control-c><Control-s>} {destroy .best}
