// MFT Asynchronous Quality Control
// David Grund
// 2024

#include "TStyle.h"
// my headers
#include "include/global-settings.h"
#include "include/load-configuration.h"
#include "include/load-list-of-histograms.h"
#include "include/ratio-plot.h"

bool make_plots (configuration cfg)
{
  return true;
}

void plot_qc_objects (string input = "_test.txt", bool verbose = false)
{
  // global run map
  run_map grm;
  if(!grm.load_from_file(verbose, false)) return;

  // global configuration
  configuration cfg;
  if(!cfg.load_from_file(input,grm, verbose)) return;

  // list of histograms
  histogram_list hlst;
  if(!hlst.load_from_csv(PATH_TO_HISTO_LIST)) return;

  vector<run_specifier> list_full = cfg.get_full_list();

  gStyle->SetLineStyleString(11,"[]");
  gStyle->SetLineStyleString(12,"16 8");
  gStyle->SetLineStyleString(13,"4 4");

  // if(cfg.get_compare() == "runs") {
  //   run_specifier ref_run = cfg.get_ref_run();
  //   cout << "\nRuns will be compared with a reference:\n"
  //     << " run:    " << cfg << "\n"
  //     << " period: " << getRunPeriod(_refRun,_refPass) << "\n"
  //     << " pass:   " << _refPass << "\n";
  // }

  cout << "Done\n\n";
  return;
}