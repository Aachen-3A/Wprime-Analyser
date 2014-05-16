#include "specialAna.hh"
#include "HistClass.hh"

specialAna::specialAna( const Tools::MConfig &cfg ) :
   m_JetAlgo(       cfg.GetItem< std::string >( "Jet.Type.Rec" ) ),
   m_BJets_algo(    cfg.GetItem< std::string >( "Jet.BJets.Algo" ) ),
   m_METType(       cfg.GetItem< std::string >( "MET.Type.Rec" ) ),
   runOnData(       cfg.GetItem< bool >( "General.RunOnData" ) ),
   d_mydisc(  {"againstElectronDeadECAL","againstElectronLoose","againstElectronLooseMVA5","againstElectronMVA5category","againstElectronMVA5raw","againstElectronMedium","againstElectronMediumMVA5","againstElectronTight","againstElectronTightMVA5","againstElectronVLooseMVA5","againstElectronVTightMVA5","againstMuonLoose","againstMuonLoose2","againstMuonLoose3","againstMuonLooseMVA","againstMuonMVAraw","againstMuonMedium","againstMuonMedium2","againstMuonMediumMVA","againstMuonTight","againstMuonTight2","againstMuonTight3","againstMuonTightMVA","byCombinedIsolationDeltaBetaCorrRaw","byCombinedIsolationDeltaBetaCorrRaw3Hits","byIsolationMVA3newDMwLTraw","byIsolationMVA3newDMwoLTraw","byIsolationMVA3oldDMwLTraw","byIsolationMVA3oldDMwoLTraw","byLooseCombinedIsolationDeltaBetaCorr","byLooseCombinedIsolationDeltaBetaCorr3Hits","byLooseIsolation","byLooseIsolationMVA3newDMwLT","byLooseIsolationMVA3newDMwoLT","byLooseIsolationMVA3oldDMwLT","byLooseIsolationMVA3oldDMwoLT","byMediumCombinedIsolationDeltaBetaCorr","byMediumCombinedIsolationDeltaBetaCorr3Hits","byMediumIsolationMVA3newDMwLT","byMediumIsolationMVA3newDMwoLT","byMediumIsolationMVA3oldDMwLT","byMediumIsolationMVA3oldDMwoLT","byTightCombinedIsolationDeltaBetaCorr","byTightCombinedIsolationDeltaBetaCorr3Hits","byTightIsolationMVA3newDMwLT","byTightIsolationMVA3newDMwoLT","byTightIsolationMVA3oldDMwLT","byTightIsolationMVA3oldDMwoLT","byVLooseCombinedIsolationDeltaBetaCorr","byVLooseIsolationMVA3newDMwLT","byVLooseIsolationMVA3newDMwoLT","byVLooseIsolationMVA3oldDMwLT","byVLooseIsolationMVA3oldDMwoLT","byVTightIsolationMVA3newDMwLT","byVTightIsolationMVA3newDMwoLT","byVTightIsolationMVA3oldDMwLT","byVTightIsolationMVA3oldDMwoLT","byVVTightIsolationMVA3newDMwLT","byVVTightIsolationMVA3newDMwoLT","byVVTightIsolationMVA3oldDMwLT","byVVTightIsolationMVA3oldDMwoLT","chargedIsoPtSum","decayModeFinding","decayModeFindingNewDMs","decayModeFindingOldDMs","neutralIsoPtSum","puCorrPtSum"}  ),
   d_mydiscmu(  {"isPFMuon","isGlobalMuon","isTrackerMuon","isStandAloneMuon","isTightMuon","isHighPtMuon"}   )
{
	  std::string safeFileName = "SpecialHistos.root";
    file1 = new TFile(safeFileName.c_str(), "RECREATE");

    HistClass::CreateHisto("Tau_nums","num_Taus", 40, 0, 39, "N_{#taus}");
    HistClass::CreateHisto(3,"Tau_pt","Tau_pt", 5000, 0, 5000, "p_{T}^{#tau} (GeV)");
    HistClass::CreateHisto(3,"Tau_eta","Tau_eta", 80, -4, 4, "#eta_{#tau}");
    HistClass::CreateHisto(3,"Tau_phi","Tau_phi", 40, -3.2, 3.2, "#phi_{#tau} (rad)");
    HistClass::CreateHisto(3,"Tau_decayMode","Tau_decayMode", 100, 0, 10, "#N_{decay mode}^{#tau}");
    HistClass::CreateHisto(3,"Tau_Vtx_X","Tau_Vtx_X", 100, -1, 1, "Vtx_{x}^{#tau} (cm)");
    HistClass::CreateHisto(3,"Tau_Vtx_Y","Tau_Vtx_Y", 100, -1, 1, "Vtx_{y}^{#tau} (cm)");
    HistClass::CreateHisto(3,"Tau_Vtx_Z","Tau_Vtx_Z", 200, -30, 30, "Vtx_{z}^{#tau} (cm)");
    HistClass::CreateHisto(3,"Tau_EtaEta","Tau_EtaEta", 200, -1, 1, "#eta#eta_{#tau}");
    HistClass::CreateHisto(3,"Tau_EtaPhi","Tau_EtaPhi", 200, -1, 1, "#eta#phi_{#tau}");
    HistClass::CreateHisto(3,"Tau_PhiPhi","Tau_PhiPhi", 200, -1, 1, "#phi#phi_{#tau}");
    HistClass::CreateHisto(3,"Tau_NumSignaltracks","Tau_NumSignaltracks", 40, -3, 37, "N_{sig tracks}^{#tau}");
    HistClass::CreateHisto(3,"Tau_NumPFChargedHadrCands","Tau_NumPFChargedHadrCands", 40, -3, 37, "N_{PFChargedHadrCands}^{#tau}");
    HistClass::CreateHisto(3,"Tau_NumPFGammaCands","Tau_NumPFGammaCands", 40, -3, 37, "N_{PFGammaCands}^{#tau}");
    HistClass::CreateHisto(3,"Tau_NumPFNeutralHadrCands","Tau_NumPFNeutralHadrCands", 40, -3, 37, "N_{PFNeutralHadrCands}^{#tau}");
    HistClass::CreateHisto(3,"Tau_LeadingHadronPt","Tau_LeadingHadronPt", 5000, 0, 5000, "p_{T}^{leading hadron} (GeV)");
    HistClass::CreateHisto(3,"Tau_EMFraction","Tau_EMFraction", 100, 0, 10, "EM Fraction^{#tau}");
    HistClass::CreateHisto(3,"Tau_HcalEoverLeadChargedP","Tau_HcalEoverLeadChargedP", 100, 0, 10, "#frac{E_{Hcal}}{P_{leading charged}}");
    HistClass::CreateHisto(3,"Tau_EcalEnergy","Tau_EcalEnergy", 5000, 0, 5000, "E_{Ecal}^{#tau} (GeV)");
    HistClass::CreateHisto(3,"Tau_HcalEnergy","Tau_HcalEnergy", 5000, 0, 5000, "E_{Hcal}^{#tau} (GeV)");
    HistClass::CreateHisto(3,"Tau_Jet_pt","Tau_Jet_pt", 5000, 0, 5000, "p_{T}^{jet} (GeV)");
    HistClass::CreateHisto(3,"Tau_Jet_eta","Tau_Jet_eta", 80, -4, 4, "#eta_{jet}");
    HistClass::CreateHisto(3,"Tau_Jet_phi","Tau_Jet_phi", 40, -3.2, 3.2, "#phi_{jet} (rad)");
    HistClass::CreateHisto(3,"Tau_dxy","Tau_dxy", 100, 0, 0.1, "d_{xy}^{#tau} (cm)");
    HistClass::CreateHisto(3,"Tau_dxy_error","Tau_dxy_error", 100, 0, 0.1, "#sigma(d_{xy}^{#tau}) (cm)");
    HistClass::CreateHisto(3,"Tau_dxy_Sig","Tau_dxy_Sig", 100, 0, 10, "Sig(d_{xy}^{#tau})");
    HistClass::CreateHisto("Tau_num_Gen","Tau_num_Gen", 40, 0, 39, "N_{#taus}^{gen}");
    HistClass::CreateHisto("Tau_pt_Gen","Tau_pt_Gen", 5000, 0, 5000, "p_{T}^{gen #tau} (GeV)");
    HistClass::CreateHisto("Tau_eta_Gen","Tau_eta_Gen", 80, -4, 4, "#eta_{gen #tau}");
    HistClass::CreateHisto("Tau_phi_Gen","Tau_phi_Gen", 40, -3.2, 3.2, "#phi_{gen #tau} (rad)");
    HistClass::CreateHisto("Tau_eta_phi","Tau_eta_phi", 80, -4, 4, 40, -3.2, 3.2,"#eta_{#tau}","#phi_{#tau} (rad)");
    HistClass::CreateHisto(3,"Tau_discriminator","Tau_discriminator", 67, 0, 67,"discriminator_{#tau}");
    HistClass::NameBins(3,"Tau_discriminator",67,d_mydisc);

    HistClass::CreateHisto("MC_LLE_Gen","MC_LLE_Gen",100,0,1,"LLE");
    HistClass::CreateHisto("MC_LQD_Gen","MC_LQD_Gen",100,0,0.001,"LQD");
    HistClass::CreateHisto("MC_MSnl_Gen","MC_MSnl_Gen",4000,0,4000,"MSnl");

    HistClass::CreateHisto("Muon_num","Muon_num", 40, 0, 39,"N_{#mus}");
    HistClass::CreateHisto(3,"Muon_pt","Muon_pt", 5000, 0, 5000,"p_{T}^{#mu} (GeV)");
    HistClass::CreateHisto(3,"Muon_eta","Muon_eta", 80, -4, 4,"#eta_{#mu}");
    HistClass::CreateHisto(3,"Muon_phi","Muon_phi", 40, -3.2, 3.2,"#phi_{#mu} (rad)");
    HistClass::CreateHisto(3,"Muon_Vtx_X","Muon_Vtx_X", 100, -1, 1,"Vtx_{x}^{#mu} (cm)");
    HistClass::CreateHisto(3,"Muon_Vtx_Y","Muon_Vtx_Y", 100, -1, 1,"Vtx_{y}^{#mu} (cm)");
    HistClass::CreateHisto(3,"Muon_Vtx_Z","Muon_Vtx_Z", 200, -30, 30,"Vtx_{z}^{#mu} (cm)");
    HistClass::CreateHisto(3,"Muon_Chi2","Muon_Chi2", 400, 0, 200,"#chi_{#mu}^{2}");
    HistClass::CreateHisto(3,"Muon_Ndof","Muon_Ndof", 200, 0, 200,"N_{dof}^{#mu}");
    HistClass::CreateHisto(3,"Muon_LHits","Muon_LHits", 20, 0, 20,"N_{lost hits}^{#mu}");
    HistClass::CreateHisto(3,"Muon_VHits","Muon_VHits", 100, 0, 100,"N_{valid hits}^{#mu}");
    HistClass::CreateHisto(3,"Muon_VHitsPixel","Muon_VHitsPixel", 20, 0, 20,"N_{valid pixel hits}^{#mu}");
    HistClass::CreateHisto(3,"Muon_VHitsTracker","Muon_VHitsTracker", 40, 0, 40,"N_{valid tracker hits}^{#mu}");
    HistClass::CreateHisto(3,"Muon_VHitsMuonSys","Muon_VHitsMuonSys", 60, 0, 60,"N_{valid muon system hits}^{#mu}");
    HistClass::CreateHisto(3,"Muon_TrackerLayersWithMeas","Muon_TrackerLayersWithMeas", 20, 0, 20,"N_{tracker layers with hits}^{#mu}");
    HistClass::CreateHisto(3,"Muon_PixelLayersWithMeas","Muon_PixelLayersWithMeas", 60, 0, 60,"N_{pixel layers with hits}^{#mu}");
    HistClass::CreateHisto(3,"Muon_NMatchedStations","Muon_NMatchedStations", 60, 0, 60,"N_{matched muon stations}^{#mu}");
    HistClass::CreateHisto(3,"Muon_qoverp","Muon_qoverp", 200, 0, 0.2,"#frac{q_{mu}}{p_{mu}}");
    HistClass::CreateHisto(3,"Muon_qoverpError","Muon_qoverpError", 100, 0, 0.001,"#sigma(#frac{q_{mu}}{p_{mu}})");
    HistClass::CreateHisto(3,"Muon_ptError","Muon_ptError", 1000, 0, 1000,"#sigma(p_{T}^{#mu}) (GeV)");
    HistClass::CreateHisto(3,"Muon_Cocktail_pt","Muon_Cocktail_pt", 5000, 0, 5000,"p_{T}^{cocktail #mu} (GeV)");
    HistClass::CreateHisto(3,"Muon_Cocktail_eta","Muon_Cocktail_eta", 80, -4, 4,"#eta_{cocktail #mu}");
    HistClass::CreateHisto(3,"Muon_Cocktail_phi","Muon_Cocktail_phi", 40, -3.2, 3.2,"#phi_{cocktail #mu} (rad)");
    HistClass::CreateHisto(3,"Muon_CaloIso","Muon_CaloIso", 100, 0, 100,"ISO_{Calo}^{#mu} (GeV)");
    HistClass::CreateHisto(3,"Muon_TrkIso","Muon_TrkIso", 100, 0, 100,"ISO_{Trk}^{#mu} (GeV)");
    HistClass::CreateHisto(3,"Muon_ECALIso","Muon_ECALIso", 100, 0, 100,"ISO_{ECAL}^{#mu} (GeV)");
    HistClass::CreateHisto(3,"Muon_HCALIso","Muon_HCALIso", 100, 0, 100,"ISO_{HCAL}^{#mu} (GeV)");
    HistClass::CreateHisto(3,"Muon_ID","Muon_ID", 6, 0, 6,"ID_{#tau}");
    HistClass::NameBins(3,"Muon_ID",6,d_mydiscmu);
    HistClass::CreateHisto("Muon_num_Gen","Muon_num_Gen", 40, 0, 39, "N_{#mu}^{gen}");
    HistClass::CreateHisto("Muon_pt_Gen","Muon_pt_Gen", 5000, 0, 5000, "p_{T}^{gen #mu} (GeV)");
    HistClass::CreateHisto("Muon_eta_Gen","Muon_eta_Gen", 80, -4, 4, "#eta_{gen #mu}");
    HistClass::CreateHisto("Muon_phi_Gen","Muon_phi_Gen", 40, -3.2, 3.2, "#phi_{gen #mu} (rad)");

    HistClass::CreateHisto(3,"TauMu_mass","TauMu_mass", 8000, 0, 8000, "M_{#tau#mu} (GeV)");
    HistClass::CreateHisto("TauMu_mass_Gen","TauMu_mass_Gen", 8000, 0, 8000, "M_{#tau#mu}^{gen} (GeV)");
    HistClass::CreateHisto(3,"TauMu_DeltaPhi","TauMu_DeltaPhi", 100, 0, 3.2, "#Delta#phi(#tau,#mu) (rad)");
    HistClass::CreateHisto("TauMu_DeltaPhi_Gen","TauMu_DeltaPhi_Gen", 100, 0, 3.2, "#Delta#phi^{gen}(#tau,#mu) (rad)");
    HistClass::CreateHisto(3,"TauMu_tauomuratio","TauMu_tauomuratio", 100, 0, 10, "#frac{p_{T}^{#tau}}{p_{T}^{#mu}}");
    HistClass::CreateHisto("TauMu_tauomuratio_Gen","TauMu_tauomuratio_Gen", 100, 0, 10, "#frac{p_{T}^{gen #tau}}{p_{T}^{gen #mu}}");
    HistClass::CreateHisto(3,"TauMu_chargeproduct","TauMu_chargeproduct", 5, -2, 2, "charge(#tau) * charge(#mu) (e)");
    HistClass::CreateHisto("TauMu_chargeproduct_Gen","TauMu_chargeproduct_Gen", 5, -2, 2, "charge(gen #tau) * charge(gen #mu) (e)");
    HistClass::CreateHisto("TauMu_resolution","TauMu_resolution", 3500, 0, 3500, 200, -2, 2,"M_{#tau#mu}^{gen} (GeV)","(M_{#tau#mu}^{reco} - M_{#tau#mu}^{gen}) / M_{#tau#mu}^{gen}");
    HistClass::CreateHisto("TauMu_zeta_zeta_vis","TauMu_zeta_zeta_vis", 1000, 0, 1000, 1500, -500, 1000,"p_{#zeta} (GeV)","p_{#zeta}^{vis} (GeV)");
   
    const char* varlist = "Run:LumiSection:Event:M_mu_tau:weight";
    HistClass::CreateTree("EventNumbers",varlist,320000);
    const char* varlist2 = "muo_pt:muo_phi:muo_eta:met_et:met_phi:tau_pt:tau_phi:tau_eta:tau_corr_pt:tau_corr_phi:tau_corr_eta:taumu_pt:taumu_eta:taumu_phi:taumu_mass:taumu_corr_pt:taumu_corr_eta:taumu_corr_phi:taumu_corr_mass:charge_product:p_zeta:p_zeta_vis:weight";
    HistClass::CreateTree("Kinematics",varlist2,320000);
}

