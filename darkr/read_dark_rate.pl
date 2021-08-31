#!/usr/bin/perl 

# Open run summary file
open (INP,"$ENV{LINAC_DIR}/runsum.dat");
@runsum =<INP>;
close INP;

my @run_list = ();

# Loop for lines in run summary file
foreach $line (@runsum){

    # Get run information
    chomp($line);
    ($runnum,$mom,$mod,$x,$y,$z,$normrun)=split(/ /,$line);

    # Only select normal LINAC run
    if ($mod==0) {
	# Add to list if it is not added yet
	if (!(grep {$_ eq $normrun} @run_list)) {
	    push(@run_list, $normrun);
	}
    }

}

# Loop for runs in the list
foreach $run_this (@run_list) {
    $f_input = "/home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.0$run_this.root";
    $f_output = "output2/darkr.0$run_this.txt";
    $cmd = "root -l -b -q 'SK5_read_dark_rate.cc+(\"$f_input\", \"$f_output\", $run_this)'";
    print "$cmd\n";
    system $cmd;
}


      

