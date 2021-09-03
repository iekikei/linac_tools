#!/usr/bin/perl 

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

if(!-d "./fit_detsim") {
    mkdir "./fit_detsim";
}

# Open run summary file
open (INP,"$ENV{LINAC_DIR}/runsum.dat");
@runsum =<INP>;
close INP;

# Loop for lines in run summary file
foreach $line (@runsum){

    chomp($line);
    ($runnum,$mom,$mod,$x,$y,$z,$badrun)=split(/ /,$line);

    # Only analyze normal LINAC run
    if ($mod==0){

      # Loop for sub-runs
      for ($count = 0; $count < 20; $count++){

	# Make a script file
        my $count2 = sprintf("%03d",$count);
        my $script_file  = sprintf("./script/detsim.0%s.%03d.csh",$runnum, $count);
        &WriteScriptFile($script_file, $runnum, $count);

	# Submit a job
        $cmd = "qsub -q all -o out/0$runnum.$count2 -e err/0$runnum.$count2 $script_file";
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
    print SCRIPT "cd $ENV{LINAC_DIR}/lowfit\n";
    print SCRIPT "source $ENV{SKOFL_ROOT}/env.csh\n";
    print SCRIPT "source $ENV{LINAC_DIR}/setup.csh\n";
    print SCRIPT "hostname\n";
    print SCRIPT "./lowfit_mc $runn ./fit_detsim/lin.${runn}.$count.root ../detsim/data/lin.${runn}.${count}.root\n";
    close SCRIPT;
    $cmd = "chmod 755 $file";
    system $cmd;
}
