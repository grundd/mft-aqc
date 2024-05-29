// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "global-settings.h"

struct run_info
{
  string period;
  int fill;
  string quality;
  string comment;
  string int_rate;
  int coll_bcs;
  run_info(string per, int fil, string qlt, string com, string irt, int cbs) {
    period = per;
    fill = fil;
    quality = qlt;
    comment = com;
    int_rate = irt;
    coll_bcs = cbs;
  }
};

class run_map
{
  private:
    map<int,run_info> r_map;
  public:
    run_map();
    void print();
    bool load_from_file (bool print = false, bool debug = false);
};

run_map::run_map():
  r_map()
{
  // default constructor
}

void run_map::print()
{
  cout << "Printing map of runs:\n"
       << "run \tper \tfill \tqlt \tIR \tcbs \tcomment \n";
  map<int,run_info>::iterator it;
  for (it = r_map.begin(); it != r_map.end(); it++) {
    cout << it->first << "\t"
      << it->second.period << "\t"
      << it->second.fill << "\t"
      << it->second.quality << "\t"
      << it->second.int_rate << "\t"
      << it->second.coll_bcs << "\t"
      << it->second.comment << "\n";
  }
  return;
}

bool run_map::load_from_file (bool verbose, bool debug)
{
	vector<vector<string>> content;
	vector<string> row;
	string line, item;
  // open and read the csv
  fstream f(PATH_TO_GLOBAL_MAP,ios::in);
	if(f.is_open()) {
		while(getline(f,line)) {
		  row.clear();
		  stringstream str(line);
      while(getline(str, item, ',')) row.push_back(item);
      content.push_back(row);
		}
	} else {
    cout << "Could not open " << PATH_TO_GLOBAL_MAP << "\n";
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
      run_info rinf(content[i][0],stoi(content[i][1]),content[i][3],content[i][4],content[i][5],stoi(content[i][6]));
      int run = stoi(content[i][2]);
      r_map.insert({run,rinf});
  }
  // print the run map
  if(verbose) print();
  cout << "Run map loaded successfully\n";
  return true;
}