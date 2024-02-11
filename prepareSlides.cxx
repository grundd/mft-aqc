// MFT asynchronous Quality Control
// prepareSlides.cxx
// David Grund, Oct 9, 2023

// root
#include "TSystem.h"
// my headers
#include "include/utils.h"
#include "include/loadConfiguration.h"
#include "include/loadGlobalRunMap.h"
#include "include/prepareSlides.h"

void createSummaryRunComparison()
{
    string path = Form("../plots/%s/",_group.data());
    string s = Form("latex/period_summaries/summary_%s.tex",_group.data());
    ofstream ofs(s.data());

    printRunList(ofs);
    // all runs in one plot; two histos per slide
    const int N = 6;
    string histos[N] = {
        "mMFTTrackEta_5_minClusters",
        //"mMFTTrackEta_6_minClusters",
        "mMFTTrackPhi_5_MinClusters",
        //"mMFTTrackPhi_6_MinClusters",
        "mMFTTrackInvQPt",
        "mMFTTrackTanl",
        //"mMFTTrackROFSize",
        //"mClustersROFSize",
        "mMFTTrackNumberOfClusters",
        "mMFTTrackChi2"
    };
    int nSl = N / 2;
    int rest = N % 2;
    if(rest > 0) nSl++;
    for(int i = 0; i < nSl; i++) {
        ofs << R"(\frame{)" << "\n";
        ofs << R"(\frametitle{)";
        if(_singlePeriod || _singlePass) {
            if(_singlePeriod && _singlePass) ofs << R"(\period~pass:)";
            else if(_singlePeriod) ofs << R"(\period:)";
            else if(_singlePass)   ofs << R"(\pass:)";
            ofs << "comparison of the runs} \n\n";
        } 
        else ofs << "Comparison of the runs} \n\n";
        float h = 58;
        float x_min = 4;
        float space = 66;
        float y_upp = 13;
        printFigure(ofs,path,histos[2*i],getFiletype(histos[2*i]),h,x_min,y_upp);
        printFigure(ofs,path,histos[2*i+1],getFiletype(histos[2*i]),h,x_min+space,y_upp);
        printFigure(ofs,path,"leg","pdf",h,x_min+2*space,y_upp);
        ofs << "}" << "\n\n";
    }
    ofs.close();

    cout << "File " << s << " created.\n";
    return;
}

void createSummaryPassComparison ()
{
    string path = Form("../plots/%s/",_group.data());
    string s = Form("latex/period_summaries/summary_%s.tex",_group.data());
    ofstream ofs(s.data());

    // unique run list
    vector<int> rListUnique = createUniqueVector(runList);
    int nRuns = rListUnique.size();

    printRunList(ofs);
    // one slide per run
    for(int r = 0; r < nRuns; r++) 
    {
        int run = rListUnique.at(r);
        if(isRun("bad",run) || isRun("not participating",run)) continue;
        string fig = Form("%i_leg_",run);
        ofs << R"(\frame{)" << "\n"
            << R"(\frametitle{)" << getRunPeriod(run) << " : " << run;
        string comment = getRunComment(run);
        if(comment != "") ofs << R"(~{\small()" << getRunComment(run) << R"()})";
        ofs << "} \n";
        float h = 37; // mm
        float x_min = 4;
        float space = 40;
        float y_upp = 11.5;
        float y_low = 49;
        printFigure(ofs,path,fig+"mMFTTrackEta_5_minClusters","pdf",h,x_min,y_upp);
        printFigure(ofs,path,fig+"mMFTTrackPhi_5_minClusters","pdf",h,x_min+space,y_upp);
        printFigure(ofs,path,fig+"mMFTTrackInvQPt","pdf",h,x_min+2*space,y_upp);
        printFigure(ofs,path,fig+"mMFTTrackNumberOfClusters","pdf",h,x_min,y_low);
        printFigure(ofs,path,fig+"mMFTTrackROFSize","pdf",h,x_min+space,y_low);
        printFigure(ofs,path,fig+"mClustersROFSize","pdf",h,x_min+2*space,y_low);
        // printFigure(ofs,path,fig+"mMFTTrackEta_6_minClusters","pdf",h,x_min,y_upp);
        // printFigure(ofs,path,fig+"mMFTTrackEta_7_minClusters","pdf",h,x_min+space,y_upp);
        // printFigure(ofs,path,fig+"mMFTTrackEta_8_minClusters","pdf",h,x_min+2*space,y_upp);
        // printFigure(ofs,path,fig+"mMFTTrackPhi_6_minClusters","pdf",h,x_min,y_low);
        // printFigure(ofs,path,fig+"mMFTTrackPhi_7_minClusters","pdf",h,x_min+space,y_low);
        // printFigure(ofs,path,fig+"mMFTTrackPhi_8_minClusters","pdf",h,x_min+2*space,y_low);
        ofs << "}" << "\n\n";
    }
    ofs.close();
    cout << "File " << s << " created.\n";
    return;
}

void prepareSlides(string input)
{
    cout << "\nprepareSlides.cxx: \n";
    gSystem->Exec("mkdir -p latex/period_summaries/");
    gSystem->Exec("mkdir -p latex/period_summaries_final/");
    if(!loadConfigFile(input)) return;
    if(!loadGlobalRunMap()) return;

    createMain();
    if(_singlePass) createSummaryRunComparison();
    else            createSummaryPassComparison();

    cout << "Finished.\n";
    return;
}