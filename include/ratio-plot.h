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
#include "TStyle.h"
#include "TLatex.h"
#include "TLegend.h"

class run_histo
{
  private:
    run_specifier rsp;
    TH1F* histo;
  public:
    run_histo ();
    run_histo (run_specifier r, TH1F* h);
    run_specifier get_rsp () { return rsp; }
    TH1F* get_histo () { return histo; }
};

run_histo::run_histo ():
  rsp(), histo()
{
  // default constructor
}

run_histo::run_histo (run_specifier r, TH1F* h):
  rsp(r), histo(h)
{
  // parameter constructor
}

class ratio_plot
{
  private:
    histogram histo_type;
    bool histo_type_empty;
    run_histo r_ref;
    vector<run_histo> r_arr;
    float range_x[3] = { 0 }; // x_min, x_min_non_zero, x_max
    float range_y[3] = { 0 }; // y_min, y_min_non_zero, y_max
    float range_y_ratio[2] = {0.3, 1.7};
    int text_size_px = 20; // in pixels
    int text_size_px_leg = 17;
    // private methods
    TH1F* load_histo (run_specifier rsp);
    void set_axes (TH1F* h);
  public:
    ratio_plot ();
    void set_histo_type (histogram h);
    void set_r_ref (run_specifier rsp);
    void set_r_arr (vector<run_specifier> v_rsp); 
    void set_ranges (configuration cfg, run_map rm);
    TCanvas* make_plot (configuration cfg, run_map rm, bool debug = false);
    TCanvas* make_legend (configuration cfg, run_map rm, bool debug = false);
};

ratio_plot::ratio_plot ():
  histo_type(), histo_type_empty(true), r_ref(), r_arr()
{
  // default constructor
}

TH1F* ratio_plot::load_histo (run_specifier rsp)
{
  if(histo_type_empty) {
    cout << "histo_type is empty, please specify using set_histo_type()\n";
    return NULL;
  }
  string fname = Form("%s%s/%i_%s.root",ROOT_FILES_FOLDER.data(),rsp.period.data(),rsp.run,rsp.pass.data());
  TFile* f = TFile::Open(fname.data(),"read");
  TH1F* h = (TH1F*)f->Get(histo_type.name_short.data());
  if(h) {
    gROOT->cd();
    TH1F* h_clone = (TH1F*)h->Clone(Form("%s_cl",h->GetName()));
    f->Close();
    //if(h.options.find("rebinROF") != string::npos) h_clone = rebin_rof(h_clone); // TODO: implement rebin_rof()
    h_clone->Scale(1./h_clone->Integral());
    return h_clone;
  } else {
    f->Close();
    cout << histo_type.name_short << " not found for (" << rsp.run << ", " << rsp.pass << ", " << rsp.period << ")\n";
    return NULL;
  }
}

void ratio_plot::set_axes (TH1F* h)
{
    // font sizes
    h->GetXaxis()->SetTitleFont(63);
    h->GetXaxis()->SetTitleSize(text_size_px);
    h->GetYaxis()->SetTitleFont(63);
    h->GetYaxis()->SetTitleSize(text_size_px);
    h->GetXaxis()->SetLabelFont(63);
    h->GetXaxis()->SetLabelSize(text_size_px);
    h->GetYaxis()->SetLabelFont(63);
    h->GetYaxis()->SetLabelSize(text_size_px);
    // other
    h->GetXaxis()->SetDecimals(1);
    h->GetXaxis()->SetTitleOffset(0.95);
    h->GetYaxis()->SetTitleOffset(1.65);
    h->GetYaxis()->SetMaxDigits(3);
    return;
}

void ratio_plot::set_histo_type (histogram h)
{
  histo_type = h;
  histo_type_empty = false;
  return;
}

void ratio_plot::set_r_ref (run_specifier rsp)
{ 
  r_ref = run_histo(rsp,load_histo(rsp));
  return;
}

void ratio_plot::set_r_arr (vector<run_specifier> v_rsp)
{
  for(auto rsp : v_rsp) {
    r_arr.push_back(run_histo(rsp,load_histo(rsp)));
  }
  return;
}

void ratio_plot::set_ranges (configuration cfg, run_map rm)
{
  TH1F* h_ref = r_ref.get_histo();
  if(h_ref == NULL || r_arr.size() == 0) {
    cout << "Cannot set ranges with r_ref or r_arr empty\n";
  } else {
    range_x[0] = h_ref->GetBinLowEdge(1); // x_min
    (range_x[0] < 1e-5) ? range_x[1] = h_ref->GetBinLowEdge(2) : range_x[1] = range_x[0]; // x_min_non_zero
    range_x[2] = h_ref->GetBinLowEdge(h_ref->GetNbinsX()+1); // x_max
    range_y[0] = h_ref->GetMinimum(); // y_min
    range_y[1] = h_ref->GetMinimum(0); // y_min_non_zero (returns min of all non-zero bins)
    range_y[2] = h_ref->GetMaximum(); // y_max
    if(r_arr.size() > 0) {
      for(auto rh : r_arr) {
        TH1F* h = (TH1F*)rh.get_histo();
        bool is_bad = rm.is_run(rh.get_rsp().run, STR_BAD);
        bool change_range = (is_bad == false) || (cfg.get_bad_runs() == "show");
        if(h->GetMinimum() < range_y[0] && change_range == true) range_y[0] = h->GetMinimum();
        if(h->GetMaximum() > range_y[2] && change_range == true) range_y[2] = h->GetMaximum();
      }
    }
  }
  return;
}

