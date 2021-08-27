cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	subroutine skwt
c
c	(purpos)fill the information of water transparancy and the day
c		 to the common defined "skwt.h" 
c
c	(data and author)	97/3/17    H.Ishino
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

	implicit none
	
#include "skwt.h"

	integer i
	real d,w
	character*200 fname
	integer index(wtday_num)

c	fname = WaterTable
c       fname = '/home/ynakano/sk5/beginnig/20190912/watert/wt_sk5_new.txt'
c	fname = '/home/sklowe/realtime_sk5_rep/solar_oct19/watert/wt_sk5_new.txt'
c	fname = '/home/sklowe/realtime_sk5_rep/solar_may20/watert/wt_sk5_new.txt'
	fname = '/home/sklowe/realtime_sk5_rep/solar_nov20/watert/wt.sk56.table'

#ifndef ONE_WRITE
#define ONE_WRITE
	write (*,*) 'This is skwt.F for SK-V'
	write (*,*) 'Water Transparancy table used is: ',fname
#endif

	open(10, file = fname, form = 'formatted', status = 'old')

	i = 0
 2	read(10,*,end=22)d,w
	i = i+1
 	wtday(i) = int(d)
	wtvalue(i) = w
	goto 2
 22	continue
	wtday_exit_num = i
	call sortzv(wtvalue,index,wtday_exit_num,1,1,0)
	wt_value_max = wtvalue(index(1))
	wt_value_min = wtvalue(index(wtday_exit_num))

	print *, 'wtday_exit_num = ', wtday_exit_num
c	print *, 'lfwatert = ', 


	close(10)
	return
	end
