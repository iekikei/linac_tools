#include <limits.h> 
#include <string>
#include <TApplication.h>
#include <iostream>
#include <vector>
#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TMath.h"
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TPostScript.h>
#include <TPad.h>
#include <TMinuit.h>
#include <TNtuple.h>
#include <TNetFile.h>
#include <TGraphErrors.h>
#include <TF2.h>
#include <TH2F.h>
#include <TSystem.h>
#include <TStyle.h>
#include "TTree.h"
#include "TChain.h"
#include <TTreeCache.h>
#include<iostream>
#include<fstream>
#include<iomanip>
#include<cmath>
#include<sstream>
#include "SuperManager.h"
#include <TPaveStats.h>
#include "TLegend.h"
#include "TLine.h"
#include "TChain.h"
#include <TGaxis.h>

std::string const LINAC_DIR = std::getenv("LINAC_DIR"); 
std::string const RUNSUM_TXT = LINAC_DIR + "/runsum.dat";

void fit_me(TH1D *plot,float *x,float *y,float *z,int nnrun,int fflag){  
  float f_mean = 0.0;
  float f_rms = 0.0;
  float start_fit = 0.0;
  float end_fit = 0.0;
  
  double par_final[3]={0.0,0.0,0.0};
  double err_final[3]={0.0,0.0,0.0};
  
  f_mean = plot->GetMean(1);
  f_rms = plot->GetRMS(1);
  
  start_fit = f_mean - f_rms;
  end_fit = f_mean + f_rms;
  
  TF1 *neff_fit0 = new TF1("neff_fit0","gaus",start_fit,end_fit);
  neff_fit0->SetParameter(0,plot->Integral(0));
  neff_fit0->SetParameter(1,plot->GetMean(1));
  neff_fit0->SetParameter(2,plot->GetRMS(1));
  neff_fit0->SetLineWidth(2);
  plot->Fit(neff_fit0,"R+");
  neff_fit0->GetParameters(&par_final[0]);
  
  start_fit = par_final[1] - 2*par_final[2];
  end_fit = par_final[1] + 2*par_final[2];
  TF1 *neff_fit = new TF1("neff_fit","gaus",start_fit,end_fit);
  neff_fit->SetParameter(0,par_final[0]);
  neff_fit->SetParameter(1,par_final[1]);
  neff_fit->SetParameter(2,par_final[2]);
  neff_fit->SetLineColor(kRed);
  neff_fit->SetLineWidth(1);
  plot->Fit(neff_fit,"R+");
  neff_fit->GetParameters(&par_final[0]);
  
  for(int p=0;p<3;p++){
    err_final[p] = neff_fit->GetParError(p);
  }
  std::cout<<"Fit parameter(Double gaussian fit): peak = "<<par_final[1]<<" sigma = "<<par_final[2]<<" peak err = "<<err_final[1]<<std::endl;
  
  *x = par_final[1];
  *y = err_final[1];
  *z = par_final[2];
  
  return;
}

void set_input_file (TreeManager* mgr, TString filename) {
  TString cmd = Form("ls %s", filename.Data());
  if (system(cmd.Data()) ){
    exit(1);
  };
  mgr->SetInputFile(filename.Data());
}

