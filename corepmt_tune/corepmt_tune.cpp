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
#include <TLegend.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TPostScript.h>
#include <TPad.h>
#include <TLine.h>
#include <TMinuit.h>
#include <TNtuple.h>
#include <TNetFile.h>
#include <TMultiGraph.h>
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
#include <TGaxis.h>
#include <TLatex.h>
#include <TText.h>
#include <TLine.h>

#define N_RUN 3

std::string const LINAC_DIR = std::getenv("LINAC_DIR"); 
std::string const RUNSUM_TXT = LINAC_DIR + "/runsum.dat";

void read_me(int flag,int run,double *x, double *y){
  double a=0.,b=0.,c=0.,d=0.,e=0.,f=0.,g=0.,h=0.,i=0.;
  double j=0., k=0., l=0.,m=0., n=0., o=0.,xx=0.,yy=0.;
  char cname[400];
  std::string fname="";
  int id = 10;
  std::string dir="";
  dir = LINAC_DIR + "/compare/txt/";
  sprintf(cname,"linac_run%06d.dat",run);
  //sprintf(cname,"linac2016_run%06d_tune.dat",run);
  fname = dir+cname;
  std::ifstream ifs;
  ifs.open(fname.c_str());
  if(!ifs){
    //std::cout<<fname<<" does not exist."<<std::endl;
    *x = -1;
    *y = -1;
    return;
  }

  if(flag==0){
    ifs>>xx>>yy>>c>>d>>e>>f>>g>>h>>i>>j>>k>>l;
  } else if(flag==1){
    ifs>>a>>b>>c>>xx>>yy>>f>>g>>h>>i>>j>>k>>l;
  } else if(flag==2){
    ifs>>a>>b>>c>>d>>e>>f>>xx>>yy>>i>>j>>k>>l;
  }
  *x=xx;
  *y=yy;
  return;
}

