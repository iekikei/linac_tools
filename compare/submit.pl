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

if(!-d "./pdf") {
    mkdir "./pdf";
}

if(!-d "./txt") {
    mkdir "./txt";
}

open (INP,"$ENV{LINAC_DIR}/runsum.dat");

@raw_data =<INP>;
close INP;

foreach $list (@raw_data){
  chomp($list);
  ($RunNumber,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$list);

  if ($mod==0){
    $runn = sprintf("%06d",$RunNumber);
    print " leggo $RunNumber $mom $x $y $z \n";

    my $files  = sprintf("./script/%s.csh",$runn);
    print " script filename $files \n";

# input file list
    open (SCRIPT,">$files");
    print SCRIPT "#!/bin/csh -f\n";
    print SCRIPT "source $ENV{LINAC_DIR}/setup.csh\n";
    print SCRIPT "cd $ENV{LINAC_DIR}/compare/\n";
    print SCRIPT "hostname\n";
    print SCRIPT "./compare $RunNumber\n";
    close SCRIPT;
    $cmd = "chmod 755 $files";
    system $cmd;

    $cmd = "qsub -q calib -o out/0$RunNumber -e err/0$RunNumber $files";
    system $cmd;
  }    
}
die "Normal End";

