#!/usr/bin/perl

use warnings;
use strict;
use Cwd qw(cwd);
use File::Find;

my $src_dir = cwd;
my $location;
my @plugin_arr;
my @audio_arr;

my @plugin_exclude_arr = ("ags/plugin/ags_vst3_manager.h" "ags/plugin/ags_vst3_conversion.h" "ags/plugin/ags_vst3_plugin.h");
my @audio_exclude_arr = ("ags/audio/file/ags_audio_toolbox.h" "ags/audio/file/ags_gstreamer_file.h" "ags/audio/wasapi/ags_w32_midiin.h" "ags/audio/task/ags_instantiate_vst3_plugin.h" "ags/audio/task/ags_write_vst3_port.h" "ags/audio/fx/ags_fx_vst3_channel.h" "ags/audio/fx/ags_fx_vst3_recycling.h" "ags/audio/fx/ags_fx_vst3_audio_signal.h" "ags/audio/fx/ags_fx_vst3_audio.h" "ags/audio/fx/ags_fx_vst3_audio_processor.h" "" "ags/audio/fx/ags_fx_vst3_channel_processor.h" "ags_alsa_hda_control_util.h" "ags/audio/audio-unit/ags_audio_unit_client.h" "ags/audio/audio-unit/ags_audio_unit_server.h" "ags/audio/audio-unit/ags_audio_unit_devout.h" "ags/audio/audio-unit/ags_audio_unit_devin.h" "ags/audio/audio-unit/ags_audio_unit_port.h" "ags/audio/midi/ags_midi_ci_1_1_port.h" "ags/audio/midi/ags_midi_clip.h");

sub find_plugin_header {
    my $F = $File::Find::name;
    my $is_excluded = 0;
    
    foreach my $excluded (@plugin_exclude_arr) {
	if($F =~ /\Q$excluded\E$/){
	    $is_excluded = 1;
	    
	    last;
	}
    }
    
    if($F =~ /.h$/ &&
       !is_excluded){
        print "$F\n";
	push(@plugin_arr, "$F");
    }
}

sub find_audio_header {
    my $F = $File::Find::name;
    my $is_excluded = 0;

    foreach my $excluded (@audio_exclude_arr) {
	if($F =~ /\Q$excluded\E$/){
	    $is_excluded = 1;
	    
	    last;
	}
    }
    
    if($F =~ /.h$/ &&
       !is_excluded){
        print "$F\n";
	push(@audio_arr, "$F");
    }
}

if($#ARGV >= 1){
    $src_dir = $ARGV[1];
}

open(my $libags_sym_fh, '>', "$src_dir/libags.sym.in");

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
		
		say $libags_sym_fh "$all_on_line[$j]\n";
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
		
		say $libags_sym_fh "$all_on_line[$j]\n";
	    }
	}
    }
}

close $libags_sym_fh;
