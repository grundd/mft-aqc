// MFT asynchronous Quality Control
// utils.h
// David Grund, Oct 5, 2023

// remove all white spaces at the end of a string
string remove_spaces (string s)
{
    int last = s.size() - 1;
    while (last >= 0 && s[last] == ' ')
    --last;
    return s.substr(0, last + 1);
}

// convert string to bool
bool to_bool (string s) 
{
    transform(s.begin(),s.end(),s.begin(),::tolower);
    istringstream iss(s);
    bool b;
    iss >> std::boolalpha >> b;
    return b;
}