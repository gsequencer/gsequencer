#!/usr/bin/perl

use warnings;
use strict;
use Cwd qw(cwd);
use File::Find;

my $src_dir = cwd;
my $location;
my @server_arr;

sub find_server_header {
    my $F = $File::Find::name;

    if($F =~ /.h$/){
        print "$F\n";
	push(@server_arr, "$F");
    }
}

if($#ARGV >= 1){
    $src_dir = $ARGV[1];
}

open(my $libags_sym_fh, '>', "$src_dir/libags.sym.in");

# server
$location = "$src_dir/ags/server";

find({ wanted => \&find_server_header, no_chdir=>1}, $location);

for(my $i=0; $i <= $#server_arr; $i++){
    open my $fh, "<", $server_arr[$i];
    
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