specialAna::~specialAna() {
}

void specialAna::analyseEvent( const pxl::Event &event ) {

    initEvent( event );

    taumu_mass_gen = Calc_gen_MuTau_mass();

		if(tail_selector(taumu_mass_gen,event)) return;

		Fill_Gen_Controll_histo(taumu_mass_gen,weight);

//     TODO: Configuration of 'accepted' has to be updated
    if ( ! m_RecEvtView->findUserRecord< bool >( "accepted" ) )return;

		taumu_mass = Calc_MuTau_mass();

    Fill_stage_0_histos(taumu_mass,weight);

		taumu_mass = Make_MET_recalculation(taumu_mass);

		Make_zeta_stuff(found,metmatched);

		Fill_stage_1_histos(found,weight);

		Fill_kinematic_tree(found,weight);

		Fill_resolution(taumu_mass_gen,found);

		Fill_stage_2_histos(found,metmatched,weight);

    endEvent( event );
}

double specialAna::Make_MET_recalculation(double taumu_mass) {
    double mass = 0.;
    found = false;
    metmatched = false;
    for(uint i = 0; i < TauList->size(); i++){
      for(uint j = 0; j < MuonList->size(); j++){
				bool tau_ID = TauList->at(i)->findUserRecord< float >("decayModeFindingNewDMs") >= 1 ? true : false;
				bool tau_ISO = TauList->at(i)->findUserRecord< float >("byTightIsolationMVA3newDMwLT") >= 1 ? true : false;
				bool tau_ELE = TauList->at(i)->findUserRecord< float >("againstElectronMediumMVA5"/*"againstElectronTightMVA5"*/) >= 1 ? true : false;
				bool tau_MUO = TauList->at(i)->findUserRecord< float >("againstMuonMedium2"/*"againstMuonTightMVA"*/) >= 1 ? true : false;
				if(MuonList->at(j)->findUserRecord< bool >("isHighPtMuon") && tau_ID && tau_ISO && tau_ELE && tau_MUO){
					pxl::Particle* dummy_taumu = ( pxl::Particle* ) MuonList->at(j)->clone();
					pxl::Particle* dummy_taumu_uncorr = ( pxl::Particle* ) MuonList->at(j)->clone();
          dummy_taumu->addP4(TauList->at(i));
          dummy_taumu_uncorr->addP4(TauList->at(i));
          pxl::Particle* dummy_met = new pxl::Particle();
          if(METList->size()>0){
						/// use tau eta to project MET
						TLorentzVector* calc_met = new TLorentzVector();
						calc_met -> SetPtEtaPhiM(METList->at(0)->getPt(),TauList->at(i)->getEta(),METList->at(0)->getPhi(),0);
						dummy_met->setP4(calc_met->Px(),calc_met->Py(),calc_met->Pz(),calc_met->E());
						dummy_taumu->addP4(dummy_met);
						delete calc_met;
						
						/// rotate MET to tau direction
						//TLorentzVector* calc_met = new TLorentzVector();
						//calc_met -> SetPtEtaPhiM(METList->at(0)->getPt(),0,METList->at(0)->getPhi(),0);
						//TVector3* tau_direction = new TVector3(TauList->at(i)->getPx(),TauList->at(i)->getPy(),TauList->at(i)->getPz());
						//*tau_direction = tau_direction -> Unit();
						//calc_met -> RotateUz(*tau_direction);
						//dummy_met->setP4(calc_met->Px(),calc_met->Py(),calc_met->Pz(),calc_met->E());
						//dummy_taumu->addP4(dummy_met);
						//delete tau_direction;
						//delete calc_met;

						/// project MET to tau direction
						//double dummy_p1 = METList->at(0)->getPx()/(TMath::Sin(TauList->at(i)->getTheta()) * TMath::Cos(TauList->at(i)->getPhi()));
						//double dummy_p2 = METList->at(0)->getPy()/(TMath::Sin(TauList->at(i)->getTheta()) * TMath::Sin(TauList->at(i)->getPhi()));
						//double dummy_p = (dummy_p1 + dummy_p2) / 2.;
						//dummy_met->setP4(dummy_p*TMath::Sin(TauList->at(i)->getTheta()) * TMath::Cos(TauList->at(i)->getPhi()),dummy_p*TMath::Sin(TauList->at(i)->getTheta()) * TMath::Sin(TauList->at(i)->getPhi()),dummy_p*TMath::Cos(TauList->at(i)->getTheta()),dummy_p);
						//dummy_taumu->addP4(dummy_met);

						/// project MET parallel to tau direction
						//double value = (METList->at(0)->getPx() * TauList->at(i)->getPx() + METList->at(0)->getPy() * TauList->at(i)->getPy()) / sqrt(pow(TauList->at(i)->getPx(),2) + pow(TauList->at(i)->getPy(),2));
						//TLorentzVector* calc_met = new TLorentzVector();
						//calc_met -> SetPtEtaPhiM(value,TauList->at(i)->getEta(),TauList->at(i)->getPhi(),0);
						//dummy_taumu->addP4(dummy_met);

						metmatched = true;
					}
          if (dummy_taumu->getMass() > mass){
						found = true;
            mass = dummy_taumu->getMass();
            sel_taumu_corr = ( pxl::Particle* ) dummy_taumu->clone();
            sel_taumu = ( pxl::Particle* ) dummy_taumu_uncorr->clone();
            sel_muon = ( pxl::Particle* ) MuonList->at(j)->clone();
            sel_tau = ( pxl::Particle* ) TauList->at(i)->clone();
            sel_tau_corr = ( pxl::Particle* ) TauList->at(j)->clone();
            sel_tau_corr->addP4(dummy_met);
          }
				}
      }
    }
    
    if(found){
			return mass;
		}
		
		return taumu_mass;
}