void fill_hist(int flag,int nrun,double x, double y, double z, TH1D *plot[],double *count1,double *peak1, double *peak2,double *peak3){

  // Make tree manager
  int id = 10;
  SuperManager* Smgr = SuperManager::GetManager(); 
  Smgr->CreateTreeManager(id,"\0","\0",2);  // mode=2
  TreeManager* mgr = Smgr->GetTreeManager(id);

  if(flag==0){
    set_input_file(mgr, Form("%s/lowfit/fit_data/lin.%06d.root", std::getenv("LINAC_DIR"), nrun));
  }
  else if(flag==1){
    for (Int_t iData=0; iData<10; iData++) {
      set_input_file(mgr, Form("%s/lowfit/fit_detsim/lin.%06d.%03d.root", std::getenv("LINAC_DIR"), nrun, iData));
    }
  }
  else if(flag==2){
    for (Int_t iData=0; iData<10; iData++) {
      set_input_file(mgr, Form("%s/lowfit/fit_skg4/lin.%06d.%03d.root", std::getenv("LINAC_DIR"), nrun, iData));
    }
  }
  else {}
  
  // Initialize
  mgr->Initialize();
  
  // Set branch status
  TTree* tree = mgr->GetTree();
  tree->SetBranchStatus("*", 0);
  tree->SetBranchStatus("HEADER");
  tree->SetBranchStatus("LOWE");
  
  // Get Header information
  Header    *HEAD    = mgr->GetHEAD();
  LoweInfo  *LOWE    = mgr->GetLOWE();
  
  double ovaq, r_from_pipe,angle;
  float poswall[3] = {0.0,0.0,0.0};
  double A_count = 0.0;

  int  linfo150;
  double bswallsk;
  double linfo151;

  int ntotal = tree->GetEntries();  
  for(int i=0; i< ntotal; i++){
    //    if(i==5000){break;}
    tree->GetEntry(i);
    
    r_from_pipe = (LOWE->bsvertex[0]-x)*(LOWE->bsvertex[0]-x)+(LOWE->bsvertex[1]-y)*(LOWE->bsvertex[1]-y)+(LOWE->bsvertex[2]-z)*(LOWE->bsvertex[2]-z);
    r_from_pipe = sqrt(r_from_pipe);
    
    plot[0]->Fill(LOWE->bsvertex[0]);
    plot[1]->Fill(LOWE->bsvertex[1]);
    plot[2]->Fill(LOWE->bsvertex[2]);
    plot[3]->Fill(LOWE->bsenergy);  
    plot[4]->Fill(*reinterpret_cast <float*> (&LOWE->linfo[7]));
    
    angle = acos(-1.0*LOWE->bsdir[2]);
    plot[5]->Fill(angle*180.0/3.14); 
    
    ovaq = LOWE->bsgood[1]*LOWE->bsgood[1]-LOWE->bsdirks*LOWE->bsdirks;
    plot[6]->Fill(ovaq);
    plot[7]->Fill(LOWE->bsgood[1]);
    //plot[7]->Fill(r_from_pipe);
    plot[8]->Fill(LOWE->bspatlik);
    plot[9]->Fill(LOWE->bsdir[0]);
    plot[10]->Fill(LOWE->bsdir[1]);
    plot[11]->Fill(LOWE->bsdir[2]);
    
    A_count++;	  
  }
  
  float A_peak[3] = {0.0,0.0,0.0};
  TH1D *neff0 = new TH1D("","",100,0.0,200.0);
  TH1D *neff1 = new TH1D("","",100,0.0,200.0);
  TH1D *neff2 = new TH1D("","",100,0.0,200.0);
  
  if(flag==0) {
    neff0 = (TH1D*) plot[4]->Clone();
    fit_me(neff0,&A_peak[0],&A_peak[1],&A_peak[2],nrun,flag);
  } else if(flag==1){
    neff1 = (TH1D*) plot[4]->Clone();
    fit_me(neff1,&A_peak[0],&A_peak[1],&A_peak[2],nrun,flag);
  } else if(flag==2){
    neff2 = (TH1D*) plot[4]->Clone();
    fit_me(neff2,&A_peak[0],&A_peak[1],&A_peak[2],nrun,flag);
  }
  gStyle->SetOptStat(111111);

  char rootname[400];
  sprintf(rootname,"root/linac_run%06d.root",nrun);

  TFile *rootfile =new TFile(rootname,"update");
  if(flag==0){
    neff0->Draw();
    neff0->Write();
  } else if(flag==1){
    neff1->Draw();
    neff1->Write();
  } else if(flag==2){
    neff2->Draw();
    neff2->Write();
  }
  rootfile->Close();
  
  *count1 = A_count;  
  *peak1 = A_peak[0];
  *peak2 = A_peak[1];
  *peak3 = A_peak[2];

  // close file
  Smgr->DeleteTreeManager(id);
  // end
  SuperManager::DestroyManager();
  
  return;
}

