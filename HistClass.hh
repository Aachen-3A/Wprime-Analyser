#ifndef HistClass_hh
#define HistClass_hh

#include <iostream>
#include <map>
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"

namespace HistClass {
  static std::map<TString, TH1D * > histo;
  static std::map<TString, TH2D * > histo2;

   static void CreateHisto(Int_t n_histos,const char* name,const char* title, Int_t nbinsx, Double_t xlow, Double_t xup,TString xtitle = ""){
     for(int i = 0; i < n_histos; i++){
       std::string dummy = Form("h1_%d_%s", i, name);
       histo[dummy] = new TH1D(Form("h1_%d_%s", i, name), title, nbinsx, xlow, xup);
       histo[dummy] -> Sumw2();
       histo[dummy] -> GetXaxis() ->SetTitle(xtitle);
     }
   }
 
   static void Fill(Int_t n_histo,const char * name, double value,double weight)
   {
     std::string dummy = Form("h1_%d_%s", n_histo, name);
     histo[dummy]->Fill(value,weight);
   }
 
//   static void Write(Int_t n_histo,const char * name)
//   {
//     std::string dummy = Form("h1_%d_%s", n_histo, name);
//     histo[dummy]->Write();
//   }
// 
//   static void SetToZero(Int_t n_histo,const char * name)
//   {
//     std::string dummy = Form("h1_%d_%s", n_histo, name);
//     int Nbins2 = histo[dummy] -> GetNbinsX();
//     for ( int bb = 0; bb < Nbins2+1; bb++) {
//       double binValue = histo[dummy] -> GetBinContent(bb);
//       if (binValue < 0) {
//         //cout << "Bin " << bb << "  " << dummy << " is negative: " << binValue << "  and is being set to zero!" << endl;
//         histo[dummy] -> SetBinContent(bb,0.);
//       }
//     }
//   }

  const static void CreateHisto(const char* name,const char* title, Int_t nbinsx, Double_t xlow, Double_t xup,TString xtitle = "")
  {
    std::string dummy = Form("h1_%s", name);
    histo[dummy] = new TH1D(Form("h1_%s", name), title, nbinsx, xlow, xup);
    histo[dummy] -> Sumw2();
    histo[dummy] -> GetXaxis() -> SetTitle(xtitle);
  }

  static void Fill(const char * name, double value,double weight)
  {
    std::string dummy = Form("h1_%s", name);
    histo[dummy]->Fill(value,weight);
  }

//   static void Write(const char * name)
//   {
//     std::string dummy = Form("h1_%s", name);
//     histo[dummy]->Write();
//   }
/*
  static TH1D* ReturnHist(const char * name)
  {
    std::string dummy = Form("h1_%s", name);
    return histo[dummy];*/
//   }

  //static void NameBins(const char * name, const uint n_bins, TString* d_mydisc)
  //{
    //std::string dummy = Form("h1_%s", name);
    //TString mydisc[n_bins];
    //for(uint i = 0; i < n_bins; i++)mydisc[i] = d_mydisc[i];
    //for(uint i = 0; i < n_bins; i++) {
      //histo[dummy]->GetXaxis()->SetBinLabel(i+1,mydisc[i]);
    //}
  //}

  static void NameBins(Int_t n_histo, const char * name, const uint n_bins, TString* d_mydisc)
  {
    for(int i = 0; i < n_histo; i++){
       std::string dummy = Form("h1_%d_%s", i, name);
		   TString mydisc[n_bins];
		   for(uint i = 0; i < n_bins; i++)mydisc[i] = d_mydisc[i];
		   for(uint i = 0; i < n_bins; i++) {
				 histo[dummy]->GetXaxis()->SetBinLabel(i+1,mydisc[i]);
			 }
    }
  }

  static void WriteAll(const char * name = "")
  {
    std::map<TString, TH1D * >::iterator it;
    for (std::map<TString, TH1D * >::iterator it=histo.begin(); it!=histo.end(); ++it){
      if(strcmp( name, "") != 0 && it->first.Contains(name)){
        it->second -> Write();
      }else if(strcmp( name, "") == 0){
        it->second -> Write();
      }
    }
  }

  static void CreateHisto(const char* name,const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup, TString xtitle,TString ytitle)
  {
    std::string dummy = Form("h2_%s", name);
    histo2[dummy] = new TH2D(Form("h2_%s", name), title, nbinsx, xlow, xup, nbinsy, ylow, yup);
    histo2[dummy] -> Sumw2();
    histo2[dummy] -> GetXaxis() -> SetTitle(xtitle);
    histo2[dummy] -> GetYaxis() -> SetTitle(ytitle);
  }

  static void Fill(const char * name, double valuex,double valuey,double weight)
  {
    std::string dummy = Form("h2_%s", name);
    histo2[dummy]->Fill(valuex,valuey,weight);
  }

  static void Write2(const char * name)
  {
    std::string dummy = Form("h2_%s", name);
    histo2[dummy]->Write();
  }
}

#endif
