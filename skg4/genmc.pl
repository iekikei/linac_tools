#!/usr/bin/perl 

# Settings
$run_min = 86119;
$run_max = 86161;
$data_dir = '/disk02/data7/sk5/lin';
$analysis_dir = "$ENV{LINAC_DIR}/data";
$skofl_env = '/usr/local/sklib_gcc4.8.5/skofl-trunk/env.csh';
$seed1 = 45097;
$seed2 = 21263;
$version = "";
$out_dir = "$ENV{LINAC_DIR}/skg4/lin$version";

# Make output directories if they do not exist
if(!-d "./card") {
    mkdir "./card";
}

if(!-d "./script") {
    mkdir "./script";
}

if(!-d "./out") {
    mkdir "./out";
}

if(!-d "./err") {
    mkdir "./err";
}

if(!-d $out_dir) {
    mkdir $out_dir;
}

# Template macro file
open(CARDEXAMPLE, "Calib_LINAC.mac");
@card_data =<CARDEXAMPLE>;
close CARDEXAMPLE;

# Runsum data
open(INP,"$ENV{LINAC_DIR}/runsum.dat");
@runsum =<INP>;
close INP;

# Loop for lines in run summary data
foreach $line (@runsum){

    # Get settings for this run
    chomp($line);
    ($RunNumber,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$line);

    # Select runs of interest
    if ($mod==0 && $RunNumber>=$run_min && $RunNumber<=$run_max){
	
      print "$RunNumber $mom $x $y $z \n";

      # Loop for sub runs
      my $count;
      for ($count = 0; $count < 20; $count++){
	  
        # Make a card file
        my $file_macro  = sprintf("./card/0%s.%03d",$RunNumber, $count);
        #print " card filename $file_macro \n";
        &WriteCardFile($file_macro, $count);
        $seed1++; 
        $seed2++;

	# Make a script file
        my $file_script  = sprintf("./script/g4_0%s.%03d.csh",$RunNumber, $count);
        #print " script filename $file_script \n";
        &WriteScriptFile($file_script,$file_macro,$seed1);

	# Submit job
        $cmd = "qsub -q lowe -o out/0$RunNumber.$count -e err/0$RunNumber.$count $file_script";
        system $cmd;

      }
    }
}
die "Normal End";

sub WriteCardFile() {

  # Output file
  my $file = $_[0];
  open (CARD,">$file");

  # Sub-run count
  my $count = sprintf("%03d",$_[1]);

  # Loop for lines in the macro file
  foreach $cardlist (@card_data){

    # Copy one line
    $cardlist2 = $cardlist;
    chomp($cardlist2);

    # Skip comment line
    if ($cardlist2 =~ /^#/) {
	print CARD "$cardlist\n";
	next;
    }

    # Change specific lines
    if($cardlist2 =~ /\/SKG4\/Detector\/Material\/IDTBAParameter RUNNUM/){
      $cardlist2 = "/SKG4/Detector/Material/IDTBAParameter $RunNumber";
    }
    if($cardlist2 =~ /\/SKG4\/Detector\/Material\/WaterTransparencyRun RUNNUM/){
      $cardlist2 = "/SKG4/Detector/Material/WaterTransparencyRun $RunNumber";
    }
    if($cardlist2 =~ /\/SKG4\/Global\/LowEGainRun RUNNUM/){
      $cardlist2 = "/SKG4/Global/LowEGainRun $badrun";
    }   
    if($cardlist2 =~ /\/SKG4\/DarkNoise\/Run RUNNUM/){
      $cardlist2 = "/SKG4/DarkNoise/Run $badrun";
    }
    if($cardlist2 =~ /\/SKG4\/Calibration\/LINAC\/SetRun RUNNUM/){
      $cardlist2 = "/SKG4/Calibration/LINAC/SetRun $RunNumber";
    }
    if($cardlist2 =~ /\/SKG4\/RootFile\/Name OUTFILE/){
      $cardlist2 = "/SKG4/RootFile/Name $out_dir/lin.0${RunNumber}.$count.root";
    }
    print CARD "$cardlist2\n";
    
  }
  close CARD;
}

sub WriteScriptFile() {

    # Get arguments
    my $file = $_[0];
    my $card = $_[1];
    my $rndm = $_[2];

    # Generate script file
    open (SCRIPT,">$file");
    print SCRIPT "#!/bin/csh -f\n";
    print SCRIPT "source $ENV{LINAC_DIR}/setup.csh\n";
    print SCRIPT "cd $ENV{LINAC_DIR}/skg4/trunk/\n";
    print SCRIPT "source $skofl_env\n";
    print SCRIPT "source $ENV{LINAC_DIR}/skg4/trunk/G4ROOTsource.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "$ENV{LINAC_DIR}/skg4/trunk/bin/Linux-g++/SKG4 $ENV{LINAC_DIR}/skg4/$card $rndm\n";

    # Close script file
    close SCRIPT;
    $cmd = "chmod 755 $file";
    system $cmd;
}
