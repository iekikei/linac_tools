#!/usr/bin/perl 

# Settings
$data_dir = '/disk02/data7/sk5/lin';

# Make directories if they do not exist
if(!-d "./script") {
    mkdir "./script";
}

if(!-d "./out") {
    mkdir "./out";
}

if(!-d "./err") {
    mkdir "./err";
}

if(!-d "./lin") {
    mkdir "./lin";
}

# Open run summary file
open (INP,"$ENV{LINAC_DIR}/runsum.dat");
@runsum =<INP>;
close INP;

# Loop for lines in run summary file
foreach $line (@runsum){

  # Get run information
  chomp($line);
  ($runnum,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$line);

  # Only analyze normal LINAC run
  if ($mod==0) {

    # Make a list of subrun files
    $run0 = sprintf("%06d",$runnum);
    $dir = substr($run0,0,4);
    $input_files = "";
    foreach $filelist (`/bin/ls -1 $data_dir/$dir/$run0/`){
      chomp($filelist);
      $input_files = $input_files." $data_dir/$dir/$run0/".$filelist;
    }

    # Make a script file
    my $script_file  = sprintf("./script/%s.csh",$run0);
    print "Run$runnum $mom $x $y $z $script_file\n";
    open (SCRIPT,">$script_file");
    print SCRIPT "#!/bin/csh -f\n";
    print SCRIPT "cd $ENV{LINAC_DIR}/data\n";
    print SCRIPT "source $ENV{LINAC_DIR}/setup.csh\n";
    print SCRIPT "source $ENV{SKOFL_ROOT}/env.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "./lowfit_sk4_root ./fit/lin.$run0.root $x $y $z $runnum $input_files\n";
    close SCRIPT;
    $cmd = "chmod 755 $script_file";
    system $cmd;

    # Submit a job for this script
    $cmd = "qsub -q lowe -o out/0$runnum -e err/0$runnum $script_file";
    system $cmd;
    
  } 
   
}

die "Normal End";

