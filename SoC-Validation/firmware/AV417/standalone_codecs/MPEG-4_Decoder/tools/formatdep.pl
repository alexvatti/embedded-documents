#!/usr/bin/perl

if ($#ARGV == -1)
{
  die "\nNo input file specified\n";
}

if ($^O eq "MSWin32")
{
  $ARGV[0] =~ m/(.*)[\\|\/]deps[\\|\/][a-zA-Z0-9_]+\.[a-zA-Z0-9_]+/;
  $path = $1."/";
  $path =~ s/\//\\/g;
}
else
{
$ARGV[0] =~ m/(.*)\/deps\/[a-zA-Z0-9_]+\.[a-zA-Z0-9_]+/;
$path = $1."/";
}

open(INP,$ARGV[0]) || die "\nFailed to open file ",$ARGV[0],"- $!\n";
open(OUT,">dep.tmp") || die "\nFailed to open file dep.tmp - $!\n";
print LOG "\n",$ARGV[0],"\n";

$returnerror = 0;
while (<INP>)
{
  $line = $_;

  if ($^O eq "MSWin32")
  {
    $line =~ s/\//\\/g;
  }

  if ($line=~/^.*:[\t ]+.*/)
  {
    if ($^O eq "MSWin32")
    {
      print OUT "${path}obj\\",$line;
    }
    else
    {
      print OUT "${path}obj/",$line;
    }
  }
  elsif ($line=~/^[\t ]*(No)|[0-9]+[\t ]*user errors?[\t ]*(No)|[0-9]+[\t ]*warnings?/)
  {
    #don't emit errors/warnings, but do remember that they occurred
    $returnerror = 1;
  } 
  else
  {
    print OUT $line;
  }
}

close(INP);
close(OUT);
unlink($ARGV[0]);
rename "dep.tmp",$ARGV[0];

exit($returnerror);

