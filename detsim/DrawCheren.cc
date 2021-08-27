void DrawCheren() {
  ifstream ifs("/disk02/lowe8/mharada/linac/sk5/detsim/out_data/081588_gen.dat");
  TH1F *h = new TH1F("h","h",100,2000,5000);

  float n, tmp;
  ifs>>n>>tmp>>tmp>>tmp;
  while(!ifs.eof()) {
    h->Fill(n);
    ifs>>n>>tmp>>tmp>>tmp;
    cout<<n<<"  "<<tmp<<endl;
  }
  h->Draw();
}
