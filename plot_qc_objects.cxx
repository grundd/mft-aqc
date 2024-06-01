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
  string path = Form("plots/%s/", cfg.get_group().data());
  gSystem->Exec(Form("mkdir -p %s", path.data()));

  // compare runs
  if(cfg.get_compare() == "runs") 
  {
    cout << "\n";
    vector<run_specifier> list_full = cfg.get_full_list();
    run_specifier ref = cfg.get_ref_run(rm);

    // loop over rounds
    int n_rounds = cfg.get_n_rounds();
    int n_per_round = (n_colors * n_styles);
    cout << "Plotting run-by-run comparison plots. Total rounds: " << n_rounds << "\n"
      << " Ref run: " << ref.run << ", " << ref.period << ", " << ref.pass << "\n";
    for (int r = 0; r < n_rounds; r++) 
    {
      cout << " Round " << r+1 << ": \n";
      int first = r * n_per_round;
      int last = first;
      r < n_rounds-1 ? last = first + n_per_round : last = list_full.size()-1;
      vector<run_specifier> list_part(&list_full[first], &list_full[last]);

      // loop over QC objects
      bool first_plot = true;
      for (auto h : hlst) 
      {
        string name = h.name_short;
        cout << "  QC object: " << name << "\n";
        ratio_plot rplt;
        rplt.create_plot(cfg, rm, h, path, ref, list_part, debug);
      }
    }
  } 
  // compare passes
  else if(cfg.get_compare() == "passes") 
  {
    
  }
  return true;
}

void plot_qc_objects (string input = "_test.txt", bool verbose = false)
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