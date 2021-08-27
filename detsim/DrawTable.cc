void DrawTable() {
  ifstream ifs("energyAtEdge.dat");
  
  TH1F *h = new TH1F("h","h;cos#theta",100,0.99,1);
  //TH1F *h = new TH1F("h","h;Energy[MeV]",100,16.5,18.5);

  float a, b;

  ifs>>a>>b;
  while(!ifs.eof()) {
    //h->Fill(a);
    h->Fill(-b);
    ifs>>a>>b;
  }
  h->Draw();
}

