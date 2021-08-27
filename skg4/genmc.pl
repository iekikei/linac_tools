#!/usr/bin/perl 

if(!-d "./card") {
    mkdir "./card";
}

#$version = "SmallFr";
#$version = "tune";
#$version = "noLat";
#$version = "woMott";
$version = "woMott_wLat";
#$version = "woMott_woPWA";
#$version = "woMott_livIoni";

if(!-d "./script") {
    mkdir "./script";
}

if(!-d "./out") {
    mkdir "./out";
}

if(!-d "./err") {
    mkdir "./err";
}

if(!-d "/disk02/lowe8/mharada/linac/sk5/skg4/out_root_$version/") {
    mkdir "/disk02/lowe8/mharada/linac/sk5/skg4/out_root_$version/";
}


open (CARDEXAMPLE,"Calib_LINAC.mac");
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

      my $count;

      for ($count = 0; $count < 20; $count++){
        my $filef  = sprintf("./card/0%s.%03d",$RunNumber, $count);
        #print " card filename $filef \n";
        &WriteCardFile($filef, $count);
        my $files  = sprintf("./script/g4_0%s.%03d.csh",$RunNumber, $count);
        #print " script filename $files \n";
        &WriteScriptFile($files,$filef,$seed1);

        $cmd = "qsub -q lowe -o out/0$RunNumber.$count -e err/0$RunNumber.$count $files";
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
    if($cardlist =~ /\/SKG4\/Detector\/Material\/IDTBAParameter/){
      $cardlist = "/SKG4/Detector/Material/IDTBAParameter $RunNumber";
    }
    if($cardlist =~ /\/SKG4\/Detector\/Material\/WaterTransparencyRun/){
      $cardlist = "/SKG4/Detector/Material/WaterTransparencyRun $RunNumber";
    }
    if($cardlist =~ /\/SKG4\/Global\/LowEGainRun/){
      $cardlist = "/SKG4/Global/LowEGainRun $badrun";
    }
    if($cardlist =~ /\/SKG4\/DarkNoise\/Run/){
      $cardlist = "/SKG4/DarkNoise/Run $badrun";
    }
    if($cardlist =~ /\/SKG4\/Calibration\/LINAC\/SetRun/){
      $cardlist = "/SKG4/Calibration/LINAC/SetRun $RunNumber";
    }
    if($cardlist =~ /\/SKG4\/RootFile\/Name/){
      $cardlist = "/SKG4/RootFile/Name /disk02/lowe8/mharada/linac/sk5/skg4/out_root_$version/lin.0${RunNumber}.$count.root";
    }
    print CARD "$cardlist\n";
  }
  close CARD;
}

sub WriteScriptFile() {
    my $file = $_[0];
    my $card = $_[1];
    my $rndm = $_[2];
    open (SCRIPT,">$file");
    print SCRIPT "#!/bin/csh -f\n";
    print SCRIPT "cd /home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/skg4_new/trunk/\n";
    print SCRIPT "source /home/sklowe/skofl/r29166/env.csh\n";
    print SCRIPT "source /home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/skg4_new/trunk/G4ROOTsource.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "/home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/skg4_new/trunk/bin/Linux-g++/SKG4 /home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/skg4_new/$card $rndm\n";

    close SCRIPT;
    $cmd = "chmod 755 $file";
    system $cmd;
}
