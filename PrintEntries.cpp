void PrintEntries(const char* filename) {
    TFile *f = TFile::Open(filename);
    TTree *t = (TTree*)f->Get("FCALdigi");
    
    long entries = t->GetEntries();
    
    UInt_t channelNum, eventNum, rocid, slot, channel, itrigger, peak_raw, integral, pedestal, time, nsamples, QF, row, column;
    float peak;
    
    t->SetBranchAddress("channelnum", &channelNum);
    t->SetBranchAddress("eventnum", &eventNum);
    t->SetBranchAddress("rocid", &rocid);
    t->SetBranchAddress("slot", &slot);
    t->SetBranchAddress("channel", &channel);
    t->SetBranchAddress("itrigger", &itrigger);
    t->SetBranchAddress("peak_raw", &peak_raw);
    t->SetBranchAddress("integral", &integral);
    t->SetBranchAddress("pedestal", &pedestal);
    t->SetBranchAddress("time", &time);
    t->SetBranchAddress("QF", &QF);
    t->SetBranchAddress("row", &row);
    t->SetBranchAddress("column", &column);
    t->SetBranchAddress("peak", &peak);
    
    TH2F* h_integral = new TH2F("hpeak", "Row vs Column vs Integral Map", 62, -2, 60, 62, -2, 60);
    TH2F* h_peak = new TH2F("hpeak", "Row vs Column vs Peak Map", 62, -2, 60, 62, -2, 60);
    TH2F* h_time = new TH2F("h_time", "Row vs Column vs Time Map", 62, -2, 60, 62, -2, 60); 
    TH1F* bottom_integral = new TH1F("bottom_integral", "Integral value at points", 100, 0, 50000);
    TH1F* bottom_time = new TH1F("bottom_time", "Time Values at points", 100, 0, 4000);
    TH1F* bottom_peak = new TH1F("bottom_peak", "Peak Values at points", 100, 0, 5000);
    TH2* h2 = new TH2F("h2", "Row vs Column", 62, -2, 60, 62, -2, 60);
    TH2F* oneEntry = new TH2F("oneEntry", "map of one Entry", 62, -2, 60, 62, -2, 60);
    
    std::vector<int> peak_raw_val;
    std::vector<int> eventNo;

    for (long i = 0; i < entries; i++) {
    	t->GetEntry(i);
    	if (eventNum == 1000) {
    	  oneEntry->Fill(row, column, integral);
    	}
    	if (row == 1 && column == 30) {
    	   peak_raw_val.push_back(peak_raw);
          eventNo.push_back(eventNum);
          bottom_integral->Fill(integral);
          bottom_time->Fill(time);
          bottom_peak->Fill(peak_raw); 
        }
        /* std::cout << peak << std::endl; */
    } 
  
        for (long i = 0; i < entries; i++) {
    	t->GetEntry(i);
    	h2->Fill(row, column);
    	int bin_peak = h_peak->FindBin(row, column);
        int bin_time = h_time->FindBin(row, column);
        int bin_integral = h_integral->FindBin(row, column);
        h_peak->SetBinContent(bin_peak, peak);
        h_time->SetBinContent(bin_time, time);
        h_integral->SetBinContent(bin_integral, integral);
    }

    /* TCanvas* c1 = new TCanvas("c1", "Row vs Column", 1000, 1000);
    h2->GetXaxis()->SetTitle("Row");
    h2->GetYaxis()->SetTitle("Column");
    h2->Draw("COLZ");
    
    TCanvas* c1 = new TCanvas("c1", "Peak", 1000, 1000);
    h_peak->GetXaxis()->SetTitle("Row");
    h_peak->GetYaxis()->SetTitle("Column");
    h_peak->Draw("COLZ");

    TCanvas* c2 = new TCanvas("c2", "Time", 1000, 1000);
    h_time->GetXaxis()->SetTitle("Row");
    h_time->GetYaxis()->SetTitle("Column");
    h_time->Draw("COLZ");

    TCanvas* c3 = new TCanvas("c3", "Integral", 1000, 1000);
    h_integral->GetXaxis()->SetTitle("Row");
    h_integral->GetYaxis()->SetTitle("Column");
    h_integral->Draw("COLZ"); */
    
    /* TCanvas* c4 = new TCanvas("c4", "A specific integral", 800, 600);
    bottom_integral->GetXaxis()->SetTitle("Integrals");
    bottom_integral->GetYaxis()->SetTitle("Counts");
    bottom_integral->Draw();
    
    TCanvas* c5 = new TCanvas("c5", "A specific time", 800, 600);
    bottom_time->GetXaxis()->SetTitle("Time");
    bottom_time->GetYaxis()->SetTitle("Counts");
    bottom_time->Draw();
    
    	TCanvas* c6 = new TCanvas("c6", "A specific peak", 800, 600);
    bottom_peak->GetXaxis()->SetTitle("Peak Values");
    bottom_peak->GetYaxis()->SetTitle("Counts");
    bottom_peak->Draw("COLZ"); */
    
    TGraph* graph = new TGraph(eventNo.size(), &eventNo[0], &peak_raw_val[0]);
    graph->Draw("COLZ");
    
    TCanvas* c7 = new TCanvas("c", "Map of One Entry", 800, 800);
oneEntry->GetXaxis()->SetTitle("Row");
oneEntry->GetYaxis()->SetTitle("Column");
oneEntry->Draw("COLZ");

    std::cout << "Total Entries: " << entries << std::endl;
}
