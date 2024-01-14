// MFT asynchronous Quality Control
// loadConfiguration.h
// David Grund, Oct 5, 2023

#include <sstream>

// parameters to be configured from the input file:
string _group = "none";
string _jira = "none";
string _period = "none";
string _periodMC = "none";
string _pass = "none";
vector<string>* passes = new vector<string>;
int nPasses = -1;
int _refRun = -1;
string _refPass = "none";
string _badRuns = "hide"; // or 'mute' or 'show'
bool _singlePeriod = true; // false if(_period == "more")
bool _singlePass = true; // false if(_pass == "more") or if more than 1 pass provided
bool _comparePasses = false;
bool _rewriteFiles = false;
bool _oldPath = false;
bool _plotBand20Pc = true;
// run list to be loaded from the input file:
int _nRuns = 0;
vector<int>* runList = new vector<int>;
vector<string>* passList = new vector<string>;

// convert loaded parameters from the input file to the actual settings
bool loadParameter (string key, string value) 
{
    if(key == "group") _group = value;
    if(key == "jira") _jira = value;
    if(key == "period") {
        _period = value;
        if(_period == "more") _singlePeriod = false;
    } 
    if(key == "periodMC") _periodMC = value;
    if(key == "pass") {
        _pass = value;
        if(_pass == "more") _singlePass = false;
        else if(_pass.find(",") != string::npos) {
            _comparePasses = true;
            _singlePass = false;
            istringstream iss(_pass);
            string p;
            while(getline(iss,p,',')) {
                p = remove_spaces(p);
                if(p.empty()) return false;
                else passes->push_back(p);
            }
            //for(int i = 0; i < passes->size(); i++) cout << passes->at(i) << "\n";
            //cout << passes->size() << "\n";
            nPasses = passes->size();
        } else nPasses = 1;
    }
    if(key == "refRun") _refRun = stoi(value);
    if(key == "refPass") _refPass = value;
    if(key == "badRuns") _badRuns = value;
    if(key == "rewriteFiles") _rewriteFiles = to_bool(value);
    if(key == "oldPath") _oldPath = to_bool(value);
    if(key == "plotBand20Pc") _plotBand20Pc = to_bool(value);
    return true;
}

bool checkConfig ()
{
    if(_comparePasses) _refRun = -1;
    // check that all the critical parameters were loaded
    bool success = true;
    string param = "";
    if(_period == "none") { success = false; param = "period"; }
    else if(_pass == "none") { success = false; param = "pass"; }
    else if(_refRun == -1 && _comparePasses == false) { success = false; param = "refRun"; }
    else if(_refPass == "none") { 
        if(_singlePass) {
            _refPass = _pass;
            cout << "NOTE: Reference pass not specified, taking the global pass.\n";
        } else { success = false; param = "refPass"; }
    }
    if(!success) {
        cout << "Parameter '" << param << "' not set properly.\n";
        return false;
    } else return true;
}

// load configuration from the input file
bool loadConfigFile (string fname, bool verbose = false) 
{
    ifstream f(fname);
    if(f.is_open()) {
        string line;
        bool loadingParams = true;
        while(getline(f,line)) {
            istringstream iss(line);
            if(remove_spaces(line) == "runs:") {
                loadingParams = false;
                continue;
            }
            if(loadingParams) { // loading the parameters
                string key;
                if(getline(iss,key,'=')) {
                    string value;
                    if(getline(iss,value)) {
                        value = remove_spaces(value);
                        if(value.empty()) continue; // protection against empty values (or only spaces)
                        if(verbose) cout << "Setting: " << key << " -> " << value << "\n";
                        if(!loadParameter(key,value)) {
                            cout << "Error setting " << key << " !\n";
                            return false;
                        }
                    } 
                }
            } else { // loading the run list
                // single pass option
                if(_singlePass) { 
                    string run;
                    iss >> run; 
                    if(verbose) cout << run << "\n";
                    runList->push_back(stoi(run));
                    passList->push_back(_pass);
                // 'more' passes -> pass specified for each run
                } else if (_pass == "more") {
                    string run;
                    if(getline(iss,run,',')) {
                        string pass;
                        if(getline(iss,pass)) {
                            if(verbose) cout << run << ", " << pass << "\n";
                            pass = remove_spaces(pass);
                            if(pass.empty()) {
                                cout << "'Arbitrary passes' requested but pass not specified for " << run << "!\n"; 
                                return false;
                            }
                            runList->push_back(stoi(run));
                            passList->push_back(pass);
                        } else {
                            cout << "'Arbitrary passes' requested but pass not specified for " << run << "!\n"; 
                            return false;
                        }
                    }
                // several passes specified (up to 3)
                } else {
                    string run;
                    iss >> run; 
                    for(int i = 0; i < nPasses; i++) {
                        if(verbose) cout << run << ", " << passes->at(i) << "\n";
                        runList->push_back(stoi(run));
                        passList->push_back(passes->at(i));
                    }
                }
                _nRuns++;
            }
        }
    } else {
        cout << "Could not open the config file " << fname << "!\n"; 
        return false;
    }
    f.close();
    // set the group name
    if(_group == "none") {
        if(_singlePeriod == true && _singlePass == true) _group = Form("%s_%s",_period.data(),_pass.data());
        else if(_jira != "none") _group = Form("%s",_jira.data());
        else {
            cout << "Specify group name!\n";
            return false;
        }
    }
    if(checkConfig()) {
        // print the loaded configuration
        int nRunPass = runList->size();

        cout << "*************************\n" 
            << "Configuration successful:\n" << std::boolalpha
            << " group:    " << _group << "\n"
            << " jira:     " << _jira << "\n"
            << " period:   " << _period << "\n"
            << " period MC " << _periodMC << "\n"
            << " pass:     " << _pass << "\n"
            << " ref run:  " << _refRun << "\n"
            << " ref pass: " << _refPass << "\n"
            << " bad runs: " << _badRuns << "\n"
            << " single period?  " << _singlePeriod << "\n"
            << " single pass?    " << _singlePass << "\n"
            << " compare passes? " << _comparePasses << "\n"
            << " rewrite files?  " << _rewriteFiles << "\n"
            << " old QC path?    " << _oldPath << "\n"
            << " band at 20%?    " << _plotBand20Pc << "\n"
            << " # runs:         " << _nRuns << "\n"
            << " # run-pass:     " << nRunPass << "\n";         
        for(int i = 0; i < nRunPass; i++) cout << Form("  %03i -> ",i+1) << runList->at(i) << "\n";
        cout << "*************************\n";
        return true;
    } else {
        cout << "Configuration incomplete!\n";
        return false;
    }
}