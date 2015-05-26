#ifndef specialAna_hh
#define specialAna_hh

#include <iostream>
#include <string>
#include <unordered_set>
#include <fstream>

///clean up the header!!!
#include "Pxl/Pxl/interface/pxl/core.hh"
#include "Pxl/Pxl/interface/pxl/hep.hh"
#include "Tools/PXL/Sort.hh"
//#include "Tools/Tools.hh"
#include "Tools/MConfig.hh"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TLorentzVector.h"
#include <TFile.h>

#include "Main/Systematics.hh"

//----------------------------------------------------------------------
using namespace std;

//class Systematics;

class specialAna : public pxl::AnalysisProcess  {
public:
    specialAna( const Tools::MConfig &config );
    virtual ~specialAna();

    virtual void endJob(const Serializable*);
    virtual void analyseEvent( const pxl::Event* event );

    TFile* file1;
    ofstream eventDisplayFile;
    std::stringstream eventsAfterCuts;

    void Fill_stage_0_histos( );
    void Fill_Gen_Controll_histo( );
    void Fill_Controll_histo(int hist_number, pxl::Particle* lepton);
    void Fill_Controll_Tau_histo(int hist_number, pxl::Particle* lepton);
    void Fill_Controll_Ele_histo(int hist_number, pxl::Particle* lepton);
    void Fill_Controll_Muon_histo(int hist_number, pxl::Particle* lepton);
    void Fill_Particle_hisos(int hist_number, pxl::Particle* lepton, string syst="");
    void Fill_Gen_Rec_histos(pxl::Particle* genPart,pxl::Particle* recoPart);
    void Fill_Tree();


    void Create_RECO_effs();
    void Create_RECO_object_effs(std::string object);
    void Fill_RECO_effs();
    void Fill_RECO_object_effs(std::string object, int id, std::vector< pxl::Particle* > part_list);

    void FillSystematics(const pxl::Event* event, std::string const particleName);
    void FillSystematicsUpDown(const pxl::Event* event, std::string const particleName, std::string const updown, std::string const shiftType);

    void initEvent( const pxl::Event* event );
    void endEvent( const pxl::Event* event );
    void applyKfactor(const pxl::Event* event, int mode);
    void aplyDataMCScaleFactors();

    bool Check_Muo_ID(pxl::Particle* muon);
    bool Check_Tau_ID(pxl::Particle* tau);
    bool Check_Ele_ID(pxl::Particle* ele);

    void cleanJets();


    void KinematicsSelector();
    bool TriggerSelector(const pxl::Event* event);
    bool triggerKinematics();
    void Create_trigger_effs();
    void TriggerAnalyser();


    bool tail_selector(const pxl::Event* event);

    pxl::Particle* Get_Truth_match(std::string name, pxl::Particle* lepton);
    pxl::Particle* Get_Truth_match_all_flavor(pxl::Particle* lepton);
    pxl::Particle* Get_tau_truth_decay_mode(pxl::Particle* truth_tau);
    double DeltaPhi(double a, double b);
    double DeltaPhi(pxl::Particle* lepton, pxl::Particle* met);
    double DeltaR(pxl::Particle* part_j, pxl::Particle* part_i);
    double MT(pxl::Particle* lepton, pxl::Particle* met);
    double Mass(pxl::Particle* p1, pxl::Particle* p2);
    double getPtHat();
    double getGenHT();
    double getWmass();
    int vetoNumber(vector< pxl::Particle* > *list, double ptTreshold);


    pxl::EventView *m_RecEvtView;
    pxl::EventView *m_GenEvtView;
    pxl::EventView *m_TrigEvtView;

    bool runOnData;
    string const m_JetAlgo, m_BJets_algo, m_METType, m_TauType;


    const double    m_pt_met_min_cut_ele,m_pt_met_max_cut_ele,m_delta_phi_cut_ele,m_pt_met_min_cut_muo,m_pt_met_max_cut_muo,m_delta_phi_cut_muo,m_pt_met_min_cut_tau,m_pt_met_max_cut_tau,m_delta_phi_cut_tau,m_pt_min_cut_ele,m_pt_min_cut_muo,m_pt_min_cut_tau;

    const std::string particles[3] = {"Ele", "Muon", "Tau"};
    const std::string particleSymbols[3] = {"e", "#mu", "#tau"};
    const std::string shifted[5]   = {"Ele", "Muon", "Tau", "met", "Jet"};
    const std::string type[2]      = {"Scale", "Resolution"};
    const std::string updown[2]    = {"Up", "Down"};


    TString d_mydisc[66];

    bool isOldPXLFile;

    double m_pt_met_min_cut;
    double m_pt_met_max_cut;
    double m_delta_phi_cut;
    double m_pt_min_cut;
    double m_pt_cut;
    double m_m_cut;
    const std::string m_cutdatafile;
    const vector< string >  m_trigger_string;
    TString d_mydiscmu[6];
    const std::string m_dataPeriod;
    const std::string m_kfactorFile_Config;
    const vector< string >  m_analyse_trigger;
    const Tools::MConfig config_;

    double temp_run;
    double temp_ls;
    double temp_event;

    double weight;

    unsigned int numMuon;
    unsigned int numEle;
    unsigned int numTau;
    unsigned int numGamma;
    unsigned int numMET;
    unsigned int numJet;
    unsigned int numBJet;

    int events_;
    unsigned int n_lepton;
    TFile* m_kfactorFile;
    TH1D* m_kfactorHist[3];

    vector< pxl::Particle* > * EleList;
    vector< pxl::Particle* > * MuonList;
    vector< pxl::Particle* > * TauList;
    vector< pxl::Particle* > * GammaList;
    vector< pxl::Particle* > * METList;
    vector< pxl::Particle* > * JetList;

    vector< pxl::Particle* > * RememberPart;
    vector< pxl::Particle* > * RememberMET;

    vector< pxl::Particle* > * EleListGen;
    vector< pxl::Particle* > * MuonListGen;
    vector< pxl::Particle* > * TauListGen;
    vector< pxl::Particle* > * TauVisListGen;
    vector< pxl::Particle* > * GammaListGen;
    vector< pxl::Particle* > * METListGen;
    vector< pxl::Particle* > * JetListGen;
    vector< pxl::Particle* > * S3ListGen;

    pxl::Particle* sel_muon_gen;
    pxl::Particle* sel_tau_gen;
    pxl::Particle* sel_ele_gen;

    pxl::Particle* sel_lepton;
    pxl::Particle* sel_met;

    pxl::Particle calc_stuff;

    unordered_set< string > triggers;


    map< string,float > mLeptonTree;


};

#endif
