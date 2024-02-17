// MFT asynchronous Quality Control
// plotQCObjects.cxx
// David Grund, Oct 6, 2023
// root 'createPlots.cxx("'input/runLists/LHC23zzc_O2-4206.txt'")'

// root headers
#include "TSystem.h"
#include "TROOT.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
// my headers
#include "include/utils.h"
#include "include/loadConfiguration.h"
#include "include/loadGlobalRunMap.h"
#include "include/loadHistoLists.h"
#include "include/style.h"
#include "include/binningROF.h"

// ****
// _badRuns:
// hide => do not show bad runs at all
// mute => show them in transparent red, disregard them in the y-range of the plot
// show => show them as all other runs
// ****

string path_wo_leg;
string path_leg;
string path;

// name, title, options
vector<vector<string>> histos;

bool loadObjectsToPlots(bool print = false)
{
    string fname = "input/objectsToPlot.csv";
	vector<string> row;
	string line, item;
    // open and read the csv
    fstream f(fname,ios::in);
	if(f.is_open()) {
		while(getline(f, line)) {
			row.clear();
			stringstream str(line);
			while(getline(str, item, ',')) row.push_back(item);
            if(row[3] == "y") histos.push_back(row);
		}
	} else {
        cout << "Could not open " << fname << "\n";
        return false;
    }
    // print the loaded values?
    if(print) {
        cout << "The following QC objects will be plotted:\n";
        for(int i = 0; i < histos.size(); i++) {
            for(int j = 0; j < histos[i].size(); j++) cout << histos[i][j] << "\t";
            cout << "\n";
        }
    }
    cout << "List of histograms loaded successfully.\n";
    return true; 
}

string shortenPeriodName(string name) {
    string lhc = "LHC";
    for(int i = 0; i < 3; i++) if(name[0] == lhc[i]) name.erase(0,1);
    return name;
}

TLegend plotLegend(int first, int N, TObjArray* arr, TH1F* hRefRun) 
{
    // create the legend
    float ytop = 0.98;
    int extraRows = 0;
    if(hRefRun != NULL) extraRows = 3;
    TLegend leg(0.0,ytop-(N+extraRows)*0.027,0.95,ytop);
    // add the reference run
    if(hRefRun != NULL) {
        leg.AddEntry((TObject*)0,"","");
        string refPeriod = getRunPeriod(_refRun,_refPass);
        if(_singlePeriod && _period == refPeriod) {
            if(_singlePass) leg.AddEntry(hRefRun,Form("%i",_refRun),"L"); 
            else            leg.AddEntry(hRefRun,Form("%i : %s",_refRun, _refPass.data()),"L"); 
        }
        else leg.AddEntry(hRefRun,Form("%i : %s",_refRun,shortenPeriodName(refPeriod).data()),"L");
        leg.AddEntry((TObject*)0,"","");
    }
    // add others
    for(int r = first; r < first+N; r++) {
        int run = runList->at(r);
        string pass = passList->at(r);
        string passPrint = pass;
        if(pass == "passMC") passPrint = shortenPeriodName(_periodMC);
        if(run == _refRun && pass == _refPass) continue;
        TH1F* h = (TH1F*)arr->At(r-first);
        bool isEmpty = true;
        if(h) if(h->GetEntries() > 0) isEmpty = false;
        bool isBad = isRun("bad",run);
        bool isNotParticipating = isRun("not participating",run);
        if(!isEmpty) {
            if(isBad == false && isNotParticipating == false) { // not bad + mft participating
                if(_singlePeriod && _singlePass) leg.AddEntry(h,Form("%i",run),"L");
                else if(_singlePeriod == false) leg.AddEntry(h,Form("%i : %s",run,shortenPeriodName(getRunPeriod(run,pass)).data()),"L");
                else if(_singlePass == false) leg.AddEntry(h,Form("%i : %s",run,passPrint.data()),"L");
            } else if(isBad) { // bad
                if(_badRuns == "hide") leg.AddEntry((TObject*)0,Form("%i : bad",run),"");
                else if(_badRuns == "mute") leg.AddEntry(h,Form("%i : bad",run),"L");
            }
        } else {
            if(isNotParticipating) leg.AddEntry((TObject*)0,Form("%i : not part.",run),""); // MFT not participating
            else leg.AddEntry((TObject*)0,Form("%i : N/A",run),""); // QC objects not available
        }
    }
    leg.SetTextFont(63);
    leg.SetTextSize(17);
    leg.SetBorderSize(0);
    leg.SetFillStyle(0);
    leg.SetMargin(0.20);
    return leg;
}

