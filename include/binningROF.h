const int nBinsROF = 40;
const double binningROF[nBinsROF+1] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    15,
    20,
    30,
    40,
    50,
    60,
    70,
    80,
    90,
    100,
    150,
    200,
    300,
    400,
    500,
    600,
    700,
    800,
    900,
    1000,
    1500,
    2000,
    3000,
    4000,
    5000,
    6000,
    7000,
    8000,
    9000,
    10000
};

TH1F* rebinROF(TH1F* hOld) 
{
    TH1F* hNew = new TH1F(Form("%s_new",hOld->GetName()),hOld->GetTitle(),nBinsROF,binningROF);
    hNew->GetXaxis()->SetTitle(hOld->GetXaxis()->GetTitle());
    for(int i = 1; i <= hOld->GetNbinsX(); i++) 
    {
        float content = hOld->GetBinContent(i);
        float center = hOld->GetBinCenter(i);
        int iBin = hNew->FindBin(center);
        hNew->SetBinContent(iBin,hNew->GetBinContent(iBin) + content);
    }
    hNew->Scale(1.,"width");
    delete hOld;
    return hNew;
}