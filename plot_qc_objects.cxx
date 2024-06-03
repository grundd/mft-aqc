// MFT Asynchronous Quality Control
// David Grund
// 2024

#include "TStyle.h"
// my headers
#include "include/load-configuration.h"
#include "include/load-list-of-histograms.h"
#include "include/ratio-plot.h"

bool create_plots (run_map rm, configuration cfg, vector<histogram> hlst, bool debug)
{
  cout << "\n";
  string path = Form("%s%s/", PLOTS_FOLDER.data(), cfg.get_group().data());
  gSystem->Exec(Form("mkdir -p %s", path.data()));

  // compare runs
  if(cfg.get_compare() == "runs") 
  {
    vector<run_specifier> list_full = cfg.get_full_list();
    run_specifier ref = cfg.get_ref_run(rm);

    // loop over rounds
    int n_rounds = cfg.get_n_rounds();
    int n_passes = cfg.get_n_passes();
    int n_per_round = (n_colors * n_styles);
    cout << "Plotting comparison of runs. Total rounds: " << n_rounds << ", passes: " << n_passes << "\n"
         << " Folder: " << path << "\n"
         << " Reference run: " << ref.run << ", " << ref.period << ", " << ref.pass << "\n";
    for (int r = 0; r < n_rounds; r++) 
    {
      cout << "\n Round " << r+1 << ":\n";
      string prefix = "";
      if(n_rounds > 1) prefix = Form("%02i_", r+1);

      int first = r * n_per_round;
      int last = first;
      r < n_rounds-1 ? last = first + n_per_round : last = list_full.size();
      vector<run_specifier> list_part(&list_full[first], &list_full[last]);

      // loop over QC objects
      for (auto h : hlst) 
      {
        string name = h.name_short;
        //cout << "  QC object: " << name << "\n";
        ratio_plot rplt;
        rplt.create_plot(cfg, rm, path, prefix, h, ref, list_part, debug);
      }
    }
  } 
  // compare passes
  else if(cfg.get_compare() == "passes") 
  {
    vector<int> list_runs = cfg.get_run_list();
    vector<string> list_pass = cfg.get_pass_list();
    vector<string> list_per_mc = cfg.get_period_mc_list();

    int n_runs = cfg.get_n_runs();
    cout << "Plotting comparison of passes. Total runs: " << n_runs << "\n";
    if(cfg.get_ref_pass() == "passMC") cout << " Reference: " << cfg.get_ref_period_mc() << "\n";
    else cout << " Reference: " << cfg.get_ref_pass() << "\n";

    // create sublist of (run, pass, period) for each run
    string ref_pass = cfg.get_ref_pass();
    string ref_period_mc = cfg.get_ref_period_mc();
    for (auto run : list_runs)
    {
      cout << "\n Run " << run << ":\n"; 
      if((rm.is_run(run,STR_BAD) && cfg.get_bad_runs() == "hide") || rm.is_run(run,STR_NOT_PART)) {
        cout << "Run BAD and option 'hide' selected or MFT not participating -> skipped\n";
        continue;
      }

      vector<run_specifier> list_part;
      run_specifier ref;
      for (auto pass : list_pass) {
        if(pass == "passMC") { 
          for (auto period_mc : list_per_mc) {
            run_specifier rsp(run,pass,period_mc);
            if (pass == ref_pass && period_mc == ref_period_mc) ref = rsp;
            else list_part.push_back(rsp);
          }
        } else {
          string period = rm.get_run_period(run);
          run_specifier rsp(run,pass,period);
          if (pass == ref_pass) ref = rsp;
          else list_part.push_back(rsp);
        }
      }
      // loop over QC objects
      for (auto h : hlst) 
      {
        string name = h.name_short;
        //cout << "  QC object: " << name << "\n";
        ratio_plot rplt;
        rplt.create_plot(cfg, rm, path, Form("%i_", run), h, ref, list_part, debug);
      }
    }
  }
  return true;
}

void plot_qc_objects (string input = "", bool verbose = false)
{
  // global run map
  run_map grm;
  if(!grm.load_from_file(verbose, false)) return;

  // global configuration
  configuration cfg;
  if(!cfg.load_from_file(input, grm, verbose)) return;

  // list of histograms
  histogram_list hlst;
  if(!hlst.load_from_csv(PATH_TO_HISTO_LIST)) return;

  gStyle->SetLineStyleString(11,"[]");
  gStyle->SetLineStyleString(12,"16 8");
  gStyle->SetLineStyleString(13,"4 4");

  vector<histogram> hlst_plot = hlst.get_list_to_plot("TH1F");

  create_plots(grm, cfg, hlst_plot, false);

  cout << "\nDone\n\n";
  return;
}