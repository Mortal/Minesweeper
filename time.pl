#!/usr/bin/env perl

use warnings;
use strict;
use List::Util 'sum';
use Time::HiRes qw(gettimeofday tv_interval);

sub addtime {
	my ($args) = @_;
	my $cmd = "/usr/bin/time ./minesweeper --ai $args 2>&1 >/dev/null";
	my $output = qx($cmd);
	chomp $output;
	my $time;
	if ($output =~ /(?|(\d+\.\d+)(?:s\s+)?user|user\s+(\d+m\d+\.\d+))/) {
		$time = $1;
	} else {
		print STDERR "Couldn't parse: $output\n";
		return;
	}
	return $time;
}

my $size = 1;
while (1) {
	my $start = [gettimeofday];
	my @runtimes;
	while (tv_interval($start, [gettimeofday]) < 1 or @runtimes < 5) {
		push @runtimes, addtime "$size $size";
	}
	print $size,'x',$size,' ',sum(@runtimes),'/',scalar(@runtimes),' = ',sum(@runtimes)/@runtimes,"\n";
	++$size;
}
