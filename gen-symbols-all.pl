#!/usr/bin/perl
use strict;

use File::Find::Rule qw( );

unlink('libags.sym.in');
unlink('libags_thread.sym.in');
unlink('libags_server.sym.in');
unlink('libags_audio.sym.in');
unlink('libags_vst.sym.in');
unlink('libags_gui.sym.in');

# libags.sym.in
my $filename = 'libags.sym.in';
my $f;

open(my $fh, '>>', $filename) or die "Could not open file '$filename' $!";

my $license = 'license-notice-gnu-gpl-3-0+-sym.txt';

open(my $license_fh, '<', $license) or die "Could not open file '$license' $!";

while(my $line = <$license_fh>){
    print $fh $line;
}

print $fh "\n";

close($license_fh);

my @files = File::Find::Rule->name("*.h")->in("ags/util");

foreach $f (@files){
    test_func($f, $fh);
}

my @files = File::Find::Rule->name("*.h")->in("ags/lib");

foreach $f (@files){
    test_func($f, $fh);
}

my @files = File::Find::Rule->name("*.h")->in("ags/object");

foreach $f (@files){
    test_func($f, $fh);
}

my @files = glob "ags/object/ags_marshal.h";

foreach $f (@files){
    test_marshallers($f, $fh);
}

my @files = File::Find::Rule->name("*.h")->in("ags/file");

foreach $f (@files){
    test_func($f, $fh);
}

close($fh);

# libags_thread.sym.in
my $filename = 'libags_thread.sym.in';

open(my $fh, '>>', $filename) or die "Could not open file '$filename' $!";

my $license = 'license-notice-gnu-gpl-3-0+-sym.txt';

open(my $license_fh, '<', $license) or die "Could not open file '$license' $!";

while(my $line = <$license_fh>){
    print $fh $line;
}

print $fh "\n";

close($license_fh);

my @files = File::Find::Rule->name("*.h")->in("ags/thread");

foreach $f (@files){
    test_func($f, $fh);
}

close($fh);

# libags_server.sym.in
my $filename = 'libags_server.sym.in';

open(my $fh, '>>', $filename) or die "Could not open file '$filename' $!";

my $license = 'license-notice-gnu-agpl-3-0+-sym.txt';

open(my $license_fh, '<', $license) or die "Could not open file '$license' $!";

while(my $line = <$license_fh>){
    print $fh $line;
}

print $fh "\n";

close($license_fh);

my @files = File::Find::Rule->name("*.h")->in("ags/server");

foreach $f (@files){
    test_func($f, $fh);
}

close($fh);

# libags_audio.sym.in
my $filename = 'libags_audio.sym.in';

open(my $fh, '>>', $filename) or die "Could not open file '$filename' $!";

my $license = 'license-notice-gnu-gpl-3-0+-sym.txt';

open(my $license_fh, '<', $license) or die "Could not open file '$license' $!";

while(my $line = <$license_fh>){
    print $fh $line;
}

print $fh "\n";

close($license_fh);

my @files = File::Find::Rule->name("*.h")->in("ags/plugin");

foreach $f (@files){
    test_func($f, $fh);
}

my @files = File::Find::Rule->name("*.h")->in("ags/audio");

foreach $f (@files){
    test_func($f, $fh);
}

close($fh);

# libags_gui.sym.in
my $filename = 'libags_gui.sym.in';

open(my $fh, '>>', $filename) or die "Could not open file '$filename' $!";

my $license = 'license-notice-gnu-gpl-3-0+-sym.txt';

open(my $license_fh, '<', $license) or die "Could not open file '$license' $!";

while(my $line = <$license_fh>){
    print $fh $line;
}

print $fh "\n";

close($license_fh);

my @files = File::Find::Rule->name("*.h")->in("ags/widget");

foreach $f (@files){
    test_func($f, $fh);
}

my @files = glob "ags/widget/ags_widget_marshal.h";

foreach $f (@files){
    test_widget_marshallers($f, $fh);
}

close($fh);

# libags_vst.sym.in
my $filename = 'libags_vst.sym.in';

open(my $fh, '>>', $filename) or die "Could not open file '$filename' $!";

my $license = 'license-notice-gnu-gpl-3-0+-sym.txt';

open(my $license_fh, '<', $license) or die "Could not open file '$license' $!";

while(my $line = <$license_fh>){
    print $fh $line;
}

print $fh "\n";

close($license_fh);

my @files = File::Find::Rule->name("*.h")->in("ags/vst3-capi");

foreach $f (@files){
    test_func($f, $fh);
}

close($fh);

# subs
sub find_header()
{
    my $F = $File::Find::name;

    if ($F =~ /\.h$/ ) {
        print "$F\n";
    }
}

sub test_func()
{
    my ($current_filename, $fh) = @_;

    print "test $current_filename\n";
    
    open(my $current_fh, '<', $current_filename) or return;

    while(my $line = <$current_fh>){
	if($line =~ /(?<=\s)(ags_[a-z0-9_]+)(?=[\s]*\()/){
	    print $fh "$1\n";
	}
    }
}

sub test_marshallers()
{
    my ($current_filename, $fh) = @_;

#    print "test $current_filename\n";
    
    open(my $current_fh, '<', $current_filename) or return;

    while(my $line = <$current_fh>){
	if($line =~ /(ags_cclosure_marshal_[a-zA-Z0-9_]+)/){
#	    print "match $1\n";
	    
	    print $fh "$1\n";
	}
    }
}

sub test_widget_marshallers()
{
    my ($current_filename, $fh) = @_;

#    print "test $current_filename\n";
    
    open(my $current_fh, '<', $current_filename) or return;

    while(my $line = <$current_fh>){
	if($line =~ /(ags_widget_cclosure_marshal_[a-zA-Z0-9_]+)/){
#	    print "match $1\n";
	    
	    print $fh "$1\n";
	}
    }
}