void specialAna::Fill_resolution(double taumu_mass_gen, bool found) {
		if(taumu_mass_gen > 0. && found){
			HistClass::Fill("TauMu_resolution",taumu_mass_gen,(bla[18]-taumu_mass_gen)/taumu_mass_gen,weight);
		}
}

double specialAna::Calc_MuTau_mass() {
    double mass = 0.;
    for(uint i = 0; i < TauList->size(); i++){
      for(uint j = 0; j < MuonList->size(); j++){
          pxl::Particle* dummy_taumu = ( pxl::Particle* ) MuonList->at(j)->clone();
          dummy_taumu->addP4(TauList->at(i));
          if (dummy_taumu->getMass() > mass){
            mass = dummy_taumu->getMass();
            sel_taumu = ( pxl::Particle* ) dummy_taumu->clone();
            sel_muon = ( pxl::Particle* ) MuonList->at(j)->clone();
            sel_tau = ( pxl::Particle* ) TauList->at(i)->clone();
          }
      }
    }
    return mass;
}

void specialAna::Fill_stage_0_histos(double taumu_mass, double weight) {
    HistClass::Fill("Tau_nums",numTau,1);
    HistClass::Fill("Muon_num",numMuon,1);

    for(uint i = 0; i < TauList->size(); i++){
			  Fill_Tau_Controll_histo(0, TauList->at(i),weight);
			  HistClass::Fill("Tau_eta_phi",TauList->at(i)->getEta(),TauList->at(i)->getPhi(),weight);
    }

    for(uint i = 0; i < MuonList->size(); i++){
				Fill_Muo_Controll_histo(0, MuonList->at(i), weight);
    }
    
    if(taumu_mass > 0.){
			Fill_TauMu_Controll_histo(0,sel_taumu,sel_tau,sel_muon,weight);
    }
}

