{
    gROOT->Reset();

    #include<iostream>
    #include<fstream>

    double x;

    TCanvas *c1 = new TCanvas("result", "result");

    TH1S *h1 = new TH1S("result", "resuslt", 10, 0, 10);

    ifstream *data;
    data->open("0711_01.dat");

    while(data>>x)h1->Fill(x);

    h1->Draw();

    data->close();
}