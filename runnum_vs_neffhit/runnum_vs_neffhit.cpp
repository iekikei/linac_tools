#include <limits.h>
#include <string>
#include <TApplication.h>
#include <iostream>
#include <vector>
#include "TFile.h"
#include "TH1F.h"
#include <TMultiGraph.h>
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

std::string const LINAC_TXT = "/home/sklowe/linac/const/linac_sk5_runsum.dat";

int const POSI = 6;
int const MODE = 6;
int LINAC_ALL = 6;

void read_me(int run,double *data,double *data_e, double *data_err, double *g3, double *g3_e, double *g3_err, double *g4, double *g4_e, double *g4_err){
  double a,b,c,d,e,f,g,h,i;
  char cname[400];
  std::string fname="";
  int id = 10;
  std::string dir="";
  dir = "/home/mharada/Lowe/LINAC/EScale/sk5_linac_tools/compare/txt/";

  sprintf(cname,"linac_run%06d.dat",run);

  fname = dir+cname;
  std::ifstream FILE;
  FILE.open(fname.c_str());
  if(!FILE){
    std::cout<<fname<<" does not exist."<<std::endl;
    return;
  }
  FILE>>a>>b>>c>>d>>e>>f>>g>>h>>i;

  *data = a;
  *data_e =b;
  *data_err = c;
  *g3 = d;
  *g3_e = e;
  *g3_err = f;
  *g4 = g;
  *g4_e = h;
  *g4_err = i;

  return;
}