void specialAna::Fill_stage_1_histos(bool found, double weight) {
    if(found){
			Fill_Tau_Controll_histo(1, sel_tau, weight);
			Fill_Muo_Controll_histo(1, sel_muon, weight);
			Fill_TauMu_Controll_histo(1,sel_taumu_corr,sel_tau,sel_muon,weight);
		}
}

void specialAna::Fill_stage_2_histos(bool found, bool metmatched, double weight) {
    if(metmatched && found && DeltaPhi(sel_tau->getPhi(),METList->at(0)->getPhi()) < 1.0 && TMath::Abs(sel_tau->getEta()) < 2.3){
			Fill_Tau_Controll_histo(2, sel_tau, weight);
			Fill_Muo_Controll_histo(2, sel_muon, weight);
			Fill_TauMu_Controll_histo(2,sel_taumu_corr,sel_tau,sel_muon,weight);
			if(sel_taumu->getMass() > 1000.){
				///"Run","LumiSection","Event","M_mu_tau","weight"
				double bla[] = {temp_run,temp_ls,temp_event,sel_taumu_corr->getMass(),weight};
				HistClass::FillTree("EventNumbers", bla);
			}
    }
}

void specialAna::Fill_kinematic_tree(bool found, double weight) {
		/// muo_pt:muo_phi:muo_eta
		/// met_et:met_phi
		/// tau_pt:tau_phi:tau_eta
		/// tau_corr_pt:tau_corr_phi:tau_corr_eta
		/// taumu_pt:taumu_eta:taumu_phi:taumu_mass
		/// taumu_corr_pt:taumu_corr_eta:taumu_corr_phi:taumu_corr_mass
		/// charge_product
		/// p_zeta:p_zeta_vis
		/// weight
    if(found){
			if(METList->size()>0){
				bla[0] = sel_muon->getPt();
				bla[1] = sel_muon->getPhi();
				bla[2] = sel_muon->getEta();
				bla[3] = METList->at(0)->getPt();
				bla[4] = METList->at(0)->getPhi();
				bla[5] = sel_tau->getPt();
				bla[6] = sel_tau->getPhi();
				bla[7] = sel_tau->getEta();
				bla[8] = sel_tau_corr->getPt();
				bla[9] = sel_tau_corr->getPhi();
				bla[10] = sel_tau_corr->getEta();
				bla[11] = sel_taumu->getPt();
				bla[12] = sel_taumu->getEta();
				bla[13] = sel_taumu->getPhi();
				bla[14] = sel_taumu->getMass();
				bla[15] = sel_taumu_corr->getPt();
				bla[16] = sel_taumu_corr->getEta();
				bla[17] = sel_taumu_corr->getPhi();
				bla[18] = sel_taumu_corr->getMass();
				bla[19] = sel_muon->getCharge() * sel_tau->getCharge();
				bla[20] = p_zeta;
				bla[21] = p_zeta_vis;
				bla[22] = weight;
			}else{
				bla[0] = sel_muon->getPt();
				bla[1] = sel_muon->getPhi();
				bla[2] = sel_muon->getEta();
				bla[3] = 0.;
				bla[4] = 0.;
				bla[5] = sel_tau->getPt();
				bla[6] = sel_tau->getPhi();
				bla[7] = sel_tau->getEta();
				bla[8] = sel_tau->getPt();
				bla[9] = sel_tau->getPhi();
				bla[10] = sel_tau->getEta();
				bla[11] = sel_taumu->getPt();
				bla[12] = sel_taumu->getEta();
				bla[13] = sel_taumu->getPhi();
				bla[14] = sel_taumu->getMass();
				bla[15] = sel_taumu->getPt();
				bla[16] = sel_taumu->getEta();
				bla[17] = sel_taumu->getPhi();
				bla[18] = sel_taumu->getMass();
				bla[19] = sel_muon->getCharge() * sel_tau->getCharge();
				bla[20] = p_zeta;
				bla[21] = p_zeta_vis;
				bla[22] = weight;
			}
			HistClass::FillTree("Kinematics",bla);
    }
}

