// MFT asynchronous Quality Control
// downloadQCObjects.cxx
// David Grund, Oct 5, 2023

#include "CCDB/CcdbApi.h"
// my headers
#include "include/utils.h"
#include "include/loadConfiguration.h"
#include "include/loadGlobalRunMap.h"
#include "include/loadHistoLists.h"

o2::ccdb::CcdbApi api_ccdb;
o2::ccdb::CcdbApi api_qcdb;
o2::ccdb::CcdbApi api_test;

long getTimestamp (int runNo, string pass) 
{
    long timestamp = -1; // 1703980800000;
    if(!_timestampMinusOne) 
    {
        map<string, string> hdRCT = api_ccdb.retrieveHeaders("RCT/Info/RunInformation", map<string, string>(), runNo);
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
TH* downloadHisto(string histName, int runNo, string pass, long timestamp, bool verbose = true)
{
    // options to specify:
    string spec_run = to_string(runNo);
    string spec_pas = "";
    string spec_per = "";
    string spec_ver = ""; //"1.119.0"; // (!)
    // evaluate metadata inputs:
    if(pass != "online") spec_pas = pass;
    if(pass == "passMC" && _periodMC != "none") spec_per = _periodMC;
    // create metadata:
    map<string, string> metadata;
    metadata["RunNumber"] = spec_run;
    if(spec_pas != "") metadata["PassName"] = spec_pas;
    if(spec_per != "") metadata["PeriodName"] = spec_per;
    if(spec_ver != "") metadata["qc_version"] = spec_ver;
    // print the used metadata:
    if(verbose) {
        cout << "\nmetadata specifications: \n";
        cout << " -> RunNumber=" << spec_run;
        if(spec_pas != "") cout << ", PassName=" << spec_pas;
        if(spec_per != "") cout << ", PeriodName=" << spec_per;
        if(spec_ver != "") cout << ", qc_version=" << spec_ver;
        cout << "\n";
    }
    TH* h = NULL;
    if(pass == "passMC") h = api_test.retrieveFromTFileAny<TH>(histName,metadata,timestamp);
    else                 h = api_qcdb.retrieveFromTFileAny<TH>(histName,metadata,timestamp);
    return h;
}

void downloadHistos(string period, int runNo, string pass)
{
    gSystem->Exec(Form("mkdir -p rootFiles/%s/",period.data()));
    // check if the file already exists
    string sFile = Form("rootFiles/%s/%i_%s.root",period.data(),runNo,pass.data());
    bool fileExists = !gSystem->AccessPathName(sFile.data());
    if(fileExists && !_rewriteFiles) {
        cout << sFile << ": already downloaded -> skipping...\n";
    } else {
        cout << sFile << ": will be downloaded now.\n";
        long timestamp = getTimestamp(runNo,pass);
        TFile* f = new TFile(sFile.data(),"recreate");
        string sCls;
        string sTrk;
        // online QC
        if(pass == "online") {
            if(_oldPath) {
                sCls = "qc/MFT/MO/MFTAsyncTask/clusters/";
                sTrk = "qc/MFT/MO/MFTAsyncTask/tracks/";
            } else {
                sCls = "qc/MFT/MO/MFTClusterTask/";
                sTrk = "qc/MFT/MO/MFTAsyncTask/";
            }
        // async QC for MC
        } else if(pass == "passMC") {
            if(_oldPath) {
                sCls = "qc_mc/MFT/MO/Tracks/clusters/";
                sTrk = "qc_mc/MFT/MO/Tracks/tracks/";
            } else {
                sCls = "qc_mc/MFT/MO/Clusters/";
                sTrk = "qc_mc/MFT/MO/Tracks/";
            }
        // async QC for data
        } else {
            if(_oldPath) {
                sCls = "qc_async/MFT/MO/Tracks/clusters/";
                sTrk = "qc_async/MFT/MO/Tracks/tracks/";
            } else {
                sCls = "qc_async/MFT/MO/Clusters/";
                sTrk = "qc_async/MFT/MO/Tracks/";
            }
        }
        // track position
        vector<string>* tracks_th2f = loadHistoList("input/QCObjectLists/Tracks_TH2F.txt",sTrk);
        for(int i = 0; i < tracks_th2f->size(); i++) {
            TH2F* h = downloadHisto<TH2F>(tracks_th2f->at(i),runNo,pass,timestamp);
            if(h) {
                cout << "run " << runNo << ", " << pass << ": " << h->GetName() << " loaded\n";
                f->cd();
                h->Write(renameHisto(h->GetName()).data());
            }
        }
        // track kinematics
        vector<string>* tracks_th1f = loadHistoList("input/QCObjectLists/Tracks_TH1F.txt",sTrk);
        for(int i = 0; i < tracks_th1f->size(); i++) {
            TH1F* h = downloadHisto<TH1F>(tracks_th1f->at(i),runNo,pass,timestamp);
            if(h) {
                cout << "run " << runNo << ", " << pass << ": " << h->GetName() << " loaded\n";
                f->cd();
                h->Write(renameHisto(h->GetName()).data());
            }
        }
        // clusters info
        vector<string>* clusters = NULL;
        if(_oldPath) clusters = loadHistoList("input/QCObjectLists/Clusters_old.txt",sCls);
        else         clusters = loadHistoList("input/QCObjectLists/Clusters_new.txt",sCls);
        if(true) {
            for(int i = 0; i < clusters->size(); i++) {
                TH1F* h = downloadHisto<TH1F>(clusters->at(i),runNo,pass,timestamp);
                if(h) {
                    cout << "run " << runNo << ", " << pass << ": " << h->GetName() << " loaded\n";
                    f->cd();
                    h->Write(renameHisto(h->GetName()).data());
                }
            }
        }
        // save the file and terminate
        f->Write("",TObject::kWriteDelete);
        f->Close();
        delete tracks_th2f;
        delete tracks_th1f;
        delete clusters;
    }
    return;
}

void downloadQCObjects (string input = "")
{
    cout << "\ndownloadQCObjects.cxx: \n";
    if(!loadConfigFile(input)) return;
    if(!loadGlobalRunMap()) return;

    // connect to ccdb    
    api_qcdb.init("ali-qcdb-gpn.cern.ch:8083");
    api_test.init("ccdb-test.cern.ch:8080");
    api_ccdb.init("alice-ccdb.cern.ch");

    int nRunPass = runList->size();
    for(int r = 0; r < nRunPass; r++) {
        int runNo = runList->at(r);
        string pass = passList->at(r);
        string period = getRunPeriod(runNo,pass);
        downloadHistos(period,runNo,pass);
    }

    // download also the reference run, in case it is not a part of the ticket
    if(!_comparePasses) downloadHistos(getRunPeriod(_refRun,_refPass),_refRun,_refPass);

    cout << "Done.\n\n";
    return;
}