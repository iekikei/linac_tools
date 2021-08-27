#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TMath.h"

const Int_t nPMT = 11146;
const Float_t DeadTime = 900.e-9;
Double_t prod_year[nPMT];
Int_t isk235[nPMT];
Double_t gain[5];

Double_t gain_nom[5] = {0.99161, 0.98805, 0.99163, 0.99177, 0.99919};// SK5(ave. 80539-80690)
//Double_t gain_nom[5] = {0.99835, 0.99946, 0.98522, 0.99014, 0.99704};// SK5(080539)
//Double_t gain_nom[5] = {0.992057, 1.00184, 0.989083, 0.98216, 0.987678};// Originally SK-IV?

/* should change to SK5 prod_year file  */
//TString pmtprod_filename = "/home/skofl/sklib_gcc4.8.5/skofl_16c/const/pmt_prod_year.dat";
TString pmtprod_filename = "/home/skofl/sklib_gcc4.8.5/skofl_20a/const/pmt_prod_year.dat";

TString gain_filename="/home/skofl/sklib_gcc4.8.5/skofl_20a/const/water.ave10.2";
//TString gain_filename="/disk01/usr5/wanly/lowe/energyrecon/const/water.ave10.2";
//TString gain_filename="/home/ocon/waterjob/src/rep10.5newgain/water.ave10newgain";

/* should change to SK5 pmtinf file  */
//TString pmtinf_filename="/home/skofl/sklib_gcc4.8.5/skofl_16c/const/pmtinf.dat";
//TString pmtinf_filename="/home/skofl/sklib_gcc4.8.5/skofl_20a_debug/const/pmtinf.dat";
TString pmtinf_filename="sk5_pmtinf.dat";/* Original one is /home/koshio/sk5/PMTinfo/pmtinf.dat */

Double_t th_paramsk23[2] = {1.74909,1.56925};/* used only until SK-IV */
Double_t spe_corr_sk235[3] = {0.9997534, 0.9995633, 0.9995633}; /* HKPMT parameter is added (currently same as nominal SK3 PMT) */

/* 1p.e. pdf */
/* sk5 PMT is added */
Double_t spe_pdf_sk235[3][5000]; 

/* HK PMT is added */
TString spe_filename_sk235[3] = {"spe/spe_func_sk2.txt", "spe/spe_func_sk3.txt", "spe/spe_func_hk.txt"};

/* for SK-V, parameters thr_slope and thr_shift should be added */
Double_t th_slopesk23[3] = {3.915, 3.307, 1};
Double_t th_shiftsk23[3] = {0.4771, 0.5657, 1};


// function to apply QBEE threshold
//ref. : dsthr_sk3.f
Double_t dsthr(Double_t x, Double_t a){ //x: pe, a: thr_param
  // function to apply QBEE threshold
   Double_t pc2pe = 2.243;
  Double_t pc = x*pc2pe;
  return 0.5 * ( TMath::Erf(17.09*(a*pc-0.649))+1 );
}
    
/* for SK-V, new function should be added */
//ref. : dsthr_sk5.f
Double_t dsthr_sk5(Double_t x, Double_t a, Double_t b, Int_t sk235){ //x: pe, a: thr_slope, b: thr_shift
   Double_t pc2pe = 2.463;
  Double_t pc = x*pc2pe;

  Double_t thr = 0.;
  if ( sk235 == 0 || sk235 == 1 )  thr = 0.5*( TMath::Erf( a*( pc - b ) ) + 1 );
  else thr = 1.;

  return thr;
}

Double_t get_dark_fac(Int_t pmtid){
  
/* ref. dscnt.F */
  Double_t gain_fac = 1.0;
  if(prod_year[pmtid] < 1000){
    gain_fac = 1.0;
  }else if(prod_year[pmtid] < 1996){
    gain_fac = gain[0]/gain_nom[0];
  }else if(prod_year[pmtid] < 1998){
    gain_fac = gain[1]/gain_nom[1];
  }else if(prod_year[pmtid] < 2004){
    gain_fac = gain[2]/gain_nom[2];
  }else if(prod_year[pmtid] < 2005){
    gain_fac = gain[3]/gain_nom[3];
  }else{
    gain_fac = gain[4]/gain_nom[4];
  }

/* currently not used */
  Double_t tunefac = 1.0;
  gain_fac *= tunefac;

  Double_t frac_hit = 0.0;
  Double_t x = 0;

/*ref. dscnt.F */
  if (isk235[pmtid] < 0) { // should be non-existing PMT in SK-5.
    frac_hit = 1.0;    
  }
  else{ // making p.e. distribution
    for (Int_t i = 0; i < 5000; i++){ // i : Q
      x = gain_fac * ((Double_t)i+0.5)/(100.*spe_corr_sk235[isk235[pmtid]]); // x is pe.
      
/* dsthr_sk5 should be used */      
      frac_hit += dsthr_sk5(x, th_slopesk23[isk235[pmtid]], th_shiftsk23[isk235[pmtid]], isk235[pmtid]) * spe_pdf_sk235[isk235[pmtid]][i];
      //frac_hit += dsthr(x,th_paramsk23[isk23[pmtid]]) * spe_pdf_sk235[isk23[pmtid]][i];
      //dsthr return 0 or 1. spe_pdf_sk235 is pdf for the Q.

      //cout<<" DEBUG : "<<x<<" "<<dsthr_sk5(x, th_slopesk23[isk235[pmtid]], th_shiftsk23[isk235[pmtid]], isk235[pmtid])<< " "<<spe_pdf_sk235[isk235[pmtid]][i]<<" "<<frac_hit<<endl;
    }
  }
  return 1/frac_hit;
}


