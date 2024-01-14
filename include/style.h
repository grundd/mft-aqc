// MFT asynchronous Quality Control
// style.h
// David Grund, Oct 6, 2023

// root headers
#include "TH1.h"

void setAxes(TH1F* h)
{
    // font sizes
    int sizePixels = 20;
    h->GetXaxis()->SetTitleFont(63);
    h->GetXaxis()->SetTitleSize(sizePixels);
    h->GetYaxis()->SetTitleFont(63);
    h->GetYaxis()->SetTitleSize(sizePixels);
    h->GetXaxis()->SetLabelFont(63);
    h->GetXaxis()->SetLabelSize(sizePixels);
    h->GetYaxis()->SetLabelFont(63);
    h->GetYaxis()->SetLabelSize(sizePixels);
    // other
    h->GetXaxis()->SetDecimals(1);
    h->GetXaxis()->SetTitleOffset(0.95);
    h->GetYaxis()->SetTitleOffset(1.65);
    h->GetYaxis()->SetMaxDigits(3);
    return;
}

const int nColors = 10; // base colors available
Color_t colorTable[nColors] = { 
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