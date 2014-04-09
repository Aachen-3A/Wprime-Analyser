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
    HistClass::CreateHisto("Tau_nums","num_Taus", 40, 0, 39, "N_{#taus}");
    HistClass::CreateHisto(2,"Tau_pt","Tau_pt", 2000, 0, 2000, "p_{T}^{#tau} (GeV)");
    HistClass::CreateHisto(2,"Tau_eta","Tau_eta", 80, -4, 4, "#eta_{#tau}");
    HistClass::CreateHisto(2,"Tau_phi","Tau_phi", 40, -3.2, 3.2, "#phi_{#tau} (rad)");
    HistClass::CreateHisto(2,"Tau_decayMode","Tau_decayMode", 100, 0, 10, "#N_{decay mode}^{#tau}");
    HistClass::CreateHisto(2,"Tau_Vtx_X","Tau_Vtx_X", 100, -1, 1, "Vtx_{x}^{#tau} (cm)");
    HistClass::CreateHisto(2,"Tau_Vtx_Y","Tau_Vtx_Y", 100, -1, 1, "Vtx_{y}^{#tau} (cm)");
    HistClass::CreateHisto(2,"Tau_Vtx_Z","Tau_Vtx_Z", 200, -30, 30, "Vtx_{z}^{#tau} (cm)");
    HistClass::CreateHisto(2,"Tau_EtaEta","Tau_EtaEta", 200, -1, 1, "#eta#eta_{#tau}");
    HistClass::CreateHisto(2,"Tau_EtaPhi","Tau_EtaPhi", 200, -1, 1, "#eta#phi_{#tau}");
    HistClass::CreateHisto(2,"Tau_PhiPhi","Tau_PhiPhi", 200, -1, 1, "#phi#phi_{#tau}");
    HistClass::CreateHisto(2,"Tau_NumSignaltracks","Tau_NumSignaltracks", 40, -3, 37, "N_{sig tracks}^{#tau}");
    HistClass::CreateHisto(2,"Tau_NumPFChargedHadrCands","Tau_NumPFChargedHadrCands", 40, -3, 37, "N_{PFChargedHadrCands}^{#tau}");
    HistClass::CreateHisto(2,"Tau_NumPFGammaCands","Tau_NumPFGammaCands", 40, -3, 37, "N_{PFGammaCands}^{#tau}");
    HistClass::CreateHisto(2,"Tau_NumPFNeutralHadrCands","Tau_NumPFNeutralHadrCands", 40, -3, 37, "N_{PFNeutralHadrCands}^{#tau}");
    HistClass::CreateHisto(2,"Tau_LeadingHadronPt","Tau_LeadingHadronPt", 2000, 0, 2000, "p_{T}^{leading hadron} (GeV)");
    HistClass::CreateHisto(2,"Tau_EMFraction","Tau_EMFraction", 100, 0, 10, "EM Fraction^{#tau}");
    HistClass::CreateHisto(2,"Tau_HcalEoverLeadChargedP","Tau_HcalEoverLeadChargedP", 100, 0, 10, "#frac{E_{Hcal}}{P_{leading charged}}");
    HistClass::CreateHisto(2,"Tau_EcalEnergy","Tau_EcalEnergy", 2000, 0, 2000, "E_{Ecal}^{#tau} (GeV)");
    HistClass::CreateHisto(2,"Tau_HcalEnergy","Tau_HcalEnergy", 2000, 0, 2000, "E_{Hcal}^{#tau} (GeV)");
    HistClass::CreateHisto(2,"Tau_Jet_pt","Tau_Jet_pt", 2000, 0, 2000, "p_{T}^{jet} (GeV)");
    HistClass::CreateHisto(2,"Tau_Jet_eta","Tau_Jet_eta", 80, -4, 4, "#eta_{jet}");
    HistClass::CreateHisto(2,"Tau_Jet_phi","Tau_Jet_phi", 40, -3.2, 3.2, "#phi_{jet} (rad)");
    HistClass::CreateHisto(2,"Tau_dxy","Tau_dxy", 100, 0, 0.1, "d_{xy}^{#tau} (cm)");
    HistClass::CreateHisto(2,"Tau_dxy_error","Tau_dxy_error", 100, 0, 0.1, "#sigma(d_{xy}^{#tau}) (cm)");
    HistClass::CreateHisto(2,"Tau_dxy_Sig","Tau_dxy_Sig", 100, 0, 10, "Sig(d_{xy}^{#tau})");
    HistClass::CreateHisto("Tau_num_Gen","Tau_num_Gen", 40, 0, 39, "N_{#taus}^{gen}");
    HistClass::CreateHisto("Tau_pt_Gen","Tau_pt_Gen", 2000, 0, 2000, "p_{T}^{gen #tau} (GeV)");
    HistClass::CreateHisto("Tau_eta_Gen","Tau_eta_Gen", 80, -4, 4, "#eta_{gen #tau}");
    HistClass::CreateHisto("Tau_phi_Gen","Tau_phi_Gen", 40, -3.2, 3.2, "#phi_{gen #tau} (rad)");
    HistClass::CreateHisto("Tau_eta_phi","Tau_eta_phi", 80, -4, 4, 40, -3.2, 3.2,"#eta_{#tau}","#phi_{#tau} (rad)");
    HistClass::CreateHisto(2,"Tau_discriminator","Tau_discriminator", 67, 0, 67,"discriminator_{#tau}");
    HistClass::NameBins(2,"Tau_discriminator",67,d_mydisc);

    HistClass::CreateHisto("MC_LLE_Gen","MC_LLE_Gen",100,0,1,"LLE");
    HistClass::CreateHisto("MC_LQD_Gen","MC_LQD_Gen",100,0,0.001,"LQD");
    HistClass::CreateHisto("MC_MSnl_Gen","MC_MSnl_Gen",4000,0,4000,"MSnl");

    HistClass::CreateHisto("Muon_num","Muon_num", 40, 0, 39,"N_{#mus}");
    HistClass::CreateHisto(2,"Muon_pt","Muon_pt", 2000, 0, 2000,"p_{T}^{#mu} (GeV)");
    HistClass::CreateHisto(2,"Muon_eta","Muon_eta", 80, -4, 4,"#eta_{#mu}");
    HistClass::CreateHisto(2,"Muon_phi","Muon_phi", 40, -3.2, 3.2,"#phi_{#mu} (rad)");
    HistClass::CreateHisto(2,"Muon_Vtx_X","Muon_Vtx_X", 100, -1, 1,"Vtx_{x}^{#mu} (cm)");
    HistClass::CreateHisto(2,"Muon_Vtx_Y","Muon_Vtx_Y", 100, -1, 1,"Vtx_{y}^{#mu} (cm)");
    HistClass::CreateHisto(2,"Muon_Vtx_Z","Muon_Vtx_Z", 200, -30, 30,"Vtx_{z}^{#mu} (cm)");
    HistClass::CreateHisto(2,"Muon_Chi2","Muon_Chi2", 400, 0, 200,"#chi_{#mu}^{2}");
    HistClass::CreateHisto(2,"Muon_Ndof","Muon_Ndof", 200, 0, 200,"N_{dof}^{#mu}");
    HistClass::CreateHisto(2,"Muon_LHits","Muon_LHits", 20, 0, 20,"N_{lost hits}^{#mu}");
    HistClass::CreateHisto(2,"Muon_VHits","Muon_VHits", 100, 0, 100,"N_{valid hits}^{#mu}");
    HistClass::CreateHisto(2,"Muon_VHitsPixel","Muon_VHitsPixel", 20, 0, 20,"N_{valid pixel hits}^{#mu}");
    HistClass::CreateHisto(2,"Muon_VHitsTracker","Muon_VHitsTracker", 40, 0, 40,"N_{valid tracker hits}^{#mu}");
    HistClass::CreateHisto(2,"Muon_VHitsMuonSys","Muon_VHitsMuonSys", 60, 0, 60,"N_{valid muon system hits}^{#mu}");
    HistClass::CreateHisto(2,"Muon_TrackerLayersWithMeas","Muon_TrackerLayersWithMeas", 20, 0, 20,"N_{tracker layers with hits}^{#mu}");
    HistClass::CreateHisto(2,"Muon_PixelLayersWithMeas","Muon_PixelLayersWithMeas", 60, 0, 60,"N_{pixel layers with hits}^{#mu}");
    HistClass::CreateHisto(2,"Muon_NMatchedStations","Muon_NMatchedStations", 60, 0, 60,"N_{matched muon stations}^{#mu}");
    HistClass::CreateHisto(2,"Muon_qoverp","Muon_qoverp", 200, 0, 0.2,"#frac{q_{mu}}{p_{mu}}");
    HistClass::CreateHisto(2,"Muon_qoverpError","Muon_qoverpError", 100, 0, 0.001,"#sigma(#frac{q_{mu}}{p_{mu}})");
    HistClass::CreateHisto(2,"Muon_ptError","Muon_ptError", 100, 0, 100,"#sigma(p_{T}^{#mu}) (GeV)");
    HistClass::CreateHisto(2,"Muon_Cocktail_pt","Muon_Cocktail_pt", 2000, 0, 2000,"p_{T}^{cocktail #mu} (GeV)");
    HistClass::CreateHisto(2,"Muon_Cocktail_eta","Muon_Cocktail_eta", 80, -4, 4,"#eta_{cocktail #mu}");
    HistClass::CreateHisto(2,"Muon_Cocktail_phi","Muon_Cocktail_phi", 40, -3.2, 3.2,"#phi_{cocktail #mu} (rad)");
    HistClass::CreateHisto(2,"Muon_CaloIso","Muon_CaloIso", 100, 0, 100,"ISO_{Calo}^{#mu} (GeV)");
    HistClass::CreateHisto(2,"Muon_TrkIso","Muon_TrkIso", 100, 0, 100,"ISO_{Trk}^{#mu} (GeV)");
    HistClass::CreateHisto(2,"Muon_ECALIso","Muon_ECALIso", 100, 0, 100,"ISO_{ECAL}^{#mu} (GeV)");
    HistClass::CreateHisto(2,"Muon_HCALIso","Muon_HCALIso", 100, 0, 100,"ISO_{HCAL}^{#mu} (GeV)");
    HistClass::CreateHisto(2,"Muon_ID","Muon_ID", 6, 0, 6,"ID_{#tau}");
    HistClass::NameBins(2,"Muon_ID",6,d_mydiscmu);
    HistClass::CreateHisto("Muon_num_Gen","Muon_num_Gen", 40, 0, 39, "N_{#mu}^{gen}");
    HistClass::CreateHisto("Muon_pt_Gen","Muon_pt_Gen", 2000, 0, 2000, "p_{T}^{gen #mu} (GeV)");
    HistClass::CreateHisto("Muon_eta_Gen","Muon_eta_Gen", 80, -4, 4, "#eta_{gen #mu}");
    HistClass::CreateHisto("Muon_phi_Gen","Muon_phi_Gen", 40, -3.2, 3.2, "#phi_{gen #mu} (rad)");

    HistClass::CreateHisto(2,"TauMu_mass","TauMu_mass", 2000, 0, 2000, "M_{#tau#mu} (GeV)");
    HistClass::CreateHisto("TauMu_mass_Gen","TauMu_mass_Gen", 2000, 0, 2000, "M_{#tau#mu}^{gen} (GeV)");
    HistClass::CreateHisto(2,"TauMu_DeltaPhi","TauMu_DeltaPhi", 100, 0, 3.2, "#Delta#phi(#tau,#mu) (rad)");
    HistClass::CreateHisto("TauMu_DeltaPhi_Gen","TauMu_DeltaPhi_Gen", 100, 0, 3.2, "#Delta#phi^{gen}(#tau,#mu) (rad)");
    HistClass::CreateHisto(2,"TauMu_tauomuratio","TauMu_tauomuratio", 100, 0, 10, "#frac{p_{T}^{#tau}}{p_{T}^{#mu}}");
    HistClass::CreateHisto("TauMu_tauomuratio_Gen","TauMu_tauomuratio_Gen", 100, 0, 10, "#frac{p_{T}^{gen #tau}}{p_{T}^{gen #mu}}");
}

