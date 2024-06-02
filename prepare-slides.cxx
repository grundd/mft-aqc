// MFT Asynchronous Quality Control
// David Grund
// 2024

#include "include/load-configuration.h"
#include "include/load-list-of-histograms.h"
#include "include/prepare-slides.h"

void prepare_slides (string input = "_test.txt", bool verbose = false)
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

  create_main_latex(cfg, grm);

  cout << "\nDone\n\n";
  return;
}