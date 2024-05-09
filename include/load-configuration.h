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
    string passes; // <pass> (can be more separated by commas) or 'more' (and specified for each run)
    string periods_mc; // <period> (can be more separeted by commas) or 'more' (and specified for each run)
    int ref_run;
    string ref_pass;
    string ref_period_mc;
    string bad_runs; // 'hide' (default) or 'mute' or 'show'
    // boolean variables:
    bool old_path; // default: false
    bool plot_band_20pct; // default: true
    bool timestamp_minus1; // default: false
    bool rewrite_root_files; // default: false
    bool recreate_plots; // default: false
    // evaluated automatically:
    bool single_pass;
    bool single_period;
    bool single_period_mc;
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
    bool check_config ();
    void print_config ();
    bool load_from_file (string fname, bool verbose = false);
};

configuration::configuration():
  compare("runs"), group(""), jira(""), 
  passes(""), periods_mc(""), 
  ref_run(0), ref_pass(""), ref_period_mc(""), 
  bad_runs("hide"), old_path(false), 
  plot_band_20pct(true), timestamp_minus1(false), 
  rewrite_root_files(false), recreate_plots(false),
  single_pass(true), single_period(true), single_period_mc(true),
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
    if(passes == "more") single_pass = false;
    else {
      istringstream iss(passes);
      string p;
      while(getline(iss,p,',')) {
        p = remove_spaces(p);
        if(p.empty()) {
          cout << "Invalid pass name provided\n";
          return false;
        } else pass_list.push_back(p);
      }
      n_passes = (int)pass_list.size();
    }
  }
  else if (key == "periods_mc") {
    periods_mc = val;
    if(periods_mc == "more") single_period_mc = false;
    else {
      istringstream iss(periods_mc);
      string p;
      while(getline(iss,p,',')) {
        p = remove_spaces(p);
        if(p.empty()) {
          cout << "Invalid MC period name provided\n";
          return false;
        } else period_mc_list.push_back(p);
      }
      n_periods_mc = (int)period_mc_list.size();
    }
  }
  else if (key == "ref_run") ref_run = stoi(val);
  else if (key == "ref_pass") ref_pass = val;
  else if (key == "ref_period_mc") ref_period_mc = val;
  else if (key == "bad_runs") bad_runs = val;
  else if (key == "old_path") old_path = to_bool(val);
  else if (key == "plot_band_20pct") plot_band_20pct = to_bool(val); 
  else if (key == "timestamp_minus1") timestamp_minus1 = to_bool(val); 
  else if (key == "rewrite_root_files") rewrite_root_files = to_bool(val);
  else if (key == "recreate_plots") recreate_plots = to_bool(val);
  else {
    cout << "Unsupported identifier: '" << key << "'\n"; 
    return false;
  }
  return true;
}

bool configuration::check_config ()
{
  bool success = true;
  string par = "";
  // check if any passes are provided
  if(n_passes == 0) { success = false; par = "passes"; }
  // check single_pass and single_period_mc
  if(n_passes == 1 && ref_pass != passes) single_pass = false;
  if(n_periods_mc == 1 && ref_period_mc != periods_mc) single_period_mc = false;
  // reference is MC but period is not specified
  if(ref_period_mc == "" && ref_pass == "passMC") {
    if(single_period_mc) {
      cout << "Ref MC period not specified, taking the global one: " << periods_mc << "\n"; 
      ref_period_mc = periods_mc;
    } else { success = false; par = "ref_period_mc"; } 
  }
  // comparison of runs
  if (compare == "runs") {
    // check ref run configuration
    if(ref_run <= 0) { success = false; par = "ref_run"; }
    if(ref_pass == "") { 
      if(single_pass) {
        cout << "Ref pass not specified, taking the global one: " << passes << "\n";
        ref_pass = passes; 
      } else { success = false; par = "ref_pass"; } 
    }
  } 
  // comparison of passes
  else if (compare == "passes") {
    if(ref_pass == "") { success = false; par = "ref_pass"; }
  }
  // other comparison type not defined
  else {
    cout << "Unsupported option for 'compare'. Use 'runs' or 'passes'\n";
    return false;
  }
  // successful?
  if(!success) {
    cout << "Parameter '" << par << "' not set properly\n";
    return false;
  } else return true;
}

void configuration::print_config ()
{
  cout << std::boolalpha
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
    << " single period?    " << single_period << "\n"
    << " single pass?      " << single_pass << "\n"
    << " single MC period? " << single_period_mc << "\n"
    << " old QC paths?     " << old_path << "\n"
    << " band at 20%?      " << plot_band_20pct << "\n"
    << " timestamp -1?     " << timestamp_minus1 << "\n"
    << " rewrite files?    " << rewrite_root_files << "\n"
    << " recreate plots?   " << recreate_plots << "\n"
    << " *** \n"
    << " # runs: " << n_runs << "\n"
    << " # (run,pass,MC period): " << n_combs << "\n";
  int i(1);
  for(auto r : comb_list) {
    if(n_passes == 1 && n_periods_mc <= 1) cout << printf("   %03i -> ", i) << r.run << "\n";
    else {
      if(n_passes > 1 && n_periods_mc > 1) cout << printf("   %03i -> ", i) << r.run << ", " << r.pass << ", " << r.period_mc << "\n";
      else if(n_passes > 1) cout << printf("   %03i -> ", i) << r.run << ", " << r.pass << "\n";
      else cout << printf("   %03i -> ", i) << r.run << ", " << r.period_mc << "\n";
    }
    i++;
  }
  return;
}

bool configuration::load_from_file (string fname, bool verbose)
{
  ifstream f(fname);
  if(f.is_open()) {
    string line;

    // single period eval

  } else {
    cout << "Could not open config file " << fname << "\n"; 
    return false;
  }
  f.close();
  if(check_config()) {
    cout << "Configuration successful\n";
    print_config();
    return true;
  } else {
    cout << "Configuration invalid\n";
    return false;
  }
}