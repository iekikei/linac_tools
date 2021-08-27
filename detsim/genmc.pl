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
$out_dir = "$ENV{LINAC_DIR}/detsim/out$version";

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

# Template card file
open (CARDEXAMPLE,"linac_sk5_example.card");
@card_data =<CARDEXAMPLE>;
close CARDEXAMPLE;

# Runsum data
open (INP,"$ENV{LINAC_DIR}/runsum.dat");
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
    $darkfile = "'$ENV{LINAC_DIR}/darkr_for_skdetsim/output/darkr.0$badrun.txt'";

    # Loop for sub runs
    my $count;
    for ($count = 0; $count < 20; $count++){

      # Make a card file
      my $file_card  = sprintf("./card/0%s.%03d",$RunNumber,$count);
      print " card filename $file_card \n";
      &WriteCardFile($file_card);
      $seed1++; 
      $seed2++; 	

      # Make a script file
      my $file_script  = sprintf("./script/0%s.%03d.csh",$RunNumber, $count);
      print " script filename $file_script \n";
      &WriteScriptFile($file_script,$file_card,$RunNumber, $count);

      # Submit job
      my $scount = sprintf("%03d",$count);
      $cmd = "qsub -q lowe -o out/0$RunNumber.$scount -e err/0$RunNumber.$scount $file_script";
      print cmd;
      #system $cmd;

    }
  }
}
die "Normal End";

sub WriteCardFile() {
  my $file = $_[0];
  open (CARD,">$file");
  foreach $cardlist (@card_data){
    chomp($cardlist);
    if($cardlist =~ /VECT-RAND/){
      $cardlist = "VECT-RAND $seed1 $seed2 0 0 0";
    }
    if($cardlist =~ /DS-TBARUN/){
      $cardlist = "DS-TBARUN $RunNumber";
    }
    if($cardlist =~ /DS-WATRUN/){
      $cardlist = "DS-WATRUN $RunNumber";
    }
    if($cardlist =~ /DS-GAINRUN/){
      $cardlist = "DS-GAINRUN $RunNumber";
    }
    if($cardlist =~ /DS-PMTNOISEFILE/){
      $cardlist = "DS-PMTNOISEFILE $darkfile";
    }
    print CARD "$cardlist\n";
  }
  close CARD;
}

sub WriteScriptFile() {
  my $file = $_[0];
  my $card = $_[1];
  my $runn = $_[2];
  my $count = sprintf("%03d",$_[3]);

  open (SCRIPT,">$file");
  print SCRIPT "#!/bin/csh -f\n";
  print SCRIPT "cd $ENV{LINAC_DIR}/detsim/trunk\n";
  print SCRIPT "source $skofl_env\n";
  print SCRIPT "hostname\n";
  print SCRIPT "./skdetsim $card $out_dir/lin.0$runn.$count.root $runn \n";

  close SCRIPT;
  $cmd = "chmod 755 $file";
  system $cmd;
}
