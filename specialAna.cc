#include "specialAna.hh"
#include "HistClass.hh"
#include "Tools/Tools.hh"
#include "boost/format.hpp"



specialAna::specialAna( const Tools::MConfig &cfg ) :
   runOnData(       cfg.GetItem< bool >( "General.RunOnData" ) ),
   m_JetAlgo(       cfg.GetItem< string >( "Jet.Type.Rec" ) ),
   m_BJets_algo(    cfg.GetItem< string >( "Jet.BJets.Algo" ) ),
   m_METType(       cfg.GetItem< string >( "MET.Type.Rec" ) ),
   m_TauType(       cfg.GetItem< string >( "Tau.Type.Rec" ) ),

   //wprime specific stuff
   m_pt_met_min_cut_ele(    cfg.GetItem< double >( "Ele.wprime.pt_met_cut_min" ) ),
   m_pt_met_max_cut_ele(    cfg.GetItem< double >( "Ele.wprime.pt_met_cut_max" ) ),
   m_delta_phi_cut_ele(     cfg.GetItem< double >( "Ele.wprime.delta_phi_cut" ) ),

   m_pt_met_min_cut_muo(    cfg.GetItem< double >( "Muo.wprime.pt_met_cut_min" ) ),
   m_pt_met_max_cut_muo(    cfg.GetItem< double >( "Muo.wprime.pt_met_cut_max" ) ),
   m_delta_phi_cut_muo(     cfg.GetItem< double >( "Muo.wprime.delta_phi_cut" ) ),

   m_pt_met_min_cut_tau(    cfg.GetItem< double >( "Tau.wprime.pt_met_cut_min" ) ),
   m_pt_met_max_cut_tau(    cfg.GetItem< double >( "Tau.wprime.pt_met_cut_max" ) ),
   m_delta_phi_cut_tau(     cfg.GetItem< double >( "Tau.wprime.delta_phi_cut" ) ),

   m_pt_cut(                cfg.GetItem< double >( "wprime.pt_cut" ) ),
   m_m_cut(                 cfg.GetItem< double >( "wprime.m_cut" ) ),
   m_cutdatafile(           cfg.GetItem< std::string >( "wprime.cutdatafile" ) ),

   m_trigger_string( Tools::splitString< string >( cfg.GetItem< string >( "wprime.TriggerList" ), true  ) ),
   d_mydiscmu(  {"isPFMuon","isGlobalMuon","isTrackerMuon","isStandAloneMuon","isTightMuon","isHighPtMuon"} ),
   m_dataPeriod(            cfg.GetItem< string >( "General.DataPeriod" ) ),
   config_(cfg)
{

    string safeFileName = "SpecialHistos.root";
    file1 = new TFile(safeFileName.c_str(), "RECREATE");
    eventDisplayFile.open(m_cutdatafile.c_str(), std::fstream::out);
    events_ = 0;

    n_lepton = 0; // counting leptons passing the selection

    // number of events, saved in a histogram
    HistClass::CreateHistoUnchangedName("h_counters", "h_counters", 10, 0, 11, "N_{events}");

    if(not runOnData){
        HistClass::CreateHisto("MC_W_m_Gen", 8000, 0, 8000, "M_{W} (GeV)");
        HistClass::CreateHisto("MC_W_pt_Gen", 8000, 0, 8000, "p_{T}^{W} (GeV)");
    }


    for(unsigned int i=0;i<3;i++){
        //str(boost::format("N_{%s}")%particleLatex[i] )
        HistClass::CreateHisto("num",particles[i].c_str(), 40, 0, 39,            TString::Format("N_{%s}", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"pt",particles[i].c_str(), 5000, 0, 5000,       TString::Format("p_{T}^{%s} (GeV)", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"pt_reciprocal",particles[i].c_str(), 5000, 0, 1,       TString::Format("1/p_{T}^{%s} (1/GeV)", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"deltapt_over_pt",particles[i].c_str(), 5000, 0, 10,       TString::Format("#frac{#Delta p_{T}}{p_{T}^{%s}}", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"eta",particles[i].c_str(), 80, -4, 4,          TString::Format("#eta_{%s}", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"phi",particles[i].c_str(), 40, -3.2, 3.2,      TString::Format("#phi_{%s} (rad)", particleSymbols[i].c_str()) );

        HistClass::CreateHisto(4,"DeltaPhi",particles[i].c_str(), 40, 0, 3.2,    TString::Format("#Delta#phi(%s,E_{T}^{miss})", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"mt",particles[i].c_str(), 5000, 0, 5000,       TString::Format("M_{T}_{%s} (GeV)", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"mt_reciprocal",particles[i].c_str(), 5000, 0, 1,       TString::Format("1/M_{T}_{%s} (1/GeV)", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"charge",particles[i].c_str(), 3, -1, 1,        TString::Format("q_{%s}", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"met",particles[i].c_str(), 5000, 0, 5000,      "E^{miss}_{T} (GeV)" );
        HistClass::CreateHisto(4,"met_phi",particles[i].c_str(),40, -3.2, 3.2,   "#phi_{E^{miss}_{T}} (rad)" );
        HistClass::CreateHisto(4,"ET_MET",particles[i].c_str(),50, 0, 6,         TString::Format("p^{%s}_{T}/E^{miss}_{T}", particleSymbols[i].c_str()) );

        HistClass::CreateHisto(4,"jet_1_pt",particles[i].c_str(), 5000, 0, 5000, "p_{T}^{jet} (GeV)" );
        HistClass::CreateHisto(4,"jet_1_eta",particles[i].c_str(), 80, -4, 4,    "#eta_{jet}" );
        HistClass::CreateHisto(4,"jet_1_phi",particles[i].c_str(), 40, -3.2, 3.2,"#phi_{jet} (rad)" );
        HistClass::CreateHisto(4,"jet_1_DeltaPhi",particles[i].c_str(), 40, 0, 3.2,TString::Format("#Delta#phi(%s,jet)", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(4,"jet_1_DeltaPhiMET",particles[i].c_str(), 40, 0, 3.2, "#Delta#phi(E_{T}^{miss},jet)");
        if(not runOnData){
            HistClass::CreateHisto(2,"recoMgen_pt",particles[i].c_str(), 400, -100, 100, "p_{T}^{reco}-p_{T}^{gen} (GeV)" );
            HistClass::CreateHisto(2,"recoMgen_pt_rel",particles[i].c_str(), 400, -10, 10, "#frac{p_{T}^{reco}-p_{T}^{gen}}{p_{T}^{gen}}" );
            HistClass::CreateHisto(2,"num_Gen",particles[i].c_str(), 40, 0, 39,        TString::Format("N_{%s}", particleSymbols[i].c_str()) );
            HistClass::CreateHisto(2,"pt_Gen",particles[i].c_str(), 5000, 0, 5000,     TString::Format("p_{T}^{%s} (GeV)", particleSymbols[i].c_str()) );
            HistClass::CreateHisto(2,"eta_Gen",particles[i].c_str(), 80, -4, 4,        TString::Format("#eta_{%s}", particleSymbols[i].c_str()) );
            HistClass::CreateHisto(2,"phi_Gen",particles[i].c_str(), 40, -3.2, 3.2,    TString::Format("#phi_{%s} (rad)", particleSymbols[i].c_str()) );
        }

    }


    HistClass::CreateHisto(4,"Tau_decayMode", 100, 0, 10, "#N_{decay mode}^{#tau}");
    HistClass::CreateHisto(4,"Tau_Vtx_X", 100, -1, 1, "Vtx_{x}^{#tau} (cm)");
    HistClass::CreateHisto(4,"Tau_Vtx_Y", 100, -1, 1, "Vtx_{y}^{#tau} (cm)");
    HistClass::CreateHisto(4,"Tau_Vtx_Z", 200, -30, 30, "Vtx_{z}^{#tau} (cm)");
    //HistClass::CreateHisto(4,"Tau_EtaEta", 200, -1, 1, "#eta#eta_{#tau}");
    //HistClass::CreateHisto(4,"Tau_EtaPhi", 200, -1, 1, "#eta#phi_{#tau}");
    //HistClass::CreateHisto(4,"Tau_PhiPhi", 200, -1, 1, "#phi#phi_{#tau}");
    //HistClass::CreateHisto(4,"Tau_NumSignaltracks", 40, -3, 37, "N_{sig tracks}^{#tau}");
    HistClass::CreateHisto(4,"Tau_NumPFChargedHadrCands", 40, -3, 37, "N_{PFChargedHadrCands}^{#tau}");
    HistClass::CreateHisto(4,"Tau_NumPFGammaCands", 40, -3, 37, "N_{PFGammaCands}^{#tau}");
    HistClass::CreateHisto(4,"Tau_NumPFNeutralHadrCands", 40, -3, 37, "N_{PFNeutralHadrCands}^{#tau}");
    HistClass::CreateHisto(4,"Tau_LeadingHadronPt", 5000, 0, 5000, "p_{T}^{leading hadron} (GeV)");
    HistClass::CreateHisto(4,"Tau_EMFraction", 100, 0, 10, "EM Fraction^{#tau}");
    HistClass::CreateHisto(4,"Tau_HcalEoverLeadChargedP", 100, 0, 10, "#frac{E_{Hcal}}{P_{leading charged}}");
    HistClass::CreateHisto(4,"Tau_EcalEnergy", 5000, 0, 5000, "E_{Ecal}^{#tau} (GeV)");
    HistClass::CreateHisto(4,"Tau_HcalEnergy", 5000, 0, 5000, "E_{Hcal}^{#tau} (GeV)");
    HistClass::CreateHisto(4,"Tau_Jet_pt", 5000, 0, 5000, "p_{T}^{jet} (GeV)");
    HistClass::CreateHisto(4,"Tau_Jet_eta", 80, -4, 4, "#eta_{jet}");
    HistClass::CreateHisto(4,"Tau_Jet_phi", 40, -3.2, 3.2, "#phi_{jet} (rad)");
    HistClass::CreateHisto(4,"Tau_dxy", 100, 0, 0.1, "d_{xy}^{#tau} (cm)");
    HistClass::CreateHisto(4,"Tau_dxy_error", 100, 0, 0.1, "#sigma(d_{xy}^{#tau}) (cm)");
    HistClass::CreateHisto(4,"Tau_dxy_Sig", 100, 0, 10, "Sig(d_{xy}^{#tau})");
    HistClass::CreateHisto("Tau_eta_phi", 80, -4, 4, 40, -3.2, 3.2,"#eta_{#tau}","#phi_{#tau} (rad)");
    HistClass::CreateHisto(4,"Tau_discriminator", 67, 0, 67,"discriminator_{#tau}");
    HistClass::NameBins(3,"Tau_discriminator",67,d_mydisc);



    HistClass::CreateHisto(4,"Muon_Vtx_X", 100, -1, 1,"Vtx_{x}^{#mu} (cm)");
    HistClass::CreateHisto(4,"Muon_Vtx_Y", 100, -1, 1,"Vtx_{y}^{#mu} (cm)");
    HistClass::CreateHisto(4,"Muon_Vtx_Z", 200, -30, 30,"Vtx_{z}^{#mu} (cm)");
    HistClass::CreateHisto(4,"Muon_Chi2", 400, 0, 200,"#chi_{#mu}^{2}");
    HistClass::CreateHisto(4,"Muon_Ndof", 200, 0, 200,"N_{dof}^{#mu}");
    HistClass::CreateHisto(4,"Muon_LHits", 20, 0, 20,"N_{lost hits}^{#mu}");
    HistClass::CreateHisto(4,"Muon_VHits", 100, 0, 100,"N_{valid hits}^{#mu}");
    HistClass::CreateHisto(4,"Muon_VHitsPixel", 20, 0, 20,"N_{valid pixel hits}^{#mu}");
    HistClass::CreateHisto(4,"Muon_VHitsTracker", 40, 0, 40,"N_{valid tracker hits}^{#mu}");
    HistClass::CreateHisto(4,"Muon_VHitsMuonSys", 60, 0, 60,"N_{valid muon system hits}^{#mu}");
    HistClass::CreateHisto(4,"Muon_TrackerLayersWithMeas", 20, 0, 20,"N_{tracker layers with hits}^{#mu}");
    HistClass::CreateHisto(4,"Muon_PixelLayersWithMeas", 60, 0, 60,"N_{pixel layers with hits}^{#mu}");
    HistClass::CreateHisto(4,"Muon_NMatchedStations", 60, 0, 60,"N_{matched muon stations}^{#mu}");
    HistClass::CreateHisto(4,"Muon_qoverp", 200, 0, 0.2,"#frac{q_{mu}}{p_{mu}}");
    HistClass::CreateHisto(4,"Muon_qoverpError", 100, 0, 0.001,"#sigma(#frac{q_{mu}}{p_{mu}})");
    HistClass::CreateHisto(4,"Muon_ptError", 1000, 0, 1000,"#sigma(p_{T}^{#mu}) (GeV)");
    HistClass::CreateHisto(4,"Muon_ptErroroverpt", 1000, 0, 1.,"#sigma(p_{T}^{#mu})/p_{T}^{#mu} (GeV)");
    HistClass::CreateHisto(4,"Muon_Cocktail_pt", 5000, 0, 5000,"p_{T}^{cocktail #mu} (GeV)");
    HistClass::CreateHisto(4,"Muon_Cocktail_eta", 80, -4, 4,"#eta_{cocktail #mu}");
    HistClass::CreateHisto(4,"Muon_Cocktail_phi", 40, -3.2, 3.2,"#phi_{cocktail #mu} (rad)");
    HistClass::CreateHisto(4,"Muon_CaloIso", 100, 0, 3,"ISO_{Calo}^{#mu} (GeV)");
    HistClass::CreateHisto(4,"Muon_TrkIso", 100, 0, 3,"ISO_{Trk}^{#mu} (GeV)");
    HistClass::CreateHisto(4,"Muon_ECALIso", 100, 0, 3,"ISO_{ECAL}^{#mu} (GeV)");
    HistClass::CreateHisto(4,"Muon_HCALIso", 100, 0, 3,"ISO_{HCAL}^{#mu} (GeV)");
    HistClass::CreateHisto(4,"Muon_ID", 6, 0, 6,"ID_{#mu}");
    HistClass::NameBins(3,"Muon_ID",6,d_mydiscmu);
    HistClass::CreateHisto(4,"Muon_pt_reciprocal", 5000, 0, 1,"1/p_{T}^{#mu} (1/GeV)");
    HistClass::CreateHisto(4,"Muon_mt_reciprocal", 5000, 0, 1,"1/M_{T}^{#mu} (1/GeV)");
    HistClass::CreateHisto(4,"Muon_dpt_over_pt", 5000, 0, 6,"#sigma_{p_{T}}/p_{T}^{#mu}");

    HistClass::CreateHisto(4,"Ele_CaloIso", 100, 0, 100,"CaloIso");
    HistClass::CreateHisto(4,"Ele_ChargeMatch", 100, 0, 100,"ChargeMatch");
    HistClass::CreateHisto(4,"Ele_Class", 100, 0, 100,"Class");
    HistClass::CreateHisto(4,"Ele_DEtaSCCalo", 100, 0, 100,"DEtaSCCalo");
    HistClass::CreateHisto(4,"Ele_DEtaSCVtx", 100, 0, 100,"DEtaSCVtx");
    HistClass::CreateHisto(4,"Ele_DEtaSeedTrk", 100, 0, 100,"DEtaSeedTrk");
    HistClass::CreateHisto(4,"Ele_DPhiSCVtx", 100, 0, 100,"DPhiSCVtx");
    HistClass::CreateHisto(4,"Ele_DPhiSeedTrk", 100, 0, 100,"DPhiSeedTrk");
    HistClass::CreateHisto(4,"Ele_Dsz", 100, 0, 100,"Dsz");
    HistClass::CreateHisto(4,"Ele_DszBS", 100, 0, 100,"DszBS");
    HistClass::CreateHisto(4,"Ele_Dxy", 100, 0, 100,"Dxy");
    HistClass::CreateHisto(4,"Ele_DxyBS", 100, 0, 100,"DxyBS");
    HistClass::CreateHisto(4,"Ele_Dz", 100, 0, 100,"Dz");
    HistClass::CreateHisto(4,"Ele_DzBS", 100, 0, 100,"DzBS");
    HistClass::CreateHisto(4,"Ele_ECALIso", 100, 0, 100,"ECALIso");
    HistClass::CreateHisto(4,"Ele_ECALIso03", 100, 0, 100,"ECALIso03");
    HistClass::CreateHisto(4,"Ele_ECALIso04", 100, 0, 100,"ECALIso04");
    HistClass::CreateHisto(4,"Ele_ESCOverPout", 100, 0, 100,"ESCOverPout");
    HistClass::CreateHisto(4,"Ele_ESCSeedOverP", 100, 0, 100,"ESCSeedOverP");
    HistClass::CreateHisto(4,"Ele_ESCSeedPout", 100, 0, 100,"ESCSeedPout");
    HistClass::CreateHisto(4,"Ele_EoP", 100, 0, 100,"EoP");
    HistClass::CreateHisto(4,"Ele_GSFNormChi2", 100, 0, 100,"GSFNormChi2");
    HistClass::CreateHisto(4,"Ele_HCALIso", 100, 0, 100,"HCALIso");
    HistClass::CreateHisto(4,"Ele_HCALIso03", 100, 0, 100,"HCALIso03");
    HistClass::CreateHisto(4,"Ele_HCALIso03d1", 100, 0, 100,"HCALIso03d1");
    HistClass::CreateHisto(4,"Ele_HCALIso03d2", 100, 0, 100,"HCALIso03d2");
    HistClass::CreateHisto(4,"Ele_HCALIso04", 100, 0, 100,"HCALIso04");
    HistClass::CreateHisto(4,"Ele_HCALIso04d1", 100, 0, 100,"HCALIso04d1");
    HistClass::CreateHisto(4,"Ele_HCALIso04d2", 100, 0, 100,"HCALIso04d2");
    HistClass::CreateHisto(4,"Ele_HCALIsoConeDR03_2012", 100, 0, 100,"HCALIsoConeDR03_2012");
    HistClass::CreateHisto(4,"Ele_HCALIsoConeDR04_2012", 100, 0, 100,"HCALIsoConeDR04_2012");
    HistClass::CreateHisto(4,"Ele_HCALOverECALd1", 100, 0, 100,"HCALOverECALd1");
    HistClass::CreateHisto(4,"Ele_HoEm", 100, 0, 100,"HoEm");
    HistClass::CreateHisto(4,"Ele_HoverE2012", 100, 0, 100,"HoverE2012");
    HistClass::CreateHisto(4,"Ele_Match", 100, 0, 100,"Match");
    HistClass::CreateHisto(4,"Ele_NinnerLayerLostHits", 20, 0, 20,"NinnerLayerLostHits");
    HistClass::CreateHisto(4,"Ele_NumBrems", 100, 0, 100,"NumBrems");
    HistClass::CreateHisto(4,"Ele_PErr", 100, 0, 100,"PErr");
    HistClass::CreateHisto(4,"Ele_SCE", 100, 0, 100,"SCE");
    HistClass::CreateHisto(4,"Ele_SCEErr", 100, 0, 100,"SCEErr");
    HistClass::CreateHisto(4,"Ele_SCEt", 100, 0, 100,"SCEt");
    HistClass::CreateHisto(4,"Ele_SCeta", 100, 0, 100,"SCeta");
    HistClass::CreateHisto(4,"Ele_TrackerP", 1000, 0, 1000,"TrackerP");
    HistClass::CreateHisto(4,"Ele_TrkIso", 100, 0, 100,"TrkIso");
    HistClass::CreateHisto(4,"Ele_TrkIso03", 100, 0, 100,"TrkIso03");
    HistClass::CreateHisto(4,"Ele_TrkIso04", 100, 0, 100,"TrkIso04");
    HistClass::CreateHisto(4,"Ele_Vtx_X", 100, -50, 50,"Vtx_X");
    HistClass::CreateHisto(4,"Ele_Vtx_Y", 100, -50, 50,"Vtx_Y");
    HistClass::CreateHisto(4,"Ele_Vtx_Z", 100, -50, 50,"Vtx_Z");
    HistClass::CreateHisto(4,"Ele_chargedHadronIso", 100, 0, 100,"chargedHadronIso");
    HistClass::CreateHisto(4,"Ele_convDcot", 100, 0, 100,"convDcot");
    HistClass::CreateHisto(4,"Ele_convDist", 100, 0, 100,"convDist");
    HistClass::CreateHisto(4,"Ele_convRadius", 100, 0, 100,"convRadius");
    HistClass::CreateHisto(4,"Ele_e1x5", 100, 0, 100,"e1x5");
    HistClass::CreateHisto(4,"Ele_e2x5", 100, 0, 100,"e2x5");
    HistClass::CreateHisto(4,"Ele_e5x5", 100, 0, 100,"e5x5");
    HistClass::CreateHisto(4,"Ele_neutralHadronIso", 100, 0, 100,"neutralHadronIso");
    HistClass::CreateHisto(4,"Ele_photonIso", 100, 0, 100,"photonIso");
    HistClass::CreateHisto(4,"Ele_pin", 1000, 0, 1000,"pin");
    HistClass::CreateHisto(4,"Ele_pout", 1000, 0, 1000,"pout");
    HistClass::CreateHisto(4,"Ele_puChargedHadronIso", 100, 0, 100,"puChargedHadronIso");
    HistClass::CreateHisto(4,"Ele_sigmaIetaIeta", 100, 0, 10,"sigmaIetaIeta");


    if(not runOnData){
        // 4 for loops to create 480 histograms
        for(unsigned char pi=0; pi<3; pi++){ // loop over particles
            for(unsigned char si=0; si<5; si++){ // loop over shifted particles
                for(unsigned char ti=0; ti<2; ti++){ // loop over type
                    for(unsigned char ui=0; ui<2; ui++){ // loop over updown
                        HistClass::CreateHisto(boost::format("3_%s_pt_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               5000, 0, 5000,
                                               std::string("p_{T}^{") + particleSymbols[pi] + "} (GeV)");
                        HistClass::CreateHisto(boost::format("3_%s_eta_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               5000, 0, 5000,
                                               std::string("#eta_{") + particleSymbols[pi] + "}");
                        HistClass::CreateHisto(boost::format("3_%s_phi_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               40, -3.2, 3.2,
                                               std::string("#phi_{") + particleSymbols[pi] + "} (rad)");
                        HistClass::CreateHisto(boost::format("3_%s_DeltaPhi_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               40, 0, 3.2,
                                               std::string("#Delta#phi(") + particleSymbols[pi] + ",E_{T}^{miss})");
                        HistClass::CreateHisto(boost::format("3_%s_mt_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               5000, 0, 5000,
                                               std::string("M_{T}_{") + particleSymbols[pi] + "} (GeV)");
                        HistClass::CreateHisto(boost::format("3_%s_ET_MET_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               50, 0, 6,
                                               std::string("p^{") + particleSymbols[pi] + "}_{T}/E^{miss}_{T}");
                        HistClass::CreateHisto(boost::format("3_%s_met_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               5000, 0, 5000,
                                               "E^{miss}_{T} (GeV)");
                        HistClass::CreateHisto(boost::format("3_%s_met_phi_syst_%s%s%s")%particles[pi] %shifted[si] %type[ti] %updown[ui],
                                               40, -3.2, 3.2,"#phi_{E^{miss}_{T}} (rad)");
                    }
                }
            }
        }

        int bins []={100,5000};
        double xmin []={-10,0};
        double xmax []={10,5000};
        string xtitle []= {"res","p_{T} (GeV)"};
        HistClass::CreateNSparse("Muon_Res",2,bins,xmin ,xmax ,xtitle  );

        bins [0]=5000;
        xmin [0]=0;
        xmax [0]=0;
        xtitle [0]= "p_{T} (GeV)";
        xtitle [1]= "M (GeV)";
        HistClass::CreateNSparse("W_pt_m_Gen",2,bins,xmin ,xmax ,xtitle  );
    }


    //Kinematics
    mLeptonTree["mt"]=0;
    mLeptonTree["delta_phi"]=0;
    mLeptonTree["pt"]=0;
    mLeptonTree["met"]=0;
    mLeptonTree["lepton_phi"]=0;
    mLeptonTree["lepton_eta"]=0;
    mLeptonTree["met_phi"]=0;
    mLeptonTree["jet1_et"]=0;
    mLeptonTree["jet1_phi"]=0;
    mLeptonTree["jet1_eta"]=0;

    //PDF
    mLeptonTree["id1"]=999;
    mLeptonTree["id2"]=999;
    mLeptonTree["x1"]=999;
    mLeptonTree["x2"]=999;
    mLeptonTree["qscale"]=-1;

    //general
    mLeptonTree["ThisWeight"]=0;
    mLeptonTree["lepton_type"]=0;
    HistClass::CreateTree( mLeptonTree, "slimtree");




}

specialAna::~specialAna() {
}

void specialAna::analyseEvent( const pxl::Event* event ) {
    initEvent( event );
    if(tail_selector(event)) return;
    if(not runOnData){
        Fill_Gen_Controll_histo();
    }

    applyKfactor(event,0);

    KinematicsSelector();
    if(not runOnData){
        aplyDataMCScaleFactors();
    }

    if (!TriggerSelector(event)) return;

    cleanJets();
    Fill_stage_0_histos();



    if(sel_lepton && sel_met){
        Fill_Tree();
        if(sel_lepton->getUserRecord("passedDeltaPhi")){
            Fill_Controll_histo(1, sel_lepton);
        }
        if(sel_lepton->getUserRecord("passedPtMet")){
            Fill_Controll_histo(2, sel_lepton);
        }
        if(sel_lepton->getUserRecord("passed")){
            Fill_Controll_histo(3, sel_lepton);
            n_lepton++;
        }

        if((sel_lepton->getPt() > m_pt_cut) && (sel_lepton->getMass() > m_m_cut)) {
            // save event information (after cuts) to stringstream which is written to disk in endJob()
            // for event display generation:
            eventsAfterCuts << event->getUserRecord("Dataset") << ":"
                            << event->getUserRecord("Run") << ":"
                            << event->getUserRecord("LumiSection") << ":"
                            << event->getUserRecord("EventNum") << "\n";
        }
    }

    // plan as follows:
    // (in music:)
    // - create a new "Systematics" object
    // - initialize it with the config file
    // - per event:
    // - give it a copy of an Event, whose particles shall be shifted
    // - let it make systematic shifts
    //   |-> different for electrons, muons and taus
    // (in specialAna:)
    // - check for newly created EventViews (did they fail?)
    // - fill (systematically shifted) particles into histograms
    if(!runOnData){
        FillSystematics(event, "Muon");
        FillSystematics(event, "Ele");
        FillSystematics(event, "Tau");
        // FillSystematics(event, "Jet");
        // FillSystematics(event, "met");
    }

    endEvent( event );
}

void specialAna::FillSystematics(const pxl::Event* event, std::string const particleName){
    FillSystematicsUpDown(event, particleName, "Up", "Scale");
    FillSystematicsUpDown(event, particleName, "Down", "Scale");
    //FillSystematicsUpDown(event, particleName, "Up", "Resolution");
    //FillSystematicsUpDown(event, particleName, "Down", "Resolution");
}

void specialAna::FillSystematicsUpDown(const pxl::Event* event, std::string const particleName, std::string const updown, std::string const shiftType){
    pxl::EventView *tempEventView;

    // extract one EventView
    // make sure the object key is the same as in Systematics.cc specified
//     tempEventView = event->getObjectOwner().findObject< pxl::EventView >(particleName + "_syst" + shiftType + updown);
    tempEventView = event->findObject< pxl::EventView >(particleName + "_syst" + shiftType + updown);



    if(tempEventView == 0){
        throw std::runtime_error("specialAna.cc: no EventView '" + particleName + "_syst" + shiftType + updown + "' found!");
    }
    // get all particles
    std::vector< pxl::Particle* > shiftedParticles;
    tempEventView->getObjectsOfType< pxl::Particle >(shiftedParticles);

    //backup OldList
    RememberMET=METList;
    METList = new vector< pxl::Particle* >;
    if(particleName=="Muon"){
        RememberPart=MuonList;
        MuonList = new vector< pxl::Particle* >;
        for( vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if(      Name == "Muon"    ) MuonList->push_back( part );
            else if( Name == m_METType ) METList->push_back( part );
        }
    }else if(particleName=="Ele"){
        RememberPart=EleList;
        EleList = new vector< pxl::Particle* >;
        for( vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if(      Name == "Ele"     ) EleList->push_back( part );
            else if( Name == m_METType ) METList->push_back( part );
        }
    }else if(particleName=="Tau"){
        RememberPart=TauList;
        TauList = new vector< pxl::Particle* >;
        for( vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if(      Name == m_TauType ) TauList->push_back( part );
            else if( Name == m_METType ) METList->push_back( part );
        }
    }//else if(particleName=="JET"){
    //}else if(particleName==m_METType){}

    // reset the chosen MET and lepton
    if(METList->size()>0){
        sel_met=METList->at(0);
    }else{
        sel_met=0;
    }
    sel_lepton=0;

    KinematicsSelector();

    if(sel_lepton && sel_met){
        //if(sel_lepton->getUserRecord("passedDeltaPhi")){
            //Fill_Particle_hisos(1, sel_lepton, shiftType + updown);
        //}
        //if(sel_lepton->getUserRecord("passedPtMet")){
            //Fill_Particle_hisos(2, sel_lepton, shiftType + updown);
        //}
        if(sel_lepton->getUserRecord("passed")){
            Fill_Particle_hisos(3, sel_lepton, particleName + shiftType + updown);
            // cout << "h1_3_" << sel_lepton->getName() << "_[val]_syst_" + particleName + shiftType + updown << endl;
        }
    }


    // return to backup
    delete METList;
    METList = RememberMET;
    if(particleName=="Muon"){
        delete MuonList;
        MuonList = RememberPart;
    }else if(particleName=="Ele"){
        delete EleList;
        EleList = RememberPart;
    }else if(particleName=="Tau"){
        delete TauList;
        TauList = RememberPart;
    }//else if(particleName=="JET"){
    //}else if(particleName==m_METType){}

}

bool specialAna::Check_Tau_ID(pxl::Particle* tau) {
    bool passed = false;
    bool tau_ID = tau->getUserRecord("decayModeFindingNewDMs").asBool();
    bool tau_ISO = tau->getUserRecord("byTightIsolationMVA3newDMwLT").asBool();
    bool tau_ELE = tau->getUserRecord("againstElectronMediumMVA5"/*"againstElectronTightMVA5"*/).asBool();
    bool tau_MUO = tau->getUserRecord("againstMuonMedium2"/*"againstMuonTightMVA"*/).asBool();
    if (tau_ID && tau_ISO && tau_ELE && tau_MUO) passed = true;
    return passed;
}

bool specialAna::Check_Muo_ID(pxl::Particle* muon) {
	bool passed = false;
	muon->getUserRecord("isHighPtMuon").asBool() ? passed = true : passed = false;
	return passed;
}


void specialAna::KinematicsSelector() {

    bool passed=false;
    bool passedPtMet=false;
    bool passedDeltaPhi=false;
    int sel_id=0;

    //make veto numbers
    //we don't need vectors, do we?
    double m_leptonVetoPt=20;
    int numVetoMuo=vetoNumber(MuonList,m_leptonVetoPt);
    int numVetoTau=vetoNumber(TauList,m_leptonVetoPt);
    int numVetoEle=vetoNumber(EleList,m_leptonVetoPt);


<<<<<<< HEAD
    if( EleList->size()==1 && numVetoTau==0 && numVetoMuo==0 ){
        sel_lepton=( pxl::Particle* ) EleList->at(0);
        m_delta_phi_cut=m_delta_phi_cut_ele;
        m_pt_met_min_cut=m_pt_met_min_cut_ele;
        m_pt_met_max_cut=m_pt_met_max_cut_ele;
        sel_id=11;
=======

    if( EleList->size()>=1 && numVetoTau==0 && numVetoMuo==0 ){
        bool passedID=false;
        for( vector< pxl::Particle* >::iterator it = EleList->begin(); it != EleList->end(); ++it ) {
            if( (*it)->hasUserRecord("IDpassed")){
                if ( not passedID &&  (*it)->getUserRecord("IDpassed").toBool() ){
                    passedID=true;
                }else if(passedID && (*it)->getUserRecord("IDpassed").toBool()){
                    passedID=false;
                    break;
                }
            }else if (EleList->size()==1){
                passedID=true;
                break;
            }
        }
        if(passedID){
            sel_lepton=( pxl::Particle* ) EleList->at(0);
            m_delta_phi_cut=m_delta_phi_cut_ele;
            m_pt_met_min_cut=m_pt_met_min_cut_ele;
            m_pt_met_max_cut=m_pt_met_max_cut_ele;
            sel_id=11;
        }
>>>>>>> Changed isolation histograms, fixed IDpassed in KinematicsSelector
    }
    if( numVetoEle==0 && numVetoTau==0 && MuonList->size()>=1 ){
        bool passedID=false;
        for( vector< pxl::Particle* >::iterator it = MuonList->begin(); it != MuonList->end(); ++it ) {
            if( (*it)->hasUserRecord("IDpassed")){
                if ( not passedID &&  (*it)->getUserRecord("IDpassed").toBool() ){
                    passedID=true;
                }else if(passedID && (*it)->getUserRecord("IDpassed").toBool()){
                    passedID=false;
                    break;
                }
            }else if (MuonList->size()==1){
                passedID=true;
                break;
            }
        }
        if(passedID){
            sel_lepton=( pxl::Particle* ) MuonList->at(0);
            m_delta_phi_cut=m_delta_phi_cut_muo;
            m_pt_met_min_cut=m_pt_met_min_cut_muo;
            m_pt_met_max_cut=m_pt_met_max_cut_muo;
            sel_id=13;
        }
    }
    if( numVetoEle==0 && TauList->size()==1 && numVetoMuo==0 ){
        sel_lepton=( pxl::Particle* ) TauList->at(0);
        m_delta_phi_cut=m_delta_phi_cut_tau;
        m_pt_met_min_cut=m_pt_met_min_cut_tau;
        m_pt_met_max_cut=m_pt_met_max_cut_tau;
        sel_id=15;
    }




    if(sel_met && sel_lepton){
        if(sel_lepton->getPt()/sel_met->getPt()>m_pt_met_min_cut && sel_lepton->getPt()/sel_met->getPt()<m_pt_met_max_cut){
            passedPtMet=true;
        }
        if(DeltaPhi(sel_lepton->getPhi(),sel_met->getPhi())>m_delta_phi_cut){
            passedDeltaPhi=true;
        }

        if (passedDeltaPhi && passedPtMet){
            passed=true;
        }

    }else{
    }
    if(sel_lepton){
        sel_lepton->setUserRecord("passedPtMet",passedPtMet);
        sel_lepton->setUserRecord("passedDeltaPhi",passedDeltaPhi);
        sel_lepton->setUserRecord("passed",passed);
        sel_lepton->setPdgNumber(sel_id);
    }
}

bool specialAna::TriggerSelector(const pxl::Event* event){
    bool triggered=false;
    bool tiggerKinematics=false;


    //I dont understand the 8TeV triggers at the moment!!
    if( m_dataPeriod =="13TeV"){
        //this is 13 TeV
        for( vector< string >::const_iterator it=m_trigger_string.begin(); it!= m_trigger_string.end();it++){
            try{
                triggered=m_TrigEvtView->getUserRecord(*it);
                if(triggered){
                    break;
                }
            } catch( std::runtime_error &exc ) {
                continue;
            }
        }
    }else{
        //this is 8TeV
        pxl::UserRecords::const_iterator us = m_TrigEvtView->getUserRecords().begin();
        for( ; us != m_TrigEvtView->getUserRecords().end(); ++us ) {
            if (
                string::npos != (*us).first.find( "HLT_HLT_Ele90_CaloIdVT_GsfTrkIdT") or
                string::npos != (*us).first.find( "HLT_Ele80_CaloIdVT_GsfTrkIdT") or
                string::npos != (*us).first.find( "HLT_Ele80_CaloIdVT_TrkIdT") or
                //string::npos != (*us).first.find( "HLT_HLT_Ele27_WP80_v") or
                string::npos != (*us).first.find( "HLT_HLT_Mu40_v") or
                string::npos != (*us).first.find( "HLT_HLT_Mu40_eta2p1_v") or
                //string::npos != (*us).first.find( "HLT_HLT_IsoMu30_v") or
                string::npos != (*us).first.find( "HLT_MonoCentralPFJet80")
            ){
                triggered=(*us).second;
                if(triggered){
                    break;
                }
            }
        }

    }


    //pxl::UserRecords::const_iterator us = m_TrigEvtView->getUserRecords().begin();
    //for( ; us != m_TrigEvtView->getUserRecords().end(); ++us ) {
        ////if ( string::npos == (*us).first.find( *it )){
        //if ( string::npos != (*us).first.find( "HLT_HLT")){
            //triggers.insert(us->first);
            ////std::cout<<us->first <<std::endl;
        //}
    //}

    if(sel_lepton && sel_met){
        if(sel_lepton->getName()==m_TauType){
            if(sel_lepton->getPt()>100 && sel_met->getPt()>150){
                tiggerKinematics=true;
            }

        }
        if(sel_lepton->getName()=="Muon"){
            if(sel_lepton->getPt()>25){
                tiggerKinematics=true;
            }
        }
        if(sel_lepton->getName()=="Ele"){
            if(sel_lepton->getPt()>30){
                tiggerKinematics=true;
            }
        }

    }

      //std::cout<<triggered <<"   "<< tiggerKinematics<<std::endl;

     return (triggered && tiggerKinematics);

}

void specialAna::Fill_Tree(){
    mLeptonTree["mt"]=MT(sel_lepton,sel_met);
    mLeptonTree["delta_phi"]=DeltaPhi(sel_lepton,sel_met);
    mLeptonTree["pt"]=sel_lepton->getPt();
    mLeptonTree["met"]=sel_met->getPt();
    mLeptonTree["lepton_phi"]=sel_lepton->getPhi();
    mLeptonTree["lepton_eta"]=sel_lepton->getEta();
    mLeptonTree["met_phi"]=sel_met->getPhi();
    if(JetList->size()>0){
        pxl::Particle* jet = (pxl::Particle*) JetList->at(0);
        mLeptonTree["jet1_et"]=jet->getPt();
        mLeptonTree["jet1_phi"]=jet->getPhi();
        mLeptonTree["jet1_eta"]=jet->getEta();
    }else{
        mLeptonTree["jet1_et"]=-1;
        mLeptonTree["jet1_phi"]=99;
        mLeptonTree["jet1_eta"]=99;

    }

    //PDF
    if( not runOnData ){
        mLeptonTree["id1"]=m_GenEvtView->getUserRecord("f1");
        mLeptonTree["id2"]=m_GenEvtView->getUserRecord("f2");
        mLeptonTree["x1"]=m_GenEvtView->getUserRecord("x1");
        mLeptonTree["x2"]=m_GenEvtView->getUserRecord("x2");
        mLeptonTree["qscale"]=m_GenEvtView->getUserRecord("Q");
    }

    //general
    mLeptonTree["ThisWeight"]=weight;
    //mLeptonTree["lepton_type"]=sel_lepton->getUserRecord("id");
    mLeptonTree["lepton_type"]=sel_lepton->getPdgNumber();

    HistClass::FillTree("slimtree");

}

void specialAna::Fill_stage_0_histos() {

    HistClass::Fill("Tau_num",numTau,1);
    HistClass::Fill("Muon_num",numMuon,1);
    HistClass::Fill("Ele_num",numEle,1);

    for(uint i = 0; i < TauList->size(); i++){
        Fill_Controll_Tau_histo(0, TauList->at(i));
    }

    for(uint i = 0; i < MuonList->size(); i++){
        Fill_Controll_Muon_histo(0, MuonList->at(i));
    }

    for(uint i = 0; i < EleList->size(); i++){
        Fill_Controll_Ele_histo(0, EleList->at(i));
    }

}




bool specialAna::tail_selector( const pxl::Event* event) {
    string datastream = event->getUserRecord( "Dataset" );
    TString Datastream = datastream;

<<<<<<< HEAD
    //std::cout << datastream << std::endl;

    /// W tail fitting
    if(Datastream.Contains("WTo") && Datastream.Contains("Nu_Tune4C_13TeV")) {
        //std::cout << "blub\n";
        for(uint i = 0; i < S3ListGen->size(); i++){
            if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){ //W
                if(S3ListGen->at(i)->getMass() > 200) return true;
            }
        }
    }
    if(Datastream.Contains("WTo") && Datastream.Contains("Nu_M_200")) {
        for(uint i = 0; i < S3ListGen->size(); i++){
            if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                if(S3ListGen->at(i)->getMass() > 500) return true;
=======
    if( m_dataPeriod=="13TeV" ){
        /// W tail fitting
        if(Datastream.Contains("WTo") && Datastream.Contains("Nu_13TeV")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){ //W
                    if(S3ListGen->at(i)->getMass() > 200) return true;
                }
            }
        }
        if(Datastream.Contains("WTo") && Datastream.Contains("Nu_M_200_13TeV")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    if(S3ListGen->at(i)->getMass() > 500) return true;
                }
            }
        }
        if(Datastream.Contains("WTo") && Datastream.Contains("Nu_M_500_13TeV")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    if(S3ListGen->at(i)->getMass() > 1000) return true;
                }
            }
        }
        if(Datastream.Contains("WTo") && Datastream.Contains("Nu_M_1000_13TeV")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    if(S3ListGen->at(i)->getMass() > 4000) return true;
                }
            }
        }
    }else if( m_dataPeriod=="8TeV" ){

        /// W tail fitting
        if(Datastream.Contains("WJetsToLNu_TuneZ2Star_8TeV")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getUserRecord("id").toInt32()) == 24){
                    if(S3ListGen->at(i)->getPt() > 55)return true;
                }
            }
        }
        if(Datastream.Contains("WJetsToLNu_PtW")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getUserRecord("id").toInt32()) == 24){
                    if(S3ListGen->at(i)->getPt() <= 55)return true;
                }
>>>>>>> added compability to 8 TeV trigger and add dataset tag
            }
        }
<<<<<<< HEAD
    }
    if(Datastream.Contains("WTo") && Datastream.Contains("Nu_M_500")) {
        for(uint i = 0; i < S3ListGen->size(); i++){
            if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                if(S3ListGen->at(i)->getMass() > 1000) return true;
            }
        }
    }
    //if(Datastream.Contains("WTo") && Datastream.Contains("Nu_M_1000_13TeV")) {
        //for(uint i = 0; i < S3ListGen->size(); i++){
            //if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                //if(S3ListGen->at(i)->getMass() > 4000) return true;
            //}
        //}
    //}
=======
        /// W mass tail fitting
        if(Datastream.Contains("WJetsToLNu")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getUserRecord("id").toInt32()) == 24){
                    if(S3ListGen->at(i)->getMass() > 300)return true;
                }
            }
        }
        if(Datastream.Contains("WTo")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getUserRecord("id").toInt32()) == 24){
                    if(S3ListGen->at(i)->getMass() < 300)return true;
                }
            }
        }
    }
