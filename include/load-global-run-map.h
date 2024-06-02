// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

struct run_info
{
  string period;
  string fill;
  string quality;
  string comment;
  string int_rate;
  string coll_bcs;
  run_info(string per, string fil, string qlt, string com, string irt, string cbs) {
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
    bool run_in_map(int r);
    bool is_run(int r, string quality);
    // get functions
    string get_run_period(int r) { return (run_in_map(r) ? r_map.at(r).period : "N/A"); }
    string get_run_fill(int r) { return (run_in_map(r) ? r_map.at(r).fill : "N/A"); }
    string get_run_quality(int r) { return (run_in_map(r) ? r_map.at(r).quality : "N/A"); }
    string get_run_comment(int r) { return (run_in_map(r) ? r_map.at(r).comment : "N/A"); }
    string get_run_int_rate(int r) { return (run_in_map(r) ? r_map.at(r).int_rate : "N/A"); }
    string get_run_coll_bcs(int r) { return (run_in_map(r) ? r_map.at(r).coll_bcs : "N/A"); }
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
    for(int i = 0; i < (int)content.size(); i++) {
      for(int j = 0; j < (int)content[i].size(); j++) cout << content[i][j] << "\t";
      cout << "\n";
    }
  }
  // fill the run map
  for(int i = 1; i < (int)content.size(); i++) {
      run_info rinf(content[i][0], content[i][1], content[i][3], content[i][4], content[i][5], content[i][6]);
      int run = stoi(content[i][2]);
      r_map.insert({run,rinf});
  }
  // print the run map
  if(verbose) print();
  cout << "Run map loaded successfully\n";
  return true;
}

bool run_map::run_in_map(int r)
{
  if(r_map.find(r) == r_map.end()) { // not found
    cout << "Run " << r << " missing in the run map\n";
    return false;
  } else return true;
}

bool run_map::is_run(int r, string quality)
{
  string q;
  run_in_map(r) ? q = r_map.at(r).quality : q = "N/A";
  return (q == quality ? true : false);
}