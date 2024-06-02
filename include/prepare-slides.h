// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <string>
#include <fstream>

void print_run_list (ofstream& ofs, configuration cfg, run_map rm)
{
  bool single_period = cfg.get_n_periods() == 1;
  bool single_pass = cfg.get_n_passes() == 1;
  string jira = cfg.get_jira();
  if(single_period) ofs << R"(\renewcommand{\period}{)" << cfg.get_period_list()[0] << "}\n";
  if(single_pass) ofs << R"(\renewcommand{\pass}{)" << cfg.get_pass_list()[0] << "}\n";
  if(jira != "") ofs << R"(\renewcommand{\jira}{)" << jira << "}\n\n";

  // print the runs, n_runs_per_slide
  vector<int> runs_to_print = cfg.get_run_list();
  bool ref_printed = false;

  // loop over all runs
  while (runs_to_print.size() > 0) 
  {
    ofs << R"(\frame[t]{)" << "\n"
        << R"(\frametitle{)";

    // print the title
    if (single_period || single_pass) {
      if (single_period && single_pass) ofs << R"(\period~\pass)";
      else if (single_period) ofs << R"(\period)";
      else if (single_pass) ofs << R"(\pass)";
      if (jira != "") ofs << R"(: \href{https://alice.its.cern.ch/jira/browse/)" << jira << "}{" << jira << "}";
    } else if (jira != "") ofs << R"(\href{https://alice.its.cern.ch/jira/browse/)" << jira << "}{" << jira << "}";
    string s_run = "run";
    if(runs_to_print.size() > 1) s_run = "runs";
    ofs << " (" << cfg.get_n_runs() << " " << s_run << ")}" << "\n\n"
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
      if(run == cfg.get_ref_run(rm).run) {
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
    if(runs_to_print.empty() && ref_printed == false) {
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
  ofstream ofs(cfg.get_latex_fname_main().data());
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
\newcommand{\period}{}
\newcommand{\jira}{}
\newcommand{\pass}{}
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
    ofs << "\n" << R"(\include)" << Form("{%s}\n",cfg.get_latex_fname_list()[r].data());
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
  bool exists = !gSystem->AccessPathName(Form("%s%s.pdf", path.data(), name.data()));
  if(exists) {
    ofs << R"(\begin{textblock*}{0mm}()" << x << "mm," << y << "mm)\n"
        << R"(\includegraphics[height=)" << h << "mm]{" << path << name << ".pdf}\n"
        << R"(\end{textblock*})" << "\n";
    return true;
  } else return false;
}

// void create_slides(configuration cfg, run_map rm)
// {
//   // one latex subfile for each round
//   for (int r = 0; r < cfg.get_n_rounds(); r++) 
//   {
//     ofstream ofs(cfg.get_latex_fname_list()[r].data());
//   }
//   return;
// }