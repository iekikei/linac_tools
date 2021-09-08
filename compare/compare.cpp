#include <fstream>
#include <sstream>
#include <cmath>
#include <TFile.h>
#include <TH1F.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TF1.h>
#include <TLegend.h>
#include "SuperManager.h"

#define N_DATA 3
#define N_HIST 12

std::string const LINAC_DIR = getenv("LINAC_DIR"); 
std::string const RUNSUM_TXT = LINAC_DIR + "/runsum.dat";

void set_input_file (TreeManager* mgr, TString filename) {
  TString cmd = Form("ls %s", filename.Data());
  if (system(cmd.Data()) ){
    exit(1);
  };
  mgr->SetInputFile(filename.Data());
}

int main(int argc,char *argv[]){

  if (argc!=2) {
    std::cout << "Usage: ./linac <Run number>" << std::endl;
    return 1;
  }
  Int_t runnum = atoi(argv[1]);

  //************* Set styles for drawing **************//

  // Canvas Style  
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetFillColor(0);
  gStyle->SetOptStat(0);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadLeftMargin(0.22);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetEndErrorSize(2);

  // Font Style
  gStyle->SetTextFont(132);
  gStyle->SetLabelFont(132,"XYZ");
  gStyle->SetTitleFont(132,"XYZ");

  // Title and Label Style
  gStyle->SetTitleSize(0.08,"X");
  gStyle->SetTitleSize(0.08,"Y");
  gStyle->SetLabelSize(0.075,"Y");
  gStyle->SetLabelSize(0.075,"X");
  gStyle->SetTitleXOffset(0.9);
  gStyle->SetTitleYOffset(1.5);
  gStyle->SetNdivisions(505,"X");
  gStyle->SetNdivisions(510,"Y");
  gStyle->SetStripDecimals(false);

  //************* Get run summary information **************//

  // Open runsum file  
  std::ifstream IN;
  IN.open(RUNSUM_TXT.c_str());
  if(!IN){
    std::cout<<RUNSUM_TXT<<" does not exist."<<std::endl;
    return 1;
  }
	
  // Read runsum file
  Int_t linac_run, e_mode, dummy;
  Float_t beam_pos[3];
  std::string line;
  while (getline(IN, line, '\n')) {
    // Read line
    std::stringstream ss(line);
    ss >> linac_run >> e_mode >> dummy >> beam_pos[0] >> beam_pos[1] >> beam_pos[2] >> dummy;
    if(linac_run == runnum){
      std::cout<<linac_run<<std::endl;
      break;
    }
  }
  if(linac_run != runnum) {
    std::cerr<< "Run" << runnum << " was not found in runsum.dat."<<std::endl;
    return 1;
  }
  IN.close();

  //************* Fill histograms **************//

  // Make histograms
  TH1F *hist[N_DATA][N_HIST];
  for (Int_t iData=0; iData<3; iData++) {
    hist[iData][0] = new TH1F(Form("h%d_vertex_x", iData), ";Vertex x [cm]", 1000, -2500.0, +2500.0);
    hist[iData][1] = new TH1F(Form("h%d_vertex_y", iData), ";Vertex y [cm]", 1000, -2500.0, +2500.0);
    hist[iData][2] = new TH1F(Form("h%d_vertex_z", iData), ";Vertex z [cm]", 1000, -2500.0, +2500.0);
    hist[iData][3] = new TH1F(Form("h%d_bsenergy", iData), ";Energy [MeV]", 160, 0.0, 40.0);
    hist[iData][4] = new TH1F(Form("h%d_neff", iData), ";Neff hit", 150, 0.0, 300.0);
    hist[iData][5] = new TH1F(Form("h%d_angle", iData), ";Angle [deg]", 90, 0.0, 180.0);
    hist[iData][6] = new TH1F(Form("h%d_ovaq", iData), ";Ovaq", 50, 0.0, 1.0);
    hist[iData][7] = new TH1F(Form("h%d_bsgood", iData), ";BS goodness", 100, 0.4, 1.0);
    //hist[iData][7] = new TH1F("h_r", "Distance from Endcap [cm]", 100, 0.0, 500.0);
    hist[iData][8] = new TH1F(Form("h%d_patlik", iData), ";Patlik", 50, -2.5, 1.0);
    hist[iData][9] = new TH1F(Form("h%d_dir_x", iData), ";Dir X", 100, -1.0, 1.0);
    hist[iData][10] = new TH1F(Form("h%d_dir_y", iData), ";Dir Y", 100, -1.0, 1.0);
    hist[iData][11] = new TH1F(Form("h%d_dir_z", iData), ";Dir Z", 100, -1.0, 1.0);
  }
 
  // Loop for data
  for(Int_t iData=0; iData<N_DATA; iData++){

    // Make tree manager
    Int_t id = 10;
    SuperManager* Smgr = SuperManager::GetManager(); 
    Smgr->CreateTreeManager(id,"\0","\0",2);  // mode=2
    TreeManager* mgr = Smgr->GetTreeManager(id);

    // Set input files
    if (iData==0) {
      set_input_file(mgr, Form("%s/lowfit/fit_data/lin.%06d.root", getenv("LINAC_DIR"), runnum));
    } else if (iData==1) {
      for (Int_t iSubRun=0; iSubRun<10; iSubRun++) {
        set_input_file(mgr, Form("%s/lowfit/fit_detsim/lin.%06d.%03d.root", getenv("LINAC_DIR"), runnum, iSubRun));
      }
    } else if (iData==2) {
      for (Int_t iSubRun=0; iSubRun<10; iSubRun++) {
        set_input_file(mgr, Form("%s/lowfit/fit_skg4/lin.%06d.%03d.root", getenv("LINAC_DIR"), runnum, iSubRun));
      }
    }
  
    // Initialize
    mgr->Initialize();
  
    // Set branch status
    TTree* tree = mgr->GetTree();
    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("LOWE");
  
    // Get LOWE information
    LoweInfo  *LOWE    = mgr->GetLOWE();
  
    // Loop for entries
    Int_t nEntries = tree->GetEntries();  
    for(Int_t iEntry=0; iEntry<nEntries; iEntry++){

      // Get entry
      tree->GetEntry(iEntry);
    
      // Calculate parameters
      Float_t angle = acos(-1.0*LOWE->bsdir[2]);
      Float_t ovaq = LOWE->bsgood[1]*LOWE->bsgood[1]-LOWE->bsdirks*LOWE->bsdirks;
      // Float_t r_from_pipe = (LOWE->bsvertex[0]-x)*(LOWE->bsvertex[0]-x)+(LOWE->bsvertex[1]-y)*(LOWE->bsvertex[1]-y)+(LOWE->bsvertex[2]-z)*(LOWE->bsvertex[2]-z);
      // r_from_pipe = sqrt(r_from_pipe);

      // Fill histograms    
      hist[iData][0]->Fill(LOWE->bsvertex[0]);
      hist[iData][1]->Fill(LOWE->bsvertex[1]);
      hist[iData][2]->Fill(LOWE->bsvertex[2]);
      hist[iData][3]->Fill(LOWE->bsenergy);  
      hist[iData][4]->Fill(*reinterpret_cast <Float_t*> (&LOWE->linfo[7]));
      hist[iData][5]->Fill(angle*180.0/3.14); 
      hist[iData][6]->Fill(ovaq);
      hist[iData][7]->Fill(LOWE->bsgood[1]);
      //hist[iData][7]->Fill(r_from_pipe);
      hist[iData][8]->Fill(LOWE->bspatlik);
      hist[iData][9]->Fill(LOWE->bsdir[0]);
      hist[iData][10]->Fill(LOWE->bsdir[1]);
      hist[iData][11]->Fill(LOWE->bsdir[2]);
    
    } // End loop for entries
  
    // Close file
    Smgr->DeleteTreeManager(id);
  
  } // End loop for data

  SuperManager::DestroyManager();
  
  //************* Fit Neff histograms **************//

  // Fitting function
  TF1 *fgaus = new TF1("fgaus","gaus");

  // Output file
  std::ofstream OUT(Form("txt/linac_run%06d.dat", runnum));

  // Loop for data
  for(Int_t iData=0; iData<N_DATA; iData++){
    
    // Set parameters
    TH1F* h_neff = hist[iData][4];
    Double_t par[3] = {h_neff->GetMaximum(), h_neff->GetMean(), h_neff->GetRMS()};
    fgaus->SetParameters(par);

    // Fit Neff histograms
    for (Int_t iFit=0; iFit<2; iFit++) {
      h_neff->Fit(fgaus,"Q0", "", par[1] - par[2], par[1] + par[2]);
      fgaus->GetParameters(par);
    }

    // Output
    OUT << par[1] << " " << fgaus->GetParError(1) << " " << par[2] << " ";

  }

  OUT.close();

  //************* Draw histograms **************//

  // Scale MC histograms to Data  
  Float_t nentries_data = hist[0][0]->GetEntries();
  for (Int_t iData=1; iData<N_DATA; iData++) {
    Float_t nentries_mc = hist[iData][0]->GetEntries();
    for (Int_t iHist=0; iHist<N_HIST; iHist++) {
      hist[iData][iHist]->Sumw2();
      hist[iData][iHist]->Scale(nentries_data/nentries_mc);
    }
  }

  // Legends
  TLegend *leg = new TLegend(0.7,0.5,0.9,0.7);
  leg->AddEntry(hist[0][0],"DATA","l");
  leg->AddEntry(hist[1][0],"DETS","l");
  leg->AddEntry(hist[2][0],"SKG4","l");
  leg->SetBorderSize(0);
  leg->SetTextSize(0.1);
  TLatex latex;
  latex.SetTextSize(0.062);

  // Draw
  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  c1->Divide(3,4);
  for(Int_t iHist=0;iHist<N_HIST;iHist++){
    c1->cd(iHist+1);

    if( iHist < 5 ){
      Float_t x_min = hist[1][iHist]->GetMean() - 4.0*hist[1][iHist]->GetRMS();
      Float_t x_max = hist[1][iHist]->GetMean() + 4.0*hist[1][iHist]->GetRMS();
      hist[1][iHist]->GetXaxis()->SetRangeUser(x_min, x_max);
    }
    hist[1][iHist]->SetLineColor(2); 
    hist[2][iHist]->SetLineColor(4); 

    hist[1][iHist]->Draw("hist");
    hist[2][iHist]->Draw("same hist"); 
    hist[0][iHist]->Draw("same e");

    // Draw legends
    latex.DrawTextNDC(0.63, 0.86, Form("%d MeV", e_mode));
    latex.DrawTextNDC(0.68, 0.78, Form("X = %4.2fm", beam_pos[0]/100.));
    latex.DrawTextNDC(0.68, 0.72, Form("Z = %4.2fm", beam_pos[2]/100.));
    leg->Draw();

  }
  c1->Update();
  c1->Print(Form("pdf/linac_run%06d.pdf", runnum));

  return 1;
}
