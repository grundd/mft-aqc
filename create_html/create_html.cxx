// MFT asynchronous Quality Control
// createHtmlTables.cxx
// David Grund, Feb 11, 2024

vector<vector<string>> loadCsv(string fname, bool print = false)
{
    vector<vector<string>> arr;
	vector<string> row;
	string line, item;
    // open and read the csv
    fstream f(fname,ios::in);
	if(f.is_open()) {
		while(getline(f, line)) {
			row.clear();
			stringstream str(line);
			while(getline(str, item, ',')) row.push_back(item);
            arr.push_back(row);
		}
	} else {
        cout << "Could not open " << fname << "\n";
        return arr;
    }
    // print the loaded values?
    if(print) {
        cout << "Loaded table (" << fname << "):\n";
        for(int i = 0; i < arr.size(); i++) {
            for(int j = 0; j < arr[i].size(); j++) cout << arr[i][j] << "\t";
            cout << "\n";
        }
    }
    return arr; 
}

bool create_html(string dataset) 
{
    string fname = Form("%s.csv",dataset.data());
    vector<vector<string>> table = loadCsv(fname, true);
    int cols = table[0].size() - 1; // the last column is technical only
    int rows = table.size();
    int passes = (table[0].size() - 1) / 2;
    if(dataset == "MC") passes = 1;
    cout << "\n"
         << "# columns: " << cols << "\n"
         << "# rows: " << rows << "\n"
         << "# passes: " << passes << "\n";
    ofstream ofs(Form("../alice-mft-aqc/%s.html",dataset.data()));

    ofs << "<table class='tb'>\n"; 
    int iRow = 0;

    // header row 1
    if(dataset != "MC") {
        ofs << "<tr> "
        << "<th></th> "; // first empty cell
        for(int i = 0; i < passes; i++) ofs << "<th colspan='2'>" << table[iRow][1+2*i] << "</th> ";
        ofs << "</tr>\n";
        iRow++;
    }

    // header row 2
    ofs << "<tr> ";
    for(int i = 0; i < cols; i++) ofs << "<th>" << table[iRow][i] << "</th> ";
    ofs << "</tr>\n";
    iRow++;

    // normal rows
    for(int i = iRow; i < rows; i++) {
        ofs << "<tr> ";
        ofs << "<td>" << table[i][0] << "</td> "; // period name
        for(int j = 0; j < passes; j++) {
            int col = 1+2*j;
            //string pass = "";
            //if(dataset != "MC") pass = table[0][col] + "/";
            // JIRA:
            string jira = table[i][col];
            if(jira == "" || jira == "-") ofs << "<td>" << jira << "</td> ";
            else ofs << "<td>" << "<a href='https://alice.its.cern.ch/jira/browse/" << jira << "'>" << jira << "</a>" << "</td> ";
            col++;
            // MC: anchored to
            if(dataset == "MC") {
                ofs << "<td>" << table[i][col] << "</td> ";
                col++;
            }
            // report:
            string report = table[i][col];
            if(report == "" 
                || report == "-" 
                || report == "VdM scans"
                || report == "not for physics"
            ) ofs << "<td>" << report << "</td> ";
            else ofs << "<td>" << "<a href='reports/" << report << ".pdf'>" << report << ".pdf</a>" << "</td> ";
            col++;
            // MC: description
            if(dataset == "MC") {
                ofs << "<td>" << table[i][col] << "</td> ";
                col++;
            }
        }
        ofs << "</tr>\n";
    }

    ofs << "</table>";
   
    ofs.close();
    return true;
}

void create_html ()
{
    create_html("2023_pp");
    create_html("2023_PbPb");
    create_html("2024_pp");
    create_html("MC");
    return;
}