#!/usr/bin/perl

use warnings;
use strict;
use Cwd qw(cwd);
use File::Find;

my $src_dir = cwd;
my $location;
my @gui_arr;
my @gui_marshal_arr;

my @gui_exclude_arr = ("ags_button.h" );

sub find_gui_header {
    my $F = $File::Find::name;
    my $F_is_excluded = 0;

    foreach my $current_excluded (@gui_exclude_arr) {
	if(index($F, $current_excluded) != -1){
	    $F_is_excluded = 1;
	    print "exclude $F - $current_excluded\n";
	    
	    last;
	}
    }
    
    if($F =~ /.h$/ && $F_is_excluded == 0){
        print "$F\n";
	push(@gui_arr, "$F");
    }
}

sub find_gui_marshal_header {
    my $F = $File::Find::name;

    if ($F =~ /^ags_widget_marshal.h$/ ) {
        print "$F\n";
	push(@gui_marshal_arr, "$F");
    }
}

if($#ARGV >= 1){
    $src_dir = $ARGV[1];
}

open(my $libags_sym_fh, '>', "$src_dir/libags.sym.in");

# gui
$location = "$src_dir/ags/widget";

find({ wanted => \&find_gui_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#gui_arr; $i++){
    open my $fh, "<", $gui_arr[$i];
    
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

# gui - marshal
$location = "$src_dir/ags/widget";

find({ wanted => \&find_gui_marshal_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#gui_marshal_arr; $i++){
    open my $fh, "<", $gui_marshal_arr[$i];

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
