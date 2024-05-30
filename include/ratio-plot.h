// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <string>

#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TFile.h"
#include "TROOT.h"

class ratio_plot
{
  private:
    histogram* histo_type;
    TCanvas* canvas;
    TH1F* h_ref;
    TObjArray* h_arr;
    float range_x[3] = { 0 }; // x_min, x_min_non_zero, x_max
    float range_y[3] = { 0 }; // y_min, y_min_non_zero, y_max
    TH1F* load_histo (run_specifier rsp);
  public:
    ratio_plot ();
    void set_histo_type (histogram h) { histo_type = &h; return; }
    void set_h_ref (run_specifier rsp) { h_ref = load_histo(rsp); }
    void set_h_arr (vector<run_specifier> v_rsp); 
    void set_ranges ();
};

ratio_plot::ratio_plot ():
  histo_type(NULL), canvas(NULL), h_ref(NULL), h_arr(NULL)
{
  // default constructor
}

TH1F* ratio_plot::load_histo (run_specifier rsp)
{
  if(histo_type == NULL) {
    cout << "histo_type is empty, please specify using set_histo_type()\n";
    return NULL;
  }
  string fname = Form("%s%s/%i_%s.root",ROOT_FILES_FOLDER.data(),rsp.period.data(),rsp.run,rsp.pass.data());
  TFile* f = TFile::Open(fname.data(),"read");
  TH1F* h = (TH1F*)f->Get(histo_type->name.data());
  if(h) {
    gROOT->cd();
    TH1F* h_clone = (TH1F*)h->Clone(Form("%s_clone",h->GetName()));
    f->Close();
    //if(h.options.find("rebinROF") != string::npos) h_clone = rebin_rof(h_clone); // TODO: implement rebin_rof()
    h_clone->Scale(1./h_clone->Integral());
    return h_clone;
  } else {
    f->Close();
    cout << histo_type->name << " not found for (" << rsp.run << ", " << rsp.pass << ", " << rsp.period << ")\n";
    return NULL;
  }
}

void ratio_plot::set_h_arr (vector<run_specifier> v_rsp)
{
  h_arr = new TObjArray((int)v_rsp.size());
  h_arr->SetOwner();
  for(auto rsp : v_rsp) {
    h_arr->Add(load_histo(rsp));
  }
  return;
}

void ratio_plot::set_ranges ()
{
  if(h_ref == NULL || h_arr == NULL) {
    cout << "Cannot set ranges with h_ref or h_arr empty\n";
  } else {
    range_x[0] = h_ref->GetBinLowEdge(1); // x_min
    (range_x[0] < 1e-5) ? range_x[1] = h_ref->GetBinLowEdge(2) : range_x[1] = range_x[0]; // x_min_non_zero
    range_x[2] = h_ref->GetBinLowEdge(h_ref->GetNbinsX()+1); // x_max
    range_y[0] = h_ref->GetMinimum(); // y_min
    range_y[1] = h_ref->GetMinimum(0); // y_min_non_zero (returns min of all non-zero bins)
    range_y[2] = h_ref->GetMaximum(); // y_max
    // TODO: loop over h_arr and check y_min and y_max
  }
  return;
}