#!/usr/bin/perl 

# Settings
$run_min = 86119;
$run_max = 86161;
$data_dir = '/disk02/data7/sk5/lin';
$analysis_dir = "$ENV{RUNSUM_DIR}/data";
$skofl_env = '/usr/local/sklib_gcc4.8.5/skofl-trunk/env.csh';

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
open (INP,"$ENV{RUNSUM_DIR}/runsum.dat");
@runsum =<INP>;
close INP;

# Loop for lines in run summary file
foreach $line (@runsum){

  # Get run information
  chomp($line);
  ($runnum,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$line);

  # Only analyze normal LINAC run
  if ($mod==0 && $runnum>=$run_min && $runnum<=$run_max){

    # Make a list of subrun files
    $runnum2 = sprintf("%06d",$runnum);
    $dir = substr($runnum2,0,4);
    $input_files = "";
    foreach $filelist (`/bin/ls -1 $data_dir/$dir/$runnum2/`){
      chomp($filelist);
      $input_files = $input_files." $data_dir/$dir/$runnum2/".$filelist;
    }

    # Make a script file
    my $script_file  = sprintf("./script/%s.csh",$runnum2);
    print "Run$runnum $mom $x $y $z $script_file\n";
    open (SCRIPT,">$script_file");
    print SCRIPT "#!/bin/csh -f\n";
    print SCRIPT "cd $analysis_dir\n";
    print SCRIPT "source $skofl_env\n";
    print SCRIPT "source $ENV{RUNSUM_DIR}/setup.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "./lowfit_sk4_root ./lin/lin.$runnum2.root $x $y $z $runnum $input_files\n";
    close SCRIPT;
    $cmd = "chmod 755 $script_file";
    system $cmd;

    # Submit a job for this script
    $cmd = "qsub -q lowe -o out/0$runnum -e err/0$runnum $script_file";
    system $cmd;
    
  } 
   
}

die "Normal End";

