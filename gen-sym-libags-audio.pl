#!/usr/bin/perl

use warnings;
use strict;
use Cwd qw(cwd);
use File::Find;

my $src_dir = cwd;
my $location;
my @plugin_arr;
my @audio_arr;

my @plugin_exclude_arr = ("ags_vst3_manager.h", "ags_vst3_conversion.h", "ags_vst3_plugin.h" );
my @audio_exclude_arr = ("ags_audio_toolbox.h", "ags_gstreamer_file.h", "ags_w32_midiin.h", "ags_instantiate_vst3_plugin.h", "ags_write_vst3_port.h", "ags_fx_vst3_channel.h", "ags_fx_vst3_recycling.h", "ags_fx_vst3_audio_signal.h", "ags_fx_vst3_audio.h", "ags_fx_vst3_audio_processor.h", "ags_fx_vst3_channel_processor.h", "ags_alsa_hda_control_util.h", "ags_audio_unit_client.h", "ags_audio_unit_server.h", "ags_audio_unit_devout.h", "ags_audio_unit_devin.h", "ags_audio_unit_port.h", "ags_midi_ci_1_1_port.h", "ags_midi_clip.h" );

sub find_plugin_header {
    my $F = $File::Find::name;
    my $F_is_excluded = 0;
    
    foreach my $current_excluded (@plugin_exclude_arr) {
	if(index($F, $current_excluded) != -1){
	    $F_is_excluded = 1;
	    print "exclude $F - $current_excluded\n";
	    
	    last;
	}
    }
    
    if($F =~ /.h$/ && $F_is_excluded == 0){
        print "$F\n";
	push(@plugin_arr, "$F");
    }
}

sub find_audio_header {
    my $F = $File::Find::name;
    my $F_is_excluded = 0;

    foreach my $current_excluded (@audio_exclude_arr) {
	if(index($F, $current_excluded) != -1){
	    $F_is_excluded = 1;
	    print "exclude $F - $current_excluded\n";
	      
	    last;
	}
    }
    
    if($F =~ /.h$/ && $F_is_excluded == 0){
        print "$F\n";
	push(@audio_arr, "$F");
    }
}

if($#ARGV >= 1){
    $src_dir = $ARGV[1];
}

open(my $libags_audio_sym_fh, '>', "$src_dir/libags_audio.sym.in");

# plugin
$location = "$src_dir/ags/plugin";

find({ wanted => \&find_plugin_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#plugin_arr; $i++){
    open my $fh, "<", $plugin_arr[$i];
    
    do {
	local $/;
	my $str = <$fh>;
	my @all_on_line = $str =~ /(?<!#define )(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()/g;

	if(@all_on_line){
	    for(my $j=0; $j <= $#all_on_line; $j++){
		print "$all_on_line[$j]\n";
		
		say $libags_audio_sym_fh "$all_on_line[$j]";
	    }
	}
    }
}

# audio
$location = "$src_dir/ags/audio";

find({ wanted => \&find_audio_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#audio_arr; $i++){
    open my $fh, "<", $audio_arr[$i];
    
    do {
	local $/;
	my $str = <$fh>;
	my @all_on_line = $str =~ /(?<!#define )(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()/g;

	if(@all_on_line){
	    for(my $j=0; $j <= $#all_on_line; $j++){
		print "$all_on_line[$j]\n";
		
		say $libags_audio_sym_fh "$all_on_line[$j]";
	    }
	}
    }
}

close $libags_audio_sym_fh;
