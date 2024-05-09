// MFT Asynchronous Quality Control
// David Grund
// 2024

// root 'downloadQCObjects.cxx("'input/runLists/2024/LHC24aa_O2-4810.txt'")'

#include "CCDB/CcdbApi.h"
// my headers
#include "include/load-list-of-histograms.h"

#include "include/utils.h"
#include "include/loadConfiguration.h"
#include "include/loadGlobalRunMap.h"

o2::ccdb::CcdbApi api_ccdb;
o2::ccdb::CcdbApi api_qcdb;
o2::ccdb::CcdbApi api_qcdbmc;
histogram_list full_hlist;
//configuration config;

long getTimestamp (int run_no, string pass) 
{
    long timestamp = -1; // 1703980800000;
    if(!_timestampMinusOne) 
    {
        map<string, string> hdRCT = api_ccdb.retrieveHeaders("RCT/Info/RunInformation", map<string, string>(), run_no);
        const auto startRCT = hdRCT.find("SOR");
        if (startRCT != hdRCT.end()) {
            timestamp = stol(startRCT->second);
            cout << "SOR found, timestamp: " << timestamp << "\n";
        } else {
            cout << "SOR not found in headers!" << "\n";
        }
    }
    return timestamp;
}

template <typename TH>
TH* downloadHisto(string hname, int run_no, string pass, long timestamp, bool verbose = true)
{
    // options to specify:
    string specify_run = std::to_string(run_no);
    string specify_pass = "";
    string specify_period = "";
    string specify_version = "";
    // evaluate metadata inputs:
    if(pass != "online") specify_pass = pass;
    if(pass == "passMC" && _periodMC != "none") specify_period = _periodMC;
    // create metadata:
    map<string, string> metadata;
    metadata["RunNumber"] = specify_run;
    if(specify_pass != "") metadata["PassName"] = specify_pass;
    if(specify_period != "") metadata["PeriodName"] = specify_period;
    if(SET_QC_VERSION != "") metadata["qc_version"] = specify_version;
    // print the used metadata:
    if(verbose) {
        cout << "\nmetadata specifications: \n";
        cout << " -> RunNumber=" << specify_run;
        if(specify_pass != "") cout << ", PassName=" << specify_pass;
        if(specify_period != "") cout << ", PeriodName=" << specify_period;
        if(specify_version != "") cout << ", qc_version=" << specify_version;
        cout << "\n";
    }
    TH* h = NULL;
    if(pass == "passMC") h = api_qcdbmc.retrieveFromTFileAny<TH>(hname,metadata,timestamp);
    else                 h = api_qcdb.retrieveFromTFileAny<TH>(hname,metadata,timestamp);
    return h;
}

void downloadHistos(string period, int run_no, string pass)
{
    gSystem->Exec(Form("mkdir -p %s%s/",ROOT_FILES_FOLDER.data(),period.data()));
    // check if the file already exists
    string fname = Form("%s%s/%i_%s.root",ROOT_FILES_FOLDER.data(),period.data(),run_no,pass.data());
    bool file_exists = !gSystem->AccessPathName(fname.data());
    if(file_exists && !_rewriteFiles) {
        cout << fname << " : already downloaded -> skipping\n";
    } else {
        cout << fname << " : will be downloaded now\n";
        long timestamp = getTimestamp(run_no,pass);
        TFile* f = new TFile(fname.data(),"recreate");
        string s_cls;
        string s_trks;
        int ver = 0;
        if (_oldPath) ver = 1;
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
        vector<histogram> hlist = full_hlist.get_list_to_download("track","TH2F");
        for(auto hist : hlist) {
            TH2F* h = downloadHisto<TH2F>(s_trks+hist.name,run_no,pass,timestamp);
            if(h) {
                cout << "run " << run_no << ", " << pass << ": " << h->GetName() << " downloaded\n";
                f->cd();
                h->Write(hist.name_short.data());
            }
        }
        // tracks : TH1F
        hlist = full_hlist.get_list_to_download("track","TH1F");
        for(auto hist : hlist) {
            TH1F* h = downloadHisto<TH1F>(s_trks+hist.name,run_no,pass,timestamp);
            if(h) {
                cout << "run " << run_no << ", " << pass << ": " << h->GetName() << " downloaded\n";
                f->cd();
                h->Write(hist.name_short.data());
            }
        }
        // clusters : TH1F
        hlist = full_hlist.get_list_to_download("cluster","TH1F");
        for(auto hist : hlist) {
            TH1F* h = downloadHisto<TH1F>(s_cls+hist.name,run_no,pass,timestamp);
            if(h) {
                cout << "run " << run_no << ", " << pass << ": " << h->GetName() << " downloaded\n";
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

void downloadQCObjects (string input = "")
{
    cout << "\ndownloadQCObjects.cxx: \n";
    if(!loadConfigFile(input)) return;
    if(!loadGlobalRunMap()) return;
    full_hlist.load_from_csv(PATH_TO_HISTO_LIST);

    // connect to ccdb, qcdb, qcdbmc    
    api_ccdb.init("alice-ccdb.cern.ch");
    api_qcdb.init("ali-qcdb-gpn.cern.ch:8083");
    api_qcdbmc.init("ali-qcdbmc-gpn.cern.ch:8083");

    int nRunPass = runList->size();
    for(int r = 0; r < nRunPass; r++) {
        int run_no = runList->at(r);
        string pass = passList->at(r);
        string period = getRunPeriod(run_no,pass);
        downloadHistos(period,run_no,pass);
    }

    // download also the reference run, in case it is not a part of the ticket
    if(!_comparePasses) downloadHistos(getRunPeriod(_refRun,_refPass),_refRun,_refPass);

    cout << "Done.\n\n";
    return;
}