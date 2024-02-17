
string getFiletype (string h) 
{
    if(h == "mClustersROFSize") return "pdf"; // or png
    else return "pdf";
}

void createMain()
{
    string s = "";
    if(_singlePeriod == true && _singlePass == true) s = Form("latex/%s_%s.tex",_period.data(),_pass.data());
    else s = Form("latex/%s.tex",_group.data());
    ofstream ofs(s.data());
    ofs << R"(
\documentclass[12pt,xcolor={dvipsnames}]{beamer}
\geometry{paperwidth=160mm,paperheight=90mm}
% packages
\usepackage{amsmath,amsfonts,amssymb,mathtools}
\usepackage[absolute,overlay]{textpos}
\usepackage{graphicx}
\usepackage{verbatim}
% hyperlinks
\usepackage{hyperref}
\hypersetup{
    colorlinks=true,    
    urlcolor=blue
}
% logos in the upper right corner:
\usepackage{eso-pic}
\AddToShipoutPictureFG{
    \put(145mm,81mm){\includegraphics[height=8mm,keepaspectratio]{logo/mft.png}}
    \put(152mm,81mm){\includegraphics[height=8mm,keepaspectratio]{logo/alice.pdf}}
}
% shortcuts
\usepackage{xspace}
\newcommand{\ra}{$\rightarrow$~}
\newcommand{\Ra}{$\Rightarrow$~}
% style
\usetheme{Madrid}
\usecolortheme{beaver}
\setbeamertemplate{navigation symbols}{}
\setbeamertemplate{enumerate items}[square]
\setbeamertemplate{itemize items}{\normalsize$\bullet$}
% title page info
\title)";
    if(_singlePeriod == true && _singlePass == true) ofs << Form("[MFT A-QC: %s of %s]{MFT A-QC: %s of %s}",_pass.data(),_period.data(),_pass.data(),_period.data());
    else ofs << Form("[MFT A-QC: %s]{MFT A-QC: %s}",_jira.data(),_jira.data());
    ofs << R"(