void plotComparison(int first, int N, string name, string title, string option, bool debug = false)
{
    if(option.find("skip") != string::npos) {
        cout << "-> skipped...\n";
        return;
    }

    float xMax = 0;
    float xMin = 1e10;
    float xMinNonZero = 1e10;
    float yMax = 0.;
    float yMin = 1e10;
    float yMinNonZero = 1e10;

    int nRuns = runList->size();
    if(nRuns != passList->size()) {
        cout << "Sizes of run and pass lists do not match!\n";
        return;
    }
    if(N > 3*nColors) {
        cout << "Too many runs (max 30).\n";
        return;
    }

    // get the ref run histograms
    if(_comparePasses) _refRun = runList->at(first);
    TH1F* hRefRun = NULL;
    string s = Form("rootFiles/%s/%i_%s.root",getRunPeriod(_refRun,_refPass).data(),_refRun,_refPass.data());
    TFile* f = TFile::Open(s.data(),"read");
    TH1F* h = (TH1F*)f->Get(name.data());
    if(h) {
        gROOT->cd();
        hRefRun = (TH1F*)h->Clone("hRefRun");
        f->Close();
        if(option.find("rebinROF") != string::npos) hRefRun = rebinROF(hRefRun);
        hRefRun->Scale(1./hRefRun->Integral());
        xMax = hRefRun->GetBinLowEdge(hRefRun->GetNbinsX()+1);
        xMin = hRefRun->GetBinLowEdge(1);
        (xMin < 1e-5) ? xMinNonZero = hRefRun->GetBinLowEdge(2) : xMinNonZero = xMin; // effectively: is xMin = 0 ?
        yMax = hRefRun->GetMaximum();
        yMin = hRefRun->GetMinimum();
        yMinNonZero = hRefRun->GetMinimum(0); // return min of all bins with non-zero content
    } else {
        f->Close();
        cout << "Ref histogram not found!\n";
        return;
    }
    // create an array of histograms from the run list
    TObjArray* arr = new TObjArray(N);
    arr->SetOwner();
    for(int r = first; r < first+N; r++) {
        int run = runList->at(r);
        string pass = passList->at(r);
        string period = getRunPeriod(run,pass);
        s = Form("rootFiles/%s/%i_%s.root",period.data(),run,pass.data());
        TFile* f = TFile::Open(s.data(),"read");
        TH1F* h = (TH1F*)f->Get(name.data());
        if(h) {
            gROOT->cd();
            TH1F* hClone = (TH1F*)h->Clone(Form("h%03i",r));
            if(option.find("rebinROF") != string::npos) hClone = rebinROF(hClone);
            hClone->Scale(1./hClone->Integral());
            bool isBad = isRun("bad",run);
            // modify y-axis range?
            bool changeRange = ((!isBad == true) || (_badRuns == "show")) == true;
            // either the run is not bad or the range is allowed to be modified by the bad runs
            if(hClone->GetMaximum() > yMax && changeRange == true) yMax = hClone->GetMaximum();
            if(hClone->GetMinimum() < yMin && changeRange == true) yMin = hClone->GetMinimum();
            arr->AddAt(hClone, r-first);
        }
        f->Close();
    }
    
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    // make the plot
    TCanvas c(name.data(),"",620,600);
    c.cd();

    TLatex* ltx = new TLatex();
    ltx->SetTextFont(63);
    ltx->SetTextSize(20);
    ltx->SetTextAlign(21);
    ltx->SetNDC();

    // will there be a ratio panel?
    float yDivide = 0.4;
    bool ratioPanel = true;
    if(option.find("noratio") != string::npos) ratioPanel = false;
    if(!ratioPanel) yDivide = 0.;

    // upper pad: QC distributions
    TPad* p1 = new TPad("p1","",0.,yDivide,1.,1.);
    p1->SetTopMargin(0.09);  if(!ratioPanel) p1->SetTopMargin(0.053);
    p1->SetBottomMargin(0.); if(!ratioPanel) p1->SetBottomMargin(0.10);
    p1->SetRightMargin(0.035);
    p1->SetLeftMargin(0.11);
    if(debug) p1->SetFillColor(kBlue-10);
    p1->Draw();
    p1->cd();

    // setting the axes
    float xMinPlot = xMin;
    float yMinPlot = yMin;
    if(option.find("logx") != string::npos) { xMinPlot = xMinNonZero; p1->SetLogx(); } // if log scale on x-axis
    if(option.find("logy") != string::npos) { yMinPlot = yMinNonZero; p1->SetLogy(); } // if log scale on y-axis
    TH1F* hAxisUpp = gPad->DrawFrame(xMinPlot,yMinPlot,xMax,yMax*1.05);
    setAxes(hAxisUpp);
    if(!ratioPanel) hAxisUpp->GetXaxis()->SetTitleOffset(1.25);
    hAxisUpp->GetYaxis()->SetTitle("# entries (scaled to 1)");
    hAxisUpp->GetXaxis()->SetTitle(hRefRun->GetXaxis()->GetTitle());
    hAxisUpp->Draw("AXIS");

    // plot the reference run
    hRefRun->SetLineColorAlpha(kYellow+1,0.5);
    hRefRun->SetLineStyle(1);
    hRefRun->SetLineWidth(4);
    hRefRun->Draw("HIST SAME");

    // plot the distributions in the upper panel
    int iLine = 0;
    for(int r = first; r < first+N; r++) {
        int run = runList->at(r);
        string pass = passList->at(r);
        TH1F* h = (TH1F*)arr->At(r-first);
        bool isBad = isRun("bad",run);
        bool plotThis = ((!isBad == true) || (_badRuns != "hide")) == true;  
        if(run == _refRun && pass == _refPass) plotThis = false; // do not plot the ref run here
        if(h && (plotThis == true)) {
            if(h->GetEntries() > 0) {
                if(!isBad || _badRuns == "show") {
                    Color_t clr = colorTable[(int)(iLine / 3)] + iLine % 3;
                    h->SetLineColor(clr);
                    h->SetLineStyle(11 + (iLine % 3));
                    h->SetLineWidth(1);
                    iLine++;
                } else if(_badRuns == "mute") {
                    h->SetLineColorAlpha(kRed-3,0.15);
                    h->SetLineStyle(1);
                    h->SetLineWidth(1);
                }
                h->Draw("HIST SAME");
            }
        }
    }

    // print the title
    float yLatex = 0.94;
    if(!ratioPanel) yLatex = 0.965;
    if(_singlePeriod && _singlePass) ltx->DrawLatex(0.5,yLatex,Form("%s %s: %s",_period.data(),_pass.data(),title.data()));
    else                             ltx->DrawLatex(0.5,yLatex,Form("%s",title.data()));

    gPad->RedrawAxis();

    // lower pad: ratios
    c.cd();
    TPad* p2 = NULL;
    TGraph* gBand = new TGraph(4);
    if(ratioPanel) {
        p2 = new TPad("p2","",0.0,0.0,1.,yDivide);
        p2->SetTopMargin(0.);
        p2->SetBottomMargin(0.19);
        p2->SetRightMargin(0.035);
        p2->SetLeftMargin(0.11);
        if(option.find("logx") != string::npos) p2->SetLogx();
        if(debug) p2->SetFillColor(kRed-10);
        p2->Draw();
        p2->cd();
        float yRatioMin = 0.3;
        float yRatioMax = 1.7;
        TH1F* hAxisLow = gPad->DrawFrame(xMinPlot,yRatioMin,xMax,yRatioMax);
        setAxes(hAxisLow);
        hAxisLow->GetYaxis()->SetTitle(Form("ratio to ref. run %i",_refRun));
        hAxisLow->GetXaxis()->SetTitle(hRefRun->GetXaxis()->GetTitle());
        hAxisLow->Draw("AXIS");

        // print a band showing the limits
        gBand->SetPoint(0,xMinPlot,0.8);
        gBand->SetPoint(1,xMax,0.8);
        gBand->SetPoint(2,xMax,1.2);
        gBand->SetPoint(3,xMinPlot,1.2);
        gBand->SetFillStyle(1001);
        gBand->SetFillColorAlpha(kGreen-4,0.20);
        if(_plotBand20Pc) gBand->Draw("F");

        // draw ratios
        for(int r = first; r < first+N; r++) {
            int run = runList->at(r);
            string pass = passList->at(r);
            if(run == _refRun && pass == _refPass) continue;
            TH1F* h = (TH1F*)arr->At(r-first);
            bool isBad = isRun("bad",run);
            bool plotThis = ((!isBad == true) || (_badRuns != "hide")) == true;
            if(h && (plotThis == true)) {
                if(h->GetEntries() > 0) {
                    TH1F* hRatio = (TH1F*)h->Clone(Form("hRatio%03i",r));
                    hRatio->Scale(1./hRatio->Integral()); // where is the problem ??
                    //hRatio->Sumw2();
                    hRatio->Divide(hRefRun);
                    hRatio->Draw("HIST SAME");
                }
            }
        }

        // print the line at 1.0
        TH1F* hOne = (TH1F*)hRefRun->Clone("hOne");
        for(int i = 1; i <= hOne->GetNbinsX(); i++) hOne->SetBinContent(i,1.0);
        hOne->SetLineColorAlpha(kYellow+1,0.4);
        hOne->SetLineStyle(1);
        hOne->SetLineWidth(2);
        hOne->Draw("HIST SAME");
    }

    gPad->RedrawAxis();

    // print the plot without legend
    if(path_wo_leg != "") c.Print(path_wo_leg.data());

    // print the legend
    TCanvas cLeg(Form("%sLeg",name.data()),"",240,600);
    cLeg.cd();
    if(debug) cLeg.SetFillColor(kYellow-10);
    // make the lines bolder
    for(int r = first; r < first+N; r++) {
        TH1F* h = (TH1F*)arr->At(r-first);
        if(h) h->SetLineWidth(2);
    }
    TLegend leg = plotLegend(first,N,arr,hRefRun);
    leg.Draw();
    TLatex* ltx2 = new TLatex();
    ltx2->SetTextFont(63);
    ltx2->SetTextSize(17);
    ltx2->SetTextAlign(11);
    ltx2->SetNDC();
    ltx2->DrawLatex(0.04,0.963,Form("Reference run:"));
    if(option.find("plotSeparateLegend") != string::npos) {
        if(path_leg != "") cLeg.Print(path_leg.data());
    }

    // print both pads at the same canvas;
    for(int r = first; r < first+N; r++) {
        TH1F* h = (TH1F*)arr->At(r-first);
        if(h) h->SetLineWidth(1);
    }
    TCanvas cBoth(Form("%sBoth",name.data()),"",860,600);
    cBoth.cd();
    TPad pLeft("pLeft","",0.,0.,0.7209,1.);
    if(debug) pLeft.SetFillColor(kMagenta-10);
    pLeft.Draw();
    pLeft.cd();
    c.DrawClonePad();
    cBoth.cd();
    TPad pRight("pRight","",0.7209,0.,1.,1.);
    if(debug) pRight.SetFillColor(kGreen-10);
    pRight.Draw();
    pRight.cd();
    cLeg.DrawClonePad();

    if(path != "") cBoth.Print(path.data());

    delete arr;
    delete p1;
    delete ltx;
    delete gBand;
    delete ltx2;
    if(p2) delete p2;
    return;
}

