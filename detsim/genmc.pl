#!/usr/bin/perl 

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

if(!-d "./data") {
    mkdir "./data";
}

# Template card file
open (CARDEXAMPLE,"linac_sk5_example.card");
@card_data =<CARDEXAMPLE>;
close CARDEXAMPLE;

# Runsum data
open (INP,"$ENV{LINAC_DIR}/runsum.dat");
@runsum =<INP>;
close INP;

# Random seed settings
$seed1 = 45097;
$seed2 = 21263;

# Loop for lines in run summary data
foreach $line (@runsum){

  # Get settings for this run
  chomp($line);
  ($RunNumber,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$line);

  # Select runs of interest
  if ($mod==0) {

    print "$RunNumber $mom $x $y $z \n";
    $darkfile = "'../darkr/output/darkr.0$badrun.txt'";
    #$darkfile = "'$ENV{LINAC_DIR}/darkr/output/darkr.0$badrun.txt'";

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
      #print "$cmd\n";
      system $cmd;

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
  print SCRIPT "source $ENV{LINAC_DIR}/setup.csh\n";
  print SCRIPT "cd $ENV{LINAC_DIR}/detsim\n";
  print SCRIPT "source $ENV{SKOFL_ROOT}/env.csh\n";
  print SCRIPT "hostname\n";
  print SCRIPT "./trunk/skdetsim $card ./data/lin.0$runn.$count.root $runn \n";

  close SCRIPT;
  $cmd = "chmod 755 $file";
  system $cmd;
}
