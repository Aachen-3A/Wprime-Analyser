#ifndef HistClass_hh
#define HistClass_hh

#include <iostream>
#include <map>
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TNtupleD.h"
#include "boost/format.hpp"

// Note that we need a special treating for h_counters as it mustn't have a standard name like h1_* to comply with other tools as SirPlotAlot.

namespace HistClass {
    static std::map<std::string, TH1D * > histo;
    static std::map<std::string, TH2D * > histo2;
    static std::map<std::string, TNtupleD * > ttupple;
    static std::map<std::string, TTree * > trees;

    static void CreateHisto(Int_t n_histos,const char* name,const char* title, Int_t nbinsx, Double_t xlow, Double_t xup,TString xtitle = ""){
        for(int i = 0; i < n_histos; i++){
            TH1D * tmphist = new TH1D(Form("h1_%d_%s", i, name), xtitle, nbinsx, xlow, xup);
            tmphist->SetXTitle(xtitle);
            tmphist->Sumw2();
            histo[Form("h1_%d_%s", i, name)] = tmphist;
        }
    }

    static void CreateHistoUnchangedName(const char* name,const char* title, Int_t nbinsx, Double_t xlow, Double_t xup,TString xtitle = ""){
            TH1D * tmphist = new TH1D(Form("%s", name), xtitle, nbinsx, xlow, xup);
            tmphist->SetXTitle(xtitle);
            tmphist->Sumw2();
            histo[Form("%s", name)] = tmphist;
    }

    static void Fill(Int_t n_histo,const char * name, double value,double weight)
    {
         std::map<string, TH1D * >::iterator it =histo.find(Form("h1_%d_%s", n_histo, name));
        if(it!=histo.end()){
            it->second->Fill(value,weight);
        }else{
            cout<<"(Fill) No hist: "<<name<<" in map "<<n_histo<<endl;
        }

    }
    static void Fill(Int_t n_histo, string name, double value,double weight)
    {
        Fill( n_histo, name.c_str(), value,weight);
    }

    //static void CreateTree(const char* name, const char* varlist,int bufsize){
        //std::string dummy = Form("tree_%s", name);
        ////std::cout << varlist << std::endl;
        //ttupple[dummy] = new TNtupleD(Form("tree_%s", name),name,varlist,bufsize);
    //}

    static void CreateTree( std::map< std::string , double > & m, const char * name){

        trees[name] = new TTree(name,name);
        for( std::map< std::string , double >::iterator it=m.begin(); it!=m.end();it++){
                trees[name]->Branch(it->first.c_str(),&(it->second),Form("%s/D",it->first.c_str()));
        }
    }

    //static void FillTree(const char * name, double* values)
    //{
        //std::string dummy = Form("tree_%s", name);
        ////std::cout << *values << std::endl;
        //ttupple[dummy]->Fill(values);
    //}
    static void FillTree( const char * name )
    {
        trees[name]->Fill();
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
        TH1D * tmphist = new TH1D(Form("h1_%s", name), xtitle, nbinsx, xlow, xup);
        tmphist->SetXTitle(xtitle);
        tmphist->Sumw2();
        histo[Form("h1_%s", name)] = tmphist;
    }

    const static void CreateHisto(boost::basic_format<char> name, boost::basic_format<char> title, Int_t nbinsx, Double_t xlow, Double_t xup,TString xtitle = "")
    {
        CreateHisto(str(name).c_str(), str(title).c_str(), nbinsx, xlow, xup, xtitle);
    }

    static void Fill(const char * name, double value,double weight)
    {
        //std::map<string, TH1D * >::iterator it =histo.find(Form("h1_%d_%s", nhist, name));
        std::map<string, TH1D * >::iterator it;
        if (name == "h_counters") {
                it =histo.find(Form("%s", name));
        }
        else {
                it =histo.find(Form("h1_%s", name));
        }

        if(it!=histo.end()){
            it->second->Fill(value,weight);
        }else{
            cout<<"(Fill) No hist: "<<name<<" in map "<<endl;
        }
    }

    //   static void Write(const char * name)
    //   {
    //     std::string dummy = Form("h1_%s", name);
    //     histo[dummy]->Write();
    //   }
<<<<<<< HEAD
<<<<<<< HEAD
    
=======

>>>>>>> specialAna.cc: writing file for event display
    static TH1D* ReturnHist(const char * name) {
      std::string dummy="";
      if (name == "h_counters") {
         dummy = Form("%s", name);
      } else {
         dummy = Form("h1_%s", name);
      }
      
      return histo[dummy];
=======
    static TH1D* ReturnHist(const char * name)
    {
        std::string dummy = Form("h1_%s", name);
        return histo[dummy];
>>>>>>> how  shoud I know
    }

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
            for(uint i = 0; i < n_bins; i++) mydisc[i] = d_mydisc[i];
            for(uint i = 0; i < n_bins; i++) {
                histo[dummy]->GetXaxis()->SetBinLabel(i+1,mydisc[i]);
            }
        }
    }

    static void WriteAll(const char * name = "")
    {
        std::map<std::string, TH1D * >::iterator it;
        for (std::map<std::string, TH1D * >::iterator it=histo.begin(); it!=histo.end(); ++it){
            if(strcmp( name, "") != 0 && std::string::npos!=it->first.find(name)){
                it->second -> Write();
            }else if(strcmp( name, "") == 0){
                it->second -> Write();
            }
        }
    }

    static void WriteAllTrees(const char * name = "")
    {
        //std::map<TString, TNtupleD * >::iterator it;
        for (std::map<std::string, TNtupleD * >::iterator it=ttupple.begin(); it!=ttupple.end(); ++it){
            if(strcmp( name, "") != 0 && std::string::npos!=it->first.find(name)){
                it->second -> Write();
            }else if(strcmp( name, "") == 0){
                it->second -> Write();
            }
        }
        //std::map<std::string, TTree * >::iterator it;
        for (std::map<std::string, TTree * >::iterator it=trees.begin(); it!=trees.end(); ++it){
            if(strcmp( name, "") != 0 && std::string::npos!=it->first.find(name)){
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

    //static void Fill(const char * name, double valuex,double valuey,double weight)
    //{
        //std::string dummy = Form("h2_%s", name);
        //histo2[dummy]->Fill(valuex,valuey,weight);
    //}

    //static void Write2(const char * name)
    //{
        //std::string dummy = Form("h2_%s", name);
        //histo2[dummy]->Write();
    //}
}

#endif
