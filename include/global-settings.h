// MFT Asynchronous Quality Control
// David Grund
// 2024

string SET_QC_VERSION = "";
char csv_delimiter = ',';
string STR_TRUE = "y";
string STR_SOR = "SOR";
string STR_GOOD = "good";
string STR_MIXED = "mixed";
string STR_BAD = "bad";
string STR_NOT_PART = "not participating";
string STR_LIM = "limited";
string ROOT_FILES_FOLDER = "root_files/";
string PATH_TO_HISTO_LIST = "input/list-of-histograms.csv";
string PATH_TO_GLOBAL_MAP = "input/run-table.csv";
string PLOTS_FOLDER = "plots/";
string STR_AUTHOR = "[D. Grund for the MFT team]{David Grund on behalf of the MFT team}";
string STR_INSTITUTE = "[]{}";
string STR_DATE = R"([\today]{\today})";
string LATEX_FOLDER = "latex/";
string LATEX_SUBFOLDER = "include/";
// qc paths: new, old
string PATH_AQC_TRKS[2] = {"qc_async/MFT/MO/Tracks/", "qc_async/MFT/MO/Tracks/tracks/"};
string PATH_AQC_CLS[2] = {"qc_async/MFT/MO/Clusters/", "qc_async/MFT/MO/Tracks/clusters/"};
string PATH_QC_TRKS[2] = {"qc/MFT/MO/MFTAsyncTask/", "qc/MFT/MO/MFTAsyncTask/tracks/"};
string PATH_QC_CLS[2] = {"qc/MFT/MO/MFTClusterTask/", "qc/MFT/MO/MFTAsyncTask/clusters/"};
string PATH_MC_TRKS[2] = {"qc_mc/MFT/MO/Tracks/", "qc_mc/MFT/MO/Tracks/tracks/"};
string PATH_MC_CLS[2] = {"qc_mc/MFT/MO/Clusters/", "qc_mc/MFT/MO/Tracks/clusters/"};

const int n_colors = 10; // base colors available
const int n_styles = 3;
Color_t color_table[n_colors] = { 
  kBlue,
  kOrange,
  kCyan,
  kRed,
  kGreen,
  kMagenta,
  kYellow+1,
  kViolet,
  kGray,
  43
};

const int n_runs_per_slide = 18;

const int n_plots_per_slide = 6;
string PLOTS_TO_PPT[6] = {
  "mMFTTrackEta_5_MinClusters",
  "mMFTTrackPhi_5_MinClusters",
  "mMFTTrackInvQPt",
  "mMFTTrackNumberOfClusters",
  "mMFTTrackROFSize",
  "mClustersROFSize"
};