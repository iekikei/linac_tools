Double_t dsthr(Double_t x, Double_t a){
  //      real th_paramsk2,th_paramsk3
  //      parameter (th_paramsk2=1.74909)
  //      parameter (th_paramsk3=1.56925)   
 
  Double_t pc2pe = 2.243;
  Double_t pc = x*pc2pe;
  return 0.5 * ( TMath::Erf(17.09*(a*pc-0.649))+1 );
}
    

void plot_spe_func(){

  string filename="spe_func_sk2.txt";
  //  string filename="spe_func_sk3.txt";
  Double_t th_paramsk2 = 1.74909;
  Double_t th_paramsk3 = 1.56925;
  //  Double_t th_par = th_paramsk3;
  Double_t th_par = th_paramsk2;
  
  Double_t p[5000];
  Double_t dp[5000];

  Double_t dp_thr[5000];
  Double_t dp_thr_p10[5000];
  Double_t dp_thr_m10[5000];
  
  Double_t dp_thr_int =0;
  Double_t dp_thr_int_p10 =0;
  Double_t dp_thr_int_m10 =0;

  Double_t x[5000];
  Double_t x_p10[5000];
  Double_t x_m10[5000];
  
  ifstream data_file(filename.c_str());
  for (Int_t i = 0; i < 5000; i++){
    data_file >> p[i];
    //    cout << p[i] << endl;
    x[i] = ((Double_t)i+0.5)/100.;
    x_p10[i] = 1.1*((Double_t)i+0.5)/100.;
    x_m10[i] = 0.9*((Double_t)i+0.5)/100.;
    
    if (i > 0){
      dp[i] = p[i] - p[i-1];
    }else{
      dp[i] = p[i];
    }
    dp_thr[i] = dsthr(x[i],th_par)*dp[i];
    dp_thr_p10[i] = dsthr(x_p10[i],th_par)*dp[i]/1.1;
    dp_thr_m10[i] = dsthr(x_m10[i],th_par)*dp[i]/0.9;

    dp_thr_int += dp_thr[i];
    dp_thr_int_p10 += dp_thr_p10[i]*1.1;
    dp_thr_int_m10 += dp_thr_m10[i]*0.9;
  }

  TGraph * g = new TGraph(5000,x,p);
  TGraph * gd = new TGraph(5000,x,dp);
  
  TGraph * gd_thr = new TGraph(5000,x,dp_thr);
  TGraph * gd_thr_p10 = new TGraph(5000,x_p10,dp_thr_p10);
  TGraph * gd_thr_m10 = new TGraph(5000,x_m10,dp_thr_m10);

  
  TCanvas * c1 = new TCanvas("c1","c1");

  g->Draw("al");

  TCanvas * c2 = new TCanvas("c2","c2");

  gd->Draw("al");
  gd_thr->SetLineColor(2);
  gd_thr->Draw("l");

  

  TCanvas * c3 = new TCanvas("c3","c3");
  TH1F *f = c3->DrawFrame(0,1.e-8,50,0.01);
  c3->SetLogy(1);


  gd_thr->Draw("l");
  gd_thr_p10->SetLineColor(4);
  gd_thr_m10->SetLineColor(8);
  gd_thr_p10->Draw("l");
  gd_thr_m10->Draw("l");


  cout << "Integral (nominal) = " << dp_thr_int << endl;
  cout << "Integral (+10%) = " << dp_thr_int_p10 << " " << (dp_thr_int_p10 / dp_thr_int - 1.0 )*100.0  << "%" << endl;
  cout << "Integral (-10%) = " << dp_thr_int_m10 << " " << (dp_thr_int_m10 / dp_thr_int - 1.0) * 100.0 << "%" << endl;

  
}
