#
# In case of building locally, set SKOFL_ROOT variable 
#      setenv SKOFL_ROOT   ... 
#  or directly set in this makefile 
# SKOFL_ROOT = /skofl
#

#ifndef SKOFL_ROOT
SKOFL_ROOT = /usr/local/sklib_gcc4.8.5/skofl-trunk
#SKOFL_ROOT =/home/sklowe/skofl/r29166
#endif

include $(SKOFL_ROOT)/config.gmk

LOCAL_INC	= 
LOCAL_LIBS	= skwt.o lfneweff_sk4.o lfallfit_sk4_mc.o -lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska

#
#  Objects
#

OBJS   = skwt.o lfallfit_sk4_mc.o lfneweff_sk4.o lowfit_mc 

all: $(OBJS) 

clean: 
	$(RM) *.o *~ core fort.* *.log $(OBJS) output.dat*
