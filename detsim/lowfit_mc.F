c 
c     lowfit.F         02-NOV-2006     Y.Takeuchi
c
c     for SK-III (re)process1
c
c   input:  (arg1)  run number
c
c 
c
      program lowfit

      implicit none

#include "skhead.h"
#include "skparm.h"
#include "sktq.h"
#include "skroot_lowe.h"
#include "geopmt.h" 
#include "skday.h"
#include "skwt.h"
#include "skbadc.h"
#include "lininfo2.h"

      integer lun
      parameter (lun=10)

      integer nrun, iret, ipbank, rlp,lfflag,iday
      real watert, rver
      character*200 fout,fname1,fname2
      character*200 tmpchar

c*** number of hit limit for clusfit
      integer i, narg
      integer NHITCUT
      parameter (NHITCUT = 400) 

      integer lecompte,n,nnrun
      real px, py, pz,width

      integer log_level,iskip
cccccccccccccccccccccccccccccccccccccccccccccccccc
c*** set SK_GEOMETRY
      SK_GEOMETRY = 5

      narg = iargc()
      print *, narg
      call getarg(1,tmpchar)
      read(tmpchar,*)nrun
      call getarg(2,tmpchar)
      fname2 = tmpchar
      call skroot_open(lun,fname2(1:lnblnk(fname2)))

      do i = 3, narg
         call getarg(i, tmpchar) 
         call skroot_set_input_file(lun,tmpchar(1:lnblnk(tmpchar)))
        print *, "input file  : ", tmpchar
      enddo

c*** init.
      call kzinit
      call geoset
      call skoptn('31,30,25')
      call skbadopt(55)

c*** Set bad channel
      call skrunday
      call skwt
c      call skwt_gain_corr
      call lfwater(relapse(nrun),watert)
     
      print *,'watert,nrun=',watert,nrun

c*** check lininfo
      call linsum2(nrun)
      print *, 'lininfo2:', lin_mode(nrun),lin_trg(nrun),
     $     lin_x(nrun),lin_y(nrun),lin_z(nrun),
     $     lin_badrun(nrun), lin_runnum, lin_mom(nrun),
     $     lin_dark, darkfac
      px = lin_x(nrun)
      py = lin_y(nrun)
      pz = lin_z(nrun)

      call skbadch(lin_badrun(nrun), 1, iret)
               
      call lfwater(relapse(lin_badrun(nrun)), watert)
      print *, 'nrunsk, lin_badrun, watert = ',
     $     nrun, lin_badrun(nrun), watert
      
c*** init. bonsai 
      CALL cfbsinit(MAXPM,xyzpm)

c*** open skroot file
c      fname1 = '/disk2/usr5/hmakoto/linac/mc2012_wtvar_yfree_oldtba_ver2016/lin.'//tmpchar(1:6)//'.root'
c      fname2='/disk2/usr5/hmakoto/linac/mc2012_wtvar_yfree_oldtba_ver2016/wtf_lin.'//tmpchar(1:6)//'.root'
c      fname1 = '/disk02/lowe8/mharada/linac/sk5/skg4/out_root_tune/lin.'//tmpchar(1:6)//'.root'
c      fname2 = '/disk02/lowe8/mharada/linac/sk5/skg4/lowfit_tune/wtf_lin.'//tmpchar(1:6)//'.root'
      
c      call skroot_open(lun,fname2(1:lnblnk(fname2)))
c      call skroot_set_input_file(lun,fname1(1:lnblnk(fname1)))
      call skroot_init(lun)

c*** main loop 
 10   CONTINUE

c*** read an event
      CALL SKREAD(lun,*1002,*1001,*10,*10)

      nrunsk = nrun
      if(mod(nevsk,1000).eq.0) write(6,*) nrunsk,nevsk,nqisk
      call lfclear_all()
c      call lfallfit_sk4_mc(watert,NHITCUT,lfflag)
      call lfallfit_sk4_mc(watert,NHITCUT,iskip,log_level,lfflag) 
c      call lfallfit_sk4_gain_corr_mc(watert,NHITCUT,iskip,log_level,lfflag) 
c      if(mod(nevsk,100).eq.0) write(6,*) nrunsk,nevsk,nqisk,bsvertex(1),bsenergy,bsn50
c      if(nevsk.eq.100)  goto 9999

c      write(6,*) nrunsk,nevsk,nqisk,bsvertex(1),bsenergy,bsn50
      n=lecompte(px,py,pz,width)
      if(n .ne. 1)then
         goto 10
      end if

      rver = sqrt((px - bsvertex(1))**2 + (py - bsvertex(2))**2+ (pz - bsvertex(3))**2)
      if(rver.gt.300)then
         print *,'rver is big',rver,nevsk,nqisk, nevsk
         goto 10
      end if

c*** store lowe info.
      call skroot_set_lowe(lun,bsvertex, bsresult, bsdir, 
     $     bsgood, bsdirks, bseffhit, bsenergy, bsn50, bscossun,
     $     clvertex, clresult, cldir, clgoodness,     
     $     cldirks, cleffhit, clenergy, cln50, clcossun,
     $     latmnum, latmh, lmx24, ltimediff, lnsratio, lsdir, 
     $     spaevnum, spaloglike, sparesq, spadt, spadll, spadlt,
     $     spamuyn, spamugdn,
     $     posmc, dirmc, pabsmc, energymc, darkmc, islekeep,
     $     bspatlik, clpatlik, lwatert, lninfo, linfo)

      call skroot_set_tree(lun)
      call skroot_fill_tree(lun)

      goto 10
         
 1003 continue
      goto 10
 1002 continue
      write(0,*)'read error'
      goto 10
         
 1001 continue
         
c***  close files
      call skclosef(lun)
      call skroot_close(lun)
      call skroot_end

 9999 continue

c*** for bonsai 
      call cfbsexit

      stop
      end

c#include "lfallfit_sk4_gain_corr_mc.F"
#include "lecompte.F"
#include "linsum2.F"
