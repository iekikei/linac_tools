#!/usr/bin/perl 

if(!-d "./script") {
    mkdir "./script";
}

if(!-d "./out") {
    mkdir "./out";
}

if(!-d "./err") {
    mkdir "./err";
}

open (INP,"/home/sklowe/linac/const/linac_sk5_runsum.dat");

@raw_data =<INP>;
close INP;

foreach $list (@raw_data){
  chomp($list);
  ($RunNumber,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$list);

  if ($RunNumber>=81500 && $mod==0 && $RunNumber < 81850){
    $runn = sprintf("%06d",$RunNumber);
    print " leggo $RunNumber $mom $x $y $z \n";

    my $files  = sprintf("./script/%s.csh",$runn);
    print " script filename $files \n";

# input file list
    open (SCRIPT,">$files");
    print SCRIPT "#!/bin/csh -f\n";
    print SCRIPT "cd /home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/compare/\n";
    print SCRIPT "source /home/sklowe/skofl/r29166/env.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "./linac $RunNumber\n";
    close SCRIPT;
    $cmd = "chmod 755 $files";
    system $cmd;

    sleep(1);	    
    $cmd = "qsub -q calib -o out/0$RunNumber -e err/0$RunNumber $files";
    system $cmd;
  }    
}
die "Normal End";

