#!/usr/bin/perl
#############################################################################################
#############################################################################################
## BUILDMAKE.PL
##
## Perl script to generate multiple make files from a setup text file
##
## Version 0.1
##
##
#############################################################################################
#############################################################################################


#Usage buildmake [--setup=<setupfile>] [--project=<project>] [--option=<option>]*

#check the OS
if ($^O eq "MSWin32")
{
  $iswindows = 1;
  $dirsep = '\\';
  $remove = "del /F /S";
}
else
{
  $iswindows = 0;
  $dirsep = '/';
  $remove = "rm -rf";
}

###########################################################
#
#check the arguments, and extract those passed in
#
###########################################################

#no flags given so use the default 'setup' file name
$setup = "setup";
#all other options are set to their defaults
$reqbuild = "default";
@reqoptions = ("default");
$debug = 0;
$release = 0;
$architect = 0;

if ($#ARGV != -1)
{
  foreach $arg (@ARGV)
  {
    if ($arg =~/--setup=(.+)/)
    {
      $setup = $1;
    }
    elsif ($arg =~/--project=(.+)/)
    {
      $reqbuild = $1;
    }
    elsif ($arg =~/--option=(.+)/)
    {
      if ($reqoptions[0] eq "default")
      {
        shift @reqoptions;
      }
      if ($1 eq "debug")
      {
        if ($release == 1)
        {
          die "\nCan't specify both debug and release builds!\n";
        }
        $debug = 1;
      }
      elsif ($1 eq "release")
      {
        if ($debug == 1)
        {
          die "\nCan't specify both debug and release builds!\n";
        }
        $release = 1;
      }
      elsif ($1 eq "architect")
      {
	$architect = 1;
      }
      push(@reqoptions,$1);
    }
    else
    {
      die "\nInvalid option $arg\n";
    }
  }
}

#check the setup file exists, and open it
if (!open (SETUP,$setup))
{
  #also try the file in the tools sub-directory
  open (SETUP,"tools$dirsep$setup") || die "\nFailed to open setup file $setup - $!\n";
}


###########################################################
#
#parse the setup file
#
###########################################################

$insection = 0; #enumerated value for which section is currently being parsed
		#0 - none, 1 - tools, 2 - sources, 3 - builds, 4 - options, 5 - paths
#remember which sections have been visited
$foundtools = 0;
$foundsources = 0;
$foundbuilds = 0;
$foundoptions = 0;
$lineno = 0;
while (<SETUP>)
{
  $line = $_;
  $lineno++;
  if (($line=~/^[\t ]+$/) || ($line=~/^[\t ]*#/))
  {
    #ignore comment and blank lines
  }
  elsif ($line=~/^:paths:/)
  {
    $insection = 5;
  }
  elsif ($line=~/^:tools:/)
  {
    #start of the tools section
    if ($foundtools == 1)
    {
      die "\nFailed on line $lineno, section :tools: multiply defined\n";
    }
    $foundtools = 1;
    $insection = 1;
  }
  elsif ($line=~/^:sources:/)
  {
    #start of the sources section
    if ($foundsources == 1)
    {
      die "\nFailed on line $lineno, section :sources: multiply defined\n";
    }
    $foundsources = 1;
    $insection = 2;
    $infilegroups = 0;
  }
  elsif ($line=~/^:builds:/)
  {
    #start of the builds section
    if ($foundbuilds == 1)
    {
      die "\nFailed on line $lineno, section :builds: multiply defined\n";
    }
    $foundbuilds = 1;
    $insection = 3;
  }
  elsif ($line=~/^:options:/)
  {
    #start of the options section
    if ($foundoptions == 1)
    {
      die "\nFailed on line $lineno, section :options: multiply defined\n";
    }
    $foundoptions = 1;
    $insection = 4;
  }
  elsif ($line=~/^:end:/)
  {
    last;
  }
  else
  {
    #this part is used to extract content, according to the section type
    if ($insection == 1)
    {
      #the tools
      if ($line=~/^([a-zA-Z][a-zA-Z0-9_]*):/)
      {
        #start of a new toolchain description
        push @toolchain,$1; #the toolchain name
        push @toolchain,{}; #add an anonymous hash reference ready to store the data
        $tool = $toolchain[$#toolchain]; #get a reference to the anonymous hash for the tools
      }
      elsif ($line=~/^[\t ]*([a-zA-Z]+)\)[\t ]*(.*)$/)
      {
	    #start of a new tool entry
        if ($1 eq "path")
        {
          $path = $2;
          $lc = chop $path;
	      #check the path for conversions
	      if ($iswindows)
          {
            $path =~ s/\//\\/g;
          }
          else
          {
	       #ensure no volume is specified
            if ($path=~/:/)
            {
	          die "\nSetup line $lineno: Invalid to use Windows volume in path $path$lc for Linux/Unix\n";
	        }
            $path =~ s/\\/\//g;
	      }	   
          if ($lc eq $dirsep)
          {
            $$tool{$1} = $path; #add the path to the current tool hash (using the reference)	    
          }
          else
          {
            $$tool{$1} = $path.$lc; #add the path to the current tool hash (using the reference)
          }
        }
        elsif ($1 eq "libraries")
        {
          $$tool{$1} = []; #add an anonymous array to the current tool
          $toolsettings = $$tool{$1};
        }
        else
        {
          $$tool{$1} = {}; #add an anonymous hash to the current tool hash (using the reference)
          $toolsettings = $$tool{$1}; #get a reference to the anonymous hash for the settings
        }
      }
      elsif ($line=~/^[\t ]*([a-zA-Z][\-a-zA-Z]*)>[\t ]*(.*)$/)
      {
        $opt = $2;
	    #check the option for path conversions
	    if ($iswindows)
        {
          $opt =~ s/\//\\/g;
        }
        else
        {
	      #ensure no volume is specified
          if ($opt=~/[a-zA-Z]:\\/)
          {
	        die "\nSetup line $lineno: Invalid to use Windows volume in path in options $opt for Linux\n";
	      }
          $opt =~ s/\\/\//g;
	    }
	    #start of a new tool setting - add a hash entry for the setting, using the current reference
        if ($1 eq "opt-lib")
        {
          #add to the array
          push (@$toolsettings,$opt);
        }
        else
        {
          #add to the hash
          $$toolsettings{$1} = $opt;
        }
      }
      elsif (/^[\t ]*$/)
      {
        #ingore whitespace lines
      }
      else
      {
        die "\nSetup line $lineno: unrecognised tools  entry \"$line\"\n";
      }
    }
    elsif ($insection == 2)
    {
      #the source files
      if ($line=~/^([a-zA-Z][a-zA-Z0-9_]*):[\t ]*(.*)/)
      {
        #start of a new sources entry
        $entry = $1;
        if ($entry eq "basedir")
        {
          $basedir = $2;
	      #check the option for path conversions
	      if ($iswindows)
          {
            $basedir =~ s/\//\\/g;
          }
          else
          {
	        #ensure no volume is specified
            if ($basedir=~/[a-zA-Z]:\\/)
            {
	          die "\nSetup line $lineno: Invalid to use Windows volume in basedir path $basedir for Linux\n";
	        }
            $basedir =~ s/\\/\//g;
	      }
          $infilegroups = 0;
        }
        elsif ($entry eq "includes")
        {
          $inc = $2;
	      #check the option for path conversions
	      if ($iswindows)
          {
            $inc =~ s/\//\\/g;
          }
          else
          {
	        #ensure no volume is specified
            if ($inc=~/[a-zA-Z]:\\/)
            {
	          die "\nSetup line $lineno: Invalid to use Windows volume in include path $inc for Linux\n";
	        }
            $inc =~ s/\\/\//g;
	      }
          @includes = split(/,/,$inc);
          $infilegroups = 0;
        }
        elsif ($entry eq "filegroups")
        {
          $infilegroups = 1;
        }
        elsif ($entry eq "subprojects")
        {
          $infilegroups = 2;
        }
      }
      elsif (($infilegroups == 1) && ($line=~/[\t ]*([a-zA-Z]+)\)(.*)/))
      {
        #filegroups entry
        $group = $1;
        $filegroups{$group} = []; #the filegroup name references an anonymous array
        $grp = $filegroups{$group}; #get a reference to the anonymous array for the tools
        #check if a base directory is specified
        $tmp = $2;
        if ($tmp=~/[\t ]*([^:]+):[\t ]*(.*)/)
        {
          $prefixdir = $1;
          $filegroups{"$group.prefix"} = $prefixdir;
          $tmp = $2;
          @$grp = map("$prefixdir$dirsep$_",split(/[\t ]*,[\t ]*/,$tmp));
        }
        else
        {
          @$grp = split(/[\t ]*,[\t ]*/,$tmp);
        }
        foreach $item (@$grp)
        {
	      #check the option for path conversions
	      if ($iswindows)
          {
            $item =~ s/\//\\/g;
          }
          else
          {
	        #ensure no volume is specified
            if ($item=~/[a-zA-Z]:\\/)
            {
	          die "\nSetup line $lineno: Invalid to use Windows volume in source path $item for Linux\n";
	    }
            $item =~ s/\\/\//g;
	  }
        }
      }
      elsif (($infilegroups == 2) && ($line=~/[\t ]*([a-zA-Z]+)\)(.*)/))
      {
        #subprojects entry
        $group = $1;
        $subprojects{$group} = []; #the filegroup name references an anonymous array
        $grp = $subprojects{$group}; #get a reference to the anonymous array for the tools
        #check if a base directory is specified
        $tmp = $2;
        if ($tmp=~/[\t ]*([^:]+):[\t ]*(.*)/)
        {
          $prefixdir = $1;
          $subprojects{"$group.prefix"} = $prefixdir;
          $tmp = $2;
          @$grp = map("$prefixdir$dirsep$_",split(/[\t ]*,[\t ]*/,$tmp));
        }
        else
        {
          @$grp = split(/[\t ]*,[\t ]*/,$tmp);
        }
        foreach $item (@$grp)
        {
	      #check the option for path conversions
	      if ($iswindows)
          {
            $item =~ s/\//\\/g;
          }
          else
          {
	        #ensure no volume is specified
            if ($item=~/[a-zA-Z]:\\/)
            {
	          die "\nSetup line $lineno: Invalid to use Windows volume in source path $item for Linux\n";
	    }
            $item =~ s/\\/\//g;
	  }
        }
      }
      elsif (/^[\t ]*$/)
      {
        #ignore blank lines
      }
      else
      {
        die "\nSetup line $lineno:  unrecognised sources entry $line\n";
      }
    }
    elsif ($insection == 3)
    {
      #builds section
      if ($line=~/^([a-zA-Z][a-zA-Z0-9_]*):/)
      {
        #a new build project
        push @builds,$1; #the build project name
        push @builds,{}; #add an anonymous hash to the table to hold the build options
        $build = $builds[$#builds]; #get a reference to the current build options
        $buildtoolchain = undef;
      }
      elsif ($line=~/[\t ]*([a-zA-Z]+)\)[\t ]*(.*)/)
      {
        #individual option for a build
	    if ($1 eq "default")
        {
          #default has no real setting
          $$build{$1} = "set";
        }
        elsif ($1 eq "toolchain")
        {
          #add the array to hold a set of toolchains, if required
          if ($buildtoolchain == undef)
          {
            #print "\nCreating dummy hash";
            $$build{"toolchain"} = {}; 
            $buildtoolchain = $$build{"toolchain"};
          }
          #add the toolchain by name, with a dummy array
          $$buildtoolchain{$2} = [];
          #keep a handy reference to the current array of files for the toolchain
          $currenttoolchain = $$buildtoolchain{$2};
        }
        elsif ($1 eq "libtoolchain")
        {
          $$build{$1} = [$2]; #add an anonymous array to the current tool
          $toolsettings = $$build{$1};
        }
        elsif ($1 eq "libraries")
        {
          $$build{$1} = []; #add an anonymous array to the current tool
          $toolsettings = $$build{$1};
        }
        else
        {
          #add the setting
          $$build{$1} = $2;
        }
      }
      elsif ($line=~/[\t ]*files>[\t ]*(.*)/)
      {
        #add the split file entries (so this will form an array where the
        #name of the toolchain is the first entry, then come the file groups
        push @$currenttoolchain,split(/[ \t]*,[\t ]*/,$1);
        $ref1 = $$build{"toolchain"};
        $ref = $$ref1{"gnupc"};
      }
      elsif ($line=~/[\t ]*opt-lib>[\t ]*(.*)/)
      {
        $opt = $1;
	    #check the option for path conversions
	    if ($iswindows)
        {
          $opt =~ s/\//\\/g;
        }
        else
        {
	      #ensure no volume is specified
          if ($opt=~/^[a-zA-Z]:\\/)
          {
	        die "\nSetup line $lineno: Invalid to use Windows volume in library path $opt for Linux\n";
	      }
          $opt =~ s/\\/\//g;
	    }
        #add to the array
        push (@$toolsettings,$opt);
      }
      elsif ($line=~/[\t ]*opt-all>[\t ]*(.*)/)
      {
        $opt = $1;
	    #check the option for path conversions
	    if ($iswindows)
        {
          $opt =~ s/\//\\/g;
        }
        else
        {
	      #ensure no volume is specified
          if ($opt=~/[a-zA-Z]:\\/)
          {
	        die "\nSetup line $lineno: Invalid to use Windows volume in options $opt for Linux\n";
	      }
          $opt =~ s/\\/\//g;
	    }
        #add to the array
        push (@$toolsettings,$opt);
      }
    }
    elsif ($insection == 4)
    {
      #the options section
      if ($line=~/^([a-zA-Z][a-zA-Z0-9_-]*):/)
      {
        #add a new option entry
        push @options,$1; #the name of the option set
        push @options,{}; #the set of option settings (a reference for now)
        $option = $options[$#options];
      }
      elsif ($line=~/[\t ]*([a-zA-Z][a-zA-Z0-9_-]*)\)[\t ]*(.*)/)
      {
        #add an entry
        if ($1 eq "default")
        {
          $$option{$1} = "set";
        }
        elsif ($1 eq "addfiles")
        {
          $$option{"addfiles"} = {};
          $addfiles = $$option{"addfiles"};
        }
        else
        {
          $opt = $2;
	      #check the option for path conversions
	      if ($iswindows)
          {
            $opt =~ s/\//\\/g;
          }
          else
          {
	        #ensure no volume is specified
            if ($opt=~/[a-zA-Z]:\\/)
            {
	          die "\nSetup line $lineno: Invalid to use Windows volume in option cflags $opt for Linux\n";
	        }
            $opt =~ s/\\/\//g;
	      }
          $$option{$1} = $opt;
        }
      }
      elsif ($line=~/[\t ]*(.*)>[\t ]*(.*)/)
      {
        $$addfiles{$1} = $2;
      }
      elsif (/^[\t ]*$/)
      {
        #ignore blank lines
      }
      else
      {
        die "\nSetup line $lineno: unrecognised options entry $line\n";
      }
    }
    elsif ($insection == 5)
    {
      #the paths section
      if ($line=~/^[\t ]*([a-zA-Z][a-zA-Z0-9_-]*):[\t ]*(.*)/)
      {
        #add a new paths entry
        $paths{"$1"} = $2;
      }
    }
  } 
}

###########################################################
#
#check the parsed data for consistency
#
###########################################################

#sort out any path variables
sub ManagePaths
{
  my ($key) = @_;
  if ($key eq "")
  {
    return "";
  }
  while (1)
  {
    #check for any use of other paths
    if ($key =~/\$\{([^}]*)\}/)
    {
      $replacement = $paths{$1};
      if (!defined($replacement))
      {
        die "Use of path variable $1 which has not been specified";
      }
      else
      {
        $key=~s/\$\{$1\}/$replacement/g;
      }
    }
    else
    {
      last;
    }
  }
  return $key;
}