int main(int argc,char *argv[]){

  if(argc!=2){
    std::cout<<"How to use: Write -> linac2016 <Run number>"<<std::endl;
    std::cout<<"Please try again."<<std::endl;
    return 1;
  }
  int RUN_NUMBER = atoi(argv[1]);
  
  int const index_linac = 300;
  int linac_run = 0;
  double A_posi[3] = {0.0,0.0,0.0};
  int e_mode = 0;
  int run_mode = 0;
  int other_run = 0;
  char str[256];
  
  std::ifstream IN;
  IN.open(RUNSUM_TXT.c_str());
  if(!IN){
    std::cout<<RUNSUM_TXT<<" does not exist."<<std::endl;
    return 1;
  }
	
  for(int i=0;i<index_linac;i++){
    IN>>linac_run;
    if(linac_run==RUN_NUMBER){
      IN>>e_mode>>run_mode>>A_posi[0]>>A_posi[1]>>A_posi[2]>>other_run;
      std::cout<<linac_run<<std::endl;
      break;
    } else {
      IN.getline(str,256);
    }
  }
  if(IN.eof()) {
    std::cerr<<" does not exist this run."<<std::endl;
    return 1;
  }
  IN.close();

  ////// Plot info ///////
  char ene_txt[400];
  sprintf(ene_txt,"%d MeV (2019)",e_mode);
  char xpos_txt[400],zpos_txt[400];
  sprintf(xpos_txt,"X = %4.2fm",A_posi[0]/100.);
  sprintf(zpos_txt,"Z = %4.2fm",A_posi[2]/100.);

  ////// Define Plot //// 
  TH1D *plot[3][12];
  plot[0][0] = new TH1D("sampleA vertex x","x(data)",1000,-2500.0,+2500.0);
  plot[0][1] = new TH1D("sampleA vertex y","y(data)",1000,-2500.0,+2500.0);
  plot[0][2] = new TH1D("sampleA vertex z","z(data)",1000,-2500.0,+2500.0);
  plot[0][3] = new TH1D("sampleA bsenergy","bsenergy(data)",160,0.0,40.0);
  plot[0][4] = new TH1D("sampleA neff","neff(data)",150,0.0,300.0);
  plot[0][5] = new TH1D("sampleA angle","angle(data)",90,0.0,180.0);
  plot[0][6] = new TH1D("sampleA ovaq","ovaq(data)",50,0.0,1.0);
  plot[0][7] = new TH1D("sampleA bsgood","bsgood(data)",100,0.4,1.0);
  //plot[0][7] = new TH1D("sampleA r","r(data)",100,0.0,500.0);
  plot[0][8] = new TH1D("sampleA patlik","patlik(data)",50,-2.5,1.0);
  plot[0][9] = new TH1D("sampleA dir x","dir x(data)",100,-1.0,1.0);
  plot[0][10] = new TH1D("sampleA dir y","dir y(data)",100,-1.0,1.0);
  plot[0][11] = new TH1D("sampleA dir z","dir z(data)",100,-1.0,1.0);
 
  plot[1][0] = new TH1D("sampleB vertex x","x(MC)",1000,-2500.0,+2500.0);
  plot[1][1] = new TH1D("sampleB vertex y","y(MC)",1000,-2500.0,+2500.0);
  plot[1][2] = new TH1D("sampleB vertex z","z(MC)",1000,-2500.0,+2500.0);
  plot[1][3] = new TH1D("sampleB bsenergy","bsenergy(MC)",160,0.0,40.0);
  plot[1][4] = new TH1D("sampleB neff","neff(MC)",150,0.0,300.0);
  plot[1][5] = new TH1D("sampleB angle","angle(MC)",90,0.0,180.0);
  plot[1][6] = new TH1D("sampleB ovaq","ovaq(MC)",50,0.0,1.0);
  plot[1][7] = new TH1D("sampleB bsgood","bsgood(MC)",100,0.4,1.0);
  //plot[1][7] = new TH1D("sampleB r","r(MC)",100,0.0,500.0);
  plot[1][8] = new TH1D("sampleB patlik","patlik(MC)",50,-2.5,1.0);
  plot[1][9] = new TH1D("sampleB dir x","dir x(MC)",100,-1.0,1.0);
  plot[1][10] = new TH1D("sampleB dir y","dir y(MC)",100,-1.0,1.0);
  plot[1][11] = new TH1D("sampleB dir z","dir z(MC)",100,-1.0,1.0);
  
  plot[2][0] = new TH1D("sampleC vertex x","x(MC)",1000,-2500.0,+2500.0);
  plot[2][1] = new TH1D("sampleC vertex y","y(MC)",1000,-2500.0,+2500.0);
  plot[2][2] = new TH1D("sampleC vertex z","z(MC)",1000,-2500.0,+2500.0);
  plot[2][3] = new TH1D("sampleC bsenergy","bsenergy(MC)",160,0.0,40.0);
  plot[2][4] = new TH1D("sampleC neff","neff(MC)",150,0.0,300.0);
  plot[2][5] = new TH1D("sampleC angle","angle(MC)",90,0.0,180.0);
  plot[2][6] = new TH1D("sampleC ovaq","ovaq(MC)",50,0.0,1.0);
  plot[2][7] = new TH1D("sampleC bsgood","bsgood(MC)",100,0.4,1.0);
  //plot[2][7] = new TH1D("sampleC r","r(MC)",100,0.0,500.0);
  plot[2][8] = new TH1D("sampleC patlik","patlik(MC)",50,-2.5,1.0);
  plot[2][9] = new TH1D("sampleC dir x","dir x(MC)",100,-1.0,1.0);
  plot[2][10] = new TH1D("sampleC dir y","dir y(MC)",100,-1.0,1.0);
  plot[2][11] = new TH1D("sampleC dir z","dir z(MC)",100,-1.0,1.0);
  
  double count[3]={0.0,0.0};
  double peak[3][3] = {{0.}};

  for(int i=0;i<3;i++){
    fill_hist(i,RUN_NUMBER,A_posi[0],A_posi[1],A_posi[2],plot[i],&count[i],&peak[i][0],&peak[i][1],&peak[i][2]);
  }
  
  for(int i=0;i<12;i++){	  
    plot[0][i]->Sumw2();
    plot[2][i]->Sumw2();
    plot[0][i]->Scale(count[1]/count[0]);
    plot[2][i]->Scale(count[1]/count[2]);
  }
  
  //Canvas Style  
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetStatColor(0);
  gStyle->SetFillColor(0);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadLeftMargin(0.22);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetPadBottomMargin(0.15);
  //gStyle->SetPadGridX(1);
  //gStyle->SetPadGridY(1);
  gStyle->SetEndErrorSize(2);

  //Font Style
  gStyle->SetTextFont(132);
  gStyle->SetLabelFont(132,"XYZ");
  gStyle->SetTitleFont(132,"XYZ");

  //Title and Label Style
  gStyle->SetTitleSize(0.08,"X");
  gStyle->SetTitleSize(0.08,"Y");
  gStyle->SetLabelSize(0.075,"Y");
  gStyle->SetLabelSize(0.075,"X");
  gStyle->SetTitleXOffset(0.9);
  gStyle->SetTitleYOffset(1.5);
  gStyle->SetNdivisions(505,"X");
  gStyle->SetNdivisions(510,"Y");
  gStyle->SetStripDecimals(false);
  
  /////// Output to pdf ////
  TCanvas *c1 = new TCanvas("c","CANVAS",800,800);
  char pdf[400];
  std::string PDFNAME = "";
  sprintf(pdf,"pdf/linac_run%06d.pdf",RUN_NUMBER);
  PDFNAME= pdf;

  c1->Divide(3,4);
  for(int i=0;i<12;i++){
    c1->cd(i+1);

    if(i<5){
      plot[1][i]->GetXaxis()->SetRangeUser(plot[1][i]->GetMean()-4.0*plot[1][i]->GetRMS(),plot[1][i]->GetMean()+4.0*plot[1][i]->GetRMS());
    }
    plot[1][i]->UseCurrentStyle();
    plot[1][i]->SetLineColor(2); 
    plot[2][i]->SetLineColor(4); 

    plot[1][i]->Draw("");
    if(i==0)      { plot[1][i]->SetTitle(";Vertex x[cm];# of event");}
    else if(i==1) { plot[1][i]->SetTitle(";Vertex y[cm];# of event"); }
    else if(i==2) { plot[1][i]->SetTitle(";Vertex z[cm];# of event"); }
    else if(i==3) { plot[1][i]->SetTitle(";Energy[MeV];# of event"); }
    else if(i==4) { plot[1][i]->SetTitle(";Neff hit;# of event"); }
    else if(i==5) { plot[1][i]->SetTitle(";Angle[deg];# of event");}
    else if(i==6) { plot[1][i]->SetTitle(";Ovaq;# of event"); }
    else if(i==7) { plot[1][i]->SetTitle(";BS goodness;# of event"); }
    //else if(i==7) { plot[1][i]->SetTitle(";Distance from endcap[cm];# of event"); }
    else if(i==8) { plot[1][i]->SetTitle(";Patlik;# of event"); }
    else if(i==9) { plot[1][i]->SetTitle(";Dir x;# of event"); }
    else if(i==10){ plot[1][i]->SetTitle(";Dir y;# of event"); }
    else if(i==11){ plot[1][i]->SetTitle(";Dir z;# of event"); }
 
    plot[2][i]->Draw("same hist"); // Added by Harada
    plot[0][i]->Draw("same");

    TText *t_ene = new TText(0.63,0.86,ene_txt);
    t_ene->Draw();
    t_ene->SetTextSize(0.062);
    t_ene->SetNDC(1);

    TText *t_xpos = new TText(0.68,0.78,xpos_txt);
    t_xpos ->Draw();
    t_xpos ->SetTextSize(0.062);
    t_xpos ->SetNDC(1);

    TText *t_zpos = new TText(0.68,0.72,zpos_txt);
    t_zpos ->Draw();
    t_zpos ->SetTextSize(0.062);
    t_zpos ->SetNDC(1);

    TLegend *leg = new TLegend(0.7,0.5,0.9,0.7);
    leg->AddEntry(plot[0][i],"DATA","l");
    leg->AddEntry(plot[1][i],"DETS","l");
    leg->AddEntry(plot[2][i],"SKG4","l");
    leg->SetBorderSize(0);
    leg->SetTextSize(0.1);
    leg->Draw();

  }
  c1->Update();
  c1->Print(PDFNAME.c_str());

  /////// Output to txt //////
  char outname[400];
  std::string OUTNAME="";
  sprintf(outname,"txt/linac_run%06d.dat",RUN_NUMBER);
  OUTNAME = outname;
  
  std::ofstream OUT;
  OUT.open(OUTNAME.c_str());
  OUT<<peak[0][0]<<"\t"<<peak[0][1]<<"\t"<<peak[0][2]<<"\t"<<peak[1][0]<<"\t"<<peak[1][1]<<"\t"<<peak[1][2]<<"\t"<<peak[2][0]<<"\t"<<peak[2][1]<<"\t"<<peak[2][2];
  OUT.close();

  return 1;
}