int main(const int argc,char *argv[]){
  if(argc!=3){
    std::cout<<"How to use: Write -> runnum_vs_neffhit <first runnum> <end runnum>"<<std::endl;
    std::cout<<"Please try again."<<std::endl;
    return 1;
  }
  int linac_run;
  double pipe_x;
  double pipe_y;
  double pipe_z;
  int e_mode;
  int run_mode;
  int other_run;

  int Start_RUNNUM = atoi(argv[1]);
  int End_RUNNUM = atoi(argv[2]);

  std::ifstream IN;
  IN.open(LINAC_TXT.c_str());
  if(!IN){
    std::cout<<LINAC_TXT<<" does not exist."<<std::endl;
    return 1;
  }
  int index=0;
  int const list=300;
  int A_linac_run[list];
  int A_e_mode[list];
  int mode =0;

  for(int i=0;i<list;i++){
    IN>>linac_run>>e_mode>>run_mode>>pipe_x>>pipe_y>>pipe_z>>other_run;
    if((Start_RUNNUM <= linac_run) && (linac_run <= End_RUNNUM) && (run_mode == 0)){
      if((linac_run != 74824) && (linac_run != 74970)){
        A_linac_run[index] = linac_run;

        if(e_mode == 3){
          A_e_mode[index] = 0;
        } else if(e_mode ==4){
          A_e_mode[index] = 1;
        } else if(e_mode ==5){
          A_e_mode[index] = 2;
        } else if(e_mode ==8){
          A_e_mode[index] = 3;
        } else if(e_mode ==10){
          A_e_mode[index] = 4;
        } else if(e_mode ==12){
          A_e_mode[index] = 5;
        } else if(e_mode ==15){
          A_e_mode[index] = 6;
        } else if(e_mode ==18){
          A_e_mode[index] = 7;
        }
      } else{
        continue;
      }
      index=index+1;
    }
  }
  IN.close();

  double mean[index][3],rms[index][3];
  double dp,g3p,g4p,dpe,g3pe,g4pe,derr,g3err,g4err;
  double val_data[index],val_g3[index],val_g4[index],val_data_rms[index],val_g3_rms[index],val_g4_rms[index];
  double valr[index][2],valr_r[index][2],ratio_err[index][2];

  for(int i=0;i<index;i++){
    int nrun = A_linac_run[i];
    read_me(nrun,&dp,&dpe,&derr,&g3p,&g3pe,&g3err,&g4p,&g4pe,&g4err);
    mean[i][0] = dp;
    rms[i][0] = dpe;
    mean[i][1] = g3p;
    rms[i][1] = g3pe;
    mean[i][2] = g4p;
    rms[i][2] = g4pe;
    ratio_err[i][0] = (g3p/dp)*sqrt((derr/dp)*(derr/dp)+(g3err/g3p)*(g3err/g3p))*100;
    ratio_err[i][1] = (g4p/dp)*sqrt((derr/dp)*(derr/dp)+(g4err/g4p)*(g4err/g4p))*100;
  }

  for(int i=0;i<index;i++){
    val_data[i] = mean[i][0];
    val_g3[i]   = mean[i][1];
    val_g4[i]   = mean[i][2];
    val_data_rms[i] = rms[i][0];
    val_g3_rms[i]   = rms[i][1];
    val_g4_rms[i]   = rms[i][2];

    valr[i][0] = (mean[i][0]-mean[i][1])/mean[i][1]*100.0;
    valr[i][1] = (mean[i][0]-mean[i][2])/mean[i][2]*100.0;
    valr_r[i][0] = sqrt((rms[i][0]/mean[i][0])*(rms[i][0]/mean[i][0]) + (rms[i][1]/mean[i][1])*(rms[i][1]/mean[i][1]));
    valr_r[i][1] = sqrt((rms[i][0]/mean[i][0])*(rms[i][0]/mean[i][0]) + (rms[i][2]/mean[i][2])*(rms[i][2]/mean[i][2]));
  }

  double val[10][index];
  double num[index];
  double ratio[index];
  double zero[index];

  for(int i=0;i<index;i++){
    zero[i]=0.0;
  }  
  for(Int_t i=0;i<index;i++){
    num[i]=A_linac_run[i];
    val[0][i]=val_data[i];
    val[1][i]=val_data_rms[i];
    val[2][i]=val_g3[i];
    val[3][i]=val_g3_rms[i];
    val[4][i]=val_g4[i];
    val[5][i]=val_g4_rms[i];
    val[6][i]=valr[i][0];
    val[7][i]=valr_r[i][0];
    //val[7][i]=ratio_err[i][0];
    val[8][i]=valr[i][1];
    val[9][i]=valr_r[i][1];
    //val[9][i]=ratio_err[i][1];
    for(int j=0;j<10;j++) std::cout<<val[j][i]<<"  ";
    std::cout<<std::endl;
  }

  IN.close();

  double num_d[index],num_3[index], num_4[index];  
  for(int i =0;i<index;i++){
    num_d[i]=num[i]-1.0;
    num_3[i]=num[i]+1.0;
    num_4[i]=num[i]+3.0;
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
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetEndErrorSize(2);

  //Font Style
  gStyle->SetTextFont(132);
  gStyle->SetLabelFont(132,"XYZ");
  gStyle->SetTitleFont(132,"XYZ");

  //Title and Label Style
  gStyle->SetTitleSize(0.09,"XY");
  gStyle->SetLabelSize(0.07,"XY");
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(0.4);
  gStyle->SetNdivisions(505,"X");
  gStyle->SetNdivisions(510,"Y");
  gStyle->SetStripDecimals(false);

  TCanvas *c1 = new TCanvas("c1","CANVAS",700,500);
  char eps[400];
  std::string EPSNAME = "runnum_vs_neffhit.eps";
  c1->UseCurrentStyle();
  c1->Divide(1,2);

  c1->cd(1);
  TGraphErrors *g_d = new TGraphErrors(index,num,val[0],zero,val[1]);
  g_d->SetMarkerStyle(8);
  g_d->SetTitle(";Runnumber;Neffhit");
  g_d->GetXaxis()->SetLimits(81580,81850);
  g_d->Draw("AP");

  TGraphErrors *g_3 = new TGraphErrors(index,num_3,val[2],zero,val[3]);
  g_3->SetMarkerStyle(8);
  g_3->SetMarkerColor(2);
  g_3->SetLineColor(2);
  g_3->Draw("P");

  TGraphErrors *g_4 = new TGraphErrors(index,num_4,val[4],zero,val[5]);
  g_4->SetMarkerStyle(8);
  g_4->SetMarkerColor(4);
  g_4->SetLineColor(4);
  g_4->Draw("P");

  TLegend *leg = new TLegend(0.1,0.2,0.2,0.4);
  std::string te[3];
  te[0] = "DATA";
  te[1] = "G3";
  te[2] = "G4";

  leg->AddEntry(g_d,te[0].c_str(),"lp");
  leg->AddEntry(g_3,te[1].c_str(),"lp");
  leg->AddEntry(g_4,te[2].c_str(),"lp");
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->Draw();


  c1->cd(2);
  TGraphErrors *gratio_3 = new TGraphErrors(index,num_3,val[6],zero,val[7]);
  gratio_3->UseCurrentStyle();
  gratio_3->SetTitle(";Runnumber;Difference of Neffhit[%]");
  gratio_3->GetYaxis()->SetRangeUser(-6.0,6.0);
  gratio_3->GetXaxis()->SetLimits(81580,81850);
  gratio_3->SetMarkerStyle(20);
  gratio_3->SetMarkerColor(2);
  gratio_3->Draw("AP");

  TGraphErrors *gratio_4= new TGraphErrors(index,num_4,val[8],zero,val[9]);
  gratio_4->UseCurrentStyle();
  gratio_4->SetMarkerStyle(20);
  gratio_4->SetMarkerColor(4);
  gratio_4->Draw("P");

  TText *text = new TText(0.15,0.93,"*Difference of Neffhit = (DATA-MC)/MC [%]");
  text->Draw();
  text->SetTextSize(0.05);
  text->SetNDC(1);


  c1->Update();
  c1->Print(EPSNAME.c_str());

  return 1; 
}
