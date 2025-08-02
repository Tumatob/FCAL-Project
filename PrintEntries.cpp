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
    //Removes legend from all histograms
    gStyle->SetOptStat(0);
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
    std::vector<int> LED_Values = {3, 6 , 9, 12, 15, 18, 21, 24, 27}; 
    //std::vector<int> LED_Values = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
    std::vector<int> HV_Values = {1400, 1450 ,1500 ,1550, 1600, 1650, 1700, 1750, 1800};
    TH2F* hvledComparison = new TH2F("hvledComparison", "Avg Peak: LED vs HV", 10, 1.5, 28.5, 9, 1375, 1825);
    
    TH2F* h_avg_per_cell = new TH2F("h_avg_per_cell", "Average Peak per Cell;Column;Row", 60, 0, 60, 59, 0, 59);
    std::vector<std::vector<std::vector<int>>> peak_map(59, std::vector<std::vector<int>>(60, std::vector<int>(2520, 0)));
    std::vector<std::vector<TH2F*>> hHVLED_all(59, std::vector<TH2F*>(60, nullptr));
    
    //Assign the row and specific column being looked at
    int fixedRow = 10;
    //The columns that need to be looked at
    int colMin = 0;
    int colMax = 29;
    int colMin2 = 30;
    int colMax2 = 58;
    //Number of columns being analyzed
    int nCols2 = colMax2 - colMin2 + 1;
    int nCols = colMax - colMin + 1;
    //Define two standard vecotrs
    std::vector<std::vector<int>> full_peak_col(nCols, std::vector<int>(2520, 0));
    std::vector<std::vector<int>> full_peak_col2(nCols2, std::vector<int>(2520, 0));
    //Assign a pointer
    std::vector<TH2F*>    hHVLED(nCols);
    std::vector<TCanvas*> cHVLED(nCols);
    std::vector<TH2F*>    hHVLED2(nCols2);
    std::vector<TCanvas*> cHVLED2(nCols2);
    std::vector<float> fptrigger_vals;
    std::vector<float> peak_vals;
    
    std::vector<float> fptrigger_vals_new;
    std::vector<float> peak_vals_new;
    
    std::vector<int> peak_vals_single(2520, 0);
    TH2F* h_singleDetector = new TH2F("h_singleDetector", "Avg Peak: LED vs HV (Row 10, Col 22);LED Value;HV Value", 9, 1.5, 28.5, 9, 1375, 1825);
    for (long i = 0; i < entries; i++) {
    	t->GetEntry(i);
    	if (row == 10 && column == 22) {

        	int new_fptrigger = fptrigger - 20;
        		peak_vals_single[new_fptrigger] = peak_raw;
        	
        	if (new_fptrigger > 0) {
            		fptrigger_vals_new.push_back(new_fptrigger);
            		peak_vals_new.push_back(peak_raw);
        	}
    	}
    }
    
    int entry_location2 = 0;
    for (int hv : HV_Values) {
    	for (int led : LED_Values) {
        int sum = 0;
        int count_nonzero = 0;
        	for (int i = 0; i < 10; ++i) {
            		int val = peak_vals_single[entry_location2];
            		if (val != 0) {
                	sum += val;
                	++count_nonzero;
            		}
            		entry_location2++;
        	}
        	int avg_int;
        	if (count_nonzero > 0) {
        		avg_int = sum/count_nonzero;
        	} else {
        		avg_int = 0;
        	}
        	h_singleDetector->Fill(led, hv, avg_int);
    	}
    }
    
    
    //Loops through 0-29 to create histograms
    for (int i = 0; i <= colMax; i++) {
    	
    	//Create a new TH2F and storing it into hHVLED
    	hHVLED[i] = new TH2F(Form("hHVLED_col%d", i),
    	Form("Avg Peak: LED vs HV (Row %d, Col %d);LED Value;HV Value", fixedRow, i), 9, 1.5, 28.5, 9, 1375, 1825);
    }
    //Loops through 30-58 to create histograms
    for (int i = 0; i < nCols2; i++) {
    	int colNumber = i + colMin2;
    	
    	hHVLED2[i] = new TH2F(Form("hHVLED_col%d", colNumber),
        Form("Avg Peak: LED vs HV (Row %d, Col %d);LED Value;HV Value", fixedRow, colNumber), 9, 1.5, 28.5, 9, 1375, 1825);
    }
    //Loop through all the entries in the run
    for (long i = 0; i < entries; i++) {
        t->GetEntry(i);
        /* if (eventNum == 2000) {
    	  oneEntry->Fill(row, column, integral);
    	} */
	//Takes data from one block in the calorimeter
	
	
	//Loops through the row

	if (row == fixedRow) {
	
    		/* peak_raw_val.push_back(peak_raw);
    		eventNo.push_back(fptrigger); */
    		
    		
    		//Loops through the two sets of columns and pushes back the fptrigger in location of full_peak_col
    		if (column >= colMin && column <= colMax) {
    		
    		
    			int Newfptrigger = fptrigger - 20;
    			if (Newfptrigger > 0) {
    			
    			    fptrigger_vals.push_back(Newfptrigger);
    		            peak_vals.push_back(peak_raw);
    
    			    full_peak_col[column - colMin][Newfptrigger] = peak_raw;
    			    
    			    if (column == 10) {
    				//std::cout << "fp trigger: " << Newfptrigger << " peak_raw = " << peak_raw << std::endl;
    				}
    			}
    		
    			//full_peak_col[column - colMin][fptrigger] = peak_raw;
    			
    			
    		}
    		else if (column >= colMin2 && column <= colMax2) {
    		
        		int Newfptrigger2 = fptrigger - 20;
    			if (Newfptrigger2 > 0) {
    			    full_peak_col2[column - colMin2][Newfptrigger2] = peak_raw;
    			}
        		
        	}
    		/* std::cout << "fptrigger: " << fptrigger << " Peak Raw: " << peak_raw << std::endl; */
    		
    		/* bottom_integral->Fill(integral);
    		bottom_time->Fill(time);
    		bottom_peak->Fill(peak_raw); */
    		
    	}
    	 
     }
    
     
    //std::cout << "Total entries in full_peak_col " << full_peak_col[10].size() << std::endl;
     
    //Initialie an integer called entry_location
    int entry_location = 0;
    for (int hv : HV_Values) {
        for (int led : LED_Values) {
            std::vector<int> values;
            int sum = 0;
            int count_nonzero = 0;
            for (int i = 0; i < 10; i++) {
                int val = full_peak[entry_location];
                values.push_back(val);
                if (val != 0) {
                	sum += val; 
                	count_nonzero++;
                }
                entry_location++;
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
    int usedNum = 0;
    for (int col = colMin; col <= colMax; col++) {
    	int colIdx = col - colMin;
    	std::vector<int>& colBuf = full_peak_col[colIdx];
    	std::size_t pos = colMin;
    	for (int hv : HV_Values) {
        	for (int led : LED_Values) {
            	int sum = 0;
            	int count_nonzero = 0;  	
            		for (int trg = 0; trg < 20; ++trg, ++pos) {
                		if (pos >= colBuf.size()) break;
                		int val = colBuf[pos];

                		if (val != 0) {
                    		sum += val;
                    		count_nonzero++;
                		}
                		usedNum++;
            		}
            		int avg_int;
        		if (count_nonzero > 0) {
        			avg_int = sum/count_nonzero;
        		} else {
        			avg_int = 0;
        		}
            		//std::cout << "average: " << avg_int << std::endl;
            		
            		hHVLED[colIdx]->Fill(led, hv, avg_int);
        	}
        }
     }

    
    for (int col = colMin2; col <= colMax2; col++) {
    int colIdx = col - colMin2;
    std::vector<int>& colBuf = full_peak_col2[colIdx];
    std::size_t pos = 0;
    	for (int hv : HV_Values) {
        	for (int led : LED_Values) {
            	int sum = 0;
            	int count_nonzero = 0;
            	for (int trg = 0; trg < 20; ++trg, ++pos) {
                	if (pos >= colBuf.size()) break;
                	int val = colBuf[pos];
                	if (val != 0) {
                    	sum += val;
                    	count_nonzero++;
                	}
                	usedNum++;
            	}
            	int avg_int = (count_nonzero > 0) ? (sum / count_nonzero) : 0;
            	hHVLED2[colIdx]->Fill(led, hv, avg_int);
        	}
    	}
    }
    
    
    /////////////////////////////////////////////////////////////////////
    for (int r = 0; r < 59; r++) {
        for (int c = 0; c < 60; c++) {
            hHVLED_all[r][c] = new TH2F(Form("hHVLED_row%d_col%d", r, c), Form("Avg Peak: LED vs HV (Row %d, Col %d);LED Value;HV Value", r, c), 9, 1.5, 28.5, 9, 1375, 1825);
        }
    }
    // Loop through entries and fill the peak_map
    for (long i = 0; i < entries; i++) {
        t->GetEntry(i);
        if (row < 59 && column < 60) {
            int shifted_fp = fptrigger - 20;
            if (shifted_fp >= 0 && shifted_fp < 2520) {
                peak_map[row][column][shifted_fp] = peak_raw;
            }
        }
    }
    // Loop through each detector cell and compute average peak values
    for (int r = 0; r < 59; ++r) {
        for (int c = 0; c < 60; ++c) {
            std::vector<int>& buffer = peak_map[r][c];
            size_t pos = 0;
            for (int hv : HV_Values) {
                for (int led : LED_Values) {
                    int sum = 0;
                    int count = 0;
                    for (int k = 0; k < 20 && pos < buffer.size(); ++k, ++pos) {
                        int val = buffer[pos];
                        if (val != 0) {
                            sum += val;
                            count++;
                        }
                    }
                    int avg = (count > 0) ? (sum / count) : 0;
                    hHVLED_all[r][c]->Fill(led, hv, avg);
                }
            }
        }
    }
    std::vector<std::vector<double>> histogramAverages(59, std::vector<double>(60, 0.0));
    for (int r = 0; r < 59; ++r) {
    	for (int c = 0; c < 60; ++c) {
        	TH2F* hist = hHVLED_all[r][c];
        	double sum = 0.0;
        	int count = 0;
        	for (int x = 1; x <= hist->GetNbinsX(); ++x) {
            		for (int y = 1; y <= hist->GetNbinsY(); ++y) {
                	int val = hist->GetBinContent(x, y);
                		if (val != 0) {
                    			sum += val;
                    		count++;
                		}
            		}
        	}
        	int avg = (count > 0) ? (sum / count) : 0.0;
        	histogramAverages[r][c] = avg;
    	}
    }

    for (int r = 0; r < 59; ++r) {
    	for (int c = 0; c < 60; ++c) {
        	int histogramEntries = hHVLED_all[r][c]->GetEntries();
    	}
    }
    
    for (int r = 0; r < 59; ++r) {
    	for (int c = 0; c < 60; ++c) {
        	int avg_val = histogramAverages[r][c];
        	h_avg_per_cell->SetBinContent(c + 1, r + 1, avg_val); // ROOT bins start at 1
    	}
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    
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
    /* TCanvas* c7_canvas = new TCanvas("c7_canvas", "HVLED Comparison", 800, 600);
    TGraph* c7 = new TGraph(eventNo.size(), &eventNo[0], &peak_raw_val[0]);
    c7->Draw("AP"); */
    
    //Creates 1D histogram graph of values at one entry
    /* TCanvas* c8 = new TCanvas("c8", "Map of One Entry", 800, 800);
    oneEntry->GetXaxis()->SetTitle("Row");
    oneEntry->GetYaxis()->SetTitle("Column");
    oneEntry->Draw("COLZ"); */
    
    /* TCanvas* c9 = new TCanvas("c9", "Peak_raw per LED and HV", 800, 800);
    hvledComparison->GetXaxis()->SetTitle("LED Value");
    hvledComparison->GetYaxis()->SetTitle("HV Value");
    hvledComparison->Draw("COLZ"); */
    /* for (int i = 0; i < nCols; ++i) {
    	cHVLED[i] = new TCanvas(Form("cHVLED_col%d", i),
    	Form("Peak_raw per LED and HV – Row %d, Col %d", fixedRow, i), 800, 800);
    	hHVLED[i]->Draw("COLZ");
    } */
    
    TCanvas* c_avg_map = new TCanvas("c_avg_map", "Average Peak Map", 1000, 800);
    h_avg_per_cell->Draw("COLZ");

    /* TCanvas* c_singleCell = new TCanvas("c_singleCell", "Single Detector Histogram (Row 10, Col 22)", 800, 600);
    h_singleDetector->Draw("COLZ"); */

    // Plot shifted fptrigger
	/* TCanvas* c_shift = new TCanvas("c_shift", "Shifted fptrigger - 20", 800, 600);
	TGraph* gr_shift = new TGraph(fptrigger_vals_new.size(), &fptrigger_vals_new[0], &peak_vals_new[0]);
	c_shift->cd();
	gr_shift->Draw("AP"); */

   //Create a canvas that stores histograms from column 0 - 29
   /* TCanvas *cAll = new TCanvas("cAll", "All Column Histograms 0-29", 1600, 1000);
   int rows = 6;
   int cols = (nCols + 1) / 6;
   cAll->Divide(cols, rows);
   for (int i = 0; i < nCols; i++) {
   	cAll->cd(i+1);
    	hHVLED[i]->Draw("COLZ");
    }
    cAll->Update(); */
    
    //Create a canvas that stores histograms from column 30 - 58
    /* TCanvas *cAll2 = new TCanvas("cAll2", "All Column Histograms 30–58", 1600, 1000);
    int cols2 = 6;
    int rows2 = (nCols2 + cols2 - 1) / cols2;
    cAll2->Divide(cols2, rows2);
    for (int i = 0; i < nCols2; i++) {
    	cAll2->cd(i+1);
    	hHVLED2[i]->Draw("COLZ");
    }
    cAll2->Update(); */
    
    
    //Print the total entries in the data
    std::cout << "Total Entries: " << entries << std::endl;
     
}