specialAna::~specialAna() {
}

void specialAna::analyseEvent( const pxl::Event &event ) {

    initEvent( event );

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

    taumu_mass_gen = 0.;
    for(uint i = 0; i < S3ListGen->size(); i++){
      if(TMath::Abs(S3ListGen->at(i)->findUserRecord< int >("id")) == 15){
        for(uint j = 0; j < S3ListGen->size(); j++){
          if(TMath::Abs(S3ListGen->at(j)->findUserRecord< int >("id")) == 13){
            pxl::Particle* dummy_taumu = ( pxl::Particle* ) S3ListGen->at(j)->clone();
            dummy_taumu->addP4(S3ListGen->at(i));
            if (dummy_taumu->getMass() > taumu_mass_gen){
              taumu_mass_gen = dummy_taumu->getMass();
              sel_taumu_gen = ( pxl::Particle* ) dummy_taumu->clone();
              sel_muon_gen = ( pxl::Particle* ) S3ListGen->at(j)->clone();
              sel_tau_gen = ( pxl::Particle* ) S3ListGen->at(i)->clone();
            }
          }
        }
      }
    }
    HistClass::Fill("MC_LLE_Gen",m_GenEvtView->findUserRecord< double >( "MC_LLE" ),weight);
    HistClass::Fill("MC_LQD_Gen",m_GenEvtView->findUserRecord< double >( "MC_LQD" ),weight);
    HistClass::Fill("MC_MSnl_Gen",m_GenEvtView->findUserRecord< double >( "MC_MSnl" ),weight);
    
    if(!(taumu_mass_gen > 580. && taumu_mass_gen < 620.))return;
    
    if(taumu_mass_gen > 0.){
      HistClass::Fill("TauMu_mass_Gen",sel_taumu_gen->getMass(),weight);
      HistClass::Fill("TauMu_DeltaPhi_Gen",DeltaPhi(sel_tau_gen->getPhi(),sel_muon_gen->getPhi()),weight);
      HistClass::Fill("TauMu_tauomuratio_Gen",sel_tau_gen->getPt()/sel_muon_gen->getPt(),weight);
    }
		
//     TODO: Configuration of 'accepted' has to be updated
    if ( ! m_RecEvtView->findUserRecord< bool >( "accepted" ) )return;

    HistClass::Fill("Tau_nums",numTau,1);
    HistClass::Fill("Muon_num",numMuon,1);

    for(uint i = 0; i < TauList->size(); i++){
			  Fill_Tau_Controll_histo(0, TauList->at(i),weight);
			  HistClass::Fill("Tau_eta_phi",TauList->at(i)->getEta(),TauList->at(i)->getPhi(),weight);
    }

    for(uint i = 0; i < MuonList->size(); i++){
				Fill_Muo_Controll_histo(0, MuonList->at(i), weight);
    }

    taumu_mass = 0.;
    for(uint i = 0; i < TauList->size(); i++){
      for(uint j = 0; j < MuonList->size(); j++){
          pxl::Particle* dummy_taumu = ( pxl::Particle* ) MuonList->at(j)->clone();
          dummy_taumu->addP4(TauList->at(i));
          if (dummy_taumu->getMass() > taumu_mass){
            taumu_mass = dummy_taumu->getMass();
            sel_taumu = ( pxl::Particle* ) dummy_taumu->clone();
            sel_muon = ( pxl::Particle* ) MuonList->at(j)->clone();
            sel_tau = ( pxl::Particle* ) TauList->at(i)->clone();
          }
      }
    }
    if(taumu_mass > 0.){
			Fill_TauMu_Controll_histo(0,sel_taumu,sel_tau,sel_muon,weight);
    }

    taumu_mass = 0.;
    bool found = false;
    for(uint i = 0; i < TauList->size(); i++){
      for(uint j = 0; j < MuonList->size(); j++){
				bool tau_ID = TauList->at(i)->findUserRecord< float >("decayModeFindingNewDMs") >= 1 ? true : false;
				bool tau_ISO = TauList->at(i)->findUserRecord< float >("byTightIsolationMVA3newDMwLT") >= 1 ? true : false;
				bool tau_ELE = TauList->at(i)->findUserRecord< float >("againstElectronMediumMVA5"/*"againstElectronTightMVA5"*/) >= 1 ? true : false;
				bool tau_MUO = TauList->at(i)->findUserRecord< float >("againstMuonMedium2"/*"againstMuonTightMVA"*/) >= 1 ? true : false;
				if(MuonList->at(j)->findUserRecord< bool >("isHighPtMuon") && tau_ID && tau_ISO && tau_ELE && tau_MUO){
					pxl::Particle* dummy_taumu = ( pxl::Particle* ) MuonList->at(j)->clone();
          dummy_taumu->addP4(TauList->at(i));
          if(METList->size()>0){
						double dummy_p1 = METList->at(0)->getPx()/(TMath::Sin(TauList->at(i)->getTheta()) * TMath::Cos(TauList->at(i)->getPhi()));
						double dummy_p2 = METList->at(0)->getPy()/(TMath::Sin(TauList->at(i)->getTheta()) * TMath::Sin(TauList->at(i)->getPhi()));
						double dummy_p = (dummy_p1 + dummy_p2) / 2.;
						pxl::Particle* dummy_met = new pxl::Particle();
						dummy_met->setP4(dummy_p*TMath::Sin(TauList->at(i)->getTheta()) * TMath::Cos(TauList->at(i)->getPhi()),dummy_p*TMath::Sin(TauList->at(i)->getTheta()) * TMath::Sin(TauList->at(i)->getPhi()),dummy_p*TMath::Cos(TauList->at(i)->getTheta()),dummy_p);
						dummy_taumu->addP4(dummy_met);
					}
          if (dummy_taumu->getMass() > taumu_mass){
						found = true;
            taumu_mass = dummy_taumu->getMass();
            sel_taumu = ( pxl::Particle* ) dummy_taumu->clone();
            sel_muon = ( pxl::Particle* ) MuonList->at(j)->clone();
            sel_tau = ( pxl::Particle* ) TauList->at(i)->clone();
          }
				}
      }
    }

    if(found){
			Fill_Tau_Controll_histo(1, sel_tau, weight);
			Fill_Muo_Controll_histo(1, sel_muon, weight);
			Fill_TauMu_Controll_histo(1,sel_taumu,sel_tau,sel_muon,weight);
    }

    endEvent( event );
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
}

double specialAna::DeltaPhi(double a, double b) {
  double temp = fabs(a-b);
  if (temp <= TMath::Pi())
    return temp;
  else
    return  2.*TMath::Pi() - temp;
}

void specialAna::endJob( const pxl::ObjectOwner* input ) {

   std::string safeFileName = "SpecialHistos.root";
   TFile file1(safeFileName.c_str(), "RECREATE");
   file1.mkdir("MC");
   file1.cd("MC/");
   HistClass::WriteAll("MC_");
   file1.cd();
   file1.mkdir("Taus");
   file1.cd("Taus/");
   HistClass::WriteAll("_Tau_");
   HistClass::Write2("Tau_eta_phi");
   file1.cd();
   file1.mkdir("Muons");
   file1.cd("Muons/");
   HistClass::WriteAll("_Muon_");
   file1.cd();
   file1.mkdir("TauMu");
   file1.cd("TauMu/");
   HistClass::WriteAll("_TauMu_");
   file1.Close();
}

void specialAna::initEvent( const pxl::Event &event ){

   weight = 1;
   m_RecEvtView = event.getObjectOwner().findObject< pxl::EventView >( "Rec" );
   m_GenEvtView = event.getObjectOwner().findObject< pxl::EventView >( "Gen" );

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
