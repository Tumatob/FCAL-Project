void PrintEntries(const char* filename) {

    //Open the rootfile
    TFile *f = TFile::Open(filename);
    
    //Get the TTree namd FCALdigi
    TTree *t = (TTree*)f->Get("FCALdigi");
    
    //Get total number of entries in the tree
    long entries = t->GetEntries();

    //Declare varaibles to hold branch data
    UInt_t channelNum, eventNum, rocid, slot, channel, itrigger, peak_raw, integral, pedestal, time, nsamples, QF, row, column, fptrigger;
    float peak;

    //Set Branch Address
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
    t->SetBranchAddress("fptrigger", &fptrigger);

    //Create Histogram for visualizing different quantities
    TH2F* h_integral = new TH2F("h_integral", "Row vs Column vs Integral Map", 62, -2, 60, 62, -2, 60);
    TH2F* h_peak = new TH2F("h_peak", "Row vs Column vs Peak Map", 62, -2, 60, 62, -2, 60);
    TH2F* h_time = new TH2F("h_time", "Row vs Column vs Time Map", 62, -2, 60, 62, -2, 60);
    TH1F* bottom_integral = new TH1F("bottom_integral", "Integral value at points", 100, 0, 50000);
    TH1F* bottom_time = new TH1F("bottom_time", "Time Values at points", 100, 0, 4000);
    TH1F* bottom_peak = new TH1F("bottom_peak", "Peak Values at points", 100, 0, 5000);
    TH2* h2 = new TH2F("h2", "Row vs Column", 62, -2, 60, 62, -2, 60);
    TH2F* oneEntry = new TH2F("oneEntry", "map of one Entry", 62, -2, 60, 62, -2, 60);
    std::vector<int> peak_raw_val;
    std::vector<int> eventNo;
    std::vector<int> full_peak(2520, 0);
    /* std::vector<int> LED_Values = {2, 5 ,8, 11, 14, 17, 20, 23, 26, 29}; */
    std::vector<int> LED_Values = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
    std::vector<int> HV_Values = {1400, 1450 ,1500 ,1550, 1600, 1650, 1700, 1750, 1800};
    TH2F* hvledComparison = new TH2F("hvledComparison", "Avg Peak: LED vs HV", 28, 1.5, 29.5, 9, 1375, 1825);
    
    
    int fixedRow = 10;
    int colMin   = 0;
    int colMax   = 10;
    int nCols    = colMax - colMin + 1; 
    std::vector<std::vector<int>> full_peak_col(nCols, std::vector<int>(2520, 0));
    std::vector<TH2F*>    hHVLED(nCols);
    std::vector<TCanvas*> cHVLED(nCols);


    for (int col = colMin; col <= colMax; col++) { 
    	int idx = col - colMin; 

    	hHVLED[idx] = new TH2F(Form("hHVLED_col%d", col),
    	Form("Avg Peak: LED vs HV (Row %d, Col %d);LED Value;HV Value", fixedRow, col), 28, 1.5, 29.5, 9,1375,1825);
    }


    for (long i = 0; i < entries; ++i) {
        t->GetEntry(i);
        
        /* if (eventNum == 2000) {
    	  oneEntry->Fill(row, column, integral);
    	} */

	//Takes data from one block in the calorimeter
        if (row == fixedRow && column <= colMax) {
    	
    		peak_raw_val.push_back(peak_raw);
    		eventNo.push_back(fptrigger);
    		int idx = column - colMin;
    		full_peak_col[idx][fptrigger] = peak_raw;
    		/* std::cout << "fptrigger: " << fptrigger << " Peak Raw: " << peak_raw << std::endl; */
    	  
          	/* bottom_integral->Fill(integral);
          	bottom_time->Fill(time);
          	bottom_peak->Fill(peak_raw); */
        }
        //Print all values
        /* std::cout << peak << std::endl; */

    }
    


    //Initialie an integer called entry_location
    int entry_location = 0;
    
    for (int hv : HV_Values) {
        for (int led : LED_Values) {
            std::vector<int> values;
            int sum = 0;
            int count_nonzero = 0;

            for (int i = 0; i < 10; ++i) {
            
                int val = full_peak[entry_location];
                values.push_back(val);
                if (val != 0) { 
                	sum += val; ++count_nonzero; 
                }
                ++entry_location;
            }

            int avg_int;
            if (count_nonzero > 0) {
            	avg_int = sum / count_nonzero;
            } else {
            	avg_int = 0;
            }
            /* *std::cout << "entry_location: " << entry_location << " fptrigger: " << entry_location - 1 << " HV: " << hv << ", LED: " << led << " -> Values: "; 
            for (int val : values) std::cout << val << " ";
            	std::cout << "-> Avg: " << avg_int << std::endl; */
            hvledComparison->Fill(led, hv, avg_int);
        }
    }
    
    //Fill histogram with data on all entries in the calorimeter
    /* for (long i = 0; i < entries; i++) {
    	t->GetEntry(i);
    	h2->Fill(row, column);
    	int bin_peak = h_peak->FindBin(row, column);
        int bin_time = h_time->FindBin(row, column);
        int bin_integral = h_integral->FindBin(row, column);
        h_peak->Fill(bin_peak, peak);
        h_time->SetBinContent(bin_time, time);
        h_integral->SetBinContent(bin_integral, integral);
    } */
    
    
    
    
    int TriggersPerPoint = 10;

    
    for (int col = colMin; col <= colMax; ++col) {
    int colIdx   = col - colMin;
    auto& colBuf = full_peak_col[colIdx];


    std::size_t pos = 0;

    for (int hv : HV_Values) {
        for (int led : LED_Values) {
            int sum = 0;
            int good = 0;

            for (int trg = 0; trg < TriggersPerPoint; ++trg, ++pos)
            {
                int val = colBuf[pos];
                if (val != 0) { sum += val; ++good; }
            }

            int avg_int;
            if (good > 0)
            {
    		avg_int = sum / good;
    	    }
    	    else
    	    {
    	    	avg_int = 0;
    	    }
    	    
            hHVLED[colIdx]->Fill(led, hv, avg_int);
        }
    }
    }
    
    //Creates a 2D histogram to display calorimeter
    /* TCanvas* c1 = new TCanvas("c1", "Row vs Column", 1000, 1000);
    h2->GetXaxis()->SetTitle("Row");
    h2->GetYaxis()->SetTitle("Column");
    h2->Draw("COLZ");
    
    //Display histogram that fills with peak
    TCanvas* c1 = new TCanvas("c1", "Peak", 1000, 1000);
    h_peak->GetXaxis()->SetTitle("Row");
    h_peak->GetYaxis()->SetTitle("Column");
    h_peak->Draw("COLZ");

    //Display 2D histogram that fills with Time
    TCanvas* c2 = new TCanvas("c2", "Time", 1000, 1000);
    h_time->GetXaxis()->SetTitle("Row");
    h_time->GetYaxis()->SetTitle("Column");
    h_time->Draw("COLZ");

    //Displays 2D histogram that fills with Integral
    TCanvas* c3 = new TCanvas("c3", "Integral", 1000, 1000);
    h_integral->GetXaxis()->SetTitle("Row");
    h_integral->GetYaxis()->SetTitle("Column");
    h_integral->Draw("COLZ");

    //DIsplays a 1D histogram at a specific integral value
    TCanvas* c4 = new TCanvas("c4", "A specific integral", 800, 600);
    bottom_integral->GetXaxis()->SetTitle("Integrals");
    bottom_integral->GetYaxis()->SetTitle("Counts");
    bottom_integral->Draw();

    DIsplays a 1D histogram at a specific time value
    TCanvas* c5 = new TCanvas("c5", "A specific time", 800, 600);
    bottom_time->GetXaxis()->SetTitle("Time");
    bottom_time->GetYaxis()->SetTitle("Counts");
    bottom_time->Draw();
    
    //DIsplays a 1D histogram at a specific peak value
    TCanvas* c6 = new TCanvas("c6", "A specific peak", 800, 600);
    bottom_peak->GetXaxis()->SetTitle("Peak Values");
    bottom_peak->GetYaxis()->SetTitle("Counts");
    bottom_peak->Draw("COLZ"); */

    //Creates a plot with peak_raw_val and loops over the event numbers
    TGraph* graph = new TGraph(eventNo.size(), &eventNo[0], &peak_raw_val[0]);
    graph->Draw("AP");
    
    //Creates 1D histogram graph of values at one entry
    /* TCanvas* c7 = new TCanvas("c7", "Map of One Entry", 800, 800);
    oneEntry->GetXaxis()->SetTitle("Row");
    oneEntry->GetYaxis()->SetTitle("Column");
    oneEntry->Draw("COLZ"); */

    TCanvas* c8 = new TCanvas("c8", "Peak_raw per LED and HV", 800, 800);
    hvledComparison->GetXaxis()->SetTitle("LED Value");
    hvledComparison->GetYaxis()->SetTitle("HV Value");
    hvledComparison->Draw("COLZ");


    for (int col = colMin; col <= colMax; ++col) {
        int idx = col - colMin;
        
        cHVLED[idx] = new TCanvas(Form("cHVLED_col%d", col), Form("Peak_raw per LED and HV – Row %d, Col %d", fixedRow, col), 800, 800);
        hHVLED[idx]->Draw("COLZ");
    }
    
    
    //Print the total entries in the data
    std::cout << "Total Entries: " << entries << std::endl;
    /* std::cout << "Total Entries in peak_raw: " << peak_raw_val.size() << std::endl; */
}

