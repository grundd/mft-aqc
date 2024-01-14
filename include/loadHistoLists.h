// MFT asynchronous Quality Control
// loadHistoLists.h
// David Grund, Oct 5, 2023

// erase everything in the name of a histo up to the last slash, '/'
string renameHisto (string oldName)
{
    string newName = oldName;
    char slash = '/';
    size_t index = newName.find_last_of(slash);
    if (index < string::npos) {
        // erase everything up to 'index'
        newName.erase(0,index+1);
    }
    return newName;
}

vector<string>* loadHistoList (string fname, string path, bool verbose = false)
{
    vector<string>* histoList = new std::vector<string>;
    ifstream f(fname);
    if(f.is_open()) {
        string line;
        while(getline(f,line)) {
            line = remove_spaces(line);
            histoList->push_back(path+line);
            if(verbose) cout << path << line << "\n";
        }
    } else {
        cout << "Could not open " << fname << "!\n"; 
        return NULL;
    }
    if(verbose) {
        cout << "\nLoaded list of histograms to download:\n";
        for(int i = 0; i < histoList->size(); i++) cout << histoList->at(i) << "\n"; 
    }
    f.close();
    return histoList;
}