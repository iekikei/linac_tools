#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <TLatex.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TStyle.h>

#define N_DATA 3

std::string const LINAC_DIR = std::getenv("LINAC_DIR");
std::string const LINAC_TXT = LINAC_DIR + "/runsum.dat";

Int_t main(const int argc,char *argv[]){

  if (argc != 2) {
    printf("Usage: ./compare_all [Energy mode]\n");    
    return 1;
  }
  Int_t EMode = atoi(argv[1]);

  // Canvas Style
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetOptStat(0);
  gStyle->SetFillColor(0);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);

  //Font Style
  gStyle->SetTextFont(132);
  gStyle->SetLabelFont(132,"XYZ");
  gStyle->SetTitleFont(132,"XYZ");

  //Title and Label Style
  gStyle->SetTitleSize(0.09,"XY");
  gStyle->SetLabelSize(0.07,"XY");
  gStyle->SetTitleXOffset(0.5);
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetNdivisions(505,"X");
  gStyle->SetNdivisions(510,"Y");
  gStyle->SetStripDecimals(false);
  gStyle->SetLegendBorderSize(0);

  // Open run sumamry file
  std::ifstream ifs_runsum(LINAC_TXT.c_str());
  if(!ifs_runsum){
    std::cout<<LINAC_TXT<<" does not exist."<<std::endl;
    return 1;
  }

  // Make a list of runs from runsum file
  Int_t linac_run, run_mode, e_mode;
  std::string line, dummy;
  std::vector<Int_t> list_run;
  std::vector<Int_t> list_emode;
  Int_t nRun_sel = 0;
  while (getline(ifs_runsum, line, '\n')) {
    // Read line
    std::stringstream ss(line);
    ss >> linac_run >> e_mode >> run_mode;
    if(run_mode == 0){
      list_run.push_back(linac_run);
      list_emode.push_back(e_mode);
      if (e_mode == EMode) {
        nRun_sel++;
      }
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

  // Make histograms
  TH1F* h_neff[N_DATA];
  TH1F* h_ratio[N_DATA];
  Int_t color_data[N_DATA] = {1, 2, 4};
  for (Int_t iData=0; iData<N_DATA; iData++) {
    h_neff[iData] = new TH1F(Form("h_neff%d", iData), ";Run number;Neff", nRun_sel, 0, nRun_sel);
    h_ratio[iData] = new TH1F(Form("h_ratio%d", iData), ";Run number;MC/Data", nRun_sel, 0, nRun_sel);
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
      ifs_data >> neff[iData] >> neff_err[iData] >> sigma;

      // Set points to graphs
      gr_neff[iData]->SetPoint(gr_neff[iData]->GetN(), run_this+iData, neff[iData]);
      gr_neff[iData]->SetPointError(gr_neff[iData]->GetN()-1, 0., neff_err[iData]);
      if (iData > 0 && neff[0] > 0.) {  // For MC
        Float_t diff = (neff[iData] - neff[0]) / neff[0] * 100.0;
        Float_t diff_err = diff * sqrt( pow(neff_err[iData]/neff[iData], 2) + pow(neff_err[0]/neff[0], 2) );
        gr_diff[iData]->SetPoint(gr_diff[iData]->GetN(), run_this+iData, diff);
        gr_diff[iData]->SetPointError(gr_diff[iData]->GetN()-1, 0., diff_err);
      }

      // Fill histograms for selected runs
      if (list_emode[iRun] == EMode) {
        Int_t bin_this = h_neff[iData]->GetEntries();
        h_neff[iData]->SetBinContent(bin_this+1, neff[iData]);
        h_neff[iData]->SetBinError(bin_this+1, neff_err[iData]);
        h_neff[iData]->GetXaxis()->SetBinLabel(bin_this+1, Form("%d", run_this));
        v_neff.push_back(neff[iData]);
        if (iData > 0) {
          Float_t ratio = neff[iData]/neff[0];
          Float_t ratio_err = ratio * sqrt( pow(neff_err[iData]/neff[iData], 2) + pow(neff_err[0]/neff[0], 2) );
          h_ratio[iData]->SetBinContent(bin_this+1, ratio);
          h_ratio[iData]->SetBinError(bin_this+1, ratio_err);
          h_ratio[iData]->GetXaxis()->SetBinLabel(bin_this+1, Form("%d", run_this));
          v_ratio[iData].push_back(ratio);
        }
      }

    }
    ifs_data.close();

  }

  // Draw
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadBottomMargin(0.15);
  TCanvas *c1 = new TCanvas("c1","c1",700,500);
  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  c1->Divide(1,2);
  gStyle->SetPadLeftMargin(0.18);
  c2->Divide(2,1);

  c1->cd(1);
  gr_neff[1]->SetMarkerColor(2);
  gr_neff[2]->SetMarkerColor(4);
  mgr_neff->SetTitle(";Runnumber;Neffhit");
  mgr_neff->Draw("APE");

  TLegend *leg1 = new TLegend(0.1,0.6,0.2,0.8);
  TString str_data[N_DATA] = {"DATA", "DETSIM", "SKG4"};
  for (Int_t iData=0; iData<N_DATA; iData++) {
    leg1->AddEntry(gr_neff[iData], str_data[iData], "lp");
  }
  leg1->Draw();

  c1->cd(2);
  gr_diff[1]->SetMarkerColor(2);
  gr_diff[2]->SetMarkerColor(4);
  mgr_diff->SetTitle(";Runnumber;Difference of Neffhit[%]");
  mgr_diff->Draw("APE");
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleYOffset(0.4);
  mgr_diff->GetYaxis()->SetRangeUser(-6.0,6.0);

  TLatex *latex = new TLatex();
  latex->SetTextSize(0.05);
  latex->DrawTextNDC(0.15, 0.93, "*Difference of Neffhit = (MC-DATA)/DATA [%]");

  c1->Update();
  c1->Print("compare_all.pdf(");

  c2->cd(1);
  h_neff[0]->Draw("PE");
  Float_t neff_max = *max_element(v_neff.begin(), v_neff.end());
  Float_t neff_min = *max_element(v_neff.begin(), v_neff.end());
  h_neff[0]->GetYaxis()->SetRangeUser(neff_min*0.95, neff_max*1.03);
  for (Int_t iData=1; iData<N_DATA; iData++) {
    h_neff[iData]->Draw("PE same");
  }
  TLegend *leg2 = new TLegend(0.65,0.7,0.95,0.85);
  for (Int_t iData=0; iData<N_DATA; iData++) {
    leg2->AddEntry(h_neff[iData], str_data[iData], "l");
  }
  leg2->Draw();

  c2->cd(2); 
  h_ratio[1]->Draw("PE");
  h_ratio[1]->SetMaximum(1.03);
  h_ratio[1]->SetMinimum(0.97);
  h_ratio[2]->Draw("PE same");

  latex->SetTextSize(0.1);
  latex->DrawTextNDC(0.6, 0.8, Form("%d MeV", EMode));
  latex->SetTextSize(0.05);

  // Open output file
  std::ofstream ofs("corepmt.dat", std::ios::app);
  ofs << EMode;

  // Calculate and draw mean and RMS
  for (Int_t iData=1; iData<N_DATA; iData++) {

    // Calculate mean and RMS
    Float_t ratio_sum = std::accumulate(v_ratio[iData].begin(), v_ratio[iData].end(), 0.0);
    Float_t ratio_mean = ratio_sum / v_ratio[iData].size();
    Float_t ratio_sq_sum = std::inner_product(v_ratio[iData].begin(), v_ratio[iData].end(), v_ratio[iData].begin(), 0.0);
    Float_t ratio_rms = sqrt(ratio_sq_sum / v_ratio[iData].size() - ratio_mean * ratio_mean);  

    // Draw
    latex->SetTextColor(color_data[iData]);
    latex->DrawTextNDC(0.35, 0.35-0.05*iData, Form("AVE = %5.4lf  RMS = %5.4lf", ratio_mean, ratio_rms));

    // Write
    printf("AVE (%s)\t\t:=  %5.4lf  RMS = %5.4lf\n", str_data[iData].Data(), ratio_mean, ratio_rms);
    ofs << ratio_mean << " " << ratio_rms;
  }

  ofs.close();

  // Draw lines
  TLine* lin = new TLine();
  lin->SetLineColor(1);
  lin->SetLineWidth(3);
  lin->DrawLine(0., 1.00, nRun_sel, 1.00);
  lin->SetLineStyle(2);
  lin->SetLineWidth(2);
  lin->DrawLine(0., 1.01, nRun_sel, 1.01);
  lin->DrawLine(0., 0.99, nRun_sel, 0.99);

  // Output PDF
  c2->Update();
  c2->Print("compare_all.pdf)");

  c2->SaveAs("test.root");

  return 1; 
}
