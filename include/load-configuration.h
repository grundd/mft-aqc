// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>

#include "utils.h"

struct run_specifier
{
  int run;
  string pass;
  string period_mc;
  run_specifier (int r, string ps, string per_mc = "") {
    run = r;
    pass = ps;
    period_mc = per_mc;
  }
};

class configuration
{
  private:
    string compare; // 'runs' (default) or 'passes'
    string group; // ideally: <period>_<pass>
    string jira; // format: O2-xxxx
    string passes; // <pass> (can be more separated by commas)
    string periods_mc; // <period> (can be more separeted by commas)
    int ref_run;
    string ref_pass;
    string ref_period_mc;
    string bad_runs; // 'hide' (default) or 'mute' or 'show'
    float plot_band; // default: 0.2 (20 % green band)
    long timestamp; // default: 0 (-> will use SOR timestamp)
    bool old_path; // default: false
    bool rewrite_qc_files; // default: false
    bool recreate_plots; // default: false
    // evaluated automatically:
    int n_passes;
    vector<string> pass_list;
    int n_periods_mc;
    vector<string> period_mc_list;
    int n_runs;
    vector<int> run_list;
    int n_combs;
    vector<run_specifier> comb_list; // (run, pass, period_mc) list
  public:
    configuration ();
    bool load_parameter (string key, string val);
    bool check ();
    void print ();
    bool load_from_file (string fname, bool verbose = false);
};

configuration::configuration():
  compare("runs"), group(""), jira(""), 
  passes(""), periods_mc(""), 
  ref_run(0), ref_pass(""), ref_period_mc(""), 
  bad_runs("hide"), old_path(false), 
  plot_band(0.2), timestamp(0), 
  rewrite_qc_files(false), recreate_plots(false),
  n_passes(0), pass_list(), 
  n_periods_mc(0), period_mc_list(),
  n_runs(0), run_list(),
  n_combs(0), comb_list()
{
  // default constructor
}

bool configuration::load_parameter (string key, string val)
{
  if (key == "compare") compare = val; 
  else if (key == "group") group = val;
  else if (key == "jira") jira = val;
  else if (key == "passes") {
    passes = val;
    istringstream iss(passes);
    string p;
    while(getline(iss,p,',')) {
      p = remove_spaces(p);
      if(p.empty()) {
        cout << "Empty pass name provided\n";
        return false;
      } else pass_list.push_back(p);
    }
    n_passes = (int)pass_list.size();
  }
  else if (key == "periods_mc") {
    periods_mc = val;
    istringstream iss(periods_mc);
    string p;
    while(getline(iss,p,',')) {
      p = remove_spaces(p);
      if(p.empty()) {
        cout << "Empty MC period name provided\n";
        return false;
      } else period_mc_list.push_back(p);
    }
    n_periods_mc = (int)period_mc_list.size();
  }
  else if (key == "ref_run") ref_run = stoi(val);
  else if (key == "ref_pass") ref_pass = val;
  else if (key == "ref_period_mc") ref_period_mc = val;
  else if (key == "bad_runs") bad_runs = val;
  else if (key == "old_path") old_path = to_bool(val);
  else if (key == "plot_band") plot_band = stof(val); 
  else if (key == "timestamp") timestamp = stol(val); 
  else if (key == "rewrite_root_files") rewrite_qc_files = to_bool(val);
  else if (key == "recreate_plots") recreate_plots = to_bool(val);
  else {
    cout << "Unsupported identifier: '" << key << "'\n"; 
    return false;
  }
  return true;
}

