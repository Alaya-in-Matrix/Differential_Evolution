#!/usr/bin/perl
use strict;
use warnings;
use 5.010;

system "cat run.log | grep -i best";
print "------------------------------------------\n";
system "tail run.log -n 3";
print "------------------------------------------\n";
system "cat run.log | grep 'penalty = 0' > current_bests";
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
system "cat ../tangzhangwen_course_project/spec.log | grep -i best";
print "--------------------------------------------\n";
printf "MSP Iter\n";
system "tail ../tangzhangwen_course_project/spec.log -n 3";
