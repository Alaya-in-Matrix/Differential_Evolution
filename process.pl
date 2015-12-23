#!/usr/bin/perl
use strict;
use warnings;
use 5.010;
system "clear";
system "cat run.log | grep -i best";
print "------------------------------------------\n";
system "tail run.log -n 3";
print "------------------------------------------\n";
system "cat run.log | grep 'idx' > current_bests";
system "wc -l run.log";
open my $fh, "<", "current_bests" or die "can't open file:$!\n";
my @contents;
while(my $line = <$fh>)
{
    $line =~ /^.*fom = (.*)$/;
    my $gain = $1;
    push @contents, $gain;
}
my @sorted = sort {$b <=> $a} @contents;
for(my $i = 0; $i < ($#sorted < 20 ? $#sorted + 1 : 20); ++$i)
{
    print "$sorted[$i]\n";
}
print "============================================\n";
printf "MSP best:\n";
system "cat ~/tmp/tangzhangwen_course_project-gain-no-constraint/spec.log | grep -i best";
print "--------------------------------------------\n";
printf "MSP Iter\n";
system "tail ~/tmp/tangzhangwen_course_project-gain-no-constraint/spec.log -n 3";