void specialAna::Make_zeta_stuff(bool found, bool metmatched) {
		p_zeta_vis = 0;
		p_zeta = 0;

		if(found && metmatched){
			TVector3* vec_mu = new TVector3();
			TVector3* vec_tau = new TVector3();
			double mu_norm = sqrt( pow(sel_muon->getPx(),2) + pow(sel_muon->getPy(),2) + pow(sel_muon->getPz(),2) );
			vec_mu -> SetXYZ(sel_muon->getPx()/mu_norm,sel_muon->getPy()/mu_norm,sel_muon->getPz()/mu_norm);
			double tau_norm = sqrt( pow(sel_tau->getPx(),2) + pow(sel_tau->getPy(),2) + pow(sel_tau->getPz(),2) );
			vec_tau -> SetXYZ(sel_tau->getPx()/tau_norm,sel_tau->getPy()/tau_norm,sel_tau->getPz()/tau_norm);

			TVector3 bisec = *vec_mu + *vec_tau;
			TVector3 bisec_norm = bisec.Unit();
			
			p_zeta_vis = (sel_tau->getPx() * bisec_norm.X() + sel_tau->getPy() * bisec_norm.Y() + sel_tau->getPz() * bisec_norm.Z()) + (sel_muon->getPx() * bisec_norm.X() + sel_muon->getPy() * bisec_norm.Y() + sel_muon->getPz() * bisec_norm.Z());
			p_zeta = p_zeta_vis + (METList->at(0)->getPx() * bisec_norm.X() + METList->at(0)->getPy() * bisec_norm.Y() + METList->at(0)->getPz() * bisec_norm.Z());
			HistClass::Fill("TauMu_zeta_zeta_vis",p_zeta,p_zeta_vis,weight);
			delete vec_mu;
			delete vec_tau;
		}
}

