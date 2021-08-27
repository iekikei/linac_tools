#include <vector>

void CheckRIndex() {


  ifstream ifs("trunk/dat_file/MaterialProperty/Latest/WaterProperty.dat");
  if(!ifs) return;
  vector<double> WaveL;
  vector<double> RIndex;
  string str;

  double a, b;
  int index = 0;
  getline(ifs, str);
  cout<<str<<endl;
  ifs>>a>>b>>str>>str>>str>>str>>str>>str;
  while (!ifs.eof()) {
    index++;
    WaveL.push_back(1239.84/a);
    RIndex.push_back(b);
    std::cout<<a<<"  "<<b<<std::endl;
    ifs>>a>>b>>str>>str>>str>>str>>str>>str;
  }

  double *arWaveL = &WaveL[0];
  double *arRIndex = &RIndex[0];
  TGraph *g = new TGraph(index, arWaveL, arRIndex);
  g->SetTitle(";Wave length [nm];Refractive index");
  g->Draw("al");


  ifstream ifs2("trunk/dat_file/MaterialProperty/20181010/Dummy/WaterProperty_constRindex.dat");
  vector<double> WaveL2;
  vector<double> RIndex2;

  int index2 = 0;
  getline(ifs2, str);
  cout<<str<<endl;
  ifs2>>a>>b;//>>str>>str>>str>>str>>str>>str;
  while (!ifs2.eof()) {
    index2++;
    WaveL2.push_back(1239.84/a);
    RIndex2.push_back(b);
    std::cout<<a<<"  "<<b<<std::endl;
    ifs2>>a>>b;//>>str>>str>>str>>str>>str>>str;
  }

  double *arWaveL2 = &WaveL2[0];
  double *arRIndex2 = &RIndex2[0];
  TGraph *g2 = new TGraph(index2, arWaveL2, arRIndex2);
  g2->SetLineColor(2);
  g2->Draw("l");
}