void plotQCObjects(string input)
{   
    cout << "\nplotQCObjects.cxx: \n";
    if(!loadConfigFile(input)) return;
    if(!loadGlobalRunMap()) return;
    if(!loadObjectsToPlots()) return;

    //gROOT->SetBatch(kTRUE); // https://root-forum.cern.ch/t/saving-canvas-to-root-file-without-displaying-it/6661/2

    gStyle->SetLineStyleString(11,"[]");
    gStyle->SetLineStyleString(12,"16 8");
    gStyle->SetLineStyleString(13,"4 4");

    if(!_comparePasses) {
        cout << "\nRuns will be compared with a reference:\n"
            << " run:    " << _refRun << "\n"
            << " period: " << getRunPeriod(_refRun,_refPass) << "\n"
            << " pass:   " << _refPass << "\n";
    }
    
    gSystem->Exec(Form("mkdir -p plots/%s/",_group.data()));

    // how many runs to plot
    int N = _nRuns;
    if(_comparePasses) N = nPasses;
    int nRunPass = runList->size();
    int nRounds = (int)nRunPass/N;
    cout << "# loops: " << nRounds << "\n"; 

    for(int round = 0; round < nRounds; round++) {
        int first = round * N;
        cout << "\nProcessing: " 
             << "(" << runList->at(first) << ", " << passList->at(first) << ") to "
             << "(" << runList->at(first+N-1) << ", " << passList->at(first+N-1) << ")\n";
        if(_comparePasses) {
            int run = runList->at(first);
            if(isRun("bad",run) || isRun("not participating",run)){
                cout << "Run quality '" << getRunQuality(run) << "' -> skipping.\n";
                continue;
            }
        }
        for(int h = 0; h < histos.size(); h++) 
        {
            string name = histos.at(h).at(0);
            string title = histos.at(h).at(1);
            string option = histos.at(h).at(2);
            cout << " " << name << ":";

            string filetype = "pdf";
            if(name == "mClustersROFSize") filetype = "pdf";

            if(!_comparePasses) {
                path_wo_leg = Form("plots/%s/%s.%s",_group.data(),name.data(),filetype.data());
                path_leg = Form("plots/%s/leg.pdf",_group.data());
                path = Form("plots/%s/leg_%s.%s",_group.data(),name.data(),filetype.data());
            } else {
                path_wo_leg = "";
                path_leg = "";
                path = Form("plots/%s/%i_leg_%s.%s",_group.data(),runList->at(first),name.data(),filetype.data()); 
            }
            bool plotExists = !gSystem->AccessPathName(path.data());
            if(!plotExists || (plotExists && _recreatePlots)) {
                cout << "\n";
                plotComparison(first,N,name, title, option);
            } else cout << " already plotted \n";
        }
    }

    cout << "\nPlotting finished!\n";
    return;
}