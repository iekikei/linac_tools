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

if(!-d "./card") {
    mkdir "./card";
}

if(!-d "./fit_skg4") {
    mkdir "./fit_skg4";
}

open (INP,"$ENV{LINAC_DIR}/runsum.dat");
@runsum =<INP>;
close INP;

foreach $line (@runsum){
    chomp($line);
    ($RunNumber,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$line);
    if ($mod==0){
      for ($count = 0; $count < 20; $count++){
        my $count2 = sprintf("%03d",$count);
        my $script_file  = sprintf("./script/skg4.0%s.%03d.csh",$RunNumber, $count);
        print " script filename $script_file \n";
        &WriteScriptFile($script_file, $RunNumber, $count);

        $cmd = "qsub -q lowe -o out/g4.0$RunNumber.$count2 -e err/g4.0$RunNumber.$count2 $script_file";
	    #print "$cmd\n";
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
    print SCRIPT "source $ENV{LINAC_DIR}/setup.csh\n";
    print SCRIPT "cd $ENV{LINAC_DIR}/lowfit\n";
    print SCRIPT "source $ENV{SKOFL_ROOT}/env.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "./lowfit_mc $runn ./fit_skg4/lin.${runn}.$count.root ../skg4/data/lin.${runn}.${count}.root\n";
    close SCRIPT;
    $cmd = "chmod 755 $file";
    system $cmd;
}