bool specialAna::tail_selector(double taumu_mass_gen, const pxl::Event &event) {
		std::string datastream = event.findUserRecord< std::string >( "Dataset" );
		TString Datastream = datastream;

		/// W tail fitting		
		if(Datastream.Contains("WJetsToLNu_TuneZ2Star_8TeV")) {
	    for(uint i = 0; i < S3ListGen->size(); i++){
	      if(TMath::Abs(S3ListGen->at(i)->findUserRecord< int >("id")) == 23){
					if(S3ListGen->at(i)->getPt() > 50)return true;
	      }
	    }
		}

		/// Diboson tail fitting
		if(Datastream.Contains("WW_") || Datastream.Contains("WZ_") || Datastream.Contains("ZZ_")) {
	    for(uint i = 0; i < S3ListGen->size(); i++){
				int part_id = TMath::Abs(S3ListGen->at(i)->findUserRecord< int >("id"));
	      if(part_id == 23 || part_id == 22){
					if(S3ListGen->at(i)->getPt() > 500)return true;
	      }
	    }
		}

		/// Signal parameter selection
    HistClass::Fill("MC_LLE_Gen",m_GenEvtView->findUserRecord< double >( "MC_LLE" ,0.),weight);
    HistClass::Fill("MC_LQD_Gen",m_GenEvtView->findUserRecord< double >( "MC_LQD" ,0.),weight);
    HistClass::Fill("MC_MSnl_Gen",m_GenEvtView->findUserRecord< double >( "MC_MSnl" ,0.),weight);
    
    if(Datastream.Contains("RPVresonantToMuTau")){
	    if(!(taumu_mass_gen > 1250. && taumu_mass_gen < 1350.))return true;
    }
    
    return false;
}

double specialAna::Calc_gen_MuTau_mass() {
	double mass = 0;
	for(uint i = 0; i < S3ListGen->size(); i++){
		if(TMath::Abs(S3ListGen->at(i)->findUserRecord< int >("id")) == 15){
			for(uint j = 0; j < S3ListGen->size(); j++){
				if(TMath::Abs(S3ListGen->at(j)->findUserRecord< int >("id")) == 13){
					pxl::Particle* dummy_taumu = ( pxl::Particle* ) S3ListGen->at(j)->clone();
					dummy_taumu->addP4(S3ListGen->at(i));
					if (dummy_taumu->getMass() > mass){
						mass = dummy_taumu->getMass();
						sel_taumu_gen = ( pxl::Particle* ) dummy_taumu->clone();
						sel_muon_gen = ( pxl::Particle* ) S3ListGen->at(j)->clone();
						sel_tau_gen = ( pxl::Particle* ) S3ListGen->at(i)->clone();
					}
					delete dummy_taumu;
				}
			}
		}
	}
	return mass;
}

void specialAna::Fill_Gen_Controll_histo(double taumu_mass_gen, double weight) {
    HistClass::Fill("Tau_num_Gen",TauListGen->size(),weight);

    HistClass::Fill("Muon_num_Gen",MuonListGen->size(),weight);

    for(uint i = 0; i < S3ListGen->size(); i++){
        if(TMath::Abs(S3ListGen->at(i)->findUserRecord< int >("id")) == 13){
            HistClass::Fill("Muon_pt_Gen",S3ListGen->at(i)->getPt(),weight);
            HistClass::Fill("Muon_eta_Gen",S3ListGen->at(i)->getEta(),weight);
            HistClass::Fill("Muon_phi_Gen",S3ListGen->at(i)->getPhi(),weight);
        }else if(TMath::Abs(S3ListGen->at(i)->findUserRecord< int >("id")) == 15){
            HistClass::Fill("Tau_pt_Gen",S3ListGen->at(i)->getPt(),weight);
            HistClass::Fill("Tau_eta_Gen",S3ListGen->at(i)->getEta(),weight);
            HistClass::Fill("Tau_phi_Gen",S3ListGen->at(i)->getPhi(),weight);
        }
    }
    
    if(taumu_mass_gen > 0.){
      HistClass::Fill("TauMu_mass_Gen",sel_taumu_gen->getMass(),weight);
      HistClass::Fill("TauMu_DeltaPhi_Gen",DeltaPhi(sel_tau_gen->getPhi(),sel_muon_gen->getPhi()),weight);
      HistClass::Fill("TauMu_tauomuratio_Gen",sel_tau_gen->getPt()/sel_muon_gen->getPt(),weight);
      HistClass::Fill("TauMu_chargeproduct_Gen",(sel_tau_gen->findUserRecord< int >("id") * sel_muon_gen->findUserRecord< int >("id"))/(13.*15.),weight);
    }
}

void specialAna::Fill_Muo_Controll_histo(int hist_number, pxl::Particle* muon, double weight) {
	HistClass::Fill(hist_number,"Muon_pt",muon->getPt(),weight);
	HistClass::Fill(hist_number,"Muon_eta",muon->getEta(),weight);
	HistClass::Fill(hist_number,"Muon_phi",muon->getPhi(),weight);
	HistClass::Fill(hist_number,"Muon_Vtx_X",muon->findUserRecord< double >("Vtx_X"),weight);
	HistClass::Fill(hist_number,"Muon_Vtx_Y",muon->findUserRecord< double >("Vtx_Y"),weight);
	HistClass::Fill(hist_number,"Muon_Vtx_Z",muon->findUserRecord< double >("Vtx_Z"),weight);
	for(uint j = 0; j < 6; j++) {
			if(muon->findUserRecord< bool >( (std::string)d_mydiscmu[j] ))
				HistClass::Fill(hist_number,"Muon_ID",j+1,weight);
	}
	HistClass::Fill(hist_number,"Muon_Chi2",muon->findUserRecord< double >("chi2"),weight);
	HistClass::Fill(hist_number,"Muon_Ndof",muon->findUserRecord< double >("ndof"),weight);
	HistClass::Fill(hist_number,"Muon_LHits",muon->findUserRecord< int >("LHits"),weight);
	HistClass::Fill(hist_number,"Muon_VHits",muon->findUserRecord< int >("VHits"),weight);
	HistClass::Fill(hist_number,"Muon_VHitsPixel",muon->findUserRecord< int >("VHitsPixel"),weight);
	HistClass::Fill(hist_number,"Muon_VHitsTracker",muon->findUserRecord< int >("VHitsTracker"),weight);
	HistClass::Fill(hist_number,"Muon_VHitsMuonSys",muon->findUserRecord< int >("VHitsMuonSys"),weight);
	HistClass::Fill(hist_number,"Muon_TrackerLayersWithMeas",muon->findUserRecord< int >("TrackerLayersWithMeas"),weight);
	HistClass::Fill(hist_number,"Muon_PixelLayersWithMeas",muon->findUserRecord< int >("PixelLayersWithMeas"),weight);
	HistClass::Fill(hist_number,"Muon_NMatchedStations",muon->findUserRecord< int >("NMatchedStations"),weight);
	HistClass::Fill(hist_number,"Muon_qoverp",muon->findUserRecord< double >("qoverp"),weight);
	HistClass::Fill(hist_number,"Muon_qoverpError",muon->findUserRecord< double >("qoverpError"),weight);
	HistClass::Fill(hist_number,"Muon_ptError",muon->findUserRecord< double >("ptError"),weight);
	if(muon->findUserRecord< bool >("validCocktail")){
			TLorentzVector* cocktailMuon = new TLorentzVector();
			cocktailMuon->SetXYZM(muon->findUserRecord< double >("pxCocktail"),muon->findUserRecord< double >("pyCocktail"),muon->findUserRecord< double >("pzCocktail"),0.105);
			HistClass::Fill(hist_number,"Muon_Cocktail_pt",cocktailMuon->Pt(),weight);
			HistClass::Fill(hist_number,"Muon_Cocktail_eta",cocktailMuon->Eta(),weight);
			HistClass::Fill(hist_number,"Muon_Cocktail_phi",cocktailMuon->Phi(),weight);
			delete cocktailMuon;
	}
	HistClass::Fill(hist_number,"Muon_CaloIso",muon->findUserRecord< double >("CaloIso"),weight);
	HistClass::Fill(hist_number,"Muon_TrkIso",muon->findUserRecord< double >("TrkIso"),weight);
	HistClass::Fill(hist_number,"Muon_ECALIso",muon->findUserRecord< double >("ECALIso"),weight);
	HistClass::Fill(hist_number,"Muon_HCALIso",muon->findUserRecord< double >("HCALIso"),weight);
}

