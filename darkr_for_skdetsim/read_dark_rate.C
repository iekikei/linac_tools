#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TMath.h"

const Int_t nPMT = 11146;
Double_t prod_year[nPMT];
Int_t isk23[nPMT];
Double_t gain[5];
Double_t gain_nom[5] = {0.992057, 1.00184, 0.989083, 0.98216, 0.987678};

TString pmtprod_filename = "/home/skofl/sklib_gcc4.8.5/skofl_16c/const/pmt_prod_year.dat";
//TString gain_filename="/disk01/usr5/wanly/lowe/energyrecon/const/water.ave10.2";
TString gain_filename="/home/ocon/waterjob/src/rep10.5newgain/water.ave10newgain";
TString pmtinf_filename="/home/skofl/sklib_gcc4.8.5/skofl_16c/const/pmtinf.dat";

Double_t th_paramsk23[2] = {1.74909,1.56925};
Double_t spe_corr_sk23[2] = {0.9997534,0.9995633};
Double_t spe_pdf_sk23[2][5000];

TString spe_filename_sk23[2] = {"spe/spe_func_sk2.txt","spe/spe_func_sk3.txt"};


Double_t dsthr(Double_t x, Double_t a){
  // function to apply QBEE threshold
   Double_t pc2pe = 2.243;
  Double_t pc = x*pc2pe;
  return 0.5 * ( TMath::Erf(17.09*(a*pc-0.649))+1 );
}
    

Double_t get_dark_fac(Int_t pmtid){
  
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

  Double_t frac_hit = 0.0;
  Double_t x = 0;

  if (isk23[pmtid] < 0) { // should be non-existing PMT in SK-4.
    frac_hit = 1.0;    
  }else{
    for (Int_t i = 0; i < 5000; i++){
      x = gain_fac * ((Double_t)i+0.5)/(100.*spe_corr_sk23[isk23[pmtid]]);
      frac_hit += dsthr(x,th_paramsk23[isk23[pmtid]]) * spe_pdf_sk23[isk23[pmtid]][i];
    }
  }
  return 1/frac_hit;
}


void read_dark_rate(TString input_filename = "/home/skofl/sklib_gcc4.8.5/skofl-trunk/const/darkr/darkr.061505.root",
		    TString output_filename = "output/darkr.061505.txt",
		    Int_t runno = 61505){
  
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



  cout << "Reading PMT info table from " << pmtinf_filename << endl;
  ifstream fpmtinf(pmtinf_filename.Data());
  if (!fpmtinf.is_open()){
    cout << "Cannot find PMT info table " << pmtinf_filename << endl;
    return;
  }
  Int_t pmtid;
  Int_t itmp;
  Int_t idummy;
  for (Int_t i  = 0; i < nPMT; i++){
    fpmtinf >> pmtid>> itmp >> idummy;
    if (itmp == 2)
      isk23[pmtid-1] = 0;
    else if (itmp == 3)
      isk23[pmtid-1] = 1;
    else{
      isk23[pmtid-1] = -1;
      //      cout << "found non SK2 or SK3 PMT!" << pmtid << endl;
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
  // if (!found_gain){
  //   cout << "Cannot find gain value for run " << runno << endl;
  //   cout << "Quiting" << endl;
  //   return;
  // }

  // read SPE pdf
  
  for (Int_t j = 0; j < 2; j++){
    Double_t p = 0;
    Double_t p_prev = 0;
    cout << "Reading SPE shape from " << spe_filename_sk23[j] << endl;

    ifstream data_file(spe_filename_sk23[j].Data());
    if (!data_file.is_open()){
      cout << "Cannot find SPE shape file: " << spe_filename_sk23[j] << endl;
      return;
    }
    for (Int_t i = 0; i < 5000; i++){
      data_file >> p;
      if (i > 0){
	spe_pdf_sk23[j][i] = p - p_prev;
      }else{
	spe_pdf_sk23[j][i] = p;
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
    
    fout << i+1 << "\t" << dark_rate[i]*dark_fac << endl;
    //    cout << i+1 << "\t" << dark_rate[i] << "\t" << dark_fac << endl;
  }
  
  fout.close();
  
  f->Close();
  cout << "Finish!" << endl;
}
