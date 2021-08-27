#!/usr/bin/perl 

if(!-d "./card") {
    mkdir "./card";
}

$version = "tune";

if(!-d "./script") {
    mkdir "./script";
}

if(!-d "./out") {
    mkdir "./out";
}

if(!-d "./err") {
    mkdir "./err";
}

if(!-d "/disk02/lowe8/mharada/linac/sk5/detsim/out_root_$version/") {
    mkdir "/disk02/lowe8/mharada/linac/sk5/detsim/out_root_$version/";
}

open (CARDEXAMPLE,"linac_sk5_example.card");
@card_data =<CARDEXAMPLE>;
close CARDEXAMPLE;

open (INP,"/home/sklowe/linac/const/linac_sk5_runsum.dat");
@raw_data =<INP>;
close INP;

$seed1 = 45097;
$seed2 = 21263;

foreach $list (@raw_data){
  chomp($list);
  ($RunNumber,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$list);

  if ($RunNumber>=80000 && $mod==0 && $RunNumber<85000){
    print " leggo $RunNumber $mom $x $y $z \n";
    $darkfile = "'/home/mharada/Lowe/darkr_for_skdetsim/output/darkr.0$badrun.txt'";

    my $count;
    for ($count = 0; $count < 20; $count++){
      my $scount = sprintf("%03d",$count);
      my $filef  = sprintf("./card/0%s.%03d",$RunNumber,$count);
      print " card filename $filef \n";
      &WriteCardFile($filef);
      my $files  = sprintf("./script/0%s.%03d.csh",$RunNumber, $count);
      print " script filename $files \n";
      &WriteScriptFile($files,$filef,$RunNumber, $count);

      $cmd = "qsub -q lowe -o out/0$RunNumber.$scount -e err/0$RunNumber.$scount $files";
      system $cmd;

      $seed1++; 
      $seed2++; 	
    }
  }
}
die "Normal End";

sub WriteCardFile() {
  my $file = $_[0];
  my $count = sprintf("%03d",$_[1]);
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
  print SCRIPT "cd /home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/detsim_new/\n";
  print SCRIPT "source /home/sklowe/skofl/r29166/env.csh\n";
  print SCRIPT "hostname\n";
  print SCRIPT "/home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/detsim_new/trunk/skdetsim $card /disk02/lowe8/mharada/linac/sk5/detsim/out_root_$version/lin.0$runn.$count.root $runn \n";

  close SCRIPT;
  $cmd = "chmod 755 $file";
  system $cmd;
}
