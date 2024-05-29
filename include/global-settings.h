// MFT Asynchronous Quality Control
// David Grund
// 2024

string SET_QC_VERSION = "";
char csv_delimiter = ',';
string STR_TRUE = "y";
string STR_SOR = "SOR";
string ROOT_FILES_FOLDER = "root_files/";
string PATH_TO_HISTO_LIST = "input/list-of-histograms.csv";
string PATH_TO_GLOBAL_MAP = "input/run_table.csv";
// qc paths: new, old
string PATH_AQC_TRKS[2] = {"qc_async/MFT/MO/Tracks/", "qc_async/MFT/MO/Tracks/tracks/"};
string PATH_AQC_CLS[2] = {"qc_async/MFT/MO/Clusters/", "qc_async/MFT/MO/Tracks/clusters/"};
string PATH_QC_TRKS[2] = {"qc/MFT/MO/MFTAsyncTask/", "qc/MFT/MO/MFTAsyncTask/tracks/"};
string PATH_QC_CLS[2] = {"qc/MFT/MO/MFTClusterTask/", "qc/MFT/MO/MFTAsyncTask/clusters/"};
string PATH_MC_TRKS[2] = {"qc_mc/MFT/MO/Tracks/", "qc_mc/MFT/MO/Tracks/tracks/"};
string PATH_MC_CLS[2] = {"qc_mc/MFT/MO/Clusters/", "qc_mc/MFT/MO/Tracks/clusters/"};