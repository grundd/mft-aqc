// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <string>
#include <fstream>

void create_main_latex (configuration cfg)
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

  for (int r = 0; r < cfg.get_n_rounds(); r++) {
    ofs << "\n" << R"(\include)" << Form("{%s}\n",cfg.get_latex_fname_list()[r].data());
  }
  ofs << R"(\end{document})";
  ofs.close();
  cout << "File " << cfg.get_latex_fname_main() << " created.\n";
  return;
}

void print_run_list (ofstream& ofs)
{
  return;
}