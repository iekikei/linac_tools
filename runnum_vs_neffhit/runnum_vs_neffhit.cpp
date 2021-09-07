#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <TLatex.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TStyle.h>

#define N_DATA 3

std::string const LINAC_DIR = std::getenv("LINAC_DIR");
std::string const LINAC_TXT = LINAC_DIR + "/runsum.dat";

Int_t main(const int argc,char *argv[]){

  // Canvas Style
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetOptStat(0);
  gStyle->SetFillColor(0);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);

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

  // Open run sumamry file
  std::ifstream ifs_runsum(LINAC_TXT.c_str());
  if(!ifs_runsum){
    std::cout<<LINAC_TXT<<" does not exist."<<std::endl;
    return 1;
  }

  // Make a list of runs from runsum file
  Int_t linac_run, run_mode;
  std::string line, dummy;
  std::vector<Int_t> list_run;
  while (getline(ifs_runsum, line, '\n')) {
    // Read line
    std::stringstream ss(line);
    ss >> linac_run >> dummy >> run_mode;
    if(run_mode == 0){
      list_run.push_back(linac_run);
    }
  }
  ifs_runsum.close();

  // Make graphs
  TGraphErrors* gr_neff[N_DATA]; // Neff vs. run number
  TGraphErrors* gr_diff[N_DATA]; // Neff Data-MC diff vs. run number. First graph in the array will not be used.
  TMultiGraph* mgr_neff = new TMultiGraph();
  TMultiGraph* mgr_diff = new TMultiGraph();
  for (Int_t iData=0; iData<N_DATA; iData++) {
    gr_neff[iData] = new TGraphErrors();
    gr_diff[iData] = new TGraphErrors();
    mgr_neff->Add(gr_neff[iData]);
    if (iData > 0) {
      mgr_diff->Add(gr_diff[iData]);
    }
    gr_neff[iData]->SetMarkerStyle(20);
    gr_diff[iData]->SetMarkerStyle(20);
  }

  // Arrays to store data
  Float_t mean[N_DATA];
  Float_t mean_err[N_DATA];
  Float_t sigma;

  // Loop for runs
  for (UInt_t iRun=0; iRun<list_run.size(); iRun++) {

    // Open text file for this run
    Int_t run_this = list_run[iRun];
    TString filename = Form("%s/compare/txt/linac_run%06d.dat", LINAC_DIR.c_str(), run_this);
    std::ifstream ifs_data(filename.Data());
    if(!ifs_data){
      std::cout << filename << " does not exist." << std::endl;
      return 1;
    }

    // Read text file
    for (Int_t iData=0; iData<N_DATA; iData++) {

      // Read values
      ifs_data >> mean[iData] >> mean_err[iData] >> sigma;

      // Set points to graphs
      gr_neff[iData]->SetPoint(gr_neff[iData]->GetN(), run_this+iData, mean[iData]);
      gr_neff[iData]->SetPointError(gr_neff[iData]->GetN()-1, 0., mean_err[iData]);
      if (iData > 0 && mean[0] > 0.) {  // For MC
        Float_t diff = (mean[iData] - mean[0]) / mean[0] * 100.0;
        Float_t diff_err = diff * sqrt( pow(mean_err[iData]/mean[iData], 2) + pow(mean_err[0]/mean[0], 2) );
        gr_diff[iData]->SetPoint(gr_diff[iData]->GetN(), run_this+iData, diff);
        gr_diff[iData]->SetPointError(gr_diff[iData]->GetN()-1, 0., diff_err);
      }

    }
    ifs_data.close();

  }

  // Draw
  TCanvas *c1 = new TCanvas("c1","c1",700,500);
  c1->Divide(1,2);

  c1->cd(1);
  gr_neff[1]->SetMarkerColor(2);
  gr_neff[2]->SetMarkerColor(4);
  mgr_neff->SetTitle(";Runnumber;Neffhit");
  mgr_neff->Draw("APE");

  TLegend *leg = new TLegend(0.1,0.6,0.2,0.8);
  leg->AddEntry(gr_neff[0], "DATA", "lp");
  leg->AddEntry(gr_neff[1], "G3", "lp");
  leg->AddEntry(gr_neff[2], "G4", "lp");
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->Draw();

  c1->cd(2);
  gr_diff[1]->SetMarkerColor(2);
  gr_diff[2]->SetMarkerColor(4);
  mgr_diff->SetTitle(";Runnumber;Difference of Neffhit[%]");
  mgr_diff->Draw("APE");
  mgr_diff->GetYaxis()->SetRangeUser(-6.0,6.0);

  TLatex *latex = new TLatex();
  latex->SetTextSize(0.05);
  latex->DrawTextNDC(0.15, 0.93, "*Difference of Neffhit = (MC-DATA)/DATA [%]");

  c1->Update();
  c1->Print("runnum_vs_neffhit.pdf");

  return 1; 
}
