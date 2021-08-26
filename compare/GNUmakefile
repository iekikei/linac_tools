#
SKOFL_ROOT = /usr/local/sklib_gcc4.8.5/skofl-trunk

include $(SKOFL_ROOT)/config.gmk
LOCAL_INC	= 
LOCAL_LIBS	= -lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska 

OBJS = linac

all: $(OBJS) 

clean: 
	$(RM) *.o *~ core fort.* *.root  *.hbk $(OBJS)
