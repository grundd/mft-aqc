// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <string>
#include <fstream>

void print_run_list (ofstream& ofs, configuration cfg, run_map rm)
{
  bool single_period = cfg.get_n_periods() == 1;

  // print the runs, n_runs_per_slide
  vector<int> runs_to_print = cfg.get_run_list();
  bool ref_printed = false;

  // loop over all runs
  while (runs_to_print.size() > 0)
  {
    string str_run = "run";
    if(runs_to_print.size() > 1) str_run = "runs";
    ofs << R"(\frame[t]{)" << "\n"
        << R"(\frametitle{)" << cfg.get_latex_title() 
        << " (" << cfg.get_n_runs() << " " << str_run << ")}\n\n"
        << R"(\vspace{-2mm})" << "\n";

    // print the table
    ofs << R"(\bgroup\scriptsize\def\arraystretch{1.05})" << "\n";
    if(single_period) {
      ofs << R"(\begin{tabular}{c c c c l})" << "\n"
          << R"(\textbf{fill} & \textbf{run} & \textbf{IR (kHz)} & \textbf{coll bcs} & \textbf{MFT quality} \\)" << "\n";
    } else {
      ofs << R"(\begin{tabular}{c c c c c l})" << "\n"
          << R"(\textbf{period} & \textbf{fill} & \textbf{run} & \textbf{IR (kHz)} & \textbf{coll bcs} & \textbf{MFT quality} \\)" << "\n";
    }

    bool print_ref_legend = false;
    bool print_more = true;
    int n_printed = 0;

    // loop over runs to be printed on this slide
    while (print_more)
    {
      // get the run info
      int run = runs_to_print.front();
      string quality = "";
      if(rm.is_run(run, STR_GOOD)) quality = R"(\Good)";
      if(rm.is_run(run, STR_MIXED)) quality = R"(\Mixed)";
      if(rm.is_run(run, STR_LIM)) quality = R"(\Limited)";
      if(rm.is_run(run, STR_BAD)) quality = R"(\Bad)";
      if(rm.is_run(run, STR_NOT_PART)) quality = R"(\NotPart)";
      string period = rm.get_run_period(run);
      string fill = rm.get_run_fill(run);
      string comment = rm.get_run_comment(run);
      string int_rate = rm.get_run_int_rate(run);
      string coll_bcs = rm.get_run_coll_bcs(run);

      // print the run info
      if(!single_period) ofs << period << " & ";
      ofs << fill << " & ";
      if(cfg.get_compare() == "runs" && (run == cfg.get_ref_run(rm).run)) {
        ofs << R"(\RefRun{)" << run << "*} & ";
        ref_printed = true;
        print_ref_legend = true;
      } else ofs << run << " & ";
      ofs << int_rate << " & "
          << coll_bcs << " & "
          << quality;
      if(comment != "") ofs << R"(~{\scriptsize()" << comment << ")}";
      ofs << R"(\\)" << "\n";

      runs_to_print.erase(runs_to_print.begin());
      n_printed++;
      print_more = (n_printed <= n_runs_per_slide) && runs_to_print.size() > 0;
      
    }
    ofs << R"(\end{tabular})" << "\n\n";

    // print legend for the reference?
    if(print_ref_legend) {
      ofs << R"(\begin{textblock*}{144mm}(8mm,81mm))" << "\n"
          << R"({\footnotesize * selected as a reference})" << "\n"
          << R"(\end{textblock*})" << "\n\n";
    }

    // if we are on the last slide, print the reference details
    if(cfg.get_compare() == "runs" && runs_to_print.empty() && ref_printed == false) {
      run_specifier ref = cfg.get_ref_run(rm);
      ofs << R"(\begin{textblock*}{144mm}(8mm,81mm))" << "\n"
          << R"({\footnotesize\textbf{Reference run}: \RefRun{)" << ref.run << "} "
          << "(" << rm.get_run_period(ref.run)
          << ", " << ref.pass
          << ", IR = " << rm.get_run_int_rate(ref.run) << " kHz"
          << ", " << rm.get_run_coll_bcs(ref.run) << " coll bcs)}" << "\n"
          << R"(\end{textblock*})" << "\n\n";
    }
    ofs << R"(\egroup})" << "\n\n";
  }
  return;
}

