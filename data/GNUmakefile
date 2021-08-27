#
#
#ifndef SKOFL_ROOT
#SKOFL_ROOT =/home/sklowe/skofl/r29166
#SKOFL_ROOT =/home/sklowe/skofl/r29166
#endif
SKOFL_ROOT = /usr/local/sklib_gcc4.8.5/skofl-trunk

include $(SKOFL_ROOT)/config.gmk

LOCAL_INC	= 
LOCAL_LIBS      = skwt.o  lfneweff_sk4.o lfallfit_sk4_final_qe43.o lftba_gain_corr.o -lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska $(TEST)

OBJS   = skwt.o lfneweff_sk4.o lfallfit_sk4_final_qe43.o lftba_gain_corr.o lowfit_sk4_root 

all: $(OBJS) 

clean: 
	$(RM) *.o *~ core fort.* *.root *.log  $(OBJS)