TCanvas* ratio_plot::make_plot (configuration cfg, run_map rm, bool debug)
{
  if(histo_type_empty) {
    cout << "Cannot make plot without histo_type specified\n";
    return NULL;
  }

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  TCanvas* c = new TCanvas(histo_type.name_short.data(),"",620,600);
  c->cd();

  TLatex ltx;
  ltx.SetTextFont(63);
  ltx.SetTextSize(text_size_px);
  ltx.SetTextAlign(21);
  ltx.SetNDC();

  // will there be a ratio panel?
  float y_divide = 0.4;
  bool ratio_panel = !(histo_type.options.find("noratio") != string::npos);
  if(!ratio_panel) y_divide = 0.;

  // upper pad: QC distributions
  TPad p1("p1","",0.,y_divide,1.,1.);
  p1.SetTopMargin(0.09); if(!ratio_panel) p1.SetTopMargin(0.053);
  p1.SetBottomMargin(0.); if(!ratio_panel) p1.SetBottomMargin(0.10);
  p1.SetRightMargin(0.035);
  p1.SetLeftMargin(0.11);
  if(debug) p1.SetFillColor(kBlue-10);
  p1.Draw();
  p1.cd();

  // setting the axes
  float x_min_plot = range_x[0];
  float y_min_plot = range_y[0];
  if(histo_type.options.find("logx") != string::npos) { x_min_plot = range_x[1]; p1.SetLogx(); }
  if(histo_type.options.find("logy") != string::npos) { y_min_plot = range_y[1]; p1.SetLogy(); }
  TH1F* h_axis_upp = gPad->DrawFrame(x_min_plot,y_min_plot,range_x[2],range_y[2]*1.05);
  set_axes(h_axis_upp);
  if(!ratio_panel) h_axis_upp->GetXaxis()->SetTitleOffset(1.25);
  h_axis_upp->GetYaxis()->SetTitle("# entries (scaled to 1)");
  TH1F* h_ref = r_ref.get_histo();
  if(!h_ref) {
    cout << "Cannot make plot without h_ref specified\n";
    return NULL;
  }
  h_axis_upp->GetXaxis()->SetTitle(h_ref->GetXaxis()->GetTitle());
  h_axis_upp->Draw("AXIS");

  // plot the reference run
  h_ref->SetLineColorAlpha(kYellow+1,0.5);
  h_ref->SetLineStyle(1);
  h_ref->SetLineWidth(4);
  h_ref->Draw("HIST SAME");

  // plot the distributions in the upper panel
  int i_line = 0;
  for(auto rh : r_arr) {
    int run = rh.get_rsp().run; 
    TH1F* h = (TH1F*)rh.get_histo();
    bool is_bad = rm.is_run(run, STR_BAD);
    bool plot_this = (is_bad == false) || (cfg.get_bad_runs() != "hide");
    if(h && (plot_this == true)) {
      if(h->GetEntries() > 0) {
        if(!is_bad || cfg.get_bad_runs() == "show") {
          Color_t clr = color_table[(int)(i_line / n_styles)] + i_line % n_styles;
          h->SetLineColor(clr);
          h->SetLineStyle(11 + (i_line % n_styles));
          h->SetLineWidth(1);
          i_line++;
        } else if(cfg.get_bad_runs() == "mute") {
          h->SetLineColorAlpha(kRed-3,0.15);
          h->SetLineStyle(1);
          h->SetLineWidth(1);
        }
        h->Draw("HIST SAME");
      }
    }
  }

  // print the title
  float y_ltx = 0.94;
  if(!ratio_panel) y_ltx = 0.965;
  ltx.DrawLatex(0.5,y_ltx,Form("%s: %s", cfg.get_group().data(), histo_type.title.data()));

  gPad->RedrawAxis();

  // lower pad: ratios
  c->cd();
  TPad p2("p1","",0.0,0.0,1.,y_divide);
  TGraph gr_band(4);
  if(ratio_panel) {
    p2.SetTopMargin(0.);
    p2.SetBottomMargin(0.19);
    p2.SetRightMargin(0.035);
    p2.SetLeftMargin(0.11);
    if(histo_type.options.find("logx") != string::npos) p2.SetLogx();
    if(debug) p2.SetFillColor(kRed-10);
    p2.Draw();
    p2.cd();

    TH1F* h_axis_low = gPad->DrawFrame(x_min_plot,range_y_ratio[0],range_x[2],range_y_ratio[1]);
    set_axes(h_axis_low);
    h_axis_low->GetYaxis()->SetTitle(Form("ratio to ref. run %i",r_ref.get_rsp().run));
    h_axis_low->GetXaxis()->SetTitle(h_ref->GetXaxis()->GetTitle());
    h_axis_low->Draw("AXIS");

    // print a band showing the limits
    float band = cfg.get_plot_band();
    if (band > 0) {
      gr_band.SetPoint(0,x_min_plot,1.-band);
      gr_band.SetPoint(1,range_x[2],1.-band);
      gr_band.SetPoint(2,range_x[2],1.+band);
      gr_band.SetPoint(3,x_min_plot,1.+band);
      gr_band.SetFillStyle(1001);
      gr_band.SetFillColorAlpha(kGreen-4,0.20);
      gr_band.Draw("F");
    }

    // draw ratios
    for(auto rh : r_arr) {
      int run = rh.get_rsp().run; 
      bool is_bad = rm.is_run(run, STR_BAD);
      TH1F* h = (TH1F*)rh.get_histo();
      bool plot_this = (is_bad == false) || (cfg.get_bad_runs() != "hide");
      if(h && (plot_this == true)) {
        if(h->GetEntries() > 0) {
          TH1F* h_rat = (TH1F*)h->Clone(Form("%s_rat",h->GetName()));
          h_rat->Scale(1./h_rat->Integral());
          h_rat->Divide(h_ref);
          h_rat->Draw("HIST SAME");
        }
      }
    }

    // print the line at 1.0
    TH1F* h_one = (TH1F*)h_ref->Clone("h_one");
    for(int i = 1; i <= h_one->GetNbinsX(); i++) h_one->SetBinContent(i,1.);
    h_one->SetLineColorAlpha(kYellow+1,0.4);
    h_one->SetLineStyle(1);
    h_one->SetLineWidth(2);
    h_one->Draw("HIST SAME");
  }

  gPad->RedrawAxis();

  return c;
}