>>>>>>> added compability to 8 TeV trigger and add dataset tag

    /// Diboson tail fitting
//     if(Datastream.Contains("WW_") || Datastream.Contains("WZ_") || Datastream.Contains("ZZ_")) {
//         for(uint i = 0; i < S3ListGen->size(); i++){
//                 int part_id = TMath::Abs(S3ListGen->at(i)->getUserRecord("id").toInt32());
//           if(part_id == 23 || part_id == 22){
//                     if(S3ListGen->at(i)->getPt() > 500)return true;
//           }
//         }
//     }


    return false;
}



void specialAna::Fill_Gen_Controll_histo() {


    int muon_gen_num=0;
    int ele_gen_num=0;
    int tau_gen_num=0;
    for(uint i = 0; i < S3ListGen->size(); i++){
        if (S3ListGen->at(i)->getPt()<10 && not (TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24)   ){
            continue;
        }
        if(S3ListGen->at(i)->getPdgNumber()==0){
            if(S3ListGen->at(i)->hasUserRecord("id")){
                S3ListGen->at(i)->setPdgNumber(S3ListGen->at(i)->getUserRecord("id"));
            }
        }
        if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 13){
            muon_gen_num++;
            HistClass::Fill(0,"Muon_pt_Gen",S3ListGen->at(i)->getPt(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill(0,"Muon_eta_Gen",S3ListGen->at(i)->getEta(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill(0,"Muon_phi_Gen",S3ListGen->at(i)->getPhi(),m_GenEvtView->getUserRecord( "Weight" ));
        }else if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 15){
            tau_gen_num++;
            HistClass::Fill(0,"Tau_pt_Gen",S3ListGen->at(i)->getPt(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill(0,"Tau_eta_Gen",S3ListGen->at(i)->getEta(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill(0,"Tau_phi_Gen",S3ListGen->at(i)->getPhi(),m_GenEvtView->getUserRecord( "Weight" ));
        }else if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 11){
            ele_gen_num++;
            HistClass::Fill(0,"Ele_pt_Gen",S3ListGen->at(i)->getPt(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill(0,"Ele_eta_Gen",S3ListGen->at(i)->getEta(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill(0,"Ele_phi_Gen",S3ListGen->at(i)->getPhi(),m_GenEvtView->getUserRecord( "Weight" ));
        }else if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
            if( (( pxl::Particle*)  S3ListGen->at(i)->getMother())!=0){
                if(TMath::Abs( (( pxl::Particle*)  S3ListGen->at(i)->getMother())->getPdgNumber()) == 15){
                    continue;
                }
            }
            HistClass::Fill("MC_W_m_Gen",S3ListGen->at(i)->getMass(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill("MC_W_pt_Gen",S3ListGen->at(i)->getPt(),m_GenEvtView->getUserRecord( "Weight" ));


            HistClass::FillSparse( "W_pt_m_Gen",2,S3ListGen->at(i)->getPt(),S3ListGen->at(i)->getMass());
            HistClass::FillSparse( "W_pt_m_Gen",2,S3ListGen->at(i)->getPt(),S3ListGen->at(i)->getMass());

        }

    }


    HistClass::Fill(0,"Tau_num_Gen",tau_gen_num,m_GenEvtView->getUserRecord( "Weight" ));
    HistClass::Fill(0,"Muon_num_Gen",muon_gen_num,m_GenEvtView->getUserRecord( "Weight" ));
    HistClass::Fill(0,"Ele_num_Gen",ele_gen_num,m_GenEvtView->getUserRecord( "Weight" ));



    //resolution plots:
    for( vector< pxl::Particle* >::const_iterator part_it = MuonList->begin(); part_it != MuonList->end(); ++part_it ) {
        pxl::Particle *part = *part_it;
        if(part->getSoftRelations().hasType("priv-gen-rec")){
            pxl::Particle *genPart = (pxl::Particle*) part->getSoftRelations().getFirst (m_GenEvtView->getObjectOwner(), "priv-gen-rec");
            Fill_Gen_Rec_histos(genPart,part);
        }
    }


}



void specialAna::Fill_Controll_Muon_histo(int hist_number, pxl::Particle* lepton){
    Fill_Particle_hisos(hist_number,lepton);

    HistClass::Fill(hist_number,"Muon_Vtx_X",lepton->getUserRecord("Vtx_X"),weight);
    HistClass::Fill(hist_number,"Muon_Vtx_Y",lepton->getUserRecord("Vtx_Y"),weight);
    HistClass::Fill(hist_number,"Muon_Vtx_Z",lepton->getUserRecord("Vtx_Z"),weight);
    for(uint j = 0; j < 6; j++) {
        if(lepton->getUserRecord( (string)d_mydiscmu[j] ))
            HistClass::Fill(hist_number,"Muon_ID",j+1,weight);
    }
    HistClass::Fill(hist_number,"Muon_Chi2",lepton->getUserRecord("chi2"),weight);
    HistClass::Fill(hist_number,"Muon_Ndof",lepton->getUserRecord("ndof"),weight);
    HistClass::Fill(hist_number,"Muon_LHits",lepton->getUserRecord("LHits"),weight);
    HistClass::Fill(hist_number,"Muon_VHits",lepton->getUserRecord("VHits"),weight);
    HistClass::Fill(hist_number,"Muon_VHitsPixel",lepton->getUserRecord("VHitsPixel"),weight);
    HistClass::Fill(hist_number,"Muon_VHitsTracker",lepton->getUserRecord("VHitsTracker"),weight);
    HistClass::Fill(hist_number,"Muon_VHitsMuonSys",lepton->getUserRecord("VHitsMuonSys"),weight);
    HistClass::Fill(hist_number,"Muon_TrackerLayersWithMeas",lepton->getUserRecord("TrackerLayersWithMeas"),weight);
    HistClass::Fill(hist_number,"Muon_PixelLayersWithMeas",lepton->getUserRecord("PixelLayersWithMeas"),weight);
    HistClass::Fill(hist_number,"Muon_NMatchedStations",lepton->getUserRecord("NMatchedStations"),weight);
    HistClass::Fill(hist_number,"Muon_qoverp",lepton->getUserRecord("qoverp"),weight);
    HistClass::Fill(hist_number,"Muon_qoverpError",lepton->getUserRecord("qoverpError"),weight);
    HistClass::Fill(hist_number,"Muon_ptError",lepton->getUserRecord("ptError"),weight);
    HistClass::Fill(hist_number,"Muon_ptErroroverpt",lepton->getUserRecord("ptError").toDouble()/lepton->getPt(),weight);
    if(lepton->getUserRecord("validCocktail")){
        TLorentzVector* cocktailMuon = new TLorentzVector();
        cocktailMuon->SetXYZM(lepton->getUserRecord("pxCocktail"),lepton->getUserRecord("pyCocktail"),lepton->getUserRecord("pzCocktail"),0.105);
        HistClass::Fill(hist_number,"Muon_Cocktail_pt",cocktailMuon->Pt(),weight);
        HistClass::Fill(hist_number,"Muon_Cocktail_eta",cocktailMuon->Eta(),weight);
        HistClass::Fill(hist_number,"Muon_Cocktail_phi",cocktailMuon->Phi(),weight);
        delete cocktailMuon;
    }
    HistClass::Fill(hist_number,"Muon_CaloIso",lepton->getUserRecord("CaloIso").toDouble() / lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Muon_TrkIso",lepton->getUserRecord("TrkIso").toDouble() / lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Muon_ECALIso",lepton->getUserRecord("ECALIso").toDouble() / lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Muon_HCALIso",lepton->getUserRecord("HCALIso").toDouble() / lepton->getPt(),weight);

    HistClass::Fill(hist_number,"Muon_pt_reciprocal",1/lepton->getPt(),weight);
    if(sel_met)
        HistClass::Fill(hist_number,"Muon_mt_reciprocal",1/MT(lepton,sel_met),weight);
    HistClass::Fill(hist_number,"Muon_dpt_over_pt",(lepton->getUserRecord("ptError").toDouble())/lepton->getPt(),weight);
}
void specialAna::Fill_Controll_Ele_histo(int hist_number, pxl::Particle* lepton){
    Fill_Particle_hisos(hist_number,lepton);

    HistClass::Fill(hist_number,"Ele_CaloIso",lepton->getUserRecord ("CaloIso"),weight);
    //HistClass::Fill(hist_number,"Ele_ChargeMatch",lepton->getUserRecord ("ChargeMatch"),weight);
    HistClass::Fill(hist_number,"Ele_Class",lepton->getUserRecord ("Class"),weight);
    HistClass::Fill(hist_number,"Ele_DEtaSCCalo",lepton->getUserRecord ("DEtaSCCalo"),weight);
    HistClass::Fill(hist_number,"Ele_DEtaSCVtx",lepton->getUserRecord ("DEtaSCVtx"),weight);
    HistClass::Fill(hist_number,"Ele_DEtaSeedTrk",lepton->getUserRecord ("DEtaSeedTrk"),weight);
    HistClass::Fill(hist_number,"Ele_DPhiSCVtx",lepton->getUserRecord ("DPhiSCVtx"),weight);
    HistClass::Fill(hist_number,"Ele_DPhiSeedTrk",lepton->getUserRecord ("DPhiSeedTrk"),weight);
    HistClass::Fill(hist_number,"Ele_Dsz",lepton->getUserRecord ("Dsz"),weight);
    HistClass::Fill(hist_number,"Ele_DszBS",lepton->getUserRecord ("DszBS"),weight);
    HistClass::Fill(hist_number,"Ele_Dxy",lepton->getUserRecord ("Dxy"),weight);
    HistClass::Fill(hist_number,"Ele_DxyBS",lepton->getUserRecord ("DxyBS"),weight);
    HistClass::Fill(hist_number,"Ele_Dz",lepton->getUserRecord ("Dz"),weight);
    HistClass::Fill(hist_number,"Ele_DzBS",lepton->getUserRecord ("DzBS"),weight);
    HistClass::Fill(hist_number,"Ele_ECALIso",lepton->getUserRecord ("ECALIso"),weight);
    HistClass::Fill(hist_number,"Ele_ECALIso03",lepton->getUserRecord ("ECALIso03"),weight);
    HistClass::Fill(hist_number,"Ele_ECALIso04",lepton->getUserRecord ("ECALIso04"),weight);
    HistClass::Fill(hist_number,"Ele_ESCOverPout",lepton->getUserRecord ("ESCOverPout"),weight);
    HistClass::Fill(hist_number,"Ele_ESCSeedOverP",lepton->getUserRecord ("ESCSeedOverP"),weight);
    HistClass::Fill(hist_number,"Ele_ESCSeedPout",lepton->getUserRecord ("ESCSeedPout"),weight);
    HistClass::Fill(hist_number,"Ele_EoP",lepton->getUserRecord ("EoP"),weight);
    HistClass::Fill(hist_number,"Ele_GSFNormChi2",lepton->getUserRecord ("GSFNormChi2"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso",lepton->getUserRecord ("HCALIso"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso03",lepton->getUserRecord ("HCALIso03"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso03d1",lepton->getUserRecord ("HCALIso03d1"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso03d2",lepton->getUserRecord ("HCALIso03d2"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso04",lepton->getUserRecord ("HCALIso04"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso04d1",lepton->getUserRecord ("HCALIso04d1"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso04d2",lepton->getUserRecord ("HCALIso04d2"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIsoConeDR03_2012",lepton->getUserRecord ("HCALIsoConeDR03_2012"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIsoConeDR04_2012",lepton->getUserRecord ("HCALIsoConeDR04_2012"),weight);
    HistClass::Fill(hist_number,"Ele_HCALOverECALd1",lepton->getUserRecord ("HCALOverECALd1"),weight);
    HistClass::Fill(hist_number,"Ele_HoEm",lepton->getUserRecord ("HoEm"),weight);
    HistClass::Fill(hist_number,"Ele_HoverE2012",lepton->getUserRecord ("HoverE2012"),weight);
    HistClass::Fill(hist_number,"Ele_NinnerLayerLostHits",lepton->getUserRecord ("NinnerLayerLostHits"),weight);
    HistClass::Fill(hist_number,"Ele_NumBrems",lepton->getUserRecord ("NumBrems"),weight);
    HistClass::Fill(hist_number,"Ele_PErr",lepton->getUserRecord ("PErr"),weight);
    HistClass::Fill(hist_number,"Ele_SCE",lepton->getUserRecord ("SCE"),weight);
    HistClass::Fill(hist_number,"Ele_SCEErr",lepton->getUserRecord ("SCEErr"),weight);
    HistClass::Fill(hist_number,"Ele_SCEt",lepton->getUserRecord ("SCEt"),weight);
    HistClass::Fill(hist_number,"Ele_SCeta",lepton->getUserRecord ("SCeta"),weight);
    HistClass::Fill(hist_number,"Ele_TrackerP",lepton->getUserRecord ("TrackerP"),weight);
    HistClass::Fill(hist_number,"Ele_TrkIso",lepton->getUserRecord ("TrkIso"),weight);
    HistClass::Fill(hist_number,"Ele_TrkIso03",lepton->getUserRecord ("TrkIso03"),weight);
    HistClass::Fill(hist_number,"Ele_TrkIso04",lepton->getUserRecord ("TrkIso04"),weight);
    HistClass::Fill(hist_number,"Ele_Vtx_X",lepton->getUserRecord ("Vtx_X"),weight);
    HistClass::Fill(hist_number,"Ele_Vtx_Y",lepton->getUserRecord ("Vtx_Y"),weight);
    HistClass::Fill(hist_number,"Ele_Vtx_Z",lepton->getUserRecord ("Vtx_Z"),weight);
    HistClass::Fill(hist_number,"Ele_chargedHadronIso",lepton->getUserRecord ("chargedHadronIso"),weight);
    HistClass::Fill(hist_number,"Ele_convDcot",lepton->getUserRecord ("convDcot"),weight);
    HistClass::Fill(hist_number,"Ele_convDist",lepton->getUserRecord ("convDist"),weight);
    HistClass::Fill(hist_number,"Ele_convRadius",lepton->getUserRecord ("convRadius"),weight);
    HistClass::Fill(hist_number,"Ele_e1x5",lepton->getUserRecord ("e1x5"),weight);
    HistClass::Fill(hist_number,"Ele_e2x5",lepton->getUserRecord ("e2x5"),weight);
    HistClass::Fill(hist_number,"Ele_e5x5",lepton->getUserRecord ("e5x5"),weight);
    //HistClass::Fill(hist_number,"Ele_eidLoose",lepton->getUserRecord ("eidLoose"),weight);
    //HistClass::Fill(hist_number,"Ele_eidRobustHighEnergy",lepton->getUserRecord ("eidRobustHighEnergy"),weight);
    //HistClass::Fill(hist_number,"Ele_eidRobustLoose",lepton->getUserRecord ("eidRobustLoose"),weight);
    //HistClass::Fill(hist_number,"Ele_eidRobustTight",lepton->getUserRecord ("eidRobustTight"),weight);
    //HistClass::Fill(hist_number,"Ele_eidTight",lepton->getUserRecord ("eidTight"),weight);
    //HistClass::Fill(hist_number,"Ele_fbrem",lepton->getUserRecord ("fbrem"),weight);
    //HistClass::Fill(hist_number,"Ele_id",lepton->getUserRecord ("id"),weight);
    HistClass::Fill(hist_number,"Ele_neutralHadronIso",lepton->getUserRecord ("neutralHadronIso"),weight);
    HistClass::Fill(hist_number,"Ele_photonIso",lepton->getUserRecord ("photonIso"),weight);
    HistClass::Fill(hist_number,"Ele_pin",lepton->getUserRecord ("pin"),weight);
    HistClass::Fill(hist_number,"Ele_pout",lepton->getUserRecord ("pout"),weight);
    if(lepton->hasUserRecord("puChargedHadronIso")){
        HistClass::Fill(hist_number,"Ele_puChargedHadronIso",lepton->getUserRecord ("puChargedHadronIso"),weight);
        HistClass::Fill(hist_number,"Ele_Match",lepton->getUserRecord ("Match"),weight);
    }
    HistClass::Fill(hist_number,"Ele_sigmaIetaIeta",lepton->getUserRecord ("sigmaIetaIeta"),weight);





    //cout<<"-----------------------------------------"<<endl;
    //pxl::UserRecord::const_iterator us = lepton->getUserRecord().begin();
    //for( ; us != lepton->getUserRecord().end(); ++us ) {
        //try{
            //lepton->getUserRecord(us->first);
            ////cout<<"HistClass::Fill(hist_number,\"Ele_"<<us->first<<"\",lepton->getUserRecord (\""<<us->first<<"\"),weight);"<<endl;
            ////HistClass::CreateHisto(4,"Muon_VHits","Muon_VHits", 100, 0, 100,"N_{valid hits}^{#mu}");
            //cout<<"HistClass::CreateHisto(4,\"Ele_"<< us->first<<"\",\"Ele_"<< us->first<<"\", 100, 0, 100,\""<<  us->first<<"\");"<<endl;
        //}catch( std::runtime_error &exc ) {
        //}
        //try{
            //lepton->getUserRecord(us->first);
            ////cout<<"HistClass::Fill(hist_number,\"Ele_"<<us->first<<"\",lepton->getUserRecord (\""<<us->first<<"\"),weight);"<<endl;
            //cout<<"HistClass::CreateHisto(4,\"Ele_"<< us->first<<"\",\"Ele_"<< us->first<<"\", 100, 0, 100,\""<<  us->first<<"\");"<<endl;
        //}catch( std::runtime_error &exc ) {
        //}
        //try{
            //lepton->getUserRecord(us->first);
            ////cout<<"HistClass::Fill(hist_number,\"Ele_"<<us->first<<"\",lepton->getUserRecord (\""<<us->first<<"\"),weight);"<<endl;
            //cout<<"HistClass::CreateHisto(4,\"Ele_"<< us->first<<"\",\"Ele_"<< us->first<<"\", 100, 0, 100,\""<<  us->first<<"\");"<<endl;
        //}catch( std::runtime_error &exc ) {
        //}
    //}

}
void specialAna::Fill_Controll_Tau_histo(int hist_number, pxl::Particle* lepton) {
    Fill_Particle_hisos(hist_number,lepton);
    //for(uint j = 0; j < 67; j++) {
        //HistClass::Fill(hist_number,"Tau_discriminator",j+1,lepton->getUserRecord( (string)d_mydisc[j] ));
    //}
    HistClass::Fill(hist_number,"Tau_decayMode",lepton->getUserRecord("decayMode"),weight);
    HistClass::Fill(hist_number,"Tau_Vtx_X",lepton->getUserRecord("Vtx_X"),weight);
    HistClass::Fill(hist_number,"Tau_Vtx_Y",lepton->getUserRecord("Vtx_Y"),weight);
    HistClass::Fill(hist_number,"Tau_Vtx_Z",lepton->getUserRecord("Vtx_Z"),weight);
    HistClass::Fill(hist_number,"Tau_NumPFChargedHadrCands",lepton->getUserRecord("NumPFChargedHadrCands"),weight);
    HistClass::Fill(hist_number,"Tau_NumPFGammaCands",lepton->getUserRecord("NumPFGammaCands"),weight);
    HistClass::Fill(hist_number,"Tau_NumPFNeutralHadrCands",lepton->getUserRecord("NumPFNeutralHadrCands"),weight);
    HistClass::Fill(hist_number,"Tau_LeadingHadronPt",lepton->getUserRecord("LeadingHadronPt"),weight);
    //TLorentzVector* jet = new TLorentzVector();
    //jet->SetPxPyPzE(lepton->getUserRecord("tauJetpx"),lepton->getUserRecord("tauJetpy"),lepton->getUserRecord("tauJetpz"),lepton->getUserRecord("tauJetE"));
    //HistClass::Fill(hist_number,"Tau_Jet_pt",jet->Pt(),weight);
    //HistClass::Fill(hist_number,"Tau_Jet_eta",jet->Eta(),weight);
    //HistClass::Fill(hist_number,"Tau_Jet_phi",jet->Phi(),weight);
    //delete jet;
    HistClass::Fill(hist_number,"Tau_dxy",lepton->getUserRecord("dxy"),weight);
    HistClass::Fill(hist_number,"Tau_dxy_error",lepton->getUserRecord("dxy_error"),weight);
    HistClass::Fill(hist_number,"Tau_dxy_Sig",lepton->getUserRecord("dxy_Sig"),weight);
}

void specialAna::Fill_Particle_hisos(int hist_number, pxl::Particle* lepton , string syst){
    string name=lepton->getName();
    if(lepton->getName()==m_TauType){
        name="Tau";
    }
    if(syst==""){
        HistClass::Fill(hist_number,str(boost::format("%s_charge")%name ),lepton->getCharge(),weight);
        if(JetList->size()>0){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_pt")%name ),JetList->at(0)->getPt(),weight);
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_eta")%name ),JetList->at(0)->getEta(),weight);
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_phi")%name ),JetList->at(0)->getPhi(),weight);

            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_DeltaPhi")%name ),DeltaPhi(lepton,JetList->at(0)),weight);
        }
    } else {
        syst="_syst_"+syst;
    }
    HistClass::Fill(hist_number,str(boost::format("%s_pt%s")%name %syst ),lepton->getPt(),weight);
    HistClass::Fill(hist_number,str(boost::format("%s_eta%s")%name %syst ),lepton->getEta(),weight);
    HistClass::Fill(hist_number,str(boost::format("%s_phi%s")%name %syst ),lepton->getPhi(),weight);

    if(sel_met){
        HistClass::Fill(hist_number,str(boost::format("%s_DeltaPhi%s")%name %syst ),DeltaPhi(lepton,sel_met),weight);
        HistClass::Fill(hist_number,str(boost::format("%s_mt%s")%name %syst ),MT(lepton,sel_met),weight);
<<<<<<< HEAD
        HistClass::Fill(hist_number,str(boost::format("%s_ET_MET%s")%name %syst ),sel_lepton->getPt()/sel_met->getPt(),weight);
=======
        HistClass::Fill(hist_number,str(boost::format("%s_ET_MET%s")%name %syst ),lepton->getPt()/sel_met->getPt(),weight);
>>>>>>> restored 8TeV compability and data run ability
        HistClass::Fill(hist_number,str(boost::format("%s_met%s")%name %syst ),sel_met->getPt(),weight);
        HistClass::Fill(hist_number,str(boost::format("%s_met_phi%s")%name %syst ),sel_met->getPhi(),weight);
        if(syst==""){
            if(JetList->size()>0){
                HistClass::Fill(hist_number,str(boost::format("%s_jet_1_DeltaPhiMET")%name ),DeltaPhi(sel_met,JetList->at(0)),weight);
            }
        }
    }
}

void specialAna::Fill_Controll_histo(int hist_number, pxl::Particle* lepton) {

    //if(lepton->getUserRecord("id").toInt32()==11){
    if(TMath::Abs(lepton->getPdgNumber())==11){
        Fill_Controll_Ele_histo(hist_number, lepton);
    }
    if(TMath::Abs(lepton->getPdgNumber())==13){
        Fill_Controll_Muon_histo(hist_number, lepton);
    }
    if(TMath::Abs(lepton->getPdgNumber())==15){
        Fill_Controll_Tau_histo(hist_number, lepton);
    }
}


void specialAna::Fill_Gen_Rec_histos(pxl::Particle* genPart,pxl::Particle* recoPart){
        string name=recoPart->getName();
        if(recoPart->getName()==m_TauType){
            name="Tau";
        }
        if( not ( genPart->getPt()==0 ) ){
            HistClass::Fill( (boost::format("0_%s_recoMgen_pt")%name).str().c_str() ,recoPart->getPt()-genPart->getPt(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::Fill( (boost::format("0_%s_recoMgen_pt_rel")%name ).str().c_str(),(recoPart->getPt()-genPart->getPt())/genPart->getPt(),m_GenEvtView->getUserRecord( "Weight" ));
            HistClass::FillSparse( "Muon_Res",2,(recoPart->getPt()-genPart->getPt())/genPart->getPt(),genPart->getPt());
        }

}

double specialAna::DeltaPhi(double a, double b) {
  double temp = fabs(a-b);
  if (temp <= TMath::Pi())
    return temp;
  else
    return  2.*TMath::Pi() - temp;
}
double specialAna::DeltaPhi(pxl::Particle* lepton, pxl::Particle* met) {
    double a=lepton->getPhi();
    double b=met->getPhi();
    double temp = fabs(a-b);
    if (temp <= TMath::Pi())
        return temp;
    else
        return  2.*TMath::Pi() - temp;
}

double specialAna::MT(pxl::Particle* lepton, pxl::Particle* met) {
    double mm = 2 * lepton->getPt() * met->getPt() * ( 1. - cos(lepton->getPhi() - met->getPhi()) );
    return sqrt(mm);
}

int specialAna::vetoNumber(vector< pxl::Particle* > *list, double ptTreshold){
    //make veto numbers
    //we don't need vectors, do we?
    int numVeto=0;
    bool passedID=false;
    for( vector< pxl::Particle* >::const_iterator part_it = list->begin(); part_it != list->end(); ++part_it ) {
        passedID=false;
        if((*part_it)->hasUserRecord("IDpassed")){
            passedID=(*part_it)->getUserRecord("IDpassed");
        }else{
            passedID=true;
        }
        
        if( (*part_it)->getPt()>ptTreshold && passedID ){
            numVeto++;
        }else{
            //Lists are Pt sorted
            break;
        }
    }
    return numVeto;
}

void specialAna::cleanJets(){

    bool deleteThisJet = false;
    for(vector< pxl::Particle* >::iterator jet_it = JetList->begin();jet_it!=JetList->end(); /*jet_it++ dont use this here!!*/ ){
        deleteThisJet = false;

        for(vector< pxl::Particle* >::const_iterator muon_it = MuonList->begin();muon_it!=MuonList->end(); muon_it++){
            if((*jet_it)->getVector().deltaR((*muon_it)->getVector())<0.4){
                deleteThisJet=true;
            }
        }
        for(vector< pxl::Particle* >::const_iterator ele_it = EleList->begin();ele_it!=EleList->end(); ele_it++){
            if((*jet_it)->getVector().deltaR((*ele_it)->getVector())<0.4){
                deleteThisJet=true;
            }
        }
        for(vector< pxl::Particle* >::const_iterator tau_it = TauList->begin();tau_it!=TauList->end(); tau_it++){
            if((*jet_it)->getVector().deltaR((*tau_it)->getVector())<0.4){
                deleteThisJet=true;
            }
        }
        if(deleteThisJet){
            JetList->erase(jet_it);
        }else{
            jet_it++;
        }

    }


}

void specialAna::endJob( const Serializable* ) {

    for(unordered_set< string >::iterator it=triggers.begin();it!=triggers.end();it++)
    {
        cout<<*it<<endl;
    }
    //        HistClass::CreateHisto(4,"ptoverMET",particleName[i].c_str(), 20, 0, 2,       TString::Format("#frac{p_{T}^{%s}}{MET}", particleLatex[i].c_str()) );

    cout << "n_lepton:   " << n_lepton << endl;
    cout << "h_counters: " << HistClass::ReturnHist("h_counters")->GetBinContent(1) << endl;
    cout << "efficiency: " << n_lepton / (HistClass::ReturnHist("h_counters")->GetBinContent(1)) << endl;
    file1->cd();
    HistClass::WriteAll("counters");
    if(not runOnData){
        file1->mkdir("MC");
        file1->cd("MC/");
        HistClass::WriteAll("_Gen");
    }
    file1->cd();
    file1->mkdir("Taus");
    file1->cd("Taus/");
    HistClass::WriteAll("_Tau_");
    //HistClass::Write2("Tau_eta_phi");
    file1->cd();
    file1->mkdir("Muons");
    file1->cd("Muons/");
    HistClass::WriteAll("_Muon_");
    file1->cd();
    file1->mkdir("Ele");
    file1->cd("Ele/");
    HistClass::WriteAll("_Ele_");
    file1->cd();
    file1->mkdir("Trees");
    file1->cd("Trees/");
    HistClass::WriteAllTrees();
    file1->cd();
    file1->mkdir("nDim");
    file1->cd("nDim");
    HistClass::WriteN();
    file1->Close();

    std::string outputstring = eventsAfterCuts.str();
    eventDisplayFile << outputstring;
    eventDisplayFile.close();
    delete file1;
}

void specialAna::initEvent( const pxl::Event* event ){
    HistClass::Fill("h_counters", 1, 1); // increment number of events
    events_++;

    //no pu weight at the moment!!

    weight = 1;
    m_RecEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Rec" );
    m_GenEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Gen" );
    if(event->getObjectOwner().findObject< pxl::EventView >( "Trig" )){
        m_TrigEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Trig" );
    }else{
        m_TrigEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Rec" );
    }




    temp_run = event->getUserRecord( "Run" );
    temp_ls = event->getUserRecord( "LumiSection" );
    temp_event = event->getUserRecord( "EventNum" );

    numMuon  = m_RecEvtView->getUserRecord( "NumMuon" );
    numEle   = m_RecEvtView->getUserRecord( "NumEle" );
    numGamma = m_RecEvtView->getUserRecord( "NumGamma" );
    numTau   = m_RecEvtView->getUserRecord( "Num" + m_TauType );
    numMET   = m_RecEvtView->getUserRecord( "Num" + m_METType );
    numJet   = m_RecEvtView->getUserRecord( "Num" + m_JetAlgo );
    numBJet  = m_RecEvtView->getUserRecord_def( "Num" + m_BJets_algo,-1 );

    // h1_num_Taus.Fill(numTau);

    EleList   = new vector< pxl::Particle* >;
    MuonList  = new vector< pxl::Particle* >;
    GammaList = new vector< pxl::Particle* >;
    METList   = new vector< pxl::Particle* >;
    JetList   = new vector< pxl::Particle* >;
    TauList   = new vector< pxl::Particle* >;


    // get all particles
    vector< pxl::Particle* > AllParticles;
    m_RecEvtView->getObjectsOfType< pxl::Particle >( AllParticles );
    pxl::sortParticles( AllParticles );
    // push them into the corresponding vectors
    for( vector< pxl::Particle* >::const_iterator part_it = AllParticles.begin(); part_it != AllParticles.end(); ++part_it ) {
        pxl::Particle *part = *part_it;
        string Name = part->getName();
        // Only fill the collection if we want to use the particle!
        if(      Name == "Muon"    ) MuonList->push_back( part );
        else if( Name == "Ele"     ) EleList->push_back( part );
        else if( Name == "Gamma"   ) GammaList->push_back( part );
        else if( Name == m_TauType   ) TauList->push_back( part );
        else if( Name == m_METType ) METList->push_back( part );
        else if( Name == m_JetAlgo ) JetList->push_back( part );
    }

    //cout<<"-----"<<endl;
    //cout<<MuonList->size()<<endl;
    //cout<<EleList->size()<<endl;
    //cout<<TauList->size()<<endl;
    if(METList->size()>0){
        sel_met=METList->at(0);
    }else{
        throw std::runtime_error("specialAna.cc: no MET '" + m_METType +"' found!");
        sel_met=0;
    }
    sel_lepton=0;

    //     h1_num_Taus.Fill(TauList->size());
    EleListGen     = new vector< pxl::Particle* >;
    MuonListGen    = new vector< pxl::Particle* >;
    GammaListGen   = new vector< pxl::Particle* >;
    METListGen     = new vector< pxl::Particle* >;
    JetListGen     = new vector< pxl::Particle* >;
    TauListGen     = new vector< pxl::Particle* >;
    S3ListGen      = new vector< pxl::Particle* >;

    if( not runOnData ){

        double event_weight = m_GenEvtView->getUserRecord( "Weight" );
<<<<<<< HEAD
        double varKfactor_weight = m_GenEvtView->getUserRecord_def( "kfacWeight",1. );
<<<<<<< HEAD

        //check if we have a 13 TeV sample and if so, change the weight to one:
        double pileup_weight = 1;
        string datastream = event->getUserRecord( "Dataset" );
        TString Datastream = datastream;
        if(Datastream.Contains("13TeV"))
<<<<<<< HEAD
		    pileup_weight = m_GenEvtView->getUserRecord_def( "PUWeight",1.);
=======
//        double pileup_weight = m_GenEvtView->getUserRecord_def( "PUWeight",1.);
        double pileup_weight = 1.;
>>>>>>> use absolute value of PdgNumber!
=======
            pileup_weight = m_GenEvtView->getUserRecord_def( "PUWeight",1.);
>>>>>>> fixed names in datasets (tailselector)
=======
        //double varKfactor_weight = m_GenEvtView->getUserRecord_def( "kfacWeight",1. );
        double pileup_weight = m_GenEvtView->getUserRecord_def( "PUWeight",1.);
>>>>>>> added compability to 8 TeV trigger and add dataset tag

        if(m_dataPeriod=="13TeV"){
            weight = event_weight ;
        }else if(m_dataPeriod=="8TeV"){
            weight = event_weight  * pileup_weight;
        }else{
            stringstream error;
            error << "The data period "<<m_dataPeriod<<" is not supported by this analysis!\n";
            throw Tools::config_error( error.str() );
        }


        // get all particles
        vector< pxl::Particle* > AllParticlesGen;
        m_GenEvtView->getObjectsOfType< pxl::Particle >( AllParticlesGen );
        pxl::sortParticles( AllParticlesGen );
        // push them into the corresponding vectors
        string genCollection="gen";
        if(m_dataPeriod=="8TeV"){
            genCollection="S3";
        }
        for( vector< pxl::Particle* >::const_iterator part_it = AllParticlesGen.begin(); part_it != AllParticlesGen.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            // Only fill the collection if we want to use the particle!
            if(      Name == "Muon"    ) MuonListGen->push_back( part );
            else if( Name == "Ele"     ) EleListGen->push_back( part );
            else if( Name == "Gamma"   ) GammaListGen->push_back( part );
            else if( Name == "Tau"     ) TauListGen->push_back( part );
            else if( Name == (m_METType+"_gen") ) METListGen->push_back( part );
            else if( Name == m_JetAlgo ) JetListGen->push_back( part );
            else if( Name == genCollection) S3ListGen->push_back( part );
        }

    }
}


void specialAna::applyKfactor(const pxl::Event* event , int mode){
    if( not (mode==1 || mode==0) ){
        throw std::runtime_error("specialAna.cc: The k-faktor must be additive (mode=0) or multiplicative (mode=1) yours is "+std::to_string(mode));
    }
    string datastream = event->getUserRecord( "Dataset" );
    TString Datastream = datastream;
<<<<<<< HEAD
<<<<<<< HEAD
    double wmass=0.;
    //additive
    //p0                        =      1.18304   +/-   0.00128801
    //p1                        = -2.66112e-05   +/-   2.5832e-06
    //p2                        = -2.82645e-08   +/-   1.00043e-09
    double par []={1.18304,-2.66112e-05,-2.82645e-08};
    //multiply
    //p0                        =      1.17943   +/-   0.00136125
    //p1                        = -1.79983e-05   +/-   2.61106e-06
    //p2                        = -2.94026e-08   +/-   9.2078e-10
    if(mode==1){
        par[0]= 1.17943;
        par[1]= -1.79983e-05;
        par[2]= -2.94026e-08;
    }
=======

>>>>>>> fixed k-factors for 13TeV and 8 TeV and introduced data/MC sf
=======

>>>>>>> merge with stash
    if( m_dataPeriod=="13TeV" ){
        double wmass=0.;
        //additive
        //p0                        =      1.18304   +/-   0.00128801
        //p1                        = -2.66112e-05   +/-   2.5832e-06
        //p2                        = -2.82645e-08   +/-   1.00043e-09
        double par []={1.18304,-2.66112e-05,-2.82645e-08};
        //multiply
        //p0                        =      1.17943   +/-   0.00136125
        //p1                        = -1.79983e-05   +/-   2.61106e-06
        //p2                        = -2.94026e-08   +/-   9.2078e-10
        if(mode==1){
            par[0]= 1.17943;
            par[1]= -1.79983e-05;
            par[2]= -2.94026e-08;
        }
        if(Datastream.Contains("WTo") ) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(S3ListGen->at(i)->getPdgNumber() == 24){
                    wmass=S3ListGen->at(i)->getMass();
<<<<<<< HEAD
=======
                    break;
>>>>>>> fixed k-factors for 13TeV and 8 TeV and introduced data/MC sf
                }
            }
            weight*=(par[0]+wmass*par[1]+wmass*wmass*par[2]);

        }
    }else if(m_dataPeriod=="8TeV"){
        double mt=0.;
        if(Datastream.Contains("WJets") ) {
            //mg additive
            double par []={1.42167 ,0.00036744,6.77295e-8};
            //mg multiply
            if(mode==1){
                par[0]= 1.402;
                par[1]= -0.000435535;
                par[2]= 7.43225e-8;
            }

            int l=-1;
            int nu=-1;

            for(uint i = 0; i < S3ListGen->size(); i++){
                int tmpid= TMath::Abs(S3ListGen->at(i)->getPdgNumber());
                if(tmpid == 11) l=i;
                if(tmpid == 13) l=i;
                if(tmpid == 15) l=i;

                if(tmpid == 12) nu=i;
                if(tmpid == 14) nu=i;
                if(tmpid == 16) nu=i;
            }
            if(l>=0 && nu>=0){
                mt=MT(S3ListGen->at(l), S3ListGen->at(nu));
            }
            if(mt!=0){
                weight*=(par[0]+mt*par[1]+mt*mt*par[2]);
            }
        }
        if(Datastream.Contains("WTo") ) {
            //pythia additive
            double par []={1.42167 ,0.00036744,6.77295e-8};
            //pythia multiply
            if(mode==1){
                par[0]= 1.402;
                par[1]= -0.000435535;
                par[2]= 7.43225e-8;
            }
            int l=-1;
            int nu=-1;
            for(uint i = 0; i < S3ListGen->size(); i++){
                int tmpid= TMath::Abs(S3ListGen->at(i)->getPdgNumber());
                if(tmpid == 11) l=i;
                if(tmpid == 13) l=i;
                if(tmpid == 15) l=i;

                if(tmpid == 12) nu=i;
                if(tmpid == 14) nu=i;
                if(tmpid == 16) nu=i;
            }
            if(l>=0 && nu>=0){
                mt=MT(S3ListGen->at(l), S3ListGen->at(nu));
            }
            if(mt!=0){
                weight*=(par[0]+mt*par[1]+mt*mt*par[2]);
            }
        }
    }
}


//get all the correct data/mc scale factors:
void specialAna::aplyDataMCScaleFactors(){
    double sf=1.;
    //first for 8TeV we know the scale Factors!
    if( m_dataPeriod =="8TeV"){
        //muons
        if(sel_lepton && sel_lepton->getPdgNumber()==13){
            //tirgger * iso * reco
            if(fabs(sel_lepton->getEta())>=0.0 && fabs(sel_lepton->getEta())<0.9 ){
                if(sel_lepton->getPt()>=45 && sel_lepton->getPt()<50){
                    sf*= 0.977232 *1.000328 *1.002512 ;
                }
                if(sel_lepton->getPt()>=50 && sel_lepton->getPt()<60){
                    sf*= 0.974729 *1.000386 *0.989671 ;
                }
                if(sel_lepton->getPt()>=60 && sel_lepton->getPt()<90){
                    sf*= 0.970880 *1.001741 *1.017212 ;
                }
                if(sel_lepton->getPt()>=90 && sel_lepton->getPt()<140){
                    sf*= 0.976496 *0.999684 *0.991805 ;
                }
                if(sel_lepton->getPt()>=140 && sel_lepton->getPt()<8000){
                    sf*= 0.992056 *0.999496 *0.992936 ;
                }
            }else if(fabs(sel_lepton->getEta())>=0.9 && fabs(sel_lepton->getEta())<1.2 ){
                if(sel_lepton->getPt()>=45 && sel_lepton->getPt()<50){
                    sf*= 0.956829 *0.999328 *1.009358 ;
                }
                if(sel_lepton->getPt()>=50 && sel_lepton->getPt()<60){
                    sf*= 0.952611 *0.999747 *0.990653 ;
                }
                if(sel_lepton->getPt()>=60 && sel_lepton->getPt()<90){
                    sf*= 0.943317 *1.000951 *1.009669 ;
                }
                if(sel_lepton->getPt()>=90 && sel_lepton->getPt()<140){
                    sf*= 0.950280 *0.999453 *0.995619 ;
                }
                if(sel_lepton->getPt()>=140 && sel_lepton->getPt()<8000){
                    sf*= 0.963785 *0.999288 *0.993065 ;
                }
            }else if(fabs(sel_lepton->getEta())>=1.2 && fabs(sel_lepton->getEta())<2.1 ){
                if(sel_lepton->getPt()>=45 && sel_lepton->getPt()<50){
                    sf*= 0.987652 *1.000460 *1.020144 ;
                }
                if(sel_lepton->getPt()>=50 && sel_lepton->getPt()<60){
                    sf*= 0.979974 *0.999826 *0.992034 ;
                }
                if(sel_lepton->getPt()>=60 && sel_lepton->getPt()<90){
                    sf*= 0.968852 *0.999988 *0.983324 ;
                }
                if(sel_lepton->getPt()>=90 && sel_lepton->getPt()<140){
                    sf*= 0.985779 *0.999644 *0.996766 ;
                }
                if(sel_lepton->getPt()>=140 && sel_lepton->getPt()<8000){
                    sf*= 0.981677 *0.999707 *0.997924 ;
                }
            }else if(fabs(sel_lepton->getEta())>=2.1 && fabs(sel_lepton->getEta())<2.4 ){
                if(sel_lepton->getPt()>=45 && sel_lepton->getPt()<50){
                    sf*= 0.987652 *1.000095 *1.041736 ;
                }
                if(sel_lepton->getPt()>=50 && sel_lepton->getPt()<60){
                    sf*= 0.979974 *1.000027 *0.985755 ;
                }
                if(sel_lepton->getPt()>=60 && sel_lepton->getPt()<90){
                    sf*= 0.968852 *0.996834 *0.789842 ;
                }
                if(sel_lepton->getPt()>=90 && sel_lepton->getPt()<140){
                    sf*= 0.985779 *1.000487 *0.992398 ;
                }
                if(sel_lepton->getPt()>=140 && sel_lepton->getPt()<8000){
                    sf*= 0.981677 *0.999406 *0.997955 ;
                }
            }
        }
    }
    weight*=sf;

}

void specialAna::endEvent( const pxl::Event* event ){

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

<<<<<<< HEAD
//void specialAna::SetEvents(int e){
    //events_=e;
//}
=======
//~ void specialAna::SetEvents(int e){
    //~ events_=e;
//~ }
>>>>>>> specialAna: Missing semicolon

