#ifndef specialAna_hh
#define specialAna_hh

#include <iostream>
#include "Tools/PXL/PXL.hh"
#include "Tools/PXL/Sort.hh"
//#include "Tools/Tools.hh"
#include "Tools/MConfig.hh"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TLorentzVector.h"
#include <TFile.h>

//----------------------------------------------------------------------

class specialAna : public pxl::AnalysisProcess  {
public:
   specialAna( const Tools::MConfig &config );
   virtual ~specialAna();

   virtual void endJob(const pxl::ObjectOwner* input = 0);
   virtual void analyseEvent( const pxl::Event &event );

	 TFile* file1;

	void Fill_stage_0_histos(double taumu_mass, double weight);
	void Fill_stage_1_histos(bool found, double weight);
	void Fill_stage_2_histos(bool found, bool metmatched, double weight);
	
	void Fill_kinematic_tree(bool found, double weight);

	void Fill_Gen_Controll_histo(double taumu_mass_gen, double weight);
	void Fill_Muo_Controll_histo(int hist_number, pxl::Particle* muon, double weight);
	void Fill_Tau_Controll_histo(int hist_number, pxl::Particle* tau, double weight);
	void Fill_TauMu_Controll_histo(int hist_number, pxl::Particle* sel_taumu, pxl::Particle* sel_tau, pxl::Particle* sel_muon, double weight);
	void Fill_resolution(double taumu_mass_gen, bool found);

  void initEvent( const pxl::Event &event );
  void endEvent( const pxl::Event &event );

	bool Check_Muo_ID(pxl::Particle* muon);
	bool Check_Tau_ID(pxl::Particle* tau);

	void Make_zeta_stuff(bool found, bool metmatched);

	bool tail_selector(double taumu_mass_gen, const pxl::Event &event);
	double Calc_gen_MuTau_mass();
	double Calc_MuTau_mass();
  double DeltaPhi(double a, double b);
  
  double Make_MET_recalculation(double taumu_mass);

   pxl::EventView *m_RecEvtView;
   pxl::EventView *m_GenEvtView;
   std::string const m_JetAlgo, m_BJets_algo, m_METType;
   bool runOnData;

	bool found;
	bool metmatched;

	double p_zeta_vis;
	double p_zeta;

	double bla[23];

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

   std::vector< pxl::Particle* > * EleList;
   std::vector< pxl::Particle* > * MuonList;
   std::vector< pxl::Particle* > * TauList;
   std::vector< pxl::Particle* > * GammaList;
   std::vector< pxl::Particle* > * METList;
   std::vector< pxl::Particle* > * JetList;

   std::vector< pxl::Particle* > * EleListGen;
   std::vector< pxl::Particle* > * MuonListGen;
   std::vector< pxl::Particle* > * TauListGen;
   std::vector< pxl::Particle* > * GammaListGen;
   std::vector< pxl::Particle* > * METListGen;
   std::vector< pxl::Particle* > * JetListGen;
   std::vector< pxl::Particle* > * S3ListGen;

   pxl::Particle* sel_muon_gen;
   pxl::Particle* sel_tau_gen;
   pxl::Particle* sel_taumu_gen;
   double taumu_mass_gen;

   double taumu_mass;
   pxl::Particle* sel_muon;
   pxl::Particle* sel_tau;
   pxl::Particle* sel_tau_corr;
   pxl::Particle* sel_taumu;
   pxl::Particle* sel_taumu_corr;

   TString d_mydisc[67];
   TString d_mydiscmu[6];
};

#endif
