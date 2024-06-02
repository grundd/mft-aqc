// MFT Asynchronous Quality Control
// David Grund
// 2024

// root 'download_qc_objects.cxx("'_test.txt'")'

#include "TSystem.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "CCDB/CcdbApi.h"
// my headers
#include "include/load-configuration.h"
#include "include/load-list-of-histograms.h"

o2::ccdb::CcdbApi api_ccdb;
o2::ccdb::CcdbApi api_qcdb;
o2::ccdb::CcdbApi api_qcdbmc;

long get_timestamp (configuration cfg, int run) 
{
  long timestamp = -1;
  if(cfg.get_timestamp() == 0) {
    map<string, string> hdRCT = api_ccdb.retrieveHeaders("RCT/Info/RunInformation", map<string, string>(), run);
    const auto startRCT = hdRCT.find(STR_SOR);
    if (startRCT != hdRCT.end()) {
      timestamp = stol(startRCT->second);
      cout << STR_SOR << " found, timestamp: " << timestamp << "\n";
    } else {
      cout << STR_SOR << " not found in headers!" << "\n";
    }
  } else timestamp = cfg.get_timestamp();
  return timestamp;
}

template <typename TH>
TH* download_histo(string hname, int run, string pass, string period, long timestamp, bool verbose = true)
{
  string s_run = std::to_string(run);
  string specify_pass = "";
  if(pass != "online") specify_pass = pass;
  // create metadata:
  map<string, string> metadata;
  metadata["RunNumber"] = s_run;
  metadata["PeriodName"] = period;
  if(specify_pass != "") metadata["PassName"] = specify_pass;
  if(SET_QC_VERSION != "") metadata["qc_version"] = SET_QC_VERSION;
  // print the used metadata:
  if(verbose) {
    cout << "\nmetadata specifications: \n";
    cout << " -> RunNumber=" << s_run
      << ", PeriodName=" << period;
    if(specify_pass != "") cout << ", PassName=" << specify_pass;
    if(SET_QC_VERSION != "") cout << ", qc_version=" << SET_QC_VERSION;
    cout << "\n";
  }
  TH* h = NULL;
  if(pass == "passMC") h = api_qcdbmc.retrieveFromTFileAny<TH>(hname,metadata,timestamp);
  else                 h = api_qcdb.retrieveFromTFileAny<TH>(hname,metadata,timestamp);
  return h;
}

void download_histos(configuration cfg, histogram_list hlst, run_specifier rsp)
{
  int run = rsp.run;
  string pass = rsp.pass;
  string period = rsp.period;

  gSystem->Exec(Form("mkdir -p %s%s/",ROOT_FILES_FOLDER.data(),period.data()));
  string fname = Form("%s%s/%i_%s.root",ROOT_FILES_FOLDER.data(),period.data(),run,pass.data());

  // check if the file already exists
  bool fexists = !gSystem->AccessPathName(fname.data());
  if(fexists && !cfg.is_rewrite_input()) {
    cout << fname << " : already downloaded -> skipping\n";
  } else {
    cout << fname << " : will be downloaded now\n";
    long timestamp = get_timestamp(cfg, run);
    TFile* f = new TFile(fname.data(),"recreate");
    string s_cls;
    string s_trks;
    int ver = 0;
    if (cfg.is_old_path()) ver = 1;
    // online QC
    if (pass == "online") {
      s_cls = PATH_QC_CLS[ver];
      s_trks = PATH_QC_TRKS[ver];
    // async QC of MC data
    } else if (pass == "passMC") {
      s_cls = PATH_MC_CLS[ver];
      s_trks = PATH_MC_TRKS[ver];
    // async QC of data
    } else {
      s_cls = PATH_AQC_CLS[ver];
      s_trks = PATH_AQC_TRKS[ver];
    }
    // tracks : TH2F
    for(auto hist : hlst.get_list_to_download("track","TH2F")) {
      TH2F* h = download_histo<TH2F>(s_trks+hist.name,run,pass,period,timestamp);
      if(h) {
        cout << "run " << run << ", " << pass << ": " << h->GetName() << " downloaded\n";
        f->cd();
        h->Write(hist.name_short.data());
      }
    }
    // tracks : TH1F
    for(auto hist : hlst.get_list_to_download("track","TH1F")) {
      TH1F* h = download_histo<TH1F>(s_trks+hist.name,run,pass,period,timestamp);
      if(h) {
        cout << "run " << run << ", " << pass << ": " << h->GetName() << " downloaded\n";
        f->cd();
        h->Write(hist.name_short.data());
      }
    }
    // clusters : TH1F
    for(auto hist : hlst.get_list_to_download("cluster","TH1F")) {
      TH1F* h = download_histo<TH1F>(s_cls+hist.name,run,pass,period,timestamp);
      if(h) {
        cout << "run " << run << ", " << pass << ": " << h->GetName() << " downloaded\n";
        f->cd();
        h->Write(hist.name_short.data());
      }
    }
    // save the file
    f->Write("",TObject::kWriteDelete);
    f->Close();
  }
  return;
}

void download_qc_objects (string input = "_test.txt", bool verbose = false)
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

  // connect to ccdb, qcdb, qcdbmc    
  api_ccdb.init("alice-ccdb.cern.ch");
  api_qcdb.init("ali-qcdb-gpn.cern.ch:8083");
  api_qcdbmc.init("ali-qcdbmc-gpn.cern.ch:8083");

  vector<run_specifier> list_full = cfg.get_full_list();
  for(auto r : list_full) download_histos(cfg, hlst, r);

  // download the reference run, as it might not be a part of the ticket
  if(cfg.get_compare() == "runs") download_histos(cfg, hlst, cfg.get_ref_run(grm));

  cout << "Done\n\n";
  return;
}