#sort out the paths entries themselves first of all
foreach $key (keys %paths)
{
  $paths{$key} = ManagePaths($paths{$key});
  if ($paths{$key} eq "")
  {
    $paths{$key} = ".";
  }
}
#then the base directory of the source code
$basedir = ManagePaths($basedir);
if ($basedir eq "")
{
  $basedir = ".";
}
#and all includes entries
foreach $inc (@includes)
{
  $inc = ManagePaths($inc);
  if ($inc eq "")
  {
    $inc = ".";
  }
}

#determine the build to use
$defaultfound = 0;
$selectedbuild = undef;
for ($i=0;$i<=$#builds;$i+=2)
{
  #see if a build was specified
  if ($reqbuild eq $builds[$i])
  {
    $selectedbuild = $builds[$i+1];
  }  
  $buildset = $builds[$i+1];
  #check there is only one default build (if any)
  if ($$buildset{"default"} eq "set")
  {
    if ($defaultfound != 0)
    {
      die "\nMultiple default builds specified, please check the :builds: section!\n";
    }
    $defaultfound = 1;
    #if a default build is required, then select it
    if ($reqbuild eq "default")
    {
      $selectedbuild = $buildset;
      $reqbuild = $builds[$i];
    }
  }
}


#Tell the world the project being built
$project = $$selectedbuild{"name"};
$type = $$selectedbuild{"type"};
$libtoolsarray = $$selectedbuild{"libtoolchain"};
$libtools = $$libtoolsarray[0];

print "\nBuilding Makefile for project $project, $type, using $reqbuild\n";

#check that the selected project's toolchain(s) exist
$toolchains = $$selectedbuild{"toolchain"};
foreach $key (keys %$toolchains)
{
  #$key holds the name of the required toolchain
  $found = 0;
  for ($i=0;$i<$#toolchain;$i+=2)
  {
    if ($toolchain[$i] eq $key)
    {
      $found = 1;
      #form a new hash of the selected toolchains, for quick ref.
      $selectedtoolchains{$key} = $toolchain[$i+1];
      last;
    }    
  }
  if ($found == 0)
  {
    die "\nRequired toolchain $key is not defined";
  }
}
#add the lib toolchain if not already there
if ($selectedtoolchains{$libtools} == undef)
{
  $found = 0;
  for ($i=0;$i<$#toolchain;$i+=2)
  {
    if ($toolchain[$i] eq $libtools)
    {
      $found = 1;
      #form a new hash of the selected toolchains, for quick ref.
      $selectedtoolchains{$libtools} = $toolchain[$i+1];
      last;
    }    
  }  
  if ($found == 0)
  {
    die "\nRequired library toolchain $key is not defined";
  }
}

#if no options are selected, then select all default ones
if ($reqoptions[0] eq "default")
{
  for ($i=0;$i<=$#options;$i+=2)
  {
    $option = $options[$i+1];
    if ($$option{"default"} eq "set")
    {
      $selectedoptions{$options[$i]} = $options[$i+1];
    }
  }
}
else
{
  #select the required options
  $abort = 0;
  foreach $option (@reqoptions)
  {
    $found = 0;
    for ($i=0;$i<=$#options;$i+=2)
    {
      if ($option eq $options[$i])
      {
        $selectedoptions{$options[$i]} = $options[$i+1];
        $found = 1;
        last;
      }
    }
    #check against intrinsic options
    if (($found == 0) && !(($option eq "debug") || ($option eq "release") || ($option eq "architect")))
    {
      print "\nOption $option is invalid!\n";
      $abort = 1;
    }
  }
  if ($abort == 1)
  {
    exit(1);
  }
} 

#Tell the world the options selected
print "\nSelected options are";
foreach $key (keys %selectedoptions)
{
  print "\n  $key";
}
if ($debug == 1)
{
  print "\n  debug";
}
elsif ($release == 1)
{
  print "\n  release";
}
if ($architect == 1)
{
  print "\n  architect";
}

print "\n";

###########################################################
#
#generate the make file
#
###########################################################

open(MAKE,">Makefile") || die "\nFailed to create Makefile - $1\n";

print "\nGenerating Makefile...";

print MAKE <<EOF;
##################################################################################
#
# Auto-generated Makefile, by buildmake
#
##################################################################################

#########################################################
# Declare all required build variables
#########################################################

## DELETE= rm -rf 

#options
EOF

print MAKE "DELETE= ", $remove, "\n"; 

foreach $optkey (keys %selectedoptions)
{
  if ($architect == 0)
  {
    print MAKE "\nexport ENABLE_",uc $optkey,"=true";
  }
  else
  {
    print MAKE "\nexport ENABLE_",uc $optkey,"=,uc $optkey,";
  }
}

#generate all the base directory variables, and paths to execs
print MAKE "\n\n#tools setup\n\nBASEDIR=$basedir";
foreach $key (keys %paths)
{
  print MAKE "\n$key=",$paths{$key};
}
#generate the include directories
print MAKE "\n\nINCLUDES= -I.$dirsep",join(" -I.$dirsep",@includes);
#and generic options
print MAKE "\nOPT_FLAGS= ";
foreach $key (keys %selectedoptions)
{
  $opt = $selectedoptions{$key};
  if ($opt ne "")
  {
    print MAKE "\nifeq (\${ENABLE_",uc $key,"},true)";
    print MAKE "\nOPT_FLAGS+= ",$$opt{"addcflags"};
    print MAKE "\nendif";  
  }
}
print MAKE "\nexport OPT_FLAGS";

#output the toolchain info
foreach $tcname (keys %selectedtoolchains)
{
  $tc = $selectedtoolchains{$tcname};
  $tmp = ManagePaths($$tc{"path"});
  if ($tmp ne "")
  {
    $tmp = $tmp.$dirsep;
  }
  print MAKE "\n\n",uc $tcname,"_PATH=$tmp";
  print MAKE "\n",uc $tcname,"_CC=\$\{",uc $tcname,"_PATH\}",$$tc{"compiler"}{"name"};
  $tmp = ManagePaths($$tc{"compiler"}{"opt-all"});
  print MAKE "\n",uc $tcname,"_CC_FLAGS=$tmp";
  if ($debug == 1)
  {
    print MAKE " ",$$tc{"compiler"}{"opt-dbg"};
  }
  elsif ($release == 1)
  {
    print MAKE " ",$$tc{"compiler"}{"opt-rel"};
  }
  print MAKE "\n",uc $tcname,"_LD=\$\{",uc $tcname,"_PATH\}",$$tc{"linker"}{"name"};
  $tmp = ManagePaths($$tc{"linker"}{"opt-all"});
  print MAKE "\n",uc $tcname,"_LD_FLAGS=$tmp";
  print MAKE "\n",uc $tcname,"_AR=\$\{",uc $tcname,"_PATH\}",$$tc{"archiver"}{"name"};
  $tmp = ManagePaths($$tc{"archiver"}{"opt-all"});
  print MAKE "\n",uc $tcname,"_AR_FLAGS=$tmp";
  print MAKE "\n",uc $tcname,"_DEP=\$\{",uc $tcname,"_PATH\}",$$tc{"deps"}{"name"};
  $tmp = ManagePaths($$tc{"deps"}{"opt-all"});
  print MAKE "\n",uc $tcname,"_DEP_FLAGS=$tmp";
}

#generate the library info

$ref = $selectedtoolchains{$libtools};
#get a reference to any specified libraries to include
$libref = $$ref{"libraries"};
print MAKE "\n\nLIBS = ";
foreach $lib (@$libref)
{
  #check if a path is specified
  if ($lib=~/(.*):(.*)/)
  {
    #prefix each library by the specified library path
    $liblist = $2;
    @libfiles = split(/,/,$liblist);
    if ($#libfiles != -1)
    {
      $path = ManagePaths($1);
      print MAKE " $path$dirsep",join(" $path$dirsep",@libfiles);
    }
  }
  else
  {
    #otherwise list items are  prefixed with '-l'
    @libfiles = split(/,/,$lib);
    if ($#libfiles != -1)
    {
      print MAKE " -l",join(" -l",@libfiles);
    }
  }  
}
#do the same thing for any build specific libraries
$libref = $$selectedbuild{"libraries"};
foreach $lib (@$libref)
{
  #check if a path is specified
  if ($lib=~/(.*):(.*)/)
  {
    #prefix each library by the specified library path
    $liblist = $2;
    @libfiles = split(/,/,$liblist);
    if ($#libfiles != -1)
    {
      $path = ManagePaths($1);
      print MAKE " $path$dirsep",join(" $path$dirsep",@libfiles);
    }
  }
  else
  {
    #otherwise list items are  prefixed with '-l'
    @libfiles = split(/,/,$lib);
    if ($#libfiles != -1)
    {
      print MAKE " -l",join(" -l",@libfiles);
    }
  }  
}

#work through the file groups for the selected project

print MAKE "\n\n#Individual file build rules\n";
$tc = $$selectedbuild{"toolchain"};
foreach $key (%$tc)
{
  #get the list of groups
  $groups = $$tc{$key};
  #get the set of files for each group
  foreach $group (@$groups)
  {
    $files = $filegroups{$group};
    $prefixdir = $filegroups{"$group.prefix"};
    if ($prefixdir ne "")
    {
      $prefixdir = $prefixdir.$dirsep;
    }
    #generate the rules for each file (and keep a record of all object files)
    #two rules are generated for each file, the dependency build and the actual build
    foreach $file (@$files)
    {
      if ($file=~/^[\t ]*(.*)%\.cpp$/)
      {
        #wildcard C++ files
        $dir = $1;
        $lc = chop($dir);
        if ($lc eq $dirsep)
        {
          $dir = $dirsep.$dir;
        }
        else
        {
	      $dir = $dirsep.$dir.$lc;
        }
        $tmp = ManagePaths($basedir.$dir);
        opendir(DIR,$tmp) || die "\nFailed to open directory, $tmp - $!\n";
        @srcs = grep{/.*\.cpp/} readdir(DIR);
        foreach $src (@srcs)
        {
          if ($src=~/[\t ]*(.*)\.cpp$/)
          {
            if ($group eq "main")
            {
              push(@appobjfiles,$prefixdir."obj$dirsep$1.o");
              push(@appdepfiles,$prefixdir."deps$dirsep$1.dep");
              push(@appsrcfiles,"\${BASEDIR}$dir$dirsep$1.cpp");
              push(@apptoolchain,$key);              
            }
            else
            {
              push(@libobjfiles,$prefixdir."obj$dirsep$1.o");
              push(@libdepfiles,$prefixdir."deps$dirsep$1.dep");
              push(@libsrcfiles,"\${BASEDIR}$dir$dirsep$1.cpp");
              push(@libtoolchain,$key);              
            }
	      }
        }
	    closedir(DIR);
      }
      elsif ($file=~/^[\t ]*(.*)%\.c/)
      {
        #wildcard C files
        $dir = $1;
        $lc = chop($dir);
        if ($lc eq $dirsep)
        {
          $dir = $dirsep.$dir;
        }
        else
        {
	      $dir = $dirsep.$dir.$lc;
        }
        $tmp = ManagePaths($basedir.$dir);
        opendir(DIR,$tmp) || die "\nFailed to open directory, $tmp - $!\n";
        @srcs = grep{/.*\.c/} readdir(DIR);
        foreach $src (@srcs)
        {          
	      #print "\nsrc is #$src#";
          if ($src=~/[\t ]*(.*)\.c$/)
          {
            #print "\nmatched is #$1#";
            if ($group eq "main")
            {
              push(@appobjfiles,$prefixdir."obj$dirsep$1.o");
              push(@appdepfiles,$prefixdir."deps$dirsep$1.dep");
              push(@appsrcfiles,"\${BASEDIR}$dir$dirsep$1.c");
              push(@apptoolchain,$key);              
            }
            else
            {
              push(@libobjfiles,$prefixdir."obj$dirsep$1.o");
              push(@libdepfiles,$prefixdir."deps$dirsep$1.dep");
              push(@libsrcfiles,"\${BASEDIR}$dir$dirsep$1.c");
              push(@libtoolchain,$key);              
            }
          }            
        }
	    closedir(DIR);
      }
      elsif ($file=~/[\t ]*(.*)\.cpp/)
      {
        #single C++ file
        $path = $1;
        if ($dirsep eq "\\")
        {
          @files = split(/\\/,$path);
        }
        else
        {
          @files = split(/\//,$path);
        }
        $file = pop @files;        
        if ($group eq "main")
        {
          push(@appobjfiles,$prefixdir."obj$dirsep$file.o");
          push(@appdepfiles,$prefixdir."deps$dirsep$file.dep");
          push(@appsrcfiles,"$path.cpp");
          push(@apptoolchain,$key);              
        }
        else
        {
          push(@libobjfiles,$prefixdir."obj$dirsep$file.o");
          push(@libdepfiles,$prefixdir."deps$dirsep$file.dep");
          push(@libsrcfiles,"$path.cpp");
          push(@libtoolchain,$key);              
        }
      }
      elsif ($file=~/[\t ]*(.*)\.c/)
      {
        #single C++ file
        $path = $1;
        if ($dirsep eq "\\")
        {
          @files = split(/\\/,$path);
        }
        else
        {
          @files = split(/\//,$path);
        }
        $file = pop @files;        
        if ($group eq "main")
        {
          push(@appobjfiles,$prefixdir."obj$dirsep$file.o");
          push(@appdepfiles,$prefixdir."deps$dirsep$file.dep");
          push(@appsrcfiles,"$path.c");
          push(@apptoolchain,$key);              
        }
        else
        {
          push(@libobjfiles,$prefixdir."obj$dirsep$file.o");
          push(@libdepfiles,$prefixdir."deps$dirsep$file.dep");
          push(@libsrcfiles,"$path.c");
          push(@libtoolchain,$key);              
        }
      }
    }
  }
}

print MAKE "\nAPPOBJS=",join(" ",@appobjfiles);
print MAKE "\nAPPDEPS=",join(" ",@appdepfiles);
print MAKE "\nLIBOBJS=",join(" ",@libobjfiles);
print MAKE "\nLIBDEPS=",join(" ",@libdepfiles);

$tc = $$selectedbuild{"toolchain"};
foreach $key (%$tc)
{  
  #add any optional groups owned by this toolchain
  foreach $optkey (keys %selectedoptions)
  {
    $#optappobjfiles = -1;
    $#optappdepfiles = -1;
    $#optappsrcfiles = -1;
    $#optapptoolchain = -1;
    $#optlibobjfiles = -1;
    $#optlibdepfiles = -1;
    $#optlibsrcfiles = -1;
    $#optlibtoolchain = -1;
    $#optgroups = -1;
    #print "\n $optkey";
    $opt = $selectedoptions{$optkey};
    $af = $$opt{"addfiles"};
    #print "\n ",$$af{"toolchain"};
    if ($$af{"toolchain"} eq $key)
    {
      push (@optgroups,$$af{"groups"});
    }
    if ($#optgroups == -1)
    {
      next;
    }
    print MAKE "\nifeq (\${ENABLE_",uc $optkey,"},true)";
    #get the set of files for each group
    foreach $group (@optgroups)
    {
      $files = $filegroups{$group};  
      $prefixdir = $filegroups{"$group.prefix"}; 
      if ($prefixdir ne "")
      {
        $prefixdir = $prefixdir.$dirsep;
      }
      #generate the rules for each file (and keep a record of all object files)
      #two rules are generated for each file, the dependency build and the actual build
      foreach $file (@$files)
      {
        if ($file=~/^[\t ]*(.*)%\.cpp$/)
        {
          #wildcard C++ files
          $dir = $1;
          $lc = chop($dir);
          if ($lc eq $dirsep)
          {
            $dir = $dirsep.$dir;
          }
          else
          {
	        $dir = $dirsep.$dir.$lc;
          }
          $tmp = ManagePaths($basedir.$dir);
          opendir(DIR,$tmp) || die "\nFailed to open directory, $tmp - $!\n";
          @srcs = grep{/.*\.cpp/} readdir(DIR);
          foreach $src (@srcs)
          {          
            if ($src=~/[\t ]*(.*)\.cpp$/)
            {
              if ($group eq "main")
              {
                push(@optappobjfiles,$prefixdir."obj$dirsep$1.o");
                push(@optappdepfiles,$prefixdir."deps$dirsep$1.dep");
                push(@optappsrcfiles,"\${BASEDIR}$dir$dirsep$1.cpp");
                push(@optapptoolchain,$key);              
              }
              else
              {
                push(@optlibobjfiles,$prefixdir."obj$dirsep$1.o");
                push(@optlibdepfiles,$prefixdir."deps$dirsep$1.dep");
                push(@optlibsrcfiles,"\${BASEDIR}$dir$dirsep$1.cpp");
                push(@optlibtoolchain,$key);              
              }
	        }
          }
	      closedir(DIR);
        }
        elsif ($file=~/^[\t ]*(.*)%\.c/)
        {
          #wildcard C files
          $dir = $1;
          $lc = chop($dir);
          if ($lc eq $dirsep)
          {
            $dir = $dirsep.$dir;
          }
          else
          { 
	        $dir = $dirsep.$dir.$lc;
          }
          $tmp = ManagePaths($basedir.$dir);
          opendir(DIR,$tmp) || die "\nFailed to open directory, $tmp - $!\n";
          @srcs = grep{/.*\.c/} readdir(DIR);
          foreach $src (@srcs)
          {          
	        #print "\nsrc is #$src#";
            if ($src=~/[\t ]*(.*)\.c$/)
            {
              #print "\nmatched is #$1#";
              if ($group eq "main")
              {
                push(@optappobjfiles,$prefixdir."obj$dirsep$1.o");
                push(@optappdepfiles,$prefixdir."deps$dirsep$1.dep");
                push(@optappsrcfiles,"\${BASEDIR}$dir$dirsep$1.c");
                push(@optapptoolchain,$key);              
              }
              else
              {
                push(@optlibobjfiles,$prefixdir."obj$dirsep$1.o");
                push(@optlibdepfiles,$prefixdir."deps$dirsep$1.dep");
                push(@optlibsrcfiles,"\${BASEDIR}$dir$dirsep$1.c");
                push(@optlibtoolchain,$key);              
              }
            }            
          }
	      closedir(DIR);
        }
        elsif ($file=~/[\t ]*(.*)\.cpp/)
        {
          #single C++ file
          $path = $1;
          if ($dirsep eq "\\")
          {
            @files = split(/\\/,$path);
          }
          else
          {
            @files = split(/\//,$path);
          }
          $file = pop @files;        
          if ($group eq "main")
          {
            push(@optappobjfiles,$prefixdir."obj$dirsep$file.o");
            push(@optappdepfiles,$prefixdir."deps$dirsep$file.dep");
            push(@optappsrcfiles,"$path.cpp");
            push(@optapptoolchain,$key);              
          }
          else
          {
            push(@optlibobjfiles,$prefixdir."obj$dirsep$file.o");
            push(@optlibdepfiles,$prefixdir."deps$dirsep$file.dep");
            push(@optlibsrcfiles,"$path.cpp");
            push(@optlibtoolchain,$key);
          }
        }
        elsif ($file=~/[\t ]*(.*)\.c/)
        {
          #single C++ file
          $path = $1;
          if ($dirsep eq "\\")
          {
            @files = split(/\\/,$path);
          }
          else
          {
            @files = split(/\//,$path);
          }
          $file = pop @files;        
          if ($group eq "main")
          {
            push(@optappobjfiles,$prefixdir."obj$dirsep$file.o");
            push(@optappdepfiles,$prefixdir."deps$dirsep$file.dep");
            push(@optappsrcfiles,"$path.c");
            push(@optapptoolchain,$key);              
          }
          else
          {
            push(@optlibobjfiles,$prefixdir."obj$dirsep$file.o");
            push(@optlibdepfiles,$prefixdir."deps$dirsep$file.dep");
            push(@optlibsrcfiles,"$path.c");
            push(@optlibtoolchain,$key);              
          }
        }
      }
    }
    if ($#optappobjfiles != -1)
    {
      print MAKE "\nAPPOBJS+=",join(" ",@optappobjfiles);
      print MAKE "\nAPPDEPS+=",join(" ",@optappdepfiles);
    }
    if ($#optlibobjfiles != -1)
    {
      print MAKE "\nLIBOBJS+=",join(" ",@optlibobjfiles);
      print MAKE "\nLIBDEPS+=",join(" ",@optlibdepfiles);        
    }
    print MAKE "\nendif";
    push(@appobjfiles,@optappobjfiles);
    push(@appdepfiles,@optappdepfiles);
    push(@appsrcfiles,@optappsrcfiles);
    push(@apptoolchain,@optapptoolchain);
    push(@libobjfiles,@optlibobjfiles);
    push(@libdepfiles,@optlibdepfiles);
    push(@libsrcfiles,@optlibsrcfiles);
    push(@libtoolchain,@optlibtoolchain);
  }
}



#generate the standard targets
print MAKE "\n\n";
print MAKE <<EOF;

#########################################################
# Standard build targets
#########################################################

# default build

default: dep all

all: bin

#dependencies

dep: \${APPDEPS}

libdep: \${LIBDEPS}

# clean builds

libdepclean:
	\${DELETE} \${LIBDEPS} \${LIBOBJS} \${LIBDIR}${dirsep}lib$project.a

clean:
	\${DELETE} \${APPOBJS}

depclean:
	\${DELETE} \${APPDEPS}

libclean:
	\${DELETE} \${LIBOBJS} \${LIBDIR}${dirsep}lib$project.a
EOF

#check for subproject builds
foreach $key (keys %subprojects)
{
  if ($key=~/.*\.prefix/)
  {
    print MAKE "\tmake -C ",$subprojects{$key}," clean";
  }
}

#generate the library build (always done)
print MAKE "\n\n#Generate a library of all files";
print MAKE "\n\nlib: \${LIBDIR}${dirsep}lib$project.a";
print MAKE "\n\${LIBDIR}${dirsep}lib$project.a: \${LIBOBJS}";
#check for subproject builds for dependencies
foreach $key (keys %subprojects)
{
  if ($key=~/.*\.prefix/)
  {
    #do nothing
  }
  else
  {
    $targets = $subprojects{$key};
    foreach $target (@$targets)
    {
      print MAKE " $target";
      push (@subprojecttargets,$target);
    }
  }
}
print MAKE "\n\t\${",uc $libtools,"_AR} \${",uc $libtools,"_AR_FLAGS} \${LIBDIR}${dirsep}lib${project}.a \${LIBOBJS}";

#generate the binary build (only done when required)
if ($type eq "bin")
{
  print MAKE "\n\n#Generate the executable";
  print MAKE "\n\nall: bin\nbin: $project";
  print MAKE "\n$project: \${APPOBJS} \${LIBDIR}${dirsep}lib${project}.a ",@subprojecttargets;
  print MAKE "\n\t\${",uc $libtools,"_LD} \${",uc $libtools,"_LD_FLAGS} ";
  print MAKE $$libtoolsarray[1];
  print MAKE " \${APPOBJS} \${LIBDIR}${dirsep}lib$project.a ",@subprojecttargets," \${LIBS} -o $project\n";
}
else
{
  print MAKE "\n\nall: lib",$dirsep,"lib$project.a\n\n";
}

print MAKE "\n\n";
print MAKE <<EOF;
#########################################################
# Individual file build control for library files
#########################################################
EOF

for ($i=0;$i<=$#libobjfiles;$i++)
{
  print MAKE "\n\n#dependency file generation";
  print MAKE "\n",$libdepfiles[$i],": ",$libsrcfiles[$i],"\n\t\$\{",uc $libtoolchain[$i],"_DEP\} \${OPT_FLAGS} \${INCLUDES} \${",uc $libtoolchain[$i],"_DEP_FLAGS} \${",uc $libtoolchain[$i],"_CC_FLAGS} -c ",$libsrcfiles[$i]," > ",$libdepfiles[$i];
  print MAKE " ; tools",$dirsep,"formatdep.pl ",$libdepfiles[$i];
  print MAKE "\n#include the dependency file, if it exists";
  print MAKE "\nsinclude ",$libdepfiles[$i];
  print MAKE "\n#default target for make";
  print MAKE "\n",$libobjfiles[$i],":\n\t\$\{",uc $libtoolchain[$i],"_CC\} \${OPT_FLAGS} \${INCLUDES} \${",uc $libtoolchain[$i],"_CC_FLAGS} -c ",$libsrcfiles[$i]," -o \$@";
}
print MAKE "\n\n";
print MAKE <<EOF;
#########################################################
# Individual file build control for application files
#########################################################
EOF

for ($i=0;$i<=$#appobjfiles;$i++)
{
  print MAKE "\n\n#dependency file generation";
  print MAKE "\n",$appdepfiles[$i],": ",$appsrcfiles[$i],"\n\t\$\{",uc $apptoolchain[$i],"_DEP\} \${OPT_FLAGS} \${INCLUDES} \${",uc $apptoolchain[$i],"_DEP_FLAGS} \${",uc $apptoolchain[$i],"_CC_FLAGS} -c ",$appsrcfiles[$i]," > ",$appdepfiles[$i];
  print MAKE " ; tools",$dirsep,"formatdep.pl ",$appdepfiles[$i];
  print MAKE "\n#include the dependency file, if it exists";
  print MAKE "\nsinclude ",$appdepfiles[$i];
  print MAKE "\n#default target for make";
  print MAKE "\n",$appobjfiles[$i],":\n\t\$\{",uc $apptoolchain[$i],"_CC\} \${OPT_FLAGS} \${INCLUDES} \${",uc $apptoolchain[$i],"_CC_FLAGS} -c ",$appsrcfiles[$i]," -o \$@";
}

print MAKE "\n\n";
print MAKE <<EOF;
#########################################################
# Individual file build control for subproject files
#########################################################
EOF

foreach $key (keys %subprojects)
{
  if ($key=~/.*\.prefix/)
  {
    #do nothing
  }
  else
  {
    #for each target, print out the build
    $targets = $subprojects{$key};
    foreach $target (@$targets)
    {
      print MAKE "\n\n$target:";
      print MAKE "\n\tmake -C ",$subprojects{"$key.prefix"};
    }
  }
}


print MAKE "\n\n";
print MAKE <<EOF;
#########################################################
# End of Makefile
#########################################################

EOF

#check all required directories are present
#(deps,obj)
$path = $paths{"LIBDIR"};
#print "\nChecking for directory $basedir${dirsep}$path${dirsep}obj";
if (!opendir(DIR,"$basedir${dirsep}$path${dirsep}obj"))
{
  print "\nCreating directory $basedir${dirsep}$path${dirsep}obj";
  mkdir "$basedir${dirsep}$path${dirsep}obj";
}
else
{
  closedir(DIR);
}
#print "\nChecking for directory $basedir${dirsep}$path${dirsep}deps";
if (!opendir(DIR,"$basedir${dirsep}$path${dirsep}deps"))
{
  print "\nCreating directory $basedir${dirsep}$path${dirsep}deps";
  mkdir "$basedir${dirsep}$path${dirsep}deps";
}
else
{
  closedir(DIR);
}
$path = $paths{"APPDIR"};
#print "\nChecking for directory $basedir${dirsep}$path${dirsep}obj";
if (!opendir(DIR,"$basedir${dirsep}$path${dirsep}obj"))
{
  print "\nCreating directory $basedir${dirsep}$path${dirsep}obj";
  mkdir "$basedir${dirsep}$path${dirsep}obj";
}
else
{
  closedir(DIR);
}
#print "\nChecking for directory $basedir${dirsep}$path${dirsep}deps";
if (!opendir(DIR,"$basedir${dirsep}$path${dirsep}deps"))
{
  print "\nCreating directory $basedir${dirsep}$path${dirsep}deps";
  mkdir "$basedir${dirsep}$path${dirsep}deps";
}
else
{
  closedir(DIR);
}




#########################################################
# BENCHMARK : 
# edit MPEGDecoderLib/libarcavcodec/Makefile for MPEG only
#########################################################

$mpeg_benchmark_build = "no";
$edit_makefile = "yes";
$makefile_name = "MPEGDecoderLib/libarcavcodec/Makefile";
@bench_string_list = ();
foreach $key (keys %selectedoptions)
{
  if ( "$project" eq "mpegdec" and "$key" eq "benchmark" ) { $mpeg_benchmark_build = "yes"; }
}

if ( "$mpeg_benchmark_build" eq "yes" ) { 

  print "\n\nbenchmark build : \n";
  print "checking if '$makefile_name' needs \"-D__BENCHMARK__\" top be added ...";

  # read file
  open(BENCH_FILE,"$makefile_name") or 
                        die "\nERROR : unable to open file '$makefile_name'\n";

  while ( $key = <BENCH_FILE> ) { 
    if ( $key =~ /^\s*CFLAGS\s*=/ and $key =~ /-D__BENCHMARK__/ ) {
      $edit_makefile = " no";
    } 
    push @bench_string_list, $key;
  }
  close(BENCH_FILE);


if ( "$edit_makefile" eq "yes" ) { 

  print " yes it does \n";
  #rename("$makefile_name","${makefile_name}_orig") or 
                          #die "ERROR : unable to copy '$makefile_name'\n";

  #print "saving $makefile_name to ${makefile_name}_orig \n";  
  
  # re-write file
  open(BENCH_FILE,"> $makefile_name") or 
                      die "nERROR : unable to create file '$makefile_name'\n";
  foreach $key (@bench_string_list) { 
    if ( $key =~ /^\s*CFLAGS\s*=/ and $key !~ /-D__BENCHMARK__/ ) {
      $key =~ s#^(\s*CFLAGS\s*)=(.*)$#$1=$2 -D__BENCHMARK__#;
    } 
    print BENCH_FILE "$key";
  }
  close(BENCH_FILE);

  print "flag \"-D__BENCHMARK__\" is added \n";  

} else { 
    print " no it doesn't\n";
} 
}


print "\nDone\n\n";

