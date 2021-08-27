#!/usr/bin/perl 

#$version = "tune";
#$version = "SmallFr";
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

if(!-d "./card") {
    mkdir "./card";
}

if(!-d "/disk02/lowe8/mharada/linac/sk5/skg4/lowfit_$version/") {
    mkdir "/disk02/lowe8/mharada/linac/sk5/skg4/lowfit_$version/";
}


open (INP,"/home/sklowe/linac/const/linac_sk5_runsum.dat");
@raw_data =<INP>;
close INP;

foreach $list (@raw_data){
    chomp($list);
    ($RunNumber,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$list);
    if ($RunNumber>=80000 && $mod==0 && $RunNumber<85000){
      for ($count = 0; $count < 20; $count++){
        my $count2 = sprintf("%03d",$count);
        my $files  = sprintf("./script/mcfit.0%s.%03d.csh",$RunNumber, $count);
        print " script filename $files \n";
        &WriteScriptFile($files,$RunNumber, $count);

        $cmd = "qsub -q lowe -o out/mcfit.0$RunNumber.$count2 -e err/mcfit.0$RunNumber.$count2 $files";
        system $cmd;
      }
    }

}

die "Normal End";

sub WriteScriptFile() {
    my $file = $_[0];
    my $runn = sprintf("%06d",$_[1]);
    my $count = sprintf("%03d",$_[2]);
    open (SCRIPT,">$file");
    print SCRIPT "#!/bin/csh -f\n";
    print SCRIPT "cd /home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/skg4_new/\n";
    print SCRIPT "source /usr/local/sklib_gcc4.8.5/skofl-trunk/env.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "./lowfit_mc $runn /disk02/lowe8/mharada/linac/sk5/skg4/lowfit_$version/wtf_lin.${runn}.$count.root  /disk02/lowe8/mharada/linac/sk5/skg4/out_root_$version/lin.${runn}.${count}.root\n";
    close SCRIPT;
    $cmd = "chmod 755 $file";
    system $cmd;
}
