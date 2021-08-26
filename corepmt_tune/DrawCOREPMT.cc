void DrawCOREPMT() {
  ifstream ifs("corepmt.dat");
  float ene[6]={0.};
  float ene_use[3][6]={0.};
  float zero[6]={0.};
  float ave[2][6]={0.};
  float err[2][6]={0.};
  float rav[6] = {0.};
  float rer[6] = {0.};
  float shift = 0.1;
  for (int i=0;i<6;i++) {
    ifs>>ene[i]>>ave[0][i]>>err[0][i]>>ave[1][i]>>err[1][i];
    
    rav[i] = ave[0][i]/ave[1][i];
    rer[i] = sqrt(pow(err[0][i]/ave[1][i],2) + pow(ave[0][i]*err[1][i]/(ave[1][i]*ave[1][i]),2));
  }

  for (int i=0;i<6;i++) {
    ene_use[0][i] = ene[i] - shift;
    ene_use[1][i] = ene[i] + shift;
    ene_use[2][i] = ene[i];
  }

  TGraphErrors *g[3];
  g[0] = new TGraphErrors(6,ene_use[0],ave[0],zero,err[0]);
  g[1] = new TGraphErrors(6,ene_use[1],ave[1],zero,err[1]);
  g[2] = new TGraphErrors(6,ene_use[2],rav,zero,rer);

  int color[3] = {418, 864,800};
  for (int i=0; i<3; i++) {
    g[i]->SetMarkerStyle(22);
    g[i]->SetLineWidth(2);
    g[i]->SetLineColor(color[i]);
    g[i]->SetMarkerColor(color[i]);
  }

  TCanvas *c = new TCanvas("c","c");
  c->SetGridy();
  TH1F* fr = c->DrawFrame(4,0.97,19,1.03);
  fr->SetTitle(";Target energy [MeV]; MC/DATA");
  TLegend *leg = new TLegend(0.75,0.75,0.9,0.9);
  leg->SetFillStyle(10);
  leg->AddEntry(g[0],"DETSIM","l");
  leg->AddEntry(g[1],"SKG4","l");
  leg->AddEntry(g[2],"DETSIM/SKG4","l");

  g[0]->Draw("p");
  g[1]->Draw("p");
  g[2]->Draw("p");
  leg->Draw();

  //c->SaveAs("eps/corepmt.eps","eps");
}
