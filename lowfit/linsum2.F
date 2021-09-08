cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine linsum2(run_num)
c
c     (input) run_num
c
c     (output) none
c
c     (purpos) fill the run information to lininfo.h
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      implicit none

#include "lininfo2.h"

      character*100 linac_dir,runsum
      character*100 fname,fname_ge
      integer run,mode,trg
      real x,y,z,momentum
      integer badrun,run_num
      integer i

      call get_environment_variable('LINAC_DIR', linac_dir)
      runsum = "/runsum.dat"
      fname = trim(linac_dir) // trim(runsum)
      runsum = "/ge_runsum.dat"
      fname_ge = trim(linac_dir) // trim(runsum)

      open(10,file=fname,status = 'old',form='formatted')
      open(11,file=fname_ge,status = 'old',form='formatted')

      do i=1,lin_num
         lin_mode(i) = -1
         lin_trg(i) = -1
         lin_x(i) = -20000.
         lin_y(i) = -20000.
         lin_z(i) = -20000.
         lin_badrun(i) = -1
         lin_mom(i) = -1.
      end do

      i = 0
 2    read(10,*,end=200)run,mode,trg,x,y,z,badrun
      i = i+ 1
      if (run.gt.lin_num) then
         Print *, 'linsum2.F: array size overflow!  Stop SKDETSIM'
         stop
      end if
      lin_mode(run) = mode
      lin_trg(run) = trg
      lin_x(run) = x
      lin_y(run) = y
      lin_z(run) = z
      lin_badrun(run) = badrun
      goto 2
 200  continue
      lin_runnum = i

      i = 0
 3    read(11,*,end=300)run,momentum
      i = i+ 1
      if (run.gt.lin_num) then
         Print *, 'linsum2.F: array size overflow!  Stop SKDETSIM'
         stop
      end if
      lin_mom(run) = momentum
      goto 3
 300  continue
      close(10)
      close(11)

      return
      end

