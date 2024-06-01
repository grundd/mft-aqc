// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

string get_name_short (string name)
{
    string name_short = name;
    char slash = '/';
    size_t idx = name_short.find_last_of(slash);
    if (idx < string::npos) {
        // erase everything up to 'index'
        name_short.erase(0,idx+1);
    }
    return name_short;
}

struct histogram
{
  string task;
  string type;
  string name;
  string title;
  string options;
  string download;
  string plot;
  string name_short;
  histogram () {
    task = ""; type = ""; name = ""; title = ""; options = "";
    download = ""; plot = ""; name_short = "";
  }
  histogram (string _task, string _type, string _name, 
    string _title, string _options, string _download, string _plot
  ) {
    task = _task;
    type = _type;
    name = _name;
    title = _title;
    options = _options;
    download = _download;
    plot = _plot;
    name_short = get_name_short(name);
  }
};

struct histogram_list
{
  vector<histogram> vect_histos;
  bool load_from_csv (string fname, bool verbose = false)
  {
    vect_histos.clear();
    vector<vector<string>> table;
    vector<string> row;
    string line, item;
    // open and read the csv
    fstream f(fname,ios::in);
    if(f.is_open()) {
      while(getline(f, line)) {
        row.clear();
        stringstream str(line);
        while(getline(str, item, csv_delimiter)) row.push_back(item);
        table.push_back(row);
      }
    } else {
      cout << "Cannot open " << fname << "\n";
      return false;
    }
    // print the loaded values?
    if(verbose) {
      for(int i = 0; i < (int)table.size(); i++) {
        for(int j = 0; j < (int)table[i].size(); j++) cout << table[i][j] << "\t";
          cout << "\n";
      }
    }
    // fill the list of histograms
    for(int i = 2; i < (int)table.size(); i++) {
      histogram h(table[i][0],table[i][1],table[i][2],table[i][3],
        table[i][4],table[i][5],table[i][6]);
      vect_histos.push_back(h);
    }
    cout << "List of histograms loaded successfully\n";
    return true;   
  }
  vector<histogram> get_list_to_download (string _task = "", string _type = "") 
  {
    vector<histogram> list;
    for(auto h : vect_histos) {
      if ((h.download == STR_TRUE)
        && (h.task == _task)
        && (h.type == _type)
      ) list.push_back(h);
    }
    return list;
  }
  vector<histogram> get_list_to_plot (string _type = "") 
  {
    vector<histogram> list;
    for(auto h : vect_histos) {
      if ((h.download == STR_TRUE)
        && (h.plot == STR_TRUE)
        && (h.type == _type)
      ) list.push_back(h);
    }
    return list;
  }
  void print () 
  {
    cout << "Printing the list of histograms: \n";
    for(auto h : vect_histos) {
      cout << h.task << "\t"
        << h.type << "\t"
        << h.name << "\t"
        << h.title << "\t"
        << h.options << "\t"
        << h.download << "\t"
        << h.plot << "\t"
        << h.name_short << "\n";
    }
    return;
  }
};