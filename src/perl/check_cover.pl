eval "set PATH=/usr/bin:/usr/local/bin:/Perl/bin:${PATH};exec perl -S $0 $*"
    if $running_under_some_shell;#above line won't run if we're already in Perl

if (( ! @ARGV[1] ) || ((@ARGV[0] eq "-v") && ! @ARGV[2])) {
    print "usage: check_cover.pl [-v] filename1 filename2\n";
    print " Where filename1 is a complete state coded XBM specification\n";
    print " and filename2 is the cover to be checked in EQN or PRS format.\n";
    die   " The verbose -v option prints out more detailed information.\n"; }

$id = "check cover - ";
$info = 0;
$debug = 0;

if (@ARGV[0] eq "-v") { 
    $debug = 1;
    $filename1 = @ARGV[1];
    $filename2 = @ARGV[2]; }
else {
    $filename1 = @ARGV[0];
    $filename2 = @ARGV[1]; }

if ( ! -r $filename1) { die $id."ERROR: Cannot read specified file: ".$filename1."\n"; }
elsif ( ! -r $filename2) { die $id."ERROR: Cannot read specified file: ".$filename2."\n"; }
else {
    $errors = 0;
    open(FILE,$filename1);
    @file1 = <FILE>;
    close(FILE);
    open(FILE,$filename2);
    @file2 = <FILE>;
    close(FILE);
    
    foreach $line (@file1) {
	if ($line =~ /=/) { $type1 = "eqn"; }
	elsif ($line =~ /:/) { $type1 = "prs"; }
	elsif ($line =~ /\|/) { $type1 = "xbm"; }}
    foreach $line (@file2) {
	if ($line =~ /=/) { $type2 = "eqn"; }
	elsif ($line =~ /:/) { $type2 = "prs"; }
	elsif ($line =~ /\|/) { $type2 = "xbm"; }}

    if (! (($type1 eq "xbm") && (($type2 eq "eqn") || ($type2 eq "prs")) ||
	   ($type2 eq "xbm") && (($type1 eq "eqn") || ($type1 eq "prs")))) {
	die $id."ERROR: Must specify both an XBM file and a cover (EQN or PRS) file.\n"; }
    
    %xbm = ();
    $start_signal_vector = "";
    if ($type1 eq "xbm") { &parse_xbm(@file1); }
    elsif ($type2 eq "xbm") { &parse_xbm(@file2); }
    for ($i = 0; $i < length($start_signal_vector); $i++) { @dc_vector[$i] = "-"; }
    if ($debug) { print "Initial vector: ".$start_signal_vector."\n"; }

    if ($debug) {
	print "\nBurst-mode spec:\n----------------\n";
	foreach $curr_state (keys %xbm) {
	    $in_bursts = $xbm{$curr_state}{in_burst};
	    $in_bursts =~ s/(.*),/$1/;
	    @in_bursts = split(/,/,$in_bursts);
	    $state_bursts = $xbm{$curr_state}{state_burst};
	    $state_bursts =~ s/(.*),/$1/;
	    @state_bursts = split(/,/,$state_bursts);
	    $out_bursts = $xbm{$curr_state}{out_burst};
	    $out_bursts =~ s/(.*),/$1/;
	    @out_bursts = split(/,/,$out_bursts);
	    $next_states = $xbm{$curr_state}{next_state};
	    $next_states =~ s/(.*),/$1/;
	    @next_states = split(/,/,$next_states);
	    $i = 0;
	    foreach $next_state (@next_states) {
		print $curr_state."\t".$next_state."\t".@in_bursts[$i]."\t| ".@state_bursts[$i]." |\t".@out_bursts[$i++]."\n"; }}
	print "\n";
    }

    %cover = ();
    if ($type1 eq "eqn") { &parse_eqn(@file1); }
    elsif ($type2 eq "eqn") { &parse_eqn(@file2); }
    elsif ($type1 eq "prs") { &parse_prs(@file1); }
    elsif ($type2 eq "prs") { &parse_prs(@file2); }

    if ($debug) {
	print "\nCover cubes:\n------------\n";
	foreach $output (keys %cover) {
	    $cubes = $cover{$output};
	    $cubes =~ s/(.*),/$1/;
	    $cubes =~ s/,/ /g;
	    print $output."\t: ".$cubes."\n"; }
    }

    %visited = ();
    %req_cubes = ();
    %priv_cubes = ();
    &get_req_priv_cubes(0,$start_signal_vector);

    if ($debug) {
	print "\nRequired cubes:\n---------------\n";
	foreach $output (keys %req_cubes) {
	    print "$output:\n";
	    $cubes = $req_cubes{$output}{cubes};
	    $trans = $req_cubes{$output}{trans};
	    $cubes =~ s/(.*),/$1/;
	    $trans =~ s/(.*),/$1/;
	    @cubes = split(/,/,$cubes);	    
	    @trans = split(/,/,$trans);	    
	    $i = 0;
	    foreach $cube (@cubes) {
		print "  ".$cube."  \t".@trans[$i++]."\n"; }}
	print "\n";
    }

    if ($debug) { print "Checking required cube covers:\n------------------------------\n"; }
    if ($debug) { print "(* = static trans, - = falling, + = rising)\n"; }
    foreach $output (keys %req_cubes) {
	$trans = $req_cubes{$output}{trans};
	$req_cubes = $req_cubes{$output}{cubes};
	$cover_cubes = $cover{$output};
	$trans =~ s/(.*),/$1/;
	$req_cubes =~ s/(.*),/$1/;
	$req_cubes =~ s/R|F|U|D|X/-/g;
	$cover_cubes =~ s/(.*),/$1/;
	@trans = split(/,/,$trans);
	@req_cubes = split(/,/,$req_cubes);
	@cover_cubes = split(/,/,$cover_cubes);
	$i = 0;
	foreach $req_cube (@req_cubes) {
	    @req_cube = split("",$req_cube);
	    $trans = @trans[$i++];
	    $sign = $trans;
	    $sign =~ s/(\+|-|\*).*/$1/;
	    $trans =~ s/\+\s|-\s|\*\s//;
	    $cover_found = 0;
	    if ($debug) { print "[$output] req-cube: $req_cube   trans: $trans\n"; }
	    foreach $cover_cube (@cover_cubes) {
		@cover_cube = split("",$cover_cube);
		$contains = 1; 
		for ($j = 0; $j < length($start_signal_vector); $j++) {
		    if (((@req_cube[$j] eq "1") && (@cover_cube[$j] eq "0")) ||
			((@req_cube[$j] eq "0") && (@cover_cube[$j] eq "1")) ||
			((@req_cube[$j] eq "-") && !(@cover_cube[$j] eq "-"))) {
			$contains = 0; }}
		$cover_found += $contains;
		if ($debug) { 
		    if ($contains) { print "\tcover-cube: $cover_cube   OK\n"; }
		    else { print "\tcover-cube: $cover_cube \n"; }}
	    }
	    if ($cover_found == 0) {
		$errors++;
		print $id."ERROR: Output $output($sign) missing cover for required cube $req_cube of transition $trans.\n"; }
	}
    }

    if ($debug) { print "\nChecking dynamic hazards:\n-------------------------\n"; }
    foreach $output (keys %priv_cubes) {
	$trans = $priv_cubes{$output}{trans};
	$priv_cubes = $priv_cubes{$output}{cubes};
	$start_subcubes = $priv_cubes{$output}{start_subcubes};
	$end_subcubes = $priv_cubes{$output}{end_subcubes};
	$end_cubes = $priv_cubes{$output}{end_cubes};
	$cover_cubes = $cover{$output};
	$trans =~ s/(.*),/$1/;
	$priv_cubes =~ s/(.*),/$1/;
	$priv_cubes =~ s/R|F|U|D|X/-/g;
	$start_subcubes =~ s/(.*),/$1/;
	$end_subcubes =~ s/(.*),/$1/;
	$end_cubes =~ s/(.*),/$1/;
	$cover_cubes =~ s/(.*),/$1/;
	@trans = split(/,/,$trans);
	@priv_cubes = split(/,/,$priv_cubes);
	@start_subcubes = split(/,/,$start_subcubes);
	@end_subcubes = split(/,/,$end_subcubes);
	@end_cubes = split(/,/,$end_cubes);
	@cover_cubes = split(/,/,$cover_cubes);
	$i = 0;
	foreach $priv_cube (@priv_cubes) {
	    @priv_cube = split(//,$priv_cube);
	    $start_subcube = @start_subcubes[$i];
	    @start_subcube = split(//,$start_subcube);
	    $end_subcube = @end_subcubes[$i];
	    @end_subcube = split(//,$end_subcube);
	    $end_cube = @end_cubes[$i];
	    @end_cube = split(//,$end_cube);
	    $trans = @trans[$i++];
	    $sign = $trans;
	    $sign =~ s/(\+|-).*/$1/;
	    $trans =~ s/\+\s|-\s//;
	    if ($debug) { print "[$output$sign] priv-cube: $priv_cube   trans: $trans\n"; }
	    $illegal_intersection = 0;
	    foreach $cover_cube (@cover_cubes) {
		@cover_cube = split(//,$cover_cube);
		$intersects_trans = 1; 
		for ($j = 0; $j < length($start_signal_vector); $j++) {
		    if (((@priv_cube[$j] eq "1") && (@cover_cube[$j] eq "0")) ||
			((@priv_cube[$j] eq "0") && (@cover_cube[$j] eq "1"))) {
			$intersects_trans = 0; }}
		$intersects_end = 1;
		for ($j = 0; $j < length($start_signal_vector); $j++) {
		    if (((@end_cube[$j] eq "1") && (@cover_cube[$j] eq "0")) ||
			((@end_cube[$j] eq "0") && (@cover_cube[$j] eq "1"))) {
			$intersects_end = 0; }}
		$contains_startsub = 1;
		for ($j = 0; $j < length($start_signal_vector); $j++) {
		    if (((@start_subcube[$j] eq "1") && (@cover_cube[$j] eq "0")) ||
			((@start_subcube[$j] eq "0") && (@cover_cube[$j] eq "1")) ||
			((@start_subcube[$j] eq "-") && !(@cover_cube[$j] eq "-"))) {
			$contains_startsub = 0; }}
		$contains_endsub = 1;
		for ($j = 0; $j < length($start_signal_vector); $j++) {
		    if (((@end_subcube[$j] eq "1") && (@cover_cube[$j] eq "0")) ||
			((@end_subcube[$j] eq "0") && (@cover_cube[$j] eq "1")) ||
			((@end_subcube[$j] eq "-") && !(@cover_cube[$j] eq "-"))) {
			$contains_endsub = 0; }}
		if ($debug) { print "\tcover-cube: $cover_cube  \t($intersects_trans $contains_startsub $intersects_end $contains_endsub)     "; }
		if ($intersects_end && ! $contains_endsub) {
		    $errors++;
		    print "\n".$id."ERROR: Cover cube $cover_cube of output $output($sign) illegally intersects transition end cube $end_cube of transition $trans\n"; }
		elsif ($intersects_trans && ! $intersects_end && ! $contains_startsub) {
		    $errors++;
		    print "\n".$id."ERROR: Cover cube $cover_cube of output $output($sign) illegally intersects transition cube $priv_cube of transition $trans\n"; }
		else {
		    if ($debug) { print "OK\n"; }}
		# If cover_cube intersects priv_cube (and not end_cube) but does not contain start_subcube -> hazard
		# If cover_cube intersects end_cube but does not contain end_subcube -> hazard
	    }
	}
    }
    print $id."Finished checking design $filename1. There were $errors errors.\n"; 

}


sub parse_xbm {

    local($lineno,$i,$j,@file,$line,$name,$value,@start_signal_vector,@signal_names,@output_signal_vector);
    local(@output_signal_names,%output_signal_values,$curr_state,$next_state,$in_burst,$state_burst);
    local($out_burst,$level_signals,@level_signals,$level_signal,$state,@signals,$start_prev_burst,$position);
    $lineno = 0;
    $i = 0; $j = 0;
    @file = @_;
    foreach $line (@file) {
	$line =~ s/;.*\n//;
	if ($line !~ /\w|\d/) { }
	elsif ($line =~ /^\s*(?:input|output)\s+\w+\s+\d+/i) {
	    # Record I/O name and initial value.
	    $name = $value = $line;
	    $name =~ s/\s*(?:input|output)\s+(\w+).*\s*/$1/i;
	    $value =~ s/\s*(?:input|output)\s+\w+\s+(\d+).*\s*/$1/i;
	    if ($line =~ /^\s*input\s+/i) {
		$signal_position{$name} = $i;
		@start_signal_vector[$i] = $value;
		@signal_names[$i++] = "INP $name "; }
	    elsif ($line =~ /^\s*output\s+/i) {
		@output_signal_vector[$j] = $value;
		@output_signal_names[$j++] = "$name ";
		$output_signal_values{$name} = $value;
		$is_output{$name} = 1; }
	}
#	elsif ($line =~ /^\s*\d+\s+\d+\s+\[?\w+(?:\+|-|\*)\]?(?:\s+\[?\w+(?:\+|-|\*)\]?)*\s*\|\s*\w*(?:\+?|-?)(?:\s+\w+(?:\+|-))*(?:\s*\|\s*\w*(?:\+?|-?)(?:\s+\w+(?:\+|-))*)/) {
	elsif ($line =~ /^\s*\d+\s+\d+\s+\[?\w+(?:\+|-|\*)\]?(?:\s+\[?\w+(?:\+|-|\*)\]?)*\s*\|\s*\w*(?:\+?|-?)(?:\s+\w+(?:\+|-))*/) {
	    $curr_state = $line;
	    $next_state = $line;
	    $in_burst = $line;
	    $state_burst = $line;
	    $out_burst = $line;
	    $level_signals = $line;
	    $curr_state =~ s/\s*(\d+).*\n/$1/;
	    $next_state =~ s/\s*\d+\s+(\d+).*\n/$1/;
	    $in_burst    =~ s/\s*\d+\s+\d+\s+(.*?)\s*\|.*\n/$1/;
	    $out_burst   =~ s/.*\|\s*(.*?)\s*\n/$1/;
	    if ($line =~ /\|.*\|/) { $state_burst =~ s/.*?\|\s*(.*?)\s*\|.*\n/$1/; }
	    else { $state_burst = ""; }
	    $xbm{$curr_state}{next_state} .= $next_state.",";
	    $xbm{$curr_state}{in_burst} .= $in_burst.",";
	    $xbm{$curr_state}{out_burst} .= $out_burst.",";
	    $xbm{$curr_state}{state_burst} .= $state_burst.",";
	    if ($level_signals =~ /\[/) {
		$level_signals =~ s/\n//;
		$level_signals =~ s/.*?(\[\w+(?:\+|-)\])/$1/g;
		$level_signals =~ s/((?:\[\w+(?:\+|-)\])*).*/$1/;
		$level_signals =~ s/\]\[/,/g;
		$level_signals =~ s/\[|\]//g;
		@level_signals = split(/,/,$level_signals);
		foreach $level_signal (@level_signals) {
		    $level_signal =~ s/\+|-//;
		    $is_level{$level_signal} = 1; }}
	    if ($next_state eq "0") { $start_prev_burst = $in_burst; }
	}
	else {
	    print $id."Error on line ".$lineno.", skipping this line.\n"; }
	$lineno++;
    }
    # Append outputs to signal state vector.
    for ($k = 0; $k < $j; $k++) {
	$name = @output_signal_names[$k];
	$name =~ s/\s*//g;
	$signal_position{$name} = $i + $k;
	@start_signal_vector[($i+$k)] = @output_signal_vector[$k]; }
    # Insert correct values for ddc's starting before and continuing through start state.
    @signals = split(/\s+/,$start_prev_burst);
    foreach $signal (@signals) {
	$name = $signal;
	$name =~ s/\+|-|\*|\[|\]//g;
	$position = $signal_position{$name};
	if ($signal =~ /\*/) {
	    if (@start_signal_vector[$position] eq "0") {
		@start_signal_vector[$position] = "U"; }
	    elsif (@start_signal_vector[$position] eq "1") {
		@start_signal_vector[$position] = "D"; }}}
    $start_signal_vector = join("",@start_signal_vector);
    @signal_names = (@signal_names,@output_signal_names);

    if($deep_debug){print "GRAPH:\n"; foreach $state (keys %xbm) {
	print $state."\t".$xbm{$state}{next_state}."\t".$xbm{$state}{in_burst}."\t| ".$xbm{$state}{state_burst}." |\t".$xbm{$state}{out_burst}."\n"; }}
    if($debug){print "\nSignal name vector: ".join("",@signal_names)."\n";}
    if($deep_debug){print "level signals: ",%is_level,"\n";}
}


sub get_req_priv_cubes {
    
    local($curr_state,$prev_vector,$in_bursts,@in_bursts,$state_bursts,@state_bursts,$out_bursts,@out_bursts);
    local($next_states,@next_states,$i,$next_state,@state_vector,$name,$in_burst,$state_burst,$out_burst);
    local(@in_burst_signals,@state_burst_signals,@out_burst_signals,$state_names,@state_burst_names);
    local($out_names,@out_burst_names,$iter,%is_enabled,$trans,@burst_signals,$signal);
    local(@static_burst_cube,$name,$j,@dynamic_burst_cubes,@dynamic_burst_cube,$output);
    local(@start_subcube,@end_subcube,$dynamic_burst_cube,$position);

    $curr_state = @_[0];
    $prev_vector = @_[1];
    
    $prev_vector =~ s/R/1/g;
    $prev_vector =~ s/F/0/g;
    $visited{$curr_state} = 1;
    $in_bursts = $xbm{$curr_state}{in_burst};
    $in_bursts =~ s/(.*),/$1/;
    @in_bursts = split(/,/,$in_bursts);
    $state_bursts = $xbm{$curr_state}{state_burst};
    $state_bursts =~ s/(.*),/$1/;
    @state_bursts = split(/,/,$state_bursts);
    $out_bursts = $xbm{$curr_state}{out_burst};
    $out_bursts =~ s/(.*),/$1/;
    @out_bursts = split(/,/,$out_bursts);
    $next_states = $xbm{$curr_state}{next_state};
    $next_states =~ s/(.*),/$1/;
    @next_states = split(/,/,$next_states);
    $i = 0;
    foreach $next_state (@next_states) {
	@state_vector = split(//,$prev_vector);
	foreach $name (keys %is_level) {
	    @state_vector[$signal_position{$name}] = "X"; }
	$in_burst = @in_bursts[$i];
	$state_burst = @state_bursts[$i];
	$out_burst = @out_bursts[$i];
	@in_burst_signals = split(/\s+/,$in_burst);
	@state_burst_signals = split(/\s+/,$state_burst);
	@out_burst_signals = split(/\s+/,$out_burst);
	$state_names = $state_burst;
	$state_names =~ s/\+|-//g;
	@state_burst_names = split(/\s+/,$state_names);
	$out_names = $out_burst;
	$out_names =~ s/\+|-//g;
	@out_burst_names = split(/\s+/,$out_names);
	# Derive required cubes
	#  iterate 3 times to cover input, state, and output bursts
	for ($iter = 1; $iter <= 3; $iter++) {
	    #  setup the parameters for the current iteration
	    %is_enabled = ();
	    if ($iter == 1) {
		$trans = "$curr_state $next_state $in_burst";
		@burst_signals = @in_burst_signals;
		if ($state_burst =~ /\w/) {
		    foreach $signal (@state_burst_signals) { 
			$name = $signal; $name =~ s/\+|-//;
			if ($signal =~ /-/) { $is_enabled{$name} = "F"; }
			else { $is_enabled{$name} = "R"; }}}
		else { 
		    $iter++;  # no state burst - so skip 2:nd iteration
		    foreach $signal (@out_burst_signals) { 
			$name = $signal; $name =~ s/\+|-//;
			if ($signal =~ /-/) { $is_enabled{$name} = "F"; }
			else { $is_enabled{$name} = "R"; }}}}
	    elsif ($iter == 2) {
		$trans = "$curr_state $next_state $state_burst";
		@burst_signals = @state_burst_signals;
		foreach $signal (@out_burst_signals) { 
		    $name = $signal; $name =~ s/\+|-//;
		    if ($signal =~ /-/) { $is_enabled{$name} = "F"; }
		    else { $is_enabled{$name} = "R"; }}}
	    elsif ($iter == 3) {
		$trans = "$curr_state $next_state $out_burst";
		@burst_signals = @out_burst_signals; 
	    }
	    #  get static cube:
	    @static_burst_cube = @state_vector;
	    @start_subcube = @state_vector;
	    @end_subcube = @state_vector;
	    @end_cube = @state_vector;
	    foreach $signal (@burst_signals) {
		$name = $signal;
		$name =~ s/\[|\]|\+|-|\*//g;
		$position = $signal_position{$name};
		if ($is_level{$name}) {
		    if ($signal =~ /\+/) {
			@start_subcube[$position] = "1";
			@end_subcube[$position] = "1";
			@end_cube[$position] = "1";
			@state_vector[$position] = "1";
			@static_burst_cube[$position] = "1"; }
		    elsif ($signal =~ /\-/) {
			@start_subcube[$position] = "0";
			@end_subcube[$position] = "0";
			@end_cube[$position] = "0";
			@state_vector[$position] = "0";
			@static_burst_cube[$position] = "0"; }
		    else { $errors++; die $id."ERROR: Illegal polarity of level signal $signal\n"; }}
		else {
		    if ($signal =~ /\+/) { 
			@start_subcube[$position] = "0";
			@end_subcube[$position] = "1";
			@end_cube[$position] = "1";
			@static_burst_cube[$position] = "R";
			@state_vector[$position] = "1"; }
		    elsif ($signal =~ /-/) { 
			@start_subcube[$position] = "1";
			@end_subcube[$position] = "0";
			@end_cube[$position] = "0";
			@static_burst_cube[$position] = "F";
			@state_vector[$position] = "0"; }
		    elsif ($signal =~ /\*/) { 
			if (@state_vector[$position] eq "0") {
			    @start_subcube[$position] = "0";
			    @end_subcube[$position] = "1";
			    @end_cube[$position] = "U";
			    @static_burst_cube[$position] = "U";
			    @state_vector[$position] = "U"; }
			elsif (@state_vector[$position] eq "1") {
			    @start_subcube[$position] = "1";
			    @end_subcube[$position] = "0";
			    @end_cube[$position] = "D";
			    @static_burst_cube[$position] = "D";
			    @state_vector[$position] = "D"; }
			elsif (@state_vector[$position] eq "D") {
			    @start_subcube[$position] = "1";
			    @end_subcube[$position] = "0";
			    @end_cube[$position] = "D"; }
			elsif (@state_vector[$position] eq "U") {
			    @start_subcube[$position] = "0";
			    @end_subcube[$position] = "1";
			    @end_cube[$position] = "U"; }
			else { $errors++; die $id."ERROR: Illegal polarity in state vector\n"; }}
		    else { $errors++; die $id."ERROR: Illegal polarity of edge signal $signal\n"; }}
	    }
	    #  get dynamic cubes:
	    $j = 0;
	    @dynamic_burst_cubes = ();
	    foreach $signal (@burst_signals) {
		@dynamic_burst_cube = @static_burst_cube;
		$name = $signal;
		$name =~ s/\[|\]|\+|-|\*//g;
		if (! $is_level{$name}) {
		    if ($signal =~ /\+/) {
			@dynamic_burst_cube[$signal_position{$name}] = "0"; 
			@dynamic_burst_cubes[$j++] = join("",@dynamic_burst_cube); }
		    elsif ($signal =~ /\-/) {
			@dynamic_burst_cube[$signal_position{$name}] = "1";
			@dynamic_burst_cubes[$j++] = join("",@dynamic_burst_cube); }}
	    }
	    #  add static or dynamic cubes as required cubes, and add privileged cube with subcubes
	    foreach $output (keys %is_output) {
		if ($is_enabled{$output} eq "F") {
		    $priv_cubes{$output}{cubes} .= join("",@static_burst_cube).",";
		    $priv_cubes{$output}{start_subcubes} .= join("",@start_subcube).",";
		    $priv_cubes{$output}{end_subcubes} .= join("",@end_subcube).",";
		    $priv_cubes{$output}{end_cubes} .= join("",@end_cube).",";
		    $priv_cubes{$output}{trans} .= "- $trans,";
		    foreach $dynamic_burst_cube (@dynamic_burst_cubes) {
			$req_cubes{$output}{cubes} .= $dynamic_burst_cube.",";
			$req_cubes{$output}{trans} .= "- $trans,"; }}
		elsif ($is_enabled{$output} eq "R") {
		    $priv_cubes{$output}{cubes} .= join("",@static_burst_cube).",";
		    $priv_cubes{$output}{start_subcubes} .= join("",@start_subcube).",";
		    $priv_cubes{$output}{end_subcubes} .= join("",@end_subcube).",";
		    $priv_cubes{$output}{end_cubes} .= join("",@end_cube).",";
		    $priv_cubes{$output}{trans} .= "+ $trans,";
		    $req_cubes{$output}{cubes} .= join("",@end_cube).",";
		    $req_cubes{$output}{trans} .= "+ $trans,"; }
		else { # static input or output transition
		    if ((@static_burst_cube[$signal_position{$output}] eq "1") ||
			(@static_burst_cube[$signal_position{$output}] eq "R")) {
			$req_cubes{$output}{cubes} .= join("",@static_burst_cube).",";
			$req_cubes{$output}{trans} .= "* $trans,"; }}
	    }
	}
	if (! $visited{$next_state}) {
	    &get_req_priv_cubes($next_state,join("",@state_vector)); }
	$i++;
    }
}


sub parse_eqn {
    local(%equations,@file,%is_output);
    @file = @_;
    foreach $line (@file) {
	if (($line =~ /\w/) && ($line =~ /=/)) {
	    $output = $line;
	    $output =~ s/\s*=|\s*\n//g;
	    if ($output =~ /_Set\b/) { $errors++; print $id."ERROR: Non-combinational 2-level cover (set) $line\n"; }
	    elsif ($output =~ /_Reset\b/) { $errors++; die $id."ERROR: Illegal 2-level cover (reset) $line\n"; }}
	elsif (($line =~ /\w/) && ($line !~ /=/)) {
	    $line =~ s/(\w+)\'/~$1/g;
	    $line =~ s/^\s*|\s*\+\s*\n//g;
	    @signals = split(/\s+/,$line);
	    @cube = @dc_vector;
	    foreach $signal (@signals) {
		$name = $signal;
		$name =~ s/~//;
		if (! exists($signal_position{$name})) { $errors++; die $id."ERROR: Cover contains signal $name not present in specification\n"; }
		if ($signal =~ /~/) { @cube[$signal_position{$name}] = "0"; }
		else { @cube[$signal_position{$name}] = "1"; }}
	    $cover{$output} .= join("",@cube).","; 
	}
    }
}

sub parse_prs {
    local(%equations);
    @file = @_;
    foreach $line (@file) {
	if ($line =~ /\w/) {
	    $line =~ s/\s*\n//;
	    $line =~ s/^\s*//;
	    if ($line =~ /\+/) { print $id."WARNING: Potential non-combinational 2-level cover (set) $line\n"; }
	    if ($line =~ /-/) { $errors++; die $id."ERROR: Illegal 2-level cover (reset) $line\n"; }
	    $line =~ s/\[\s*(?:\+|-)?\s*(\w+)\s*:\s*\(\s*(.*?)\s*\)\s*\].*/$1:$2/;
	    $output = $input = $line;
	    $output =~ s/(.*):.*/$1/;
	    $input =~ s/.*:(.*)/$1/;
	    $input =~ s/\s*&\s*/ /g;
	    $input =~ s/^\s*|\s*$//g;
	    @signals = split(/\s+/,$input);
	    @cube = @dc_vector;
	    foreach $signal (@signals) {
		$name = $signal;
		$name =~ s/~//;
		if (! exists($signal_position{$name})) { $errors++; die $id."ERROR: Cover contains signal $name not present in specification\n"; }
		if ($signal =~ /~/) { @cube[$signal_position{$name}] = "0"; }
		else { @cube[$signal_position{$name}] = "1"; }}
	    $cover{$output} .= join("",@cube).",";
	}
    }
}
