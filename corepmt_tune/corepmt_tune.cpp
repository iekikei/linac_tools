#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <TLatex.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TH1F.h>

#define N_DATA 3

std::string const LINAC_DIR = std::getenv("LINAC_DIR"); 
std::string const RUNSUM_TXT = LINAC_DIR + "/runsum.dat";

int main(int argc,char *argv[]){

  if (argc != 2) {
    printf("Usage: ./corepmt_tune [Energy mode]\n");    
    return 1;
  }
  Int_t EMode = atoi(argv[1]);

  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetOptStat(0);
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
  gStyle->SetLabelSize(0.09,"X");
  gStyle->SetTitleXOffset(0.6);
  gStyle->SetTitleYOffset(1.2);
  gStyle->SetPadTopMargin(0.02);
  gStyle->SetPadLeftMargin(0.25);
  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadBottomMargin(0.15);

  // Open run sumamry file
  std::ifstream ifs_runsum(RUNSUM_TXT.c_str());
  if(!ifs_runsum){
    std::cout<<RUNSUM_TXT<<" does not exist."<<std::endl;
    return 1;
  }

  // Make a list of runs from runsum file
  Int_t linac_run, e_mode, run_mode;
  std::string line, dummy;
  std::vector<Int_t> list_run;
  while (getline(ifs_runsum, line, '\n')) {
    // Read line
    std::stringstream ss(line);
    ss >> linac_run >> e_mode >> run_mode;
    if(e_mode==EMode && run_mode == 0){
      list_run.push_back(linac_run);
    }
  }
  ifs_runsum.close();
  Int_t nRun = list_run.size();

  // Create histograms
  TH1F* h_neff[N_DATA];
  TH1F* h_ratio[N_DATA];
  Int_t color_data[N_DATA] = {1, 2, 4};
  for (Int_t iData=0; iData<N_DATA; iData++) {
    h_neff[iData] = new TH1F(Form("h_neff%d", iData), ";Run number;Neff", nRun, 0, nRun);
    h_ratio[iData] = new TH1F(Form("h_ratio%d", iData), ";Run number;MC/Data", nRun, 0, nRun);
    h_neff[iData]->SetLineColor(color_data[iData]);
    h_ratio[iData]->SetLineColor(color_data[iData]);
  }

  // Arrays to store Neff data
  Float_t neff[N_DATA];
  Float_t neff_err[N_DATA];
  Float_t sigma;

  // Vector to store all values
  std::vector<Float_t> v_neff;
  std::vector<Float_t> v_ratio[N_DATA];

  // Loop for runs
  for (Int_t iRun=0; iRun<nRun; iRun++) {

    // Open text file
    Int_t run_this = list_run[iRun];
    TString fname = Form("%s/compare/txt/linac_run%06d.dat", LINAC_DIR.c_str(), run_this);
    ifstream ifs_data(fname.Data());
    if(!ifs_data){
      std::cout << fname << " does not exist." << std::endl;
      continue;
    }

    // Loop for data
    for (Int_t iData=0; iData<N_DATA; iData++) {

      // Read text data
      ifs_data >> neff[iData] >> neff_err[iData] >> sigma;

      // Fill histograms
      h_neff[iData]->SetBinContent(iRun+1, neff[iData]);
      h_neff[iData]->SetBinError(iRun+1, neff_err[iData]);
      h_neff[iData]->GetXaxis()->SetBinLabel(iRun+1, Form("%d", run_this));
      v_neff.push_back(neff[iData]);
      if (iData > 0) {
        Float_t ratio = neff[iData]/neff[0];
        Float_t ratio_err = ratio * sqrt( pow(neff_err[iData]/neff[iData], 2) + pow(neff_err[0]/neff[0], 2) );
        h_ratio[iData]->SetBinContent(iRun+1, ratio);
        h_ratio[iData]->SetBinError(iRun+1, ratio_err);
        h_ratio[iData]->GetXaxis()->SetBinLabel(iRun+1, Form("%d", run_this));
        v_ratio[iData].push_back(ratio);
      }

    } // End data loop

  } // End run loop

  // Draw
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->Divide(2,1);
  c1->cd(1);
  h_neff[0]->Draw("PE");
  Float_t neff_max = *max_element(v_neff.begin(), v_neff.end());
  Float_t neff_min = *max_element(v_neff.begin(), v_neff.end());
  h_neff[0]->GetYaxis()->SetRangeUser(neff_min*0.95, neff_max*1.03);
  for (Int_t iData=1; iData<N_DATA; iData++) {
    h_neff[iData]->Draw("PE same");
  }

  // Draw legend
  TString str_data[N_DATA] = {"DATA", "DETSIM", "SKG4"};
  TLegend *leg = new TLegend(0.7,0.7,0.85,0.85);
  for (Int_t iData=0; iData<N_DATA; iData++) {
    leg->AddEntry(h_neff[iData], str_data[iData], "l");
  }
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->Draw();

  c1->cd(2); 
  h_ratio[1]->Draw("PE");
  h_ratio[1]->SetMaximum(1.03);
  h_ratio[1]->SetMinimum(0.97);
  h_ratio[2]->Draw("PE same");

  TLatex* latex = new TLatex();
  latex->SetTextSize(0.1);
  latex->DrawTextNDC(0.6, 0.85, Form("%d MeV", EMode));
  latex->SetTextSize(0.05);

  // Open output file
  std::ofstream ofs("corepmt.dat", std::ios::app);
  ofs << EMode;

  // Calculate and draw mean and RMS
  for (Int_t iData=1; iData<N_DATA; iData++) {
    Float_t ratio_sum = std::accumulate(v_ratio[iData].begin(), v_ratio[iData].end(), 0.0);
    Float_t ratio_mean = ratio_sum / v_ratio[iData].size();

    Float_t ratio_sq_sum = std::inner_product(v_ratio[iData].begin(), v_ratio[iData].end(), v_ratio[iData].begin(), 0.0);
    Float_t ratio_rms = sqrt(ratio_sq_sum / v_ratio[iData].size() - ratio_mean * ratio_mean);  

    latex->SetTextColor(color_data[iData]);
    latex->DrawTextNDC(0.35, 0.35-0.05*iData, Form("AVE = %5.4lf  RMS = %5.4lf", ratio_mean, ratio_rms));
    printf("AVE (%s)\t\t:=  %5.4lf  RMS = %5.4lf\n", str_data[iData].Data(), ratio_mean, ratio_rms);
    ofs << ratio_mean << " " << ratio_rms;
  }

  ofs.close();

  // Draw lines
  TLine* lin = new TLine();
  lin->SetLineColor(1);
  lin->SetLineWidth(3);
  lin->DrawLine(0., 1.00, nRun, 1.00);
  lin->SetLineStyle(2);
  lin->SetLineWidth(2);
  lin->DrawLine(0., 1.01, nRun, 1.01);
  lin->DrawLine(0., 0.99, nRun, 0.99);

  // Output PDF
  c1->Update();
  c1->Print(Form("Neff_%dMeV.pdf", EMode));

  return 1;

}
