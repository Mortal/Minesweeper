#!/usr/bin/env perl

use warnings;
use strict;
use List::Util 'reduce';

sub addtime {
	my ($dim, $mines) = @_;
	my $cmd;
	if (defined $mines) {
		$mines = "--mines $mines";
	} else {
		$mines = '';
	}
	{	local $, = ' ';
		$cmd = "/usr/bin/time ./minesweeper --ai @$dim $mines 2>&1 >/dev/null";
	}
	my $output = qx($cmd);
	chomp $output;
	my $time;
	if ($output =~ /(?|(\d+\.\d+)(?:s\s+)?user|user\s+(\d+m\d+\.\d+))/) {
		$time = $1;
	}
	$time //= 'unknown';
	print "dim: @$dim mines: $mines time: $time\n";
	return ($dim, $mines, $time);
}

my %runs = ();
for my $i (1..3) {
	for my $y (1..10) {
		for my $x ($y..10) {
			my $xx = $x*5;
			my $yy = $y*5;
			my $product = $xx*$yy;
			if (!exists $runs{$product}) {$runs{$product} = [];}
			my (undef, 
			push @{$runs{$product}}, addtim
		}
	}
}