int main(int argc,char *argv[]){
  double index[N_RUN]={0.5,1.5,2.5};
  int linac_run[N_RUN];
  double a,b,c,d;
  int const index_linac =300;
  int A_linac_run = 0;
  int e_mode = 0;
  int run_mode = 0;
  int isecond = 0;
  char str[256];

  if (argc != 2) {
    printf("Usage: ./corepmt_tune [Energy mode]");    
  }
  int EMode = atoi(argv[1]);

  std::ifstream IN;
  IN.open(RUNSUM_TXT.c_str());
  if(!IN){
    std::cout<<RUNSUM_TXT<<" does not exist."<<std::endl;
    return 1;
  }

  for(int i=0;i<index_linac;i++){
    IN>>A_linac_run>>e_mode>>run_mode>>a>>b>>c>>d ;
    if(e_mode==EMode && run_mode==0){
      linac_run[isecond] = A_linac_run;
      isecond++;
    } else {
      IN.getline(str,256);
    }
  }
  IN.close();

  char runnum[400];
  sprintf(runnum,"          %d           %d           %d",linac_run[0],linac_run[1],linac_run[2]);

  double zero[N_RUN]={0.5,0.5,0.5};

  for(int i=0;i<N_RUN;i++){
    //std::cout<<"linac_run="<<linac_run[i]<<std::endl;
  }

  double val[2][N_RUN]={0.0};
  double val_err[2][N_RUN]={0.0};
  double peak[3][N_RUN],peakerr[3][N_RUN];
  double rave[2]={0.0},rave_err[2]={0.0},rrms[2]={0.0};
  double p;
  double pe;
  float nzero = 0;
  for(int i=0;i<N_RUN;i++){
    for(int j=0;j<3;j++){
      read_me(j, linac_run[i],&p, &pe);
      
      peak[j][i] = p;
      peakerr[j][i] = pe;
    }
  }

  for(int i=0;i<N_RUN;i++){
    if ( peak[0][i] < 0.) {
      nzero ++;
      continue;
    }
    val[0][i] =peak[1][i]/peak[0][i];
    val[1][i] =peak[2][i]/peak[0][i];
    val_err[0][i] = sqrt((pow(peakerr[1][i],2)/pow(peak[0][i],2))+(pow(peak[1][i],2)*pow(peakerr[0][i],2)/pow(peak[0][i],4)));
    val_err[1][i] = sqrt((pow(peakerr[2][i],2)/pow(peak[0][i],2))+(pow(peak[2][i],2)*pow(peakerr[0][i],2)/pow(peak[0][i],4)));
    rave[0] = rave[0] + val[0][i];
    rave[1] = rave[1] + val[1][i];
    rave_err[0] = pow(val_err[0][i],2); 
    rave_err[1] = pow(val_err[1][i],2); 
  }

  rave[0]  = rave[0]/(N_RUN - nzero);
  rave[1]  = rave[1]/(N_RUN - nzero);

  float nzero2 = 0.;
  for(int i=0;i<N_RUN;i++){
    if ( peak[0][i] < 0.) {
      nzero2 ++;
      continue;
    }
    rrms[0] = rrms[0] + pow(val[0][i]-rave[0],2);
    rrms[1] = rrms[1] + pow(val[1][i]-rave[1],2);
  }

  rrms[0] = sqrt(rrms[0] /(N_RUN-nzero2));
  rrms[1] = sqrt(rrms[1] /(N_RUN-nzero2));

  char rrave[2][400];
  sprintf(rrave[0],"AVE = %5.4lf  RMS = %5.4lf",rave[0],rrms[0]);
  sprintf(rrave[1],"AVE = %5.4lf  RMS = %5.4lf",rave[1],rrms[1]);
  
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetOptStat(10);
  gStyle->SetPalette(1);
  gStyle->SetStatColor(0);
  gStyle->SetFillColor(0);
  gStyle->SetEndErrorSize(2);
  gStyle->SetNdivisions(000006);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetGridStyle(3);
  gStyle->SetTextFont(22);
  gStyle->SetTitleSize(0.085,"X");
  gStyle->SetTitleSize(0.085,"Y");
  gStyle->SetLabelSize(0.07,"Y");
  gStyle->SetLabelSize(0.01,"X");
  gStyle->SetLabelOffset(29.,"X");
  gStyle->SetTitleXOffset(0.7);
  gStyle->SetTitleYOffset(1.2);
  gStyle->SetLineWidth(4.5);
  int Color[3] = {1,2,4};
  
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  char eps[400];
  std::string EPSNAME = Form("eps/PositionAve_%dMeV.eps",EMode);
  c1->Divide(2,1);
  TGraphErrors  *g[3];
  for(int flag=0;flag<3;flag++){
    g[flag] = new TGraphErrors(N_RUN,index,peak[flag],zero,peakerr[flag]);
    g[flag]->SetMarkerColor(Color[flag]);
    g[flag]->SetLineColor(Color[flag]);
  }
  TMultiGraph *mtg = new TMultiGraph();

  TGraphErrors *plot1  = new TGraphErrors(N_RUN,index,val[0],zero,val_err[0]);
  TGraphErrors *plot2  = new TGraphErrors(N_RUN,index,val[1],zero,val_err[1]);

  c1->cd(1); 
  gPad->SetLeftMargin(0.25);
  gPad->SetRightMargin(0.01);
  gPad->SetBottomMargin(0.15);
  gPad->SetTopMargin(0.02);
  for(int flag=0;flag<3;flag++){
    mtg->Add(g[flag]);
  }
  mtg->SetTitle(";Run number;# of Nhit");
  mtg->Draw("AP");
  //mtg->SetMaximum(70.);
  //mtg->SetMinimum(60.);
  mtg->SetMaximum(peak[0][5]+5);
  mtg->SetMinimum(peak[0][5]-5);
  mtg->GetXaxis()->SetLimits(0.,N_RUN);
  
  TText *tlabel = new TText(0.20,0.10,runnum);
  tlabel->Draw();
  tlabel->SetNDC(1);

  TLegend *leg = new TLegend(0.7,0.7,0.85,0.85);
  leg->SetTextSize(0.045);
  std::string te[3];
  te[0] = "DATA";
  te[1] = "DETSIM";
  te[2] = "SKG4";

  leg->AddEntry(g[0],te[0].c_str(),"l");
  leg->AddEntry(g[1],te[1].c_str(),"l");
  leg->AddEntry(g[2],te[2].c_str(),"l");
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->Draw();

  c1->cd(2); 
  gPad->SetLeftMargin(0.25);
  gPad->SetRightMargin(0.01);
  gPad->SetBottomMargin(0.15);
  gPad->SetTopMargin(0.02);
  plot1->SetLineColor(Color[1]);
  plot2->SetLineColor(Color[2]);
  plot1->Draw("AP");
  plot1->SetMaximum(1.03);
  plot1->SetMinimum(0.97);
  plot1->SetTitle(";Run number;MC/DATA");
  plot1->GetXaxis()->SetLimits(0.,N_RUN);
  plot2->Draw("P");


  TText *t = new TText(0.6,0.85,Form("%dMeV",EMode));
  t->SetTextSize(0.15);
  t->Draw();
  t->SetNDC(1);

  TText *tlabel2 = new TText(0.20,0.10,runnum);
  tlabel2->Draw();
  tlabel2->SetNDC(1);
  
  TText *t3 = new TText(0.35,0.35,rrave[0]);
  t3->SetTextSize(0.05);
  t3->SetTextColor(Color[1]);
  t3->Draw();
  t3->SetNDC(1);
  TText *t4 = new TText(0.35,0.30,rrave[1]);
  t4->SetTextSize(0.05);
  t4->SetTextColor(Color[2]);
  t4->Draw();
  t4->SetNDC(1);

  TLine *z1 = new TLine(0,1,N_RUN,1);
  z1 ->SetLineColor(1);
  z1 ->SetLineStyle(1);
  z1 ->SetLineWidth(3);
  z1 ->Draw();
  TLine *z2 = new TLine(0,1.01,N_RUN,1.01);
  z2 ->SetLineColor(1);
  z2 ->SetLineWidth(2);
  z2 ->SetLineStyle(2);
  z2 ->Draw();
  TLine *z3 = new TLine(0,0.99,N_RUN,0.99);
  z3 ->SetLineColor(1);
  z3 ->SetLineStyle(2);
  z3 ->SetLineWidth(2);
  z3 ->Draw();

  c1->Update();
  c1->Print(EPSNAME.c_str());
  
  //  thaeApp.Run();
  std::cout <<" nzero := "<<nzero<<"  "<<nzero2<<std::endl;
  std::cout<< " AVE (DETSIM)        :=  "<<rave[0]<<" ,   "<<rrave[0]<<std::endl;
  std::cout<< " AVE (SKG4)          :=  "<<rave[1]<<" ,   "<<rrave[1]<<std::endl;
  std::cout<< " RATIO (DETSIM/SKG4) :=  "<<rave[0]/rave[1]<<" ,   "<<rrave[1]<<std::endl;

  std::ofstream ofs("corepmt.dat",std::ios::app);
  ofs<<EMode<<" "<<rave[0]<<" "<<rrms[0]<<" "<<rave[1]<<" "<<rrms[1]<<std::endl;
  return 1;
}
