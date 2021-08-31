#!/bin/bash
ALL_INPUT_FILES=`ls /home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/*.root`
#ALL_INPUT_FILES="/home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.061505.root"
#ALL_INPUT_FILES="/home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.074720.root"
for INPUT_FILE in $ALL_INPUT_FILES; do
    OUTPUT_FILE=`echo $INPUT_FILE | sed -e "s/.*darkr\.\(.*\)\.root/output\/darkr\.\1\.txt/"`
    RUNNO=`echo $INPUT_FILE | sed -e "s/.*darkr\.0\(.*\)\.root/\1/"`
    if [ $RUNNO -eq 81894 ] ; then
       echo $INPUT_FILE $OUTPUT_FILE $RUNNO
       root -l -b -q "read_dark_rate.C+(\"$INPUT_FILE\",\"$OUTPUT_FILE\",$RUNNO)";
    fi
done
       