\author[D. Grund for the MFT team]{David Grund on behalf of the MFT team}
\institute[]{}
\date[\today]{\today}
% quality tags
\newcommand{\Good}{\textcolor{OliveGreen}{\textbf{GOOD}}}
\newcommand{\Mixed}{\textcolor{Orange}{\textbf{MIXED}}}
\newcommand{\Limited}{\textcolor{Brown}{\textbf{Limited}}}
\newcommand{\Bad}{\textcolor{Red}{\textbf{BAD}}}
\newcommand{\NotPart}{\textbf{not participating}}
\newcommand{\RefRun}[1]{\colorbox{Goldenrod}{\textcolor{Black}{#1}}}
% variables
\newcommand{\period}{}
\newcommand{\jira}{}
\newcommand{\pass}{}

\begin{document}

%\frame{\titlepage}
\include)";
    if(_singlePeriod == true && _singlePass == true) ofs << Form("{period_summaries_final/summary_%s.tex}\n",_group.data());
    else ofs << Form("{period_summaries_final/summary_%s.tex}\n",_group.data());
    ofs << R"(\end{document})";
    ofs.close();

    cout << "File " << s << " created.\n";
    return;
}

template<class T>
vector<T> createUniqueVector (vector<T>* v)
{
    vector<T> copy = *v;
    sort( copy.begin(), copy.end() );
    copy.erase( unique( copy.begin(), copy.end() ), copy.end() );
    return copy;
}

void printRunList (ofstream& ofs)
{
    // options to set:
    int runsPerSlide = 18;
    // # runs in total:
    vector<int> rListUnique = createUniqueVector(runList);
    int nRuns = rListUnique.size();
    // commands
    if(_singlePeriod) ofs << R"(\renewcommand{\period}{)" << _period << "}\n";
    if(_singlePass)   ofs << R"(\renewcommand{\pass}{)" << _pass << "}\n";
    ofs << R"(\renewcommand{\jira}{)" << _jira << "}\n\n";
    // calculate the # of slides:
    int nSl = nRuns / runsPerSlide;
    int rest = nRuns % runsPerSlide;
    if(rest > 0) nSl++;
    int runsToPrint = nRuns;
    int runsPrinted = 0;
    // ref run
    bool refPrinted = false;
    if(!_singlePass) refPrinted = true; // no reference for a pass comparison
    // create the slides
    for(int iSl = 0; iSl < nSl; iSl++) {
        // slide with run numbers 
        string sRun = "runs";
        if(nRuns == 1) sRun = "run";
        ofs << R"(\frame[t]{)" << "\n";
        ofs << R"(\frametitle{)";
        if(_singlePeriod || _singlePass) {
            if(_singlePeriod && _singlePass) ofs << R"(\period~\pass)";
            else if(_singlePeriod) ofs << R"(\period)";
            else if(_singlePass)   ofs << R"(\pass)";
            if(_jira != "none") ofs << R"(: \href{https://alice.its.cern.ch/jira/browse/)" << _jira << "}{" << _jira << "}";
        }
        else if(_jira != "none") ofs << R"(\href{https://alice.its.cern.ch/jira/browse/)" << _jira << "}{" << _jira << "}";
        ofs << " (" << nRuns << " " << sRun << ")}" << "\n\n"
            << R"(\vspace{-2mm})" << "\n"
            << R"(\bgroup\scriptsize\def\arraystretch{1.05})" << "\n";
        if(_singlePeriod) {
            ofs << R"(\begin{tabular}{c c c c l})" << "\n"
                << R"(\textbf{fill} & \textbf{run} & \textbf{IR (kHz)} & \textbf{coll bcs} & \textbf{MFT quality} \\ )" << "\n";
        } else {
            ofs << R"(\begin{tabular}{c c c c c l})" << "\n"
                << R"(\textbf{period} & \textbf{fill} & \textbf{run} & \textbf{IR (kHz)} & \textbf{coll bcs} & \textbf{MFT quality} \\ )" << "\n";
        }
        bool printRefExplanation = false;
        if(runsToPrint > 0) {
            int nFor = runsPerSlide;
            if(runsToPrint < runsPerSlide) nFor = runsToPrint;
            for(int iRun = runsToPrint; iRun < runsToPrint+nFor; iRun++) {
                string quality = "";
                int run = rListUnique.at(runsPrinted);
                if(isRun("good",run)) quality = R"(\Good)";
                if(isRun("mixed",run)) quality = R"(\Mixed)";
                if(isRun("limited",run)) quality = R"(\Limited)";
                if(isRun("bad",run)) quality = R"(\Bad)";
                if(isRun("not participating",run)) quality = R"(\NotPart)";
                string comment = getRunComment(run);
                if(!_singlePeriod) ofs << getRunPeriod(run) << " & ";
                ofs << getRunFill(run) << " & ";
                if(run == _refRun ) {
                    ofs << R"(\RefRun{)" << run << "*} & ";
                    refPrinted = true;
                    printRefExplanation = true;
                } else ofs << run << " & ";
                ofs << getRunIntRate(run) << " & "
                    << getRunCollBcs(run) << " & "
                    << quality;
                if(comment != "") ofs << R"(~{\scriptsize()" << comment << ")}";
                ofs << R"(\\)" << "\n";
                runsPrinted++;
            }
            runsToPrint -= nFor;
        }
        ofs << R"(\end{tabular})" << "\n\n";
        if(printRefExplanation) {
            ofs << R"(\begin{textblock*}{144mm}(8mm,81mm))" << "\n"
                << R"({\footnotesize * selected as a reference})" << "\n"
                << R"(\end{textblock*})" << "\n\n";
        }
        if(iSl == nSl-1 && refPrinted == false) {
            ofs << R"(\begin{textblock*}{144mm}(8mm,81mm))" << "\n"
                << R"({\footnotesize\textbf{Reference run}: \RefRun{)" << _refRun << "} "
                << "(" << getRunPeriod(_refRun);
            if(!(_singlePass == true && _refPass == _pass)) ofs << ", " << _refPass;
            ofs << ", IR = " << getRunIntRate(_refRun) << " kHz"
                << ", " << getRunCollBcs(_refRun) << " coll bcs)}" << "\n"
                << R"(\end{textblock*})" << "\n\n";
        }
        ofs << R"(\egroup})" << "\n\n";
    }
    return;
}

bool printFigure (ofstream& ofs, string path, string fig, string ftype,
    float height, float x1, float y1)
{
    // skip the plot if it has not been created
    // otherwise, latex throws an error
    bool figExists = !gSystem->AccessPathName(Form("plots/%s/%s.%s",_group.data(),fig.data(),ftype.data()));
    if(figExists) {
        ofs << R"(\begin{textblock*}{0mm}()" << x1 << "mm," << y1 << "mm)\n"
            << R"(\includegraphics[height=)" << height << "mm]{" << path << fig << "." << ftype << "}\n"
            << R"(\end{textblock*})" << "\n";
        return true;
    } else return false;
}