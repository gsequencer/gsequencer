#!/usr/bin/perl

use warnings;
use strict;
use Cwd qw(cwd);
use File::Find;

my $src_dir = cwd;
my $location;
my @util_arr;
my @lib_arr;
my @object_arr;
my @object_marshal_arr;
my @file_arr;

sub find_util_header {
    my $F = $File::Find::name;

    if ($F =~ /.h$/ ) {
        print "$F\n";
	push(@util_arr, "$F");
    }
}

sub find_lib_header {
    my $F = $File::Find::name;

    if ($F =~ /.h$/ ) {
        print "$F\n";
	push(@lib_arr, "$F");
    }
}

sub find_object_header {
    my $F = $File::Find::name;

    if ($F =~ /.h$/ ) {
        print "$F\n";
	push(@object_arr, "$F");
    }
}

sub find_object_marshal_header {
    my $F = $File::Find::name;

    if ($F =~ /^ags_marshal.h$/ ) {
        print "$F\n";
	push(@object_marshal_arr, "$F");
    }
}

sub find_file_header {
    my $F = $File::Find::name;

    if ($F =~ /.h$/ ) {
        print "$F\n";
	push(@file_arr, "$F");
    }
}

if($#ARGV >= 1){
    $src_dir = $ARGV[1];
}

open(my $libags_sym_fh, '>', "$src_dir/libags.sym.in");

# util
$location = "$src_dir/ags/util";

find({ wanted => \&find_util_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#util_arr; $i++){
    open my $fh, "<", $util_arr[$i];
    
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

# lib
$location = "$src_dir/ags/lib";

find({ wanted => \&find_lib_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#lib_arr; $i++){
    open my $fh, "<", $lib_arr[$i];

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

# object
$location = "$src_dir/ags/object";

find({ wanted => \&find_object_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#object_arr; $i++){
    open my $fh, "<", $object_arr[$i];

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

# object - marshal
$location = "$src_dir/ags/object";

find({ wanted => \&find_object_marshal_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#object_marshal_arr; $i++){
    open my $fh, "<", $object_marshal_arr[$i];

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

# file
$location = "$src_dir/ags/file";

find({ wanted => \&find_file_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#file_arr; $i++){
    open my $fh, "<", $file_arr[$i];

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