void SK5_read_dark_rate(TString input_filename = "/home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.081586.root",
		    TString output_filename = "output/darkr.081586.txt",
		    Int_t runno = 81586){
  
// void read_dark_rate(TString input_filename = "/home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.074720.root",
// 		    TString output_filename = "output/darkr.074720.txt",
// 		    Int_t runno = 74720){
  
  
  Int_t           nrun_dark;
  Int_t           nsub_dark;
  Int_t           nfill_dark;
  Float_t         dark_ave;
  Float_t         dark_rate[nPMT];
  Float_t         dark_rate_od[1885];
  Float_t         dark_rate_od_subped[1885];

  // List of branches
  TBranch        *b_nrun_dark;   //!
  TBranch        *b_nsub_dark;   //!
  TBranch        *b_nfill_dark;   //!
  TBranch        *b_dark_ave;   //!
  TBranch        *b_dark_rate;   //!
  TBranch        *b_dark_rate_od;   //!
  TBranch        *b_dark_rate_od_subped;   //!


/* pmtinf.dat is not corresponded to SK-5(HK) PMT */
  cout << "Reading PMT info table from " << pmtinf_filename << endl;
  ifstream fpmtinf(pmtinf_filename.Data());
  if (!fpmtinf.is_open()){
    cout << "Cannot find PMT info table " << pmtinf_filename << endl;
    return;
  }
  Int_t pmtid;
  Int_t itmp;
  Int_t idummy;
  Float_t fdummy;
  for (Int_t i  = 0; i < nPMT; i++) {
    fpmtinf >> pmtid>> itmp >> fdummy;
    if (itmp == 2)
      isk235[pmtid-1] = 0;
    else if (itmp == 3)
      isk235[pmtid-1] = 1;
    else if (itmp == 5) /* SK-V PMT is added */
      isk235[pmtid-1] = 2;
    else {
      cout << "found illegal version PMT!" << pmtid << endl;
      isk235[pmtid-1] = -1;
    }    
    
  }

  cout << "Reading PMT production year table from " << pmtprod_filename << endl;

  ifstream fpmtprod(pmtprod_filename.Data());
  if (!fpmtprod.is_open()){
    cout << "Cannot find PMT info table " << pmtprod_filename << endl;
    return;
  }

  Double_t tmp;
  for (Int_t i  = 0; i < nPMT; i++){
    fpmtprod >> pmtid >> tmp >> idummy >> idummy >> idummy >> idummy;
    prod_year[pmtid-1] = tmp;
  }


  cout << "Reading PMT gain table from " << gain_filename << endl;
  ifstream fgain(gain_filename.Data());
  if (!fgain.is_open()){
    cout << "Cannot find PMT gain table " << gain_filename << endl;
    return;
  }

  string dummy;
  Int_t runno_tmp = 0;
  
  //  bool found_gain = false;
  
  while (runno_tmp < runno){
    fgain >> runno_tmp;
    for (Int_t i = 0; i < 15; i++)
      fgain >> dummy;
    for (Int_t i = 0; i < 5; i++)
      fgain >> gain[i];
     
    if (fgain.eof()) break;
  }

  cout << "Current run: " << runno << ". Loading gain table for " << runno_tmp << endl;
  for (int i = 0; i<5; i++) cout<<gain[i]<<endl;
  // if (!found_gain){
  //   cout << "Cannot find gain value for run " << runno << endl;
  //   cout << "Quiting" << endl;
  //   return;
  // }

  // read SPE pdf
  
  for (Int_t j = 0; j < 3; j++){
    Double_t p = 0;
    Double_t p_prev = 0;
    cout << "Reading SPE shape from " << spe_filename_sk235[j] << endl;

    ifstream data_file(spe_filename_sk235[j].Data());
    if (!data_file.is_open()) {
      cout << "Cannot find SPE shape file: " << spe_filename_sk235[j] << endl;
      return;
    }
    for (Int_t i = 0; i < 5000; i++){
      data_file >> p;
      if (i > 0){
        spe_pdf_sk235[j][i] = p - p_prev;
      }else{
        spe_pdf_sk235[j][i] = p;
      }
      p_prev = p;
      
    }
  }
   
   
  TFile * f = new TFile(input_filename.Data());
  TTree * tr = (TTree*)f->Get("skdark");
  tr->SetMakeClass(1);
   
  tr->SetBranchAddress("nrun_dark", &nrun_dark, &b_nrun_dark);
  tr->SetBranchAddress("nsub_dark", &nsub_dark, &b_nsub_dark);
  tr->SetBranchAddress("nfill_dark", &nfill_dark, &b_nfill_dark);
  tr->SetBranchAddress("dark_ave", &dark_ave, &b_dark_ave);
  tr->SetBranchAddress("dark_rate", dark_rate, &b_dark_rate);
  tr->SetBranchAddress("dark_rate_od", dark_rate_od, &b_dark_rate_od);
  tr->SetBranchAddress("dark_rate_od_subped", dark_rate_od_subped, &b_dark_rate_od_subped);

  tr->GetEntry(0);
  // tr->Print();

  ofstream fout(output_filename.Data());

   
  for (Int_t i = 0; i < nPMT; i++){
    Double_t dark_fac =  get_dark_fac(i);

//2021.Feb.2 Correction for Dead Time    
    dark_rate[i] = ( 1 - sqrt( 1 - 4*DeadTime*(dark_rate[i]*1000.)))/(2*DeadTime)/1000.;
    
    fout << i+1 << "\t" << dark_rate[i]*dark_fac << endl;
    //cout << i+1 << "\t" << dark_rate[i] << "\t" << dark_fac << endl;
  }
  
  fout.close();
  
  f->Close();
  cout << "Finish!" << endl;
}