bool configuration::check ()
{
  bool success = true;
  string par = "";

  // check if any passes are provided
  if(n_passes == 0) { success = false; par = "passes"; }
  
  if (compare == "runs") {
    // comparison of runs: check ref run & ref pass
    if(ref_run <= 0) { success = false; par = "ref_run"; }
    if(ref_pass == "") { 
      if(n_passes == 1) {
        cout << "Ref pass not specified, taking the global one: " << passes << "\n";
        ref_pass = passes; 
      } else { success = false; par = "ref_pass"; } 
    }
  } else if (compare == "passes") {
    // comparison of passes: check ref pass
    if(ref_pass == "") { success = false; par = "ref_pass"; }
  } else {
    // other comparison type not defined
    cout << "Unsupported option for 'compare'. Use 'runs' or 'passes'\n";
    return false;
  }
  // reference is MC but period is not specified
  if(ref_period_mc == "" && ref_pass == "passMC") {
    if(n_periods_mc == 1) {
      cout << "Ref MC period not specified, taking the global one: " << periods_mc << "\n"; 
      ref_period_mc = periods_mc;
    } else { success = false; par = "ref_period_mc"; } 
  }
  // passMC requested but period MC not specified
  bool passMC = false;
  for(auto ps : pass_list) if(ps == "passMC") passMC = true;
  if(passMC && n_periods_mc == 0) {
    cout << "passMC requested but period_mc not specified\n";
    return false;
  }
  // create group name, if not given
  if(group == "") {
    cout << "Group name not given\n";
    if(jira != "") {
      group = jira;
      cout << " -> JIRA ticket number will be used\n";
    } else { success = false; par = "group"; }
  }
  // successful?
  if(!success) {
    cout << "Parameter '" << par << "' not set properly\n";
    return false;
  } else return true;
}

void configuration::print ()
{
  cout << std::boolalpha
    << "\nConfiguration: \n"
    << " compare:    " << compare << "\n"
    << " group:      " << group << "\n"
    << " jira:       " << jira << "\n"
    << " passes:     " << passes << " (total: " << n_passes << ")\n"
    << " MC periods: " << periods_mc << " (total: " << n_periods_mc << ")\n";
  if(compare == "runs") cout << " reference:  " << ref_run << ", " << ref_pass;
  else cout << " reference:  " << ref_pass;
  if(ref_period_mc != "") cout << ", " << ref_period_mc;
  cout << "\n"
    << " *** \n"
    << " plot band:        " << plot_band << "\n"
    << " timestamp:        " << timestamp << " (0 -> SOR)\n"
    << " old QC paths?     " << old_path << "\n"
    << " rewrite QC files? " << rewrite_qc_files << "\n"
    << " recreate plots?   " << recreate_plots << "\n"
    << " *** \n"
    << " # runs: " << n_runs << "\n"
    << " # (run,pass,MC period): " << n_combs << "\n"
    << "\n";
  int i(1);
  for(auto r : comb_list) {
    printf("   %03i -> %i, %s", i, r.run, r.pass.data());
    if (r.period_mc != "") printf(", %s\n", r.period_mc.data());
    else printf("\n");
    i++;
  }
  return;
}

bool configuration::load_from_file (string fname, bool verbose)
{
  ifstream f(fname);
  if(f.is_open()) {
    string line;
    bool loading_params = true;
    while(getline(f,line)) {
      istringstream iss(line);
      if(remove_spaces(line) == "runs:") {
          loading_params = false;
          continue;
      }
      if(loading_params) { // loading parameters
        string key;
        if(getline(iss,key,'=')) {
          string value;
          if(getline(iss,value)) {
            value = remove_spaces(value);
            if(value.empty()) continue; // protection against empty values (or whitespaces)
            if(verbose) cout << "Setting: " << key << " -> " << value << "\n";
            if(!load_parameter(key,value)) {
              cout << "Error setting " << key << "!\n";
              return false;
            }
          } 
        }
      } else { // loading the list of runs
        string run;
        iss >> run; 
        if(verbose) cout << run << "\n";
        run_list.push_back(stoi(run));
        n_runs = (int)run_list.size();
        for(auto ps : pass_list) {
          if(ps == "passMC") {
            for(auto per_mc : period_mc_list) {
              comb_list.push_back(run_specifier(stoi(run),ps,per_mc));
            }
          } else {
            comb_list.push_back(run_specifier(stoi(run),ps));
          }
          n_combs = (int)comb_list.size();
        }
      }
    }
  } else {
    cout << "Could not open config file " << fname << "\n"; 
    return false;
  }
  f.close();
  if(check()) {
    cout << "\nConfiguration successful\n";
    print();
    return true;
  } else {
    cout << "\nConfiguration invalid\n";
    return false;
  }
}