void create_main_latex (configuration cfg, run_map rm)
{
  string fname = cfg.get_latex_fname_main();
  bool file_exists = !gSystem->AccessPathName(fname.data());
  if (file_exists && cfg.is_rewrite_latex() == false) {
    cout << "File " << fname << " exists and rewrite_latex==false\n";
    return;
  }
  ofstream ofs(fname.data());
  ofs << R"(
\documentclass[12pt,xcolor={dvipsnames}]{beamer}
\geometry{paperwidth=160mm,paperheight=90mm}
\usepackage{amsmath,amsfonts,amssymb,mathtools}
\usepackage[absolute,overlay]{textpos}
\usepackage{graphicx}
\usepackage{verbatim}
\usepackage{hyperref}
\hypersetup{colorlinks=true,urlcolor=blue}
\usepackage{eso-pic}
\AddToShipoutPictureFG{
    \put(145mm,81mm){\includegraphics[height=8mm,keepaspectratio]{logo/mft.png}}
    \put(152mm,81mm){\includegraphics[height=8mm,keepaspectratio]{logo/alice.pdf}}
}
\usepackage{xspace}
\newcommand{\ra}{\ensuremath{\rightarrow\xspace}}
\newcommand{\Ra}{\ensuremath{\Rightarrow\xspace}}
\newcommand{\Good}{\textcolor{OliveGreen}{\textbf{GOOD}}}
\newcommand{\Mixed}{\textcolor{Orange}{\textbf{MIXED}}}
\newcommand{\Limited}{\textcolor{Brown}{\textbf{Limited}}}
\newcommand{\Bad}{\textcolor{Red}{\textbf{BAD}}}
\newcommand{\NotPart}{\textbf{not participating}}
\newcommand{\RefRun}[1]{\colorbox{Goldenrod}{\textcolor{Black}{#1}}}
\usetheme{Madrid}
\usecolortheme{beaver}
\setbeamertemplate{navigation symbols}{}
\setbeamertemplate{enumerate items}[square]
\setbeamertemplate{itemize items}{\normalsize$\bullet$}

\title)";
  ofs << Form("[MFT A-QC: %s]{MFT A-QC: %s}", cfg.get_latex_title().data(), cfg.get_latex_title().data()) << R"(
\author)" << STR_AUTHOR << R"(
\institute)" << STR_INSTITUTE << R"(
\date)" << STR_DATE << R"(

\begin{document}
%\frame{\titlepage})";

  ofs << "\n\n";
  print_run_list(ofs, cfg, rm);

  for (int r = 0; r < cfg.get_n_rounds(); r++) {
    ofs << "\n" << R"(\include)" << Form("{%s}\n", cfg.get_latex_fname_list()[r].data());
  }
  ofs << R"(\end{document})";
  ofs.close();
  cout << "File " << cfg.get_latex_fname_main() << " created.\n";
  return;
}

bool print_fig (ofstream& ofs, string path, string name, float h, float x, float y)
{
  // skip the plot if it has not been created
  // otherwise, latex throws an error
  string fname = Form("%s%s.pdf", path.data(), name.data());
  bool exists = !gSystem->AccessPathName(fname.data());
  if(exists) {
    ofs << R"(\begin{textblock*}{0mm}()" << x << "mm," << y << "mm)\n"
        << R"(\includegraphics[height=)" << h << "mm]{../" << path << name << ".pdf}\n"
        << R"(\end{textblock*})" << "\n\n";
    return true;
  } else {
    cout << " Plot " << fname << " not found\n";
    return false;
  }
}

void create_slide (ofstream& ofs, string path, string prefix, string title)
{
  ofs << R"(\frame[t]{)" << "\n"
      << R"(\frametitle{)" << title << "}\n\n";
  
  float h = 37; // mm
  float x_min = 4;
  float space = 40;
  float y_upp = 11.5;
  float y_low = 49;

  float coord_x[6] = {x_min, x_min + space, x_min + 2*space, x_min, x_min + space, x_min + 2*space};
  float coord_y[6] = {y_upp, y_upp, y_upp, y_low, y_low, y_low};

  bool all_plotted = true;
  for (int i = 0; i < n_plots_per_slide; i++)
    if(!print_fig(ofs, path, prefix+PLOTS_TO_PPT[i], h, coord_x[i], coord_y[i])) all_plotted = false;

  if(!all_plotted) ofs << R"({\small QC objects not available})" << "\n";
  ofs << "} \n\n";
    
  return;
}

void create_slides (configuration cfg, run_map rm)
{
  // one latex subfile for each round
  int n_rounds = cfg.get_n_rounds();
  for (int r = 0; r < n_rounds; r++) 
  {
    string fname = Form("%s%s", LATEX_FOLDER.data(), cfg.get_latex_fname_list()[r].data());
    bool file_exists = !gSystem->AccessPathName(fname.data());
    if (file_exists && cfg.is_rewrite_latex() == false) {
      cout << "File " << fname << " exists and rewrite_latex==false\n";
      return;
    }

    ofstream ofs(fname.data());
    string path = Form("%s%s/", PLOTS_FOLDER.data(), cfg.get_group().data());

    if (cfg.get_compare() == "runs")
    {
      // create a title
      string title = cfg.get_latex_title();
      if(n_rounds == 1) title.append(": comparison of the runs");
      else title.append(Form(": comparison of the runs %i/%i", r+1, n_rounds));

      string prefix = "";
      if(n_rounds > 1) prefix = Form("%02i_", r+1);
      create_slide(ofs, path, prefix, title);
    }

    if (cfg.get_compare() == "passes")
    {
      for (auto r : cfg.get_run_list())
      {
        // create a title
        string title = cfg.get_latex_title();
        title.append(Form(": %i", r));
        string comment = rm.get_run_comment(r);
        if(comment != "") title += (R"(~{\small()" + comment + R"()})");

        if((rm.is_run(r,STR_BAD) && cfg.get_bad_runs() == "hide") || rm.is_run(r,STR_NOT_PART)) 
        {
          ofs << R"(\frame[t]{)" << "\n"
              << R"(\frametitle{)" << title << "}\n"
              << R"({\small MFT quality: })" << rm.get_run_quality(r) << "\n"
              << "} \n\n";
          cout << "Run BAD and option 'hide' selected or MFT not participating -> skipped\n";
          continue;
        } else {
          string prefix = Form("%i_", r);
          create_slide(ofs, path, prefix, title);
        }
      }
    }

    ofs.close();
    cout << "File " << fname << " created\n";
  }
  return;
}