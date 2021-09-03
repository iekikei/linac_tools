#
# In case of building locally, set SKOFL_ROOT variable 
#      setenv SKOFL_ROOT   ... 
#  or directly set in this makefile 
# SKOFL_ROOT = /skofl
#

include $(SKOFL_ROOT)/config.gmk

LOCAL_INC	= 
LOCAL_LIBS	= skwt.o lfneweff_sk4.o lfneweff_sk4_final_qe43.o lfneweff_sk4_final_qe41.o lfallfit_sk4_mc.o lfallfit_sk4_final_qe43.o lfallfit_sk4_final_qe41.o lftba_gain_corr.o -lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska

#
#  Objects
#

OBJS   = skwt.o lfallfit_sk4_mc.o lfallfit_sk4_final_qe43.o lfallfit_sk4_final_qe41.o lfneweff_sk4.o lfneweff_sk4_final_qe43.o lfneweff_sk4_final_qe41.o lftba_gain_corr.o lowfit_mc lowfit_sk4_root

all: $(OBJS) 

clean: 
	$(RM) *.o *~ core fort.* *.log $(OBJS) output.dat*
