// loadGlobalRunMap.h
// David Grund, Oct 5, 2023
// MFT asynchronous Quality Control

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// run information
struct structRunInfo 
{
    string period;
    int fill;
    string quality;
    string comment;
    string intRate;
    int collBcs;
    structRunInfo(string p, int f, string q, string c, string ir, int cb) {
        period = p;
        fill = f;
        quality = q;
        comment = c;
        intRate = ir;
        collBcs = cb;
    }
};

// global run map (database)
map<int,structRunInfo> globalRunMap;

// print global run map
void printGlobalRunMap() 
{
    cout << "Printing loaded global map of runs:\n"
         << "run\tperiod\tfill\tquality\n";
    map<int,structRunInfo>::iterator it;
    for (it = globalRunMap.begin(); it != globalRunMap.end(); it++) {
        cout << it->first << "\t"
             << it->second.period << "\t"
             << it->second.fill << "\t"
             << it->second.quality << "\t"
             << it->second.comment << "\t"
             << it->second.intRate << "\t"
             << it->second.collBcs << "\n";
    }
    return;
}

// load and initialize the global run map from the .csv file
bool loadGlobalRunMap(bool print = false, bool debug = false) 
{
    string fname = "input/run_table.csv";
	vector<vector<string>> content;
	vector<string> row;
	string line, item;
    // open and read the csv
    fstream f(fname,ios::in);
	if(f.is_open()) {
		while(getline(f, line)) {
			row.clear();
			stringstream str(line);
			while(getline(str, item, ',')) row.push_back(item);
			content.push_back(row);
		}
	} else {
        cout << "Could not open " << fname << "\n";
        return false;
    }
    // print the loaded values?
    if(debug) {
        for(int i = 0; i < content.size(); i++) {
            for(int j = 0; j < content[i].size(); j++) cout << content[i][j] << "\t";
            cout << "\n";
        }
    }
    // fill the run map
    for(int i = 1; i < content.size(); i++) {
        structRunInfo rinf(content[i][0],stoi(content[i][1]),content[i][3],content[i][4],content[i][5],stoi(content[i][6]));
        int run = stoi(content[i][2]);
        globalRunMap.insert({run,rinf});
    }
    // print the run map
    if(print) printGlobalRunMap();
    cout << "Global run map loaded successfully.\n";
    return true;   
}

// find given run in the global map
bool runInMap(int run) 
{
    if(globalRunMap.find(run) == globalRunMap.end()) { // not found
        cout << "Run " << run << " missing in the global run map! \n";
        return false;
    } else { // found
        return true;
    }
}

// quality of the run ... can be accessed through the struct in the map
bool isRun(string quality, int run) 
{
    string q;
    runInMap(run) ? q = globalRunMap.at(run).quality : q = "missing!";
    return (q == quality ?  true : false);
}

// get functions
//string getRunPeriod(int run) { return (runInMap(run) ? globalRunMap.at(run).period : "missing!"); }
string getRunPeriod(int run, string pass = "") { 
    string period = "";
    if(pass == "passMC") period = _periodMC;
    else runInMap(run) ? period = globalRunMap.at(run).period : period = "missing!";
    return period;
}
int getRunFill(int run) { return (runInMap(run) ? globalRunMap.at(run).fill : -1); }
string getRunQuality(int run) { return (runInMap(run) ? globalRunMap.at(run).quality : "missing!"); }
string getRunComment(int run) { return (runInMap(run) ? globalRunMap.at(run).comment : "missing!"); }
string getRunIntRate(int run) { return (runInMap(run) ? globalRunMap.at(run).intRate : "missing!"); }
int getRunCollBcs(int run) { return (runInMap(run) ? globalRunMap.at(run).collBcs : -1); }