TCanvas* ratio_plot::make_legend (configuration cfg, run_map rm, bool debug)
{
  if(histo_type_empty) {
    cout << "Cannot make legend without histo_type specified\n";
    return NULL;
  }

  TCanvas* c = new TCanvas(Form("%s_leg",histo_type.name_short.data()),"",240,600);
  c->cd();
  if(debug) c->SetFillColor(kYellow-10);

  // make the lines bolder
  for(auto rh : r_arr) {
    TH1F* h = (TH1F*)rh.get_histo();
    if(h) h->SetLineWidth(2);
  }

  // create the legend
  float y_top = 0.98;
  int extra_rows = 3;
  
  TH1F* h_ref = r_ref.get_histo();
  if(!h_ref) {
    cout << "Cannot make legend without h_ref specified\n";
    return NULL;
  }

  TLegend l(0,y_top-(r_arr.size() + extra_rows) * 0.027, 0.95, y_top);

  // add the reference run
  l.AddEntry((TObject*)0,"","");
  l.AddEntry(h_ref,Form("%i : %s, %s", 
    r_ref.get_rsp().run, r_ref.get_rsp().period.data(), r_ref.get_rsp().pass.data()),"L");
  l.AddEntry((TObject*)0,"","");

  // add others
  for(auto rh : r_arr) {
    int run = rh.get_rsp().run; 
    string pass = rh.get_rsp().pass; 
    string period = rh.get_rsp().period; 
    TH1F* h = (TH1F*)rh.get_histo();
    bool is_empty = h->GetEntries() > 0;
    bool is_bad = rm.is_run(run, STR_BAD);
    bool is_not_part = rm.is_run(run, STR_NOT_PART);
    if(!is_empty) {
      if (is_bad == false && is_not_part == false) { // not bad and MFT participating
        string text = Form("%i", run);
        if(cfg.get_n_periods_mc() > 1 && pass == "passMC") text += Form(" : %s", shorten_period_name(period).data());
        if(cfg.get_n_passes() > 1 && pass != "passMC") text += Form(" : %s", pass.data());
        l.AddEntry(h, text.data(), "L");
      } else if (is_bad) {
        if(cfg.get_bad_runs() == "hide") l.AddEntry((TObject*)0,Form("%i : bad",run),"");
        else if(cfg.get_bad_runs() == "mute") l.AddEntry(h,Form("%i : bad",run),"L");
      }
    } else {
      if(is_not_part) l.AddEntry((TObject*)0,Form("%i : not part.",run),""); // MFT not part.
      else l.AddEntry((TObject*)0,Form("%i : not available",run),""); // QC objects not available
    }   
  }
  l.SetTextFont(63);
  l.SetTextSize(text_size_px-3);
  l.SetBorderSize(0);
  l.SetFillStyle(0);
  l.SetMargin(0.20);
  l.Draw();

  TLatex ltx;
  ltx.SetTextFont(63);
  ltx.SetTextSize(text_size_px_leg);
  ltx.SetTextAlign(11);
  ltx.SetNDC();
  ltx.DrawLatex(0.04,0.963,Form("Reference run:"));

  return c;
}