void specialAna::Fill_Tau_Controll_histo(int hist_number, pxl::Particle* tau, double weight) {
	HistClass::Fill(hist_number,"Tau_pt",tau->getPt(),weight);
	HistClass::Fill(hist_number,"Tau_eta",tau->getEta(),weight);
	HistClass::Fill(hist_number,"Tau_phi",tau->getPhi(),weight);
	for(uint j = 0; j < 67; j++) {
			HistClass::Fill(hist_number,"Tau_discriminator",j+1,tau->findUserRecord< float >( (std::string)d_mydisc[j] ));
	}
	HistClass::Fill(hist_number,"Tau_decayMode",tau->findUserRecord< int >("decayMode"),weight);
	HistClass::Fill(hist_number,"Tau_Vtx_X",tau->findUserRecord< double >("Vtx_X"),weight);
	HistClass::Fill(hist_number,"Tau_Vtx_Y",tau->findUserRecord< double >("Vtx_Y"),weight);
	HistClass::Fill(hist_number,"Tau_Vtx_Z",tau->findUserRecord< double >("Vtx_Z"),weight);
	HistClass::Fill(hist_number,"Tau_EtaEta",tau->findUserRecord< float >("EtaEta"),weight);
	HistClass::Fill(hist_number,"Tau_EtaPhi",tau->findUserRecord< float >("EtaPhi"),weight);
	HistClass::Fill(hist_number,"Tau_PhiPhi",tau->findUserRecord< float >("PhiPhi"),weight);
	HistClass::Fill(hist_number,"Tau_NumSignaltracks",tau->findUserRecord< int >("NumSignalTracks"),weight);
	HistClass::Fill(hist_number,"Tau_NumPFChargedHadrCands",tau->findUserRecord< int >("NumPFChargedHadrCands"),weight);
	HistClass::Fill(hist_number,"Tau_NumPFGammaCands",tau->findUserRecord< int >("NumPFGammaCands"),weight);
	HistClass::Fill(hist_number,"Tau_NumPFNeutralHadrCands",tau->findUserRecord< int >("NumPFNeutralHadrCands"),weight);
	HistClass::Fill(hist_number,"Tau_LeadingHadronPt",tau->findUserRecord< double >("LeadingHadronPt"),weight);
	HistClass::Fill(hist_number,"Tau_EMFraction",tau->findUserRecord< double >("EMFraction"),weight);
	HistClass::Fill(hist_number,"Tau_HcalEoverLeadChargedP",tau->findUserRecord< double >("HcalEoverLeadChargedP"),weight);
	HistClass::Fill(hist_number,"Tau_EcalEnergy",tau->findUserRecord< double >("HcalEoverLeadChargedP") * tau->findUserRecord< double >("LeadingHadronP") * tau->findUserRecord< double >("EMFraction"),weight);
	HistClass::Fill(hist_number,"Tau_HcalEnergy",tau->findUserRecord< double >("HcalEoverLeadChargedP") * tau->findUserRecord< double >("LeadingHadronP"),weight);
	TLorentzVector* jet = new TLorentzVector();
	jet->SetPxPyPzE(tau->findUserRecord< double >("tauJetpx"),tau->findUserRecord< double >("tauJetpy"),tau->findUserRecord< double >("tauJetpz"),tau->findUserRecord< double >("tauJetE"));
	HistClass::Fill(hist_number,"Tau_Jet_pt",jet->Pt(),weight);
	HistClass::Fill(hist_number,"Tau_Jet_eta",jet->Eta(),weight);
	HistClass::Fill(hist_number,"Tau_Jet_phi",jet->Phi(),weight);
	delete jet;
	HistClass::Fill(hist_number,"Tau_dxy",tau->findUserRecord< double >("dxy"),weight);
	HistClass::Fill(hist_number,"Tau_dxy_error",tau->findUserRecord< double >("dxy_error"),weight);
	HistClass::Fill(hist_number,"Tau_dxy_Sig",tau->findUserRecord< double >("dxy_Sig"),weight);
}

void specialAna::Fill_TauMu_Controll_histo(int hist_number, pxl::Particle* sel_taumu, pxl::Particle* sel_tau, pxl::Particle* sel_muon, double weight) {
	HistClass::Fill(hist_number,"TauMu_mass",sel_taumu->getMass(),weight);
	HistClass::Fill(hist_number,"TauMu_DeltaPhi",DeltaPhi(sel_tau->getPhi(),sel_muon->getPhi()),weight);
	HistClass::Fill(hist_number,"TauMu_tauomuratio",sel_tau->getPt()/sel_muon->getPt(),weight);
	HistClass::Fill(hist_number,"TauMu_chargeproduct",sel_tau->getCharge() * sel_muon->getCharge(),weight);
}

double specialAna::DeltaPhi(double a, double b) {
  double temp = fabs(a-b);
  if (temp <= TMath::Pi())
    return temp;
  else
    return  2.*TMath::Pi() - temp;
}

