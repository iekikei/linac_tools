#!/bin/bash
#ALL_INPUT_FILES=`ls /home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.081894.root`
ALL_INPUT_FILES=`ls /home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.0861*.root`
for INPUT_FILE in $ALL_INPUT_FILES; do
    OUTPUT_FILE=`echo $INPUT_FILE | sed -e "s/.*darkr\.\(.*\)\.root/output\/darkr\.\1\.txt/"`
    RUNNO=`echo $INPUT_FILE | sed -e "s/.*darkr\.0\(.*\)\.root/\1/"`
    if [ $RUNNO -gt 86100 ] ; then
	if [ $RUNNO -le 86170 ] ; then
	    echo $INPUT_FILE $OUTPUT_FILE $RUNNO
	    root -l -b -q "SK5_read_dark_rate.cc+(\"$INPUT_FILE\",\"$OUTPUT_FILE\",$RUNNO)";
	fi
    fi
done
       
