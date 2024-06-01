// MFT Asynchronous Quality Control
// David Grund
// 2024

#include <sstream>

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

// remove 'LHC' from the period name
string shorten_period_name(string n) {
  string lhc = "LHC";
  for(int i = 0; i < 3; i++) if(n[0] == lhc[i]) n.erase(0,1);
  return n;
}