void specialAna::endJob( const pxl::ObjectOwner* input ) {
   file1->mkdir("MC");
   file1->cd("MC/");
   HistClass::WriteAll("MC_");
   file1->cd();
   file1->mkdir("Taus");
   file1->cd("Taus/");
   HistClass::WriteAll("_Tau_");
   HistClass::Write2("Tau_eta_phi");
   file1->cd();
   file1->mkdir("Muons");
   file1->cd("Muons/");
   HistClass::WriteAll("_Muon_");
   file1->cd();
   file1->mkdir("TauMu");
   file1->cd("TauMu/");
   HistClass::WriteAll("_TauMu_");
   HistClass::Write2("TauMu_resolution");
   HistClass::Write2("TauMu_zeta_zeta_vis");
   file1->cd();
   file1->mkdir("Trees");
   file1->cd("Trees/");
   HistClass::WriteAllTrees();
   file1->Close();
   delete file1;
}

void specialAna::initEvent( const pxl::Event &event ){

   weight = 1;
   m_RecEvtView = event.getObjectOwner().findObject< pxl::EventView >( "Rec" );
   m_GenEvtView = event.getObjectOwner().findObject< pxl::EventView >( "Gen" );

	 temp_run = event.findUserRecord< uint >( "Run" );
	 temp_ls = event.findUserRecord< uint >( "LumiSection" );
	 temp_event = event.findUserRecord< uint >( "EventNum" );

   numMuon  = m_RecEvtView->findUserRecord< int >( "NumMuon" );
   numEle   = m_RecEvtView->findUserRecord< int >( "NumEle" );
   numGamma = m_RecEvtView->findUserRecord< int >( "NumGamma" );
   numTau   = m_RecEvtView->findUserRecord< int >( "NumTaus" );
//    std::cout << m_METType << std::endl;
   numMET   = m_RecEvtView->findUserRecord< int >( "Num" + m_METType );
   numJet   = m_RecEvtView->findUserRecord< int >( "Num" + m_JetAlgo );
   numBJet  = m_RecEvtView->findUserRecord< int >( "Num" + m_BJets_algo );

// h1_num_Taus.Fill(numTau);

   EleList   = new std::vector< pxl::Particle* >;
   MuonList  = new std::vector< pxl::Particle* >;
   GammaList = new std::vector< pxl::Particle* >;
   METList   = new std::vector< pxl::Particle* >;
   JetList   = new std::vector< pxl::Particle* >;
   TauList   = new std::vector< pxl::Particle* >;

   EleList->reserve( numEle );
   MuonList->reserve( numMuon );
   GammaList->reserve( numGamma );
   METList->reserve( numMET );
   JetList->reserve( numJet+numBJet );
   TauList->reserve( numTau );

   // get all particles
   std::vector< pxl::Particle* > AllParticles;
   m_RecEvtView->getObjectsOfType< pxl::Particle >( AllParticles );
   pxl::sortParticles( AllParticles );
   // push them into the corresponding vectors
   for( std::vector< pxl::Particle* >::const_iterator part_it = AllParticles.begin(); part_it != AllParticles.end(); ++part_it ) {
      pxl::Particle *part = *part_it;
      std::string Name = part->getName();
      // Only fill the collection if we want to use the particle!
      if(      Name == "Muon"    ) MuonList->push_back( part );
      else if( Name == "Ele"     ) EleList->push_back( part );
      else if( Name == "Gamma"   ) GammaList->push_back( part );
      else if( Name == "patTaus"   ) TauList->push_back( part );
      else if( Name == m_METType ) METList->push_back( part );
      else if( Name == m_JetAlgo ) JetList->push_back( part );
   }
//     h1_num_Taus.Fill(TauList->size());
   EleListGen     = new std::vector< pxl::Particle* >;
   MuonListGen    = new std::vector< pxl::Particle* >;
   GammaListGen   = new std::vector< pxl::Particle* >;
   METListGen     = new std::vector< pxl::Particle* >;
   JetListGen     = new std::vector< pxl::Particle* >;
   TauListGen     = new std::vector< pxl::Particle* >;
   S3ListGen      = new std::vector< pxl::Particle* >;

   if( not runOnData ){

      double event_weight = m_GenEvtView->findUserRecord< double >( "Weight" );
      double varKfactor_weight = m_GenEvtView->findUserRecord< double >( "kfacWeight", 1.0 );
      double pileup_weight = m_GenEvtView->findUserRecord< double >( "PUWeight", 1.0 );

      weight = event_weight * varKfactor_weight * pileup_weight;

      // get all particles
      std::vector< pxl::Particle* > AllParticlesGen;
      m_GenEvtView->getObjectsOfType< pxl::Particle >( AllParticlesGen );
      pxl::sortParticles( AllParticlesGen );
      // push them into the corresponding vectors
      for( std::vector< pxl::Particle* >::const_iterator part_it = AllParticlesGen.begin(); part_it != AllParticlesGen.end(); ++part_it ) {
         pxl::Particle *part = *part_it;
         std::string Name = part->getName();
         // Only fill the collection if we want to use the particle!
         if(      Name == "Muon"    ) MuonListGen->push_back( part );
         else if( Name == "Ele"     ) EleListGen->push_back( part );
         else if( Name == "Gamma"   ) GammaListGen->push_back( part );
         else if( Name == "Tau"     ) TauListGen->push_back( part );
         else if( Name == m_METType ) METListGen->push_back( part );
         else if( Name == m_JetAlgo ) JetListGen->push_back( part );
         else if( Name == "S3"      ) S3ListGen->push_back( part );
      }
   }
}

void specialAna::endEvent( const pxl::Event &event ){

   delete EleList;
   delete MuonList;
   delete GammaList;
   delete TauList;
   delete METList;
   delete JetList;

   EleList = 0;
   MuonList = 0;
   GammaList = 0;
   METList = 0;
   JetList = 0;
   TauList = 0;

   if( not runOnData ){

      delete EleListGen;
      delete MuonListGen;
      delete GammaListGen;
      delete METListGen;
      delete JetListGen;
      delete TauListGen;

      EleListGen = 0;
      MuonListGen = 0;
      GammaListGen = 0;
      METListGen = 0;
      JetListGen = 0;
      TauListGen = 0;
   }
}
