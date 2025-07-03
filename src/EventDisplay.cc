void EventDisplay() {
    // Open file and get trees
    TFile *f = new TFile("../build/Mu.root");
    if (!f || f->IsZombie()) {
        std::cerr << "Failed to open OutPut.root" << std::endl;
        return;
    }

    // Get data tree and config tree
    TTree *t = (TTree*)f->Get("PixelEDep");
    //TTree *config = (TTree*)f->Get("config");
    //if (!t || !config) {
    //    std::cerr << "Missing ntuple or config tree!" << std::endl;
    //    return;
    //}

    // Read detector configuration
    int sizeY= 140;
    int sizeZ= 60;
    double pixelSizeY=0.4;
    double pixelSizeZ=0.4;
    //config->SetBranchAddress("sizeY", &sizeY);
    //config->SetBranchAddress("sizeZ", &sizeZ);
    //config->SetBranchAddress("pixelSizeY", &pixelSizeY);
    //config->SetBranchAddress("pixelSizeZ", &pixelSizeZ);
    //config->GetEntry(0); // read the only entry

    // Compute histogram bounds and binning
    double yMin = -sizeY / 2.0;
    double yMax =  sizeY / 2.0;
    double zMin = -sizeZ / 2.0;
    double zMax =  sizeZ / 2.0;
    int yBins = sizeY / pixelSizeY;
    int zBins = sizeZ / pixelSizeZ;

    
    // Create canvas with 2x2 pads
    TCanvas *c = new TCanvas("c", "Event Display per Module", 1000, 1000);
    c->Divide(2, 2);

    
    //for (int module = 0; module < 4; ++module) {
    c->cd(1);
        
    TString hname = Form("h%d", 3);
    TString htitle = Form("Module %d;Z [cm];Y [cm];Edep [MeV]", 3);
    TH2D *h = new TH2D(hname, htitle,
                       zBins, zMin, zMax,
                       yBins, yMin, yMax);

    TString drawCmd = Form("y:z>>%s", hname.Data());
    TString cut = Form("VolName==%d", 3);

    t->Draw(drawCmd, "EnergyDep*(" + cut + ")", "COLZ");

    gPad->SetLogz();
    //}

    c->cd(1);
        
    TString hname = Form("h%d", 3);
    TString htitle = Form("Module %d;Z [cm];Y [cm];Edep [MeV]", 3);
    TH2D *h = new TH2D(hname, htitle,
                       zBins, zMin, zMax,
                       yBins, yMin, yMax);

    TString drawCmd = Form("y:z>>%s", hname.Data());
    TString cut = Form("VolName==%d", 3);

    t->Draw(drawCmd, "EnergyDep*(" + cut + ")", "COLZ");

    gPad->SetLogz();
    c->cd(2);
        
    TString hname = Form("h%d", 1);
    TString htitle = Form("Module %d;Z [cm];Y [cm];Edep [MeV]", 1);
    TH2D *h = new TH2D(hname, htitle,
                       zBins, zMin, zMax,
                       yBins, yMin, yMax);

    TString drawCmd = Form("y:z>>%s", hname.Data());
    TString cut = Form("VolName==%d", 1);

    t->Draw(drawCmd, "EnergyDep*(" + cut + ")", "COLZ");

    gPad->SetLogz();
    c->cd(3);
        
    TString hname = Form("h%d", 2);
    TString htitle = Form("Module %d;Z [cm];Y [cm];Edep [MeV]", 2);
    TH2D *h = new TH2D(hname, htitle,
                       zBins, zMin, zMax,
                       yBins, yMin, yMax);

    TString drawCmd = Form("y:z>>%s", hname.Data());
    TString cut = Form("VolName==%d", 2);

    t->Draw(drawCmd, "EnergyDep*(" + cut + ")", "COLZ");

    gPad->SetLogz();
    c->cd(4);
        
    TString hname = Form("h%d", 0);
    TString htitle = Form("Module %d;Z [cm];Y [cm];Edep [MeV]", 0);
    TH2D *h = new TH2D(hname, htitle,
                       zBins, zMin, zMax,
                       yBins, yMin, yMax);

    TString drawCmd = Form("y:z>>%s", hname.Data());
    TString cut = Form("VolName==%d", 0);

    t->Draw(drawCmd, "EnergyDep*(" + cut + ")", "COLZ");

    gPad->SetLogz();
    c->Update();

}