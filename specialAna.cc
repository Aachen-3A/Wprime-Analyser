#include "specialAna.hh"
#include "HistClass.hh"
#include "Tools/Tools.hh"
#include <csignal>
#include "TVector3.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "boost/format.hpp"
#pragma GCC diagnostic pop


specialAna::specialAna( const Tools::MConfig &cfg, Systematics &syst_shifter) :
   runOnData(       cfg.GetItem< bool >( "General.RunOnData" ) ),
   useSyst(         cfg.GetItem< bool >( "General.useSYST" ) ),

   m_JetAlgo(       cfg.GetItem< string >( "Jet.Type.Rec" ) ),
   m_BJets_algo(    cfg.GetItem< string >( "Jet.BJets.Algo" ) ),
   m_METType(       cfg.GetItem< string >( "MET.Type.Rec" ) ),
   m_TauType(       cfg.GetItem< string >( "Tau.Type.Rec" ) ),

   //wprime specific stuff
   m_pt_met_min_cut_ele(    cfg.GetItem< double >( "Ele.wprime.pt_met_cut_min" ) ),
   m_pt_met_max_cut_ele(    cfg.GetItem< double >( "Ele.wprime.pt_met_cut_max" ) ),
   m_delta_phi_cut_ele(     cfg.GetItem< double >( "Ele.wprime.delta_phi_cut" ) ),
   m_pt_min_cut_ele(        cfg.GetItem< double >( "Ele.wprime.pt_min" ) ),
   m_pt_min_cut_ele_high_pt(        cfg.GetItem< double >( "Ele.wprime.pt_min_high_pt" ) ),

   m_pt_met_min_cut_funk_root_ele(  "Ele.wprime.pt_met_cut_min_func",  cfg.GetItem< string >( "Ele.wprime.pt_met_cut_min_func" ).c_str() ,0,8000),
   m_pt_met_max_cut_funk_root_ele(  "Ele.wprime.pt_met_cut_max_func",  cfg.GetItem< string >( "Ele.wprime.pt_met_cut_max_func" ).c_str() ,0,8000),
   m_delta_phi_cut_funk_root_ele(   "Ele.wprime.delta_phi_cut_func",  cfg.GetItem< string >( "Ele.wprime.delta_phi_cut_func" ).c_str() ,0,8000),

   m_pt_met_min_cut_muo(    cfg.GetItem< double >( "Muo.wprime.pt_met_cut_min" ) ),
   m_pt_met_max_cut_muo(    cfg.GetItem< double >( "Muo.wprime.pt_met_cut_max" ) ),
   m_delta_phi_cut_muo(     cfg.GetItem< double >( "Muo.wprime.delta_phi_cut" ) ),
   m_pt_min_cut_muo(        cfg.GetItem< double >( "Muo.wprime.pt_min" ) ),

   m_pt_met_min_cut_funk_root_muo(  "Muo.wprime.pt_met_cut_min_func",  cfg.GetItem< string >( "Muo.wprime.pt_met_cut_min_func" ).c_str() ,0,8000),
   m_pt_met_max_cut_funk_root_muo(  "Muo.wprime.pt_met_cut_max_func",  cfg.GetItem< string >( "Muo.wprime.pt_met_cut_max_func" ).c_str() ,0,8000),
   m_delta_phi_cut_funk_root_muo(   "Muo.wprime.delta_phi_cut_func",  cfg.GetItem< string >( "Muo.wprime.delta_phi_cut_func" ).c_str() ,0,8000),

   m_pt_met_min_cut_tau(    cfg.GetItem< double >( "Tau.wprime.pt_met_cut_min" ) ),
   m_pt_met_max_cut_tau(    cfg.GetItem< double >( "Tau.wprime.pt_met_cut_max" ) ),
   m_delta_phi_cut_tau(     cfg.GetItem< double >( "Tau.wprime.delta_phi_cut" ) ),
   m_pt_min_cut_tau(        cfg.GetItem< double >( "Tau.wprime.pt_min" ) ),

   m_pt_met_min_cut_funk_root_tau(  "Tau.wprime.pt_met_cut_min_func",   cfg.GetItem< string >( "Tau.wprime.pt_met_cut_min_func" ).c_str() ,0,8000),
   m_pt_met_max_cut_funk_root_tau(  "Tau.wprime.pt_met_cut_max_func",  cfg.GetItem< string >( "Tau.wprime.pt_met_cut_max_func" ).c_str() ,0,8000),
   m_delta_phi_cut_funk_root_tau(  "Tau.wprime.delta_phi_cut_func",   cfg.GetItem< string >( "Tau.wprime.delta_phi_cut_func" ).c_str() ,0,8000),

   m_min_mt_for_eventinfo(  cfg.GetItem< double >( "wprime.min_mt_for_eventinfo" ) ),
   m_m_cut(                 cfg.GetItem< double >( "wprime.m_cut" ) ),
   m_cutdatafile(           cfg.GetItem< std::string >( "wprime.cutdatafile" ) ),

   m_do_ztree( cfg.GetItem<bool>( "wprime.produce_ztree" ) ) ,
   m_trigger_string( Tools::splitString< string >( cfg.GetItem< string >( "wprime.TriggerList" ), true  ) ),
   d_mydiscmu(  {"isPFMuon","isGlobalMuon","isTrackerMuon","isStandAloneMuon","isTightMuon","isHighPtMuon"} ),
   m_dataPeriod(            cfg.GetItem< string >( "General.DataPeriod" ) ),
   m_kfactorFile_Config(    Tools::ExpandPath( cfg.GetItem< std::string >("wprime.WkfactorFile"))),
   m_analyse_trigger( Tools::splitString< string >( cfg.GetItem< string >( "wprime.AnalyseTriggerList" ), true  ) ),
   config_(cfg)
{
    m_syst_shifter=&syst_shifter;
    string safeFileName = "SpecialHistos.root";
    file1 = new TFile(safeFileName.c_str(), "RECREATE");
    eventDisplayFile.open(m_cutdatafile.c_str(), std::fstream::out);
    events_ = 0;

    n_lepton = 0; // counting leptons passing the selection

    m_kfactorFile= new TFile(m_kfactorFile_Config.c_str(),"READ");
    m_kfactorHist[0] = (TH1D*) m_kfactorFile->Get("k_fakp");
    m_kfactorHist[1] = (TH1D*) m_kfactorFile->Get("k_fakm");


    file1->cd();

    // number of events, saved in a histogram
    HistClass::CreateHistoUnchangedName("h_counters", 10, 0, 11, "N_{events}");


    if(not runOnData){
        HistClass::CreateHisto("MC_W_m_Gen", 8000, 0, 8000, "M_{W} [GeV]");
        HistClass::CreateHisto("MC_W_pthat2_Gen", 8000, 0, 8000, "p_{T}^{W} [GeV]");
        HistClass::CreateHisto("MC_W_pt_Gen", 8000, 0, 8000, "p_{T}^{W} [GeV]");
        HistClass::CreateHisto("MC_W_pthat_Gen", 8000, 0, 8000, "#hat{p}_{T}^{W} [GeV]");
        HistClass::CreateHisto("MC_HT_constructed_Gen", 8000, 0, 8000, "H_{T} [GeV]");

    }
    HistClass::CreateHisto("MC_cutflow_Gen", 40, 0, 40,               "stage" );
    HistClass::CreateHisto("MC_11_cutflow_Gen", 40, 0, 40,               "stage" );
    HistClass::CreateHisto("MC_13_cutflow_Gen", 40, 0, 40,               "stage" );
    HistClass::CreateHisto("MC_15_cutflow_Gen", 40, 0, 40,               "stage" );


    for(unsigned int i=0;i<3;i++){

        //str(boost::format("N_{%s}")%particleLatex[i] )
        HistClass::CreateHisto("cutflow",particles[i].c_str(), 40, 0, 40,               "stage" );
        HistClass::CreateHisto("num",particles[i].c_str(), 40, 0, 39,                   TString::Format("N_{%s}", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(7,"IDFail",particles[i].c_str(), 6, 0, 6,                  "failcode" );
        HistClass::CreateHisto(7,"pt",particles[i].c_str(), 5000, 0, 5000,              "p_{T} [GeV]");
        HistClass::CreateHisto(7,"nPrimaryVertices",particles[i].c_str(), 100, 0, 100,  "number of primary vertices");
        HistClass::CreateHisto(7,"pt_reciprocal",particles[i].c_str(), 5000, 0, 1,      "1/p_{T} [1/GeV]" );
        HistClass::CreateHisto(7,"deltapt_over_pt",particles[i].c_str(), 5000, 0, 10,   "#frac{#Delta p_{T}}{p_{T}}" );
        HistClass::CreateHisto(7,"eta",particles[i].c_str(), 80, -4, 4,                 "#eta" );
        HistClass::CreateHisto(7,"phi",particles[i].c_str(), 40, -3.2, 3.2,             "#phi [rad]" );

        HistClass::CreateHisto(7,"DeltaPhi",particles[i].c_str(), 40, 0, 3.2,           TString::Format("#Delta#phi(%s,E_{T}^{miss})", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(7,"mt",particles[i].c_str(), 5000, 0, 5000,              "M_{T} [GeV]" );
        HistClass::CreateHisto(7,"mt_Flag_CSCTightHaloFilter",particles[i].c_str(), 5000, 0, 5000,              "M_{T} [GeV]" );
        HistClass::CreateHisto(7,"mt_Flag_goodVertices",particles[i].c_str(), 5000, 0, 5000,              "M_{T} [GeV]" );
        HistClass::CreateHisto(7,"mt_Flag_eeBadScFilter",particles[i].c_str(), 5000, 0, 5000,              "M_{T} [GeV]" );
        HistClass::CreateHisto(7,"mt_allFilter",particles[i].c_str(), 5000, 0, 5000,              "M_{T} [GeV]" );
        HistClass::CreateHisto(7,"mt_noFilterRun",particles[i].c_str(), 5000, 0, 5000,              "M_{T} [GeV]" );

        HistClass::CreateHisto(7,"mt_reciprocal",particles[i].c_str(), 5000, 0, 1,      "1/M_{T} [1/GeV]"     );
        HistClass::CreateHisto(7,"charge",particles[i].c_str(), 3, -1, 1,               TString::Format("q_{%s}", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(7,"met",particles[i].c_str(), 5000, 0, 5000,             "E^{miss}_{T} [GeV]" );
        HistClass::CreateHisto(7,"met_phi",particles[i].c_str(),40, -3.2, 3.2,          "#phi_{E^{miss}_{T}} [rad]" );
        HistClass::CreateHisto(7,"ET_MET",particles[i].c_str(),50, 0, 6,                "p_{T}/E^{miss}_{T}" );

        HistClass::CreateHisto(7,"jet_1_pt",particles[i].c_str(), 5000, 0, 5000,        "p_{T}^{jet} [GeV]" );
        HistClass::CreateHisto(7,"jet_1_eta",particles[i].c_str(), 80, -4, 4,           "#eta_{jet}" );
        HistClass::CreateHisto(7,"jet_1_phi",particles[i].c_str(), 40, -3.2, 3.2,       "#phi_{jet} [rad]" );
        HistClass::CreateHisto(7,"jet_1_DeltaPhi",particles[i].c_str(), 40, 0, 3.2,     TString::Format("#Delta#phi(%s,jet)", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(7,"jet_1_DeltaPhiMET",particles[i].c_str(), 40, 0, 3.2,  "#Delta#phi(E_{T}^{miss},jet)");
        HistClass::CreateHisto(7,"jet_1_DeltaPhiMETlep",particles[i].c_str(), 40, 0, 3.2,  TString::Format("#Delta#phi(E_{T}^{miss}+%s,jet)", particleSymbols[i].c_str()) );
        HistClass::CreateHisto(7,"jet_1_lep_Minv",particles[i].c_str(),  5000, 0, 5000,  TString::Format("Mass(%s,jet)", particleSymbols[i].c_str()) );

        HistClass::CreateHisto(7,"jet_2_pt",particles[i].c_str(), 5000, 0, 5000,        "p_{T}^{jet} [GeV]" );
        HistClass::CreateHisto(7,"jet_3_pt",particles[i].c_str(), 5000, 0, 5000,        "p_{T}^{jet} [GeV]" );
        HistClass::CreateHisto(7,"jet_4_pt",particles[i].c_str(), 5000, 0, 5000,        "p_{T}^{jet} [GeV]" );
        HistClass::CreateHisto(7,"jet_5_pt",particles[i].c_str(), 5000, 0, 5000,        "p_{T}^{jet} [GeV]" );
        HistClass::CreateHisto(7,"jet_6_pt",particles[i].c_str(), 5000, 0, 5000,        "p_{T}^{jet} [GeV]" );
        HistClass::CreateHisto(7,"jet_7_pt",particles[i].c_str(), 5000, 0, 5000,        "p_{T}^{jet} [GeV]" );
        HistClass::CreateHisto(7,"jet_ht",particles[i].c_str(), 5000, 0, 5000,        "ht_{T}^{jet} [GeV]" );

        if(not runOnData){
            HistClass::CreateHisto(2,"recoMgen_pt",particles[i].c_str(), 400, -100, 100,    "p_{T}^{reco}-p_{T}^{gen} [GeV]" );
            HistClass::CreateHisto(2,"recoMgen_pt_rel",particles[i].c_str(), 400, -10, 10,  "#frac{p_{T}^{reco}-p_{T}^{gen}}{p_{T}^{gen}}" );
            HistClass::CreateHisto(2,"num_Gen",particles[i].c_str(), 40, 0, 39,             TString::Format("N_{%s}", particleSymbols[i].c_str()) );
            HistClass::CreateHisto(2,"pt_Gen",particles[i].c_str(), 5000, 0, 5000,          "p_{T} [GeV]");
            HistClass::CreateHisto(2,"eta_Gen",particles[i].c_str(), 80, -4, 4,             "#eta" );
            HistClass::CreateHisto(2,"phi_Gen",particles[i].c_str(), 40, -3.2, 3.2,         "#phi [rad]" );
        }

    }


    HistClass::CreateHisto("0_all_triggers_Gen", 28, 0, 28, "trigger");
    HistClass::CreateHisto("1_all_triggers_Gen", 28, 0, 28, "trigger");
    HistClass::CreateHisto("2_all_triggers_Gen", 28, 0, 28, "trigger");
    HistClass::CreateHisto("3_all_triggers_Gen", 28, 0, 28, "trigger");
    HistClass::CreateHisto("4_all_triggers_Gen", 28, 0, 28, "trigger");
    for(int i=0;i<28;i++){
        HistClass::FillStr("0_all_triggers_Gen",x_bins_names[i].c_str(),0);
        HistClass::FillStr("1_all_triggers_Gen",x_bins_names[i].c_str(),0);
        HistClass::FillStr("2_all_triggers_Gen",x_bins_names[i].c_str(),0);
        HistClass::FillStr("3_all_triggers_Gen",x_bins_names[i].c_str(),0);
        HistClass::FillStr("4_all_triggers_Gen",x_bins_names[i].c_str(),0);
    }

    HistClass::CreateHisto(1,"Tau_gen_decayMode", 15, 0, 15, "#N_{decay mode}");
    HistClass::CreateHisto(7,"Tau_decayMode", 100, 0, 10, "#N_{decay mode}");
    HistClass::CreateHisto(7,"Tau_Vtx_X", 100, -1, 1, "Vtx_{x} [cm]");
    HistClass::CreateHisto(7,"Tau_Vtx_Y", 100, -1, 1, "Vtx_{y} [cm]");
    HistClass::CreateHisto(7,"Tau_Vtx_Z", 200, -30, 30, "Vtx_{z} [cm]");
    //HistClass::CreateHisto(7,"Tau_EtaEta", 200, -1, 1, "#eta#eta_{#tau}");
    //HistClass::CreateHisto(7,"Tau_EtaPhi", 200, -1, 1, "#eta#phi_{#tau}");
    //HistClass::CreateHisto(7,"Tau_PhiPhi", 200, -1, 1, "#phi#phi_{#tau}");
    //HistClass::CreateHisto(7,"Tau_NumSignaltracks", 40, -3, 37, "N_{sig tracks}");
    HistClass::CreateHisto(7,"Tau_NumPFChargedHadrCands", 40, -3, 37, "N_{PFChargedHadrCands}");
    HistClass::CreateHisto(7,"Tau_NumPFGammaCands", 40, -3, 37, "N_{PFGammaCands}");
    HistClass::CreateHisto(7,"Tau_NumPFNeutralHadrCands", 40, -3, 37, "N_{PFNeutralHadrCands}");
    HistClass::CreateHisto(7,"Tau_LeadingHadronPt", 5000, 0, 5000, "p_{T}^{leading hadron} [GeV]");
    HistClass::CreateHisto(7,"Tau_EMFraction", 100, 0, 10, "EM Fraction");
    HistClass::CreateHisto(7,"Tau_HcalEoverLeadChargedP", 100, 0, 10, "#frac{E_{Hcal}}{P_{leading charged}}");
    HistClass::CreateHisto(7,"Tau_EcalEnergy", 5000, 0, 5000, "E_{Ecal} [GeV]");
    HistClass::CreateHisto(7,"Tau_HcalEnergy", 5000, 0, 5000, "E_{Hcal} [GeV]");
    HistClass::CreateHisto(7,"Tau_Mass", 5500, -0.5, 5, "mass(#tau) [GeV]");
    HistClass::CreateHisto(7,"Tau_Jet_pt", 5000, 0, 5000, "p_{T}^{jet} [GeV]");
    HistClass::CreateHisto(7,"Tau_Jet_eta", 80, -4, 4, "#eta_{jet}");
    HistClass::CreateHisto(7,"Tau_Jet_phi", 40, -3.2, 3.2, "#phi_{jet} [rad]");
    HistClass::CreateHisto(7,"Tau_dxy", 100, 0, 0.1, "d_{xy} [cm]");
    HistClass::CreateHisto(7,"Tau_dxy_error", 100, 0, 0.1, "#sigma(d_{xy}) [cm]");
    HistClass::CreateHisto(7,"Tau_dxy_Sig", 100, 0, 10, "Sig(d_{xy})");
    HistClass::CreateHisto("Tau_eta_phi", 80, -4, 4, 40, -3.2, 3.2,"#eta_{#tau}","#phi_{#tau} [rad]");
    HistClass::CreateHisto(7,"Tau_discriminator", d_mydisc.size(), 0, d_mydisc.size(), "discriminator_{#tau}");
    for(unsigned int i=0;i<d_mydisc.size();i++){
        for(unsigned int j=0;j<6;j++){
            HistClass::FillStr(j,"Tau_discriminator",d_mydisc[i].Data(),0);
        }
    }
    HistClass::CreateHisto(15,"Tau_mt_decaymode", 5000, 0, 5000,"M_{T} [GeV]");
    HistClass::CreateHisto(25,"Tau_mt_genmatch", 5000, 0, 5000,"M_{T} [GeV]");



    HistClass::CreateHisto(7,"Muon_Vtx_X", 100, -1, 1,"Vtx_{x} [cm]");
    HistClass::CreateHisto(7,"Muon_Vtx_Y", 100, -1, 1,"Vtx_{y} [cm]");
    HistClass::CreateHisto(7,"Muon_Vtx_Z", 200, -30, 30,"Vtx_{z} [cm]");
    HistClass::CreateHisto(7,"Muon_Chi2", 400, 0, 200,"#chi_{#mu}^{2}");
    HistClass::CreateHisto(7,"Muon_Ndof", 200, 0, 200,"N_{dof}");
    //HistClass::CreateHisto(7,"Muon_LHits", 20, 0, 20,"N_{lost hits}");
    //HistClass::CreateHisto(7,"Muon_VHits", 100, 0, 100,"N_{valid hits}");
    //HistClass::CreateHisto(7,"Muon_VHitsPixel", 20, 0, 20,"N_{valid pixel hits}");
    //HistClass::CreateHisto(7,"Muon_VHitsTracker", 40, 0, 40,"N_{valid tracker hits}");
    //HistClass::CreateHisto(7,"Muon_VHitsMuonSys", 60, 0, 60,"N_{valid muon system hits}");
    //HistClass::CreateHisto(7,"Muon_TrackerLayersWithMeas", 20, 0, 20,"N_{tracker layers with hits}");
    //HistClass::CreateHisto(7,"Muon_PixelLayersWithMeas", 60, 0, 60,"N_{pixel layers with hits}");
    //HistClass::CreateHisto(7,"Muon_NMatchedStations", 60, 0, 60,"N_{matched muon stations}");
    HistClass::CreateHisto(7,"Muon_qoverp", 200, 0, 0.2,"#frac{q_{mu}}{p_{mu}}");
    HistClass::CreateHisto(7,"Muon_qoverpError", 100, 0, 0.001,"#sigma(#frac{q_{mu}}{p_{mu}})");
    HistClass::CreateHisto(7,"Muon_ptError", 1000, 0, 1000,"#sigma(p_{T}) [GeV]");
    HistClass::CreateHisto(7,"Muon_ptErroroverpt", 1000, 0, 1.,"#sigma(p_{T})/p_{T} [GeV]");
    HistClass::CreateHisto(7,"Muon_Cocktail_pt", 5000, 0, 5000,"p_{T}^{cocktail #mu} [GeV]");
    HistClass::CreateHisto(7,"Muon_Cocktail_eta", 80, -4, 4,"#eta_{cocktail #mu}");
    HistClass::CreateHisto(7,"Muon_Cocktail_phi", 40, -3.2, 3.2,"#phi_{cocktail #mu} [rad]");
    HistClass::CreateHisto(7,"Muon_CaloIso", 100, 0, 3,"ISO_{Calo} [GeV]");
    HistClass::CreateHisto(7,"Muon_TrkIso", 100, 0, 3,"ISO_{Trk} [GeV]");
    HistClass::CreateHisto(7,"Muon_ECALIso", 100, 0, 3,"ISO_{ECAL} [GeV]");
    HistClass::CreateHisto(7,"Muon_HCALIso", 100, 0, 3,"ISO_{HCAL} [GeV]");
    HistClass::CreateHisto(7,"Muon_Dxy", 1000, -3, 3,"D_{xy} [cm]");
    HistClass::CreateHisto(7,"Muon_Dz", 100, -4, 4,"D_{z} [cm]");
    HistClass::CreateHisto(7,"Muon_DxyBT", 1000, -3, 3,"D_{xy} [cm]");
    HistClass::CreateHisto(7,"Muon_DzBT", 100, -4, 4,"D_{z} [cm]");
    HistClass::CreateHisto(7,"Muon_DxyBS", 1000, -3, 3,"D_{xy} [cm]");
    HistClass::CreateHisto(7,"Muon_DzBS", 100, -4, 4,"D_{z} [cm]");


    HistClass::CreateHisto(7,"Muon_ID", 6, 0, 6,"ID_{#mu}");
    HistClass::NameBins(3,"Muon_ID",6,d_mydiscmu);
    //HistClass::CreateHisto(7,"Muon_pt_reciprocal", 5000, 0, 1,"1/p_{T} [1/GeV]");
    //HistClass::CreateHisto(7,"Muon_mt_reciprocal", 5000, 0, 1,"1/M_{T}^{#m [1/GeV]");
    HistClass::CreateHisto(7,"Muon_dpt_over_pt", 5000, 0, 6,"#sigma_{p_{T}}/p_{T}");


    HistClass::CreateHisto(7,"Ele_mt_Ele_27", 5000, 0, 5000,             "M_{T} [GeV]" );
    HistClass::CreateHisto(7,"Ele_met_Ele_27", 5000, 0, 5000,             "E^{miss}_{T} [GeV]" );
    HistClass::CreateHisto(7,"Ele_pt_Ele_27", 5000, 0, 5000,             "p_{T} [GeV]");
    HistClass::CreateHisto(7,"Ele_mt_Ele_105", 5000, 0, 5000,             "M_{T} [GeV]" );
    HistClass::CreateHisto(7,"Ele_met_Ele_105", 5000, 0, 5000,             "E^{miss}_{T} [GeV]" );
    HistClass::CreateHisto(7,"Ele_pt_Ele_105", 5000, 0, 5000,             "p_{T} [GeV]");
    HistClass::CreateHisto(7,"Ele_mt_Ele_115", 5000, 0, 5000,             "M_{T} [GeV]" );
    HistClass::CreateHisto(7,"Ele_met_Ele_115", 5000, 0, 5000,             "E^{miss}_{T} [GeV]" );
    HistClass::CreateHisto(7,"Ele_pt_Ele_115", 5000, 0, 5000,             "p_{T} [GeV]");

    HistClass::CreateHisto(7,"Ele_CaloIso", 100, 0, 10,"CaloIso");
    HistClass::CreateHisto(7,"Ele_ChargeMatch", 100, 0, 100,"ChargeMatch");
    HistClass::CreateHisto(7,"Ele_Class", 100, 0, 100,"Class");
    HistClass::CreateHisto(7,"Ele_DEtaSCCalo", 100, 0, 100,"DEtaSCCalo");
    HistClass::CreateHisto(7,"Ele_DEtaSCVtx", 100, 0, 100,"DEtaSCVtx");
    HistClass::CreateHisto(7,"Ele_DEtaSeedTrk", 100, 0, 100,"DEtaSeedTrk");
    HistClass::CreateHisto(7,"Ele_DPhiSCVtx", 100, 0, 100,"DPhiSCVtx");
    HistClass::CreateHisto(7,"Ele_DPhiSeedTrk", 100, 0, 100,"DPhiSeedTrk");
    HistClass::CreateHisto(7,"Ele_Dsz", 100, 0, 100,"Dsz");
    HistClass::CreateHisto(7,"Ele_DszBS", 100, 0, 100,"DszBS");
    HistClass::CreateHisto(7,"Ele_Dxy", 100, 0, 100,"Dxy");
    HistClass::CreateHisto(7,"Ele_DxyBS", 100, 0, 100,"DxyBS");
    HistClass::CreateHisto(7,"Ele_Dz", 100, 0, 100,"Dz");
    HistClass::CreateHisto(7,"Ele_DzBS", 100, 0, 100,"DzBS");
    HistClass::CreateHisto(7,"Ele_ECALIso", 100, 0, 10,"ECALIso");
    HistClass::CreateHisto(7,"Ele_ECALIso03", 100, 0, 10,"ECALIso03");
    HistClass::CreateHisto(7,"Ele_ECALIso04", 100, 0, 10,"ECALIso04");
    HistClass::CreateHisto(7,"Ele_ESCOverPout", 100, 0, 100,"ESCOverPout");
    HistClass::CreateHisto(7,"Ele_ESCSeedOverP", 100, 0, 100,"ESCSeedOverP");
    HistClass::CreateHisto(7,"Ele_ESCSeedPout", 100, 0, 100,"ESCSeedPout");
    HistClass::CreateHisto(7,"Ele_EoP", 100, 0, 100,"EoP");
    HistClass::CreateHisto(7,"Ele_GSFNormChi2", 100, 0, 100,"GSFNormChi2");
    HistClass::CreateHisto(7,"Ele_HCALIso", 100, 0, 10,"HCALIso");
    HistClass::CreateHisto(7,"Ele_HCALIso03", 100, 0, 10,"HCALIso03");
    HistClass::CreateHisto(7,"Ele_HCALIso03d1", 100, 0, 10,"HCALIso03d1");
    HistClass::CreateHisto(7,"Ele_HCALIso03d2", 100, 0, 10,"HCALIso03d2");
    HistClass::CreateHisto(7,"Ele_HCALIso04", 100, 0, 10,"HCALIso04");
    HistClass::CreateHisto(7,"Ele_HCALIso04d1", 100, 0, 10,"HCALIso04d1");
    HistClass::CreateHisto(7,"Ele_HCALIso04d2", 100, 0, 10,"HCALIso04d2");
    HistClass::CreateHisto(7,"Ele_HCALIsoConeDR03_2012", 100, 0, 10,"HCALIsoConeDR03_2012");
    HistClass::CreateHisto(7,"Ele_HCALIsoConeDR04_2012", 100, 0, 10,"HCALIsoConeDR04_2012");
    HistClass::CreateHisto(7,"Ele_HCALOverECALd1", 100, 0, 100,"HCALOverECALd1");
    HistClass::CreateHisto(7,"Ele_HoEm", 100, 0, 100,"HoEm");
    HistClass::CreateHisto(7,"Ele_HoverE2012", 100, 0, 100,"HoverE2012");
    HistClass::CreateHisto(7,"Ele_Match", 100, 0, 100,"Match");
    HistClass::CreateHisto(7,"Ele_NinnerLayerLostHits", 20, 0, 20,"NinnerLayerLostHits");
    HistClass::CreateHisto(7,"Ele_NumBrems", 100, 0, 100,"NumBrems");
    HistClass::CreateHisto(7,"Ele_PErr", 100, 0, 100,"PErr");
    HistClass::CreateHisto(7,"Ele_SCE", 100, 0, 100,"SCE");
    HistClass::CreateHisto(7,"Ele_SCEErr", 100, 0, 100,"SCEErr");
    HistClass::CreateHisto(7,"Ele_SCEt", 100, 0, 100,"SCEt");
    HistClass::CreateHisto(7,"Ele_SCeta", 100, 0, 100,"SCeta");
    HistClass::CreateHisto(7,"Ele_TrackerP", 1000, 0, 1000,"TrackerP");
    HistClass::CreateHisto(7,"Ele_TrkIso", 100, 0, 10,"TrkIso");
    HistClass::CreateHisto(7,"Ele_TrkIso03", 100, 0, 10,"TrkIso03");
    HistClass::CreateHisto(7,"Ele_TrkIso04", 100, 0, 10,"TrkIso04");
    HistClass::CreateHisto(7,"Ele_Vtx_X", 100, -50, 50,"Vtx_X");
    HistClass::CreateHisto(7,"Ele_Vtx_Y", 100, -50, 50,"Vtx_Y");
    HistClass::CreateHisto(7,"Ele_Vtx_Z", 100, -50, 50,"Vtx_Z");
    HistClass::CreateHisto(7,"Ele_chargedHadronIso", 100, 0, 10,"chargedHadronIso");
    HistClass::CreateHisto(7,"Ele_convDcot", 100, 0, 100,"convDcot");
    HistClass::CreateHisto(7,"Ele_convDist", 100, 0, 100,"convDist");
    HistClass::CreateHisto(7,"Ele_convRadius", 100, 0, 100,"convRadius");
    HistClass::CreateHisto(7,"Ele_e1x5", 100, 0, 100,"e1x5");
    HistClass::CreateHisto(7,"Ele_e2x5", 100, 0, 100,"e2x5");
    HistClass::CreateHisto(7,"Ele_e5x5", 100, 0, 100,"e5x5");
    HistClass::CreateHisto(7,"Ele_neutralHadronIso", 100, 0, 10,"neutralHadronIso");
    HistClass::CreateHisto(7,"Ele_photonIso", 100, 0, 100,"photonIso");
    HistClass::CreateHisto(7,"Ele_pin", 1000, 0, 1000,"pin");
    HistClass::CreateHisto(7,"Ele_pout", 1000, 0, 1000,"pout");
    HistClass::CreateHisto(7,"Ele_puChargedHadronIso", 100, 0, 10,"puChargedHadronIso");
    HistClass::CreateHisto(7,"Ele_sigmaIetaIeta", 100, 0, 10,"sigmaIetaIeta");


    //for qcd hist (as systematic)
    for(unsigned char pi=0; pi<3; pi++){
        HistClass::CreateHisto(7,str(boost::format("%s_pt_syst_iso_QCD")%particles[pi]).c_str(), 5000, 0, 5000, "p_{T} [GeV]");
        HistClass::CreateHisto(7,str(boost::format("%s_eta_syst_iso_QCD")%particles[pi]).c_str(),5000, 0, 5000, std::string("#eta (") + particleSymbols[pi] + ")");
        HistClass::CreateHisto(7,str(boost::format("%s_phi_syst_iso_QCD")%particles[pi]).c_str(),40, -3.2, 3.2, std::string("#phi (") + particleSymbols[pi] + ") [rad]");
        HistClass::CreateHisto(7,str(boost::format("%s_DeltaPhi_syst_iso_QCD")%particles[pi]).c_str(),40, 0, 3.2, std::string("#Delta#phi(") + particleSymbols[pi] + ",E_{T}^{miss})");
        HistClass::CreateHisto(7,str(boost::format("%s_mt_syst_iso_QCD")%particles[pi]).c_str(),5000, 0, 5000, "M_{T} [GeV]");
        HistClass::CreateHisto(7,str(boost::format("%s_ET_MET_syst_iso_QCD")%particles[pi]).c_str(),50, 0, 6, "p_{T}/E^{miss}_{T}");
        HistClass::CreateHisto(7,str(boost::format("%s_met_syst_iso_QCD")%particles[pi]).c_str(),5000, 0, 5000, "E^{miss}_{T} [GeV]");
        HistClass::CreateHisto(7,str(boost::format("%s_met_phi_syst_iso_QCD")%particles[pi]).c_str(),40, -3.2, 3.2,"#phi_{E^{miss}_{T}} [rad]");
    }
    HistClass::CreateHisto("general_boson_qt",         500, 0, 5000,"q_{T} (GeV)");
    HistClass::CreateHisto("general_boson_qt_ele",         500, 0, 5000,"q_{T} (GeV)");
    HistClass::CreateHisto("general_boson_qt_muo",         500, 0, 5000,"q_{T} (GeV)");
    HistClass::CreateProf("response",         500, 0, 5000,"q_{T} (GeV)","Response");
    HistClass::CreateProf("response_ele",         500, 0, 5000,"q_{T} (GeV)","Response");
    HistClass::CreateProf("response_muo",         500, 0, 5000,"q_{T} (GeV)","Response");

    if(not runOnData){
        if(useSyst){
            // 4 for loops to create 480 histograms
            for(unsigned int pi=0; pi<3; pi++){ // loop over particles
                for(auto syst : syst_shifter.m_activeSystematics){
                    if(syst->m_particleType=="MET"){
                        syst->m_particleType="met";
                    }

                //for(unsigned int si=0; si<5; si++){ // loop over shifted particles
                    //for(unsigned int ti=0; ti<2; ti++){ // loop over type
                        for(unsigned int ui=0; ui<2; ui++){ // loop over updown
                            for(unsigned int stage=1;stage<5;stage++){
                                HistClass::CreateHisto(boost::format("%d_%s_pt_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       5000, 0, 5000,
                                                       "p_{T} [GeV]");
                                HistClass::CreateHisto(boost::format("%d_%s_eta_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       5000, 0, 5000,
                                                       std::string("#eta ({") + particleSymbols[pi] + ")");
                                HistClass::CreateHisto(boost::format("%d_%s_phi_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       40, -3.2, 3.2,
                                                       std::string("#phi (") + particleSymbols[pi] + ") [rad]");
                                HistClass::CreateHisto(boost::format("%d_%s_DeltaPhi_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       40, 0, 3.2,
                                                       std::string("#Delta#phi(") + particleSymbols[pi] + ",E_{T}^{miss})");
                                HistClass::CreateHisto(boost::format("%d_%s_mt_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       5000, 0, 5000,
                                                       "M_{T} [GeV]");
                                HistClass::CreateHisto(boost::format("%d_%s_ET_MET_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       50, 0, 6,
                                                       "p_{T}/E^{miss}_{T}");
                                HistClass::CreateHisto(boost::format("%d_%s_met_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       5000, 0, 5000,
                                                       "E^{miss}_{T} [GeV]");
                                HistClass::CreateHisto(boost::format("%d_%s_met_phi_syst_%s%s%s")%stage %particles[pi] %syst->m_particleType %syst->m_sysType %updown[ui],
                                                       40, -3.2, 3.2,"#phi_{E^{miss}_{T}} [rad]");
                            }
                        }
                    //}
                }
            }
        }



        int bins []={100,5000};
        double xmin []={-10,0};
        double xmax []={10,5000};
        string xtitle []= {"res","p_{T} [GeV]"};
        HistClass::CreateNSparse("Muon_Res",2,bins,xmin ,xmax ,xtitle  );

        bins [0]=5000;
        xmin [0]=0;
        xmax [0]=0;
        xtitle [0]= "p_{T} [GeV]";
        xtitle [1]= "M [GeV]";
        HistClass::CreateNSparse("W_pt_m_Gen",2,bins,xmin ,xmax ,xtitle  );

        Create_trigger_effs();
    }

    for(auto syst : syst_shifter.m_activeSystematics){
        if(syst->m_particleType=="met"){
            syst->m_particleType=m_METType;
        }
    }

    Create_RECO_effs();

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

    mLeptonTree["bjet1"]=0;
    mLeptonTree["highEtEleTrig"]=0;



    //PDF
    mLeptonTree["id1"]=999;
    mLeptonTree["id2"]=999;
    mLeptonTree["x1"]=999;
    mLeptonTree["x2"]=999;
    mLeptonTree["qscale"]=-1;

    //general
    mLeptonTree["ThisWeight"]=0;
    mLeptonTree["lepton_type"]=0;
    HistClass::CreateTree( &mLeptonTree, "slimtree");

    mQCDTree["lepton_n"]=0;
    mQCDTree["pt"]=0;
    mQCDTree["eta"]=0;
    mQCDTree["met"]=0;
    mQCDTree["iso"]=0;
    mQCDTree["delta_phi"]=0;
    mQCDTree["ThisWeight"]=0;
    mQCDTree["QCDWeight"]=0;
    mQCDTree["lepton_type"]=0;
    mQCDTree["decay_mode"]=0;


    HistClass::CreateTree( &mQCDTree, "qcdtree");


    mZtree["lep_type"]=0;
    mZtree["lep1_pt"]=0;
    mZtree["lep1_eta"]=0;
    mZtree["lep1_phi"]=0;
    mZtree["lep1_E"]=0;

    mZtree["lep2_pt"]=0;
    mZtree["lep2_eta"]=0;
    mZtree["lep2_phi"]=0;
    mZtree["lep2_E"]=0;

    mZtree["z_pt"]=0;
    mZtree["z_eta"]=0;
    mZtree["z_phi"]=0;
    mZtree["z_E"]=0;
    mZtree["met_pt"]=0;
    mZtree["met_phi"]=0;

    HistClass::CreateTree( &mZtree, "ztree");

    TFile qcd_weight_ele(Tools::musicAbsPath(Tools::ExpandPath( cfg.GetItem< std::string >("wprime.qcd_weight.ele"))).c_str());
    qcd_weight_ele_pt=(TH1D*) qcd_weight_ele.Get("qcdFake_0_pt");
    qcd_weight_muo_pt=(TH1D*) qcd_weight_ele.Get("qcdFake_1_pt");
    qcd_weight_tau_pt=(TH1D*) qcd_weight_ele.Get("qcdFake_2_pt");

    numAllTaus=0;


}

specialAna::~specialAna() {
}

void specialAna::analyseEvent( const pxl::Event* event ) {
    initEvent( event );

    HistClass::Fill("MC_cutflow_Gen",0,weight);

    if(not runOnData){
        if(tail_selector(event)) {
            endEvent( event );
            return;
        }
        Fill_Gen_Controll_histo();
        Fill_RECO_effs();
        TriggerAnalyser();
    }
    if (m_RecEvtView->hasUserRecord("filter_accept")){
        if (!m_RecEvtView->getUserRecord("filter_accept").toBool()){
            //cout<<"filter veto"<<endl;
            //pxl::EventView *FilterView = event->getObjectOwner().findObject< pxl::EventView >( "Filter" );
            //for(auto filter :FilterView->getUserRecords()){
                //cout<<filter.first<<"  "<<  filter.second<<endl;
            //}
            endEvent( event );
            return;
        }
    }

    HistClass::Fill("MC_cutflow_Gen",1,weight);

    KinematicsSelector();
    QCDAnalyse();
    if(m_do_ztree){
        ResponseAnalyse();
    }


    Fill_stage_0_histos();
    if (sel_lepton==0 and qcd_lepton==0){
        endEvent( event );
        return;
    }
    HistClass::Fill("MC_cutflow_Gen",2,weight);

    if (!triggerKinematics()){
        endEvent( event );
        return;
    }
    HistClass::Fill("MC_cutflow_Gen",3,weight);

    if(not runOnData){
        applyKfactor(event,0);
        aplyDataMCScaleFactors();
    }

    FillTriggers(0);
    if (!TriggerSelector(event)){
        endEvent( event );
        return;
    }
    FillTriggers(1);
    HistClass::Fill("MC_cutflow_Gen",4,weight);

    cleanJets();




    if(sel_lepton && sel_met && sel_lepton->getPt()>m_pt_min_cut){
        if(sel_lepton->getPdgNumber()==11){
            highEtTriggStored=TriggerSelector_highpt(event);
        }
        HistClass::Fill("MC_cutflow_Gen",5,1.);
        Fill_Tree();
        Fill_QCD_Tree(true);
        Fill_Controll_histo(5, sel_lepton);
        if(sel_lepton->getUserRecord("passedDeltaPhi")){
            Fill_Controll_histo(1, sel_lepton);
            HistClass::Fill((boost::format("MC_%d_cutflow_Gen")%sel_lepton->getPdgNumber()).str().c_str(),6,weight);
        }
        if(sel_lepton->getUserRecord("passedPtMet")){
            Fill_Controll_histo(2, sel_lepton);
            HistClass::Fill((boost::format("MC_%d_cutflow_Gen")%sel_lepton->getPdgNumber()).str().c_str(),7,weight);
        }
        if(sel_lepton->getUserRecord("passed")){
            if(sel_lepton->getPdgNumber()==15){
                n_lepton++;
            }
            Fill_Controll_histo(3, sel_lepton);
            HistClass::Fill((boost::format("MC_%d_cutflow_Gen")%sel_lepton->getPdgNumber()).str().c_str(),8,weight);
        }
        if(sel_lepton->getUserRecord("passed") && sel_lepton->getPdgNumber()==11 && sel_lepton->getPt()>m_pt_min_cut_ele_high_pt && highEtTriggStored ){
            Fill_Controll_histo(4, sel_lepton);
            HistClass::Fill((boost::format("MC_%d_cutflow_Gen")%sel_lepton->getPdgNumber()).str().c_str(),9,weight);
        }
        if(sel_lepton->getPdgNumber()==11 && sel_lepton->getPt()>m_pt_min_cut_ele_high_pt && highEtTriggStored ){
            Fill_Controll_histo(6, sel_lepton);
            HistClass::Fill((boost::format("MC_%d_cutflow_Gen")%sel_lepton->getPdgNumber()).str().c_str(),9,weight);
        }

        if(runOnData){
            if((MT(sel_lepton,sel_met) > m_min_mt_for_eventinfo) && sel_lepton->getUserRecord("passed")) {
            // save event info for events for event display generation:
                eventsAfterCuts <<"Mt "			    <<MT(sel_lepton,sel_met)                            << ": "
                                <<"Runnumber "		<<event->getUserRecord("Run")                       << ": "
                                <<"Lumiblock "		<<event->getUserRecord("LumiSection")               << ": "
                                <<"Event "			<<event->getUserRecord("EventNum")                  << ": "
                                <<"pt "			    <<sel_lepton->getPt()                               << ": "
                                <<"eta "			<<sel_lepton->getEta()                              << ": "
                                <<"phi "			<<sel_lepton->getPhi()                              << ": "
                                <<"MET "			<<sel_met->getPt()                                  << ": "
                                <<"dphi "			<<DeltaPhi(sel_lepton->getPhi(),sel_met->getPhi())  << ": "
                                <<"charge "   		<<sel_lepton->getCharge()                           << ": "
                                <<"pt/met "			<<sel_lepton->getPt()/sel_met->getPt()              << ": "
                                <<"triggerselector "<<TriggerSelector_highpt(event)                     << endl;
            }
        }
    }
    if(qcd_lepton && sel_met && qcd_lepton->getPt()>m_pt_min_cut){
        Fill_QCD_Tree(false);
        weight*=qcd_weight;
        Fill_Particle_hisos(5, qcd_lepton , "iso_QCD");
        if(qcd_lepton->getUserRecord("passedDeltaPhi")){
            Fill_Particle_hisos(1, qcd_lepton , "iso_QCD");
        }
        if(qcd_lepton->getUserRecord("passedPtMet")){
            Fill_Particle_hisos(2, qcd_lepton , "iso_QCD");
        }
        if(qcd_lepton->getUserRecord("passed")){
            Fill_Particle_hisos(3, qcd_lepton , "iso_QCD");
        }
        if(qcd_lepton->getUserRecord("passed") && qcd_lepton->getPdgNumber()==11 && qcd_lepton->getPt()>m_pt_min_cut_ele_high_pt ){
            Fill_Particle_hisos(4, qcd_lepton , "iso_QCD");
        }
        if(qcd_lepton->getPdgNumber()==11 && qcd_lepton->getPt()>m_pt_min_cut_ele_high_pt){
            Fill_Particle_hisos(6, qcd_lepton, "iso_QCD");
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
    if(!runOnData and useSyst){
        for(auto syst : m_syst_shifter->m_activeSystematics){

            FillSystematicsUpDown(event, syst->m_particleType, "Up", syst->m_sysType);
            FillSystematicsUpDown(event, syst->m_particleType, "Down", syst->m_sysType);

            //FillSystematics(event, "Muon");
            //FillSystematicsUpDown(event, "Muon", "Up", "Resolution");
            //FillSystematicsUpDown(event, "Muon", "Down", "Resolution");
            //FillSystematics(event, "Ele");
            //FillSystematics(event, "Tau");
            //FillSystematics(event, "Jet");
            //FillSystematics(event, m_METType);
            //FillSystematicsUpDown(event, "Jet", "Up", "Resolution");
            //FillSystematicsUpDown(event, "Jet", "Down", "Resolution");
        }
        // FillSystematics(event, "met");
    }

    endEvent( event );
}

//void specialAna::FillSystematics(const pxl::Event* event, std::string const particleName){
    //FillSystematicsUpDown(event, particleName, "Up", "Scale");
    //FillSystematicsUpDown(event, particleName, "Down", "Scale");
    ////FillSystematicsUpDown(event, particleName, "Up", "Resolution");
    ////FillSystematicsUpDown(event, particleName, "Down", "Resolution");
//}

void specialAna::FillSystematicsUpDown(const pxl::Event* event, std::string const particleName, std::string const updown, std::string const shiftType){
    pxl::EventView *tempEventView;
    // extract one EventView
    // make sure the object key is the same as in Systematics.cc specified
    // tempEventView = event->getObjectOwner().findObject< pxl::EventView >(particleName + "_syst" + shiftType + updown);
    // For jets we have to use the music nameing convention
    //if(particleName=="Jet"){
        ////
        //std::string jetType="JES";
        //std::string upperUpdown= boost::to_upper_copy(updown);
        //if (shiftType=="Resolution"){
            //jetType="JER";
        //}
        //tempEventView = event->findObject< pxl::EventView >("Rec_"+jetType+ "_"+upperUpdown);
    //}else{
        //tempEventView = event->findObject< pxl::EventView >(particleName + "_syst" + shiftType + updown);
    //}
    tempEventView = event->findObject< pxl::EventView >(particleName + "_syst" + shiftType + updown);



    if(tempEventView == 0){
        //std::vector< pxl::EventView* > views;
        //event->getObjectsOfType< pxl::EventView >(views);
        //for(auto v : views){
            //cout<<v->getName()<<endl;
        //}
        //throw std::runtime_error("specialAna.cc: no EventView '" + particleName + "_syst" + shiftType + updown + "' found!");
        //cout<<"specialAna.cc: no EventView '" << particleName << "_syst" << shiftType << updown << "' found!"<<endl;
        //if the event view has no particles of this type it will not be created!
        return;
    }
    // get all particles
    std::vector< pxl::Particle* > shiftedParticles;
    tempEventView->getObjectsOfType< pxl::Particle >(shiftedParticles);

    //backup OldList
    RememberMET=METList;
    METList = new std::vector< pxl::Particle* >;
    if(particleName=="Muon"){
        RememberPart=MuonList;
        MuonList = new std::vector< pxl::Particle* >;
        for( std::vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if(      Name == "Muon"    ) MuonList->push_back( part );
            else if( Name == m_METType ) METList->push_back( part );
        }
    }else if(particleName=="Ele"){
        RememberPart=EleList;
        EleList = new std::vector< pxl::Particle* >;
        for( std::vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if(      Name == "Ele"     ) EleList->push_back( part );
            else if( Name == m_METType ) METList->push_back( part );
        }
    }else if(particleName=="Tau"){
        RememberPart=TauList;
        TauList = new std::vector< pxl::Particle* >;
        for( std::vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if(      Name == m_TauType ) TauList->push_back( part );
            else if( Name == m_METType ) METList->push_back( part );
        }
    }else if(particleName=="Jet"){
        RememberPart=JetList;
        JetList = new std::vector< pxl::Particle* >;
        for( std::vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if(      Name == m_JetAlgo ) JetList->push_back( part );
            else if( Name == m_METType ) METList->push_back( part );
        }
    }else if(particleName==m_METType){
        for( std::vector< pxl::Particle* >::const_iterator part_it = shiftedParticles.begin(); part_it != shiftedParticles.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            if( Name == m_METType ) METList->push_back( part );
        }
    }

    // reset the chosen MET and lepton
    if(METList->size()>0){
        sel_met=METList->at(0);
    }else{
        sel_met=0;
    }
    sel_lepton=0;

    KinematicsSelector();

    if(sel_lepton && sel_met){
        if(sel_lepton->getUserRecord("passedDeltaPhi")){
            if(particleName==m_METType){
                Fill_Particle_hisos(1, sel_lepton, "met" + shiftType + updown);
            }else{
                Fill_Particle_hisos(1, sel_lepton, particleName + shiftType + updown);
            }
        }
        if(sel_lepton->getUserRecord("passedPtMet")){
            if(particleName==m_METType){
                Fill_Particle_hisos(2, sel_lepton, "met" + shiftType + updown);
            }else{
                Fill_Particle_hisos(2, sel_lepton, particleName + shiftType + updown);
            }
        }
        if(sel_lepton->getUserRecord("passed")){
            if(particleName==m_METType){
                Fill_Particle_hisos(3, sel_lepton, "met" + shiftType + updown);
            }else{
                Fill_Particle_hisos(3, sel_lepton, particleName + shiftType + updown);
            }
            // cout << "h1_3_" << sel_lepton->getName() << "_[val]_syst_" + particleName + shiftType + updown << endl;
        }
        if(sel_lepton->getUserRecord("passed") && sel_lepton->getPdgNumber()==11 && sel_lepton->getPt()>m_pt_min_cut_ele_high_pt && TriggerSelector_highpt(event)){
            if(particleName==m_METType){
                Fill_Particle_hisos(4, sel_lepton, "met" + shiftType + updown);
            }else{
                Fill_Particle_hisos(4, sel_lepton, particleName + shiftType + updown);
            }
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
    }else if(particleName=="Jet"){
        delete JetList;
        JetList = RememberPart;
    }
    //else if(particleName==m_METType){
    //}

}

bool specialAna::Check_Tau_ID(pxl::Particle* tau) {
    bool passed = false;
    bool tau_ID = tau->getUserRecord("decayModeFindingNewDMs").toDouble()>0.5;
    bool tau_ISO = tau->getUserRecord("byMediumCombinedIsolationDeltaBetaCorr3Hits").toDouble()>0.5;
    bool tau_ELE = tau->getUserRecord("againstElectronLoose").toDouble()>0.5;
    bool tau_MUO = tau->getUserRecord("againstMuonLoose3").toDouble()>0.5;
    if (tau_ID && tau_ISO && tau_ELE && tau_MUO) passed = true;
    return passed;
}
bool specialAna::Check_Tau_ID_no_iso(pxl::Particle* tau) {
    bool passed = false;
    bool tau_ID = tau->getUserRecord("decayModeFindingNewDMs").toDouble()>0.5;
    bool tau_ISO = tau->getUserRecord("byMediumCombinedIsolationDeltaBetaCorr3Hits").toDouble()>0.5;
    bool tau_ELE = tau->getUserRecord("againstElectronLoose").toDouble()>0.5;
    bool tau_MUO = tau->getUserRecord("againstMuonLoose3").toDouble()>0.5;
    if (tau_ID && !tau_ISO && tau_ELE && tau_MUO) passed = true;
    return passed;
}


void specialAna::KinematicsSelector() {

    bool passed=false;
    bool passedPtMet=false;
    bool passedDeltaPhi=false;
    int sel_id=0;

    //make veto numbers
    //we don't need std::vectors, do we?
    double m_leptonVetoPt=20;
    int numVetoMuo=vetoNumber(MuonList,m_leptonVetoPt);
    int numVetoTau=vetoNumberTau(TauList,m_leptonVetoPt);
    int numVetoEle=vetoNumber(EleList,m_leptonVetoPt);


    //cout<<numVetoMuo<<"  "<<numVetoEle<<"  "<<numVetoTau<<endl;

    //the ordering is important muon is last (overrides the first two and so on)
    if( numVetoEle==0 && TauList->size()>=1 && numVetoMuo==0){
        int passedID=0;
        pxl::Particle* tmpTau;
        for( std::vector< pxl::Particle* >::iterator it = TauList->begin(); it != TauList->end(); ++it ) {
            if( Check_Tau_ID(*it) ){
                passedID++;
                tmpTau=(*it);
            }
        }
        if(passedID==1){
            sel_lepton=tmpTau;
            m_pt_min_cut=m_pt_min_cut_tau;
            m_delta_phi_cut=m_delta_phi_cut_tau;
            m_pt_met_min_cut=m_pt_met_min_cut_tau;
            m_pt_met_max_cut=m_pt_met_max_cut_tau;

            m_pt_met_min_cut_funk_root=m_pt_met_min_cut_funk_root_tau;
            m_pt_met_max_cut_funk_root=m_pt_met_max_cut_funk_root_tau;
            m_delta_phi_cut_funk_root=m_delta_phi_cut_funk_root_tau;
            sel_id=15;
        }
    }
    if( EleList->size()>=1 && numVetoTau==0 && numVetoMuo==0 ){
        int passedID=0;
        pxl::Particle* tmpEle;
        for( std::vector< pxl::Particle* >::iterator it = EleList->begin(); it != EleList->end(); ++it ) {
            if( (*it)->hasUserRecord("IDpassed")){
                if ( (*it)->getUserRecord("IDpassed").toBool() ){
                    passedID++;
                    tmpEle=(*it);
                }
            }else if (EleList->size()==1){
                passedID++;
                tmpEle=( pxl::Particle* ) EleList->at(0);
                break;
            }
        }
        if(passedID==1){
            sel_lepton=tmpEle;
            m_pt_min_cut=m_pt_min_cut_ele;
            m_delta_phi_cut=m_delta_phi_cut_ele;
            m_pt_met_min_cut=m_pt_met_min_cut_ele;
            m_pt_met_max_cut=m_pt_met_max_cut_ele;
            m_pt_met_min_cut_funk_root=m_pt_met_min_cut_funk_root_ele;
            m_pt_met_max_cut_funk_root=m_pt_met_max_cut_funk_root_ele;
            m_delta_phi_cut_funk_root=m_delta_phi_cut_funk_root_ele;
            sel_id=11;
        }
    }
    if( numVetoEle==0 && numVetoTau==0 && MuonList->size()>=1 ){
        int passedID=0;
        pxl::Particle* tmpMuo;
        for( std::vector< pxl::Particle* >::iterator it = MuonList->begin(); it != MuonList->end(); ++it ) {
            if( (*it)->hasUserRecord("IDpassed")){
                if ( not passedID &&  (*it)->getUserRecord("IDpassed").toBool() ){
                    passedID++;
                    tmpMuo=(*it);
                }
            }else if (MuonList->size()==1){
                passedID++;
                tmpMuo=( pxl::Particle* ) MuonList->at(0);
                break;
            }
        }
        if(passedID==1){
            sel_lepton=tmpMuo;
            m_pt_min_cut=m_pt_min_cut_muo;
            m_delta_phi_cut=m_delta_phi_cut_muo;
            m_pt_met_min_cut=m_pt_met_min_cut_muo;
            m_pt_met_max_cut=m_pt_met_max_cut_muo;

            m_pt_met_min_cut_funk_root=m_pt_met_min_cut_funk_root_muo;
            m_pt_met_max_cut_funk_root=m_pt_met_max_cut_funk_root_muo;
            m_delta_phi_cut_funk_root=m_delta_phi_cut_funk_root_muo;
            sel_id=13;
        }
    }





    if(sel_met && sel_lepton && sel_lepton->getPt()>m_pt_min_cut){

        double mt=MT(sel_lepton,sel_met);
        m_pt_met_min_cut=   m_pt_met_min_cut_funk_root.Eval(mt);
        m_pt_met_max_cut=   m_pt_met_max_cut_funk_root.Eval(mt);
        m_delta_phi_cut=    m_delta_phi_cut_funk_root.Eval(mt);

        if(sel_lepton->getPt()/sel_met->getPt()>m_pt_met_min_cut && sel_lepton->getPt()/sel_met->getPt()<m_pt_met_max_cut){
            passedPtMet=true;
        }
        if(DeltaPhi(sel_lepton->getPhi(),sel_met->getPhi())>m_delta_phi_cut){
            passedDeltaPhi=true;
        }

        if (passedDeltaPhi && passedPtMet){
            passed=true;
        }

    }
    if(sel_lepton){
        sel_lepton->setUserRecord("passedPtMet",passedPtMet);
        sel_lepton->setUserRecord("passedDeltaPhi",passedDeltaPhi);
        sel_lepton->setUserRecord("passed",passed);
        sel_lepton->setPdgNumber(sel_id);
    }
}

void specialAna::Create_trigger_effs() {
    //for taus only at the moment
    for (std::vector< std::string >::const_iterator it=m_analyse_trigger.begin(); it!= m_analyse_trigger.end(); it++) {
        HistClass::CreateEff((*it).c_str(), 500, 0, 5000, "p_{T} [GeV]");
    }
}

void specialAna::TriggerAnalyser(){
    bool triggered=false;

    for(pxl::UserRecords::const_iterator us = m_TrigEvtView->getUserRecords().begin() ; us != m_TrigEvtView->getUserRecords().end(); ++us ) {
        if(string::npos != (*us).first.find( "HLT_HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80")){
            triggered=(*us).second;
            if(triggered){
                break;
            }
        }
    }

    for (std::vector< std::string >::const_iterator it=m_analyse_trigger.begin(); it!= m_analyse_trigger.end(); it++) {
        if(TauList->size()>=1 && sel_met->getPt()>100 && (string::npos != it->find( "Tau")) ){
            HistClass::FillEff((*it).c_str(), TauList->at(0)->getPt() , triggered);

        }
        if(sel_met && TauList->size()>=1 && TauList->at(0)->getPt()>80 && (string::npos != it->find( "MET")) ){
            HistClass::FillEff((*it).c_str(), sel_met->getPt() , triggered);
        }
    }

}

void specialAna::FillTriggers(int ihist){
    pxl::UserRecords::const_iterator us = m_TrigEvtView->getUserRecords().begin();
    for( ; us != m_TrigEvtView->getUserRecords().end(); ++us ) {
        if(std::find(std::begin(x_bins_names), std::end(x_bins_names),(*us).first) != std::end(x_bins_names)){
            HistClass::FillStr((boost::format("%d_all_triggers_Gen")%ihist).str().c_str(),((*us).first).c_str(),1.);
            if(ihist==1 && sel_lepton){
                if(sel_lepton->getPdgNumber()==11){
                    HistClass::FillStr((boost::format("%d_all_triggers_Gen")%2).str().c_str(),((*us).first).c_str(),1.);
                }
                if(sel_lepton->getPdgNumber()==13){
                    HistClass::FillStr((boost::format("%d_all_triggers_Gen")%3).str().c_str(),((*us).first).c_str(),1.);
                }
                if(sel_lepton->getPdgNumber()==15){
                    HistClass::FillStr((boost::format("%d_all_triggers_Gen")%4).str().c_str(),((*us).first).c_str(),1.);
                }
            }
        }
    }
}

bool specialAna::TriggerSelector(const pxl::Event* event){
    bool triggered=false;


    // Warning this disables all triggers
    //triggered=true;
    string datastream = event->getUserRecord( "Dataset" );
    TString Datastream = datastream;

    int usePdgNumber=0;
    if(sel_lepton){
        usePdgNumber=abs(sel_lepton->getPdgNumber());
    }else{
        usePdgNumber=abs(qcd_lepton->getPdgNumber());
    }


    //I dont understand the 8TeV triggers at the moment!!
    if( m_dataPeriod =="13TeV"){
        //this is 13 TeV
        //for( std::vector< string >::const_iterator it=m_trigger_string.begin(); it!= m_trigger_string.end();it++){
        pxl::UserRecords::const_iterator us = m_TrigEvtView->getUserRecords().begin();
        for( ; us != m_TrigEvtView->getUserRecords().end(); ++us ) {
            if (
                string::npos != (*us).first.find( "HLT_Ele115_CaloIdVT_GsfTrkIdT_v") or
                string::npos != (*us).first.find( "HLT_Ele105_CaloIdVT_GsfTrkIdT_v") or
                //string::npos != (*us).first.find( "HLT_Ele90_CaloIdVT_GsfTrkIdT_v") or
                //string::npos != (*us).first.find( "HLT_Ele80_CaloIdVT_GsfTrkIdT") or
                //string::npos != (*us).first.find( "HLT_Ele80_CaloIdVT_TrkIdT") or
                string::npos != (*us).first.find( "HLT_Ele27_eta2p1_WP75_Gsf_v") or
                string::npos != (*us).first.find( "HLT_Ele27_eta2p1_WPLoose_Gsf_v") or

                string::npos != (*us).first.find( "HLT_Mu45_eta2p1_v") or
                string::npos != (*us).first.find( "HLT_Mu50_eta2p1_v") or
                string::npos != (*us).first.find( "HLT_Mu50_v") or
                string::npos != (*us).first.find( "HLT_IsoMu24_eta2p1_v") or
                string::npos != (*us).first.find( "HLT_IsoMu24_eta2p1_IterTrk02_v") or
                //string::npos != (*us).first.find( "HLT_IsoTkMu24_eta2p1_IterTrk02_v") or
                //string::npos != (*us).first.find( "HLT_IsoTkMu20_eta2p1_IterTrk02_v") or
                //string::npos != (*us).first.find( "HLT_LooseIsoPFTau") or
                string::npos != (*us).first.find( "HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120_v") or

                //string::npos != (*us).first.find( "HLT_Ele95_CaloIdVT_GsfTrkIdT_") or
                //string::npos != (*us).first.find( "HLT_Ele32_eta2p1_WP85_Gsf_") or
                //string::npos != (*us).first.find( "HLT_Ele27_eta2p1_WP85_Gsf_") or
                //string::npos != (*us).first.find( "HLT_Ele25WP60_SC4_Mass55_") or
                //string::npos != (*us).first.find( "HLT_Photon") or


                string::npos != (*us).first.find( "HLT_MonoCentralPFJet80") or
                string::npos != (*us).first.find( "PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight")
            ){
                if(runOnData){
                    //corss cleaning for datasets
                    bool wrongDataset=false;
                    if(Datastream.Contains("SingleElectron") and  !(string::npos != (*us).first.find( "Ele"))){
                        wrongDataset=true;
                    }else if(Datastream.Contains("SingleMuon") and  (!(string::npos != (*us).first.find( "Mu")) or (string::npos != (*us).first.find( "PFMET")) ) ){
                        wrongDataset=true;
                    }else if(Datastream.Contains("Tau") and  !(string::npos != (*us).first.find( "Tau"))  ){
                        wrongDataset=true;
                    }else if(Datastream.Contains("MET") and  !( (string::npos != (*us).first.find( "Jet")) and (string::npos != (*us).first.find( "PFMET")) ) ){
                        wrongDataset=true;
                    }
                    if(wrongDataset){
                        //cout<<"wrong PD "<<(*us).first<<" "<< datastream<<endl;
                        //break here because the event will be in a other PD!!
                        break;
                    }
                }


                if(usePdgNumber==11 and string::npos == (*us).first.find( "Ele") and qcd_lepton==0 ){
                    //cout<<"hump "<<(*us).first<<" "<< usePdgNumber<<endl;
                    continue;
                }
                if(usePdgNumber==13 and (string::npos == (*us).first.find( "Mu"))){
                    //cout<<"hump "<<(*us).first<<" "<< usePdgNumber<<endl;
                    continue;
                }
                if(usePdgNumber==15
                //){
                and !(string::npos != (*us).first.find( "Tau") or string::npos != (*us).first.find( "PFMET") or string::npos != (*us).first.find( "PFJet")) ){
                    //cout<<"hump "<<(*us).first<<" "<< usePdgNumber<<endl;
                    continue;
                }
                triggered=(*us).second;
                if(triggered){
                    if(usePdgNumber==11){
                        if(sel_lepton){
                            if(!sel_lepton->hasUserRecord("Ele_27")){
                                sel_lepton->setUserRecord("Ele_27",false);
                            }
                            if(!sel_lepton->hasUserRecord("Ele105")){
                                sel_lepton->setUserRecord("Ele105",false);
                            }
                            if(!sel_lepton->hasUserRecord("Ele115")){
                                sel_lepton->setUserRecord("Ele115",false);
                            }
                            if(string::npos != (*us).first.find( "HLT_Ele27")){
                                sel_lepton->setUserRecord("Ele_27",true);
                            }else if(string::npos != (*us).first.find( "HLT_Ele105")){
                                sel_lepton->setUserRecord("Ele105",true);
                            }else if(string::npos != (*us).first.find( "HLT_Ele115")){
                                sel_lepton->setUserRecord("Ele115",true);
                            }
                        }
                    }
                    //break;
                }
            }
        }
            //try{
                //triggered=m_TrigEvtView->getUserRecord(*it);
                //if(triggered){
                    //break;
                //}
            //} catch( std::runtime_error &exc ) {
                //continue;
            //}
        //}
    }else{
        //this is 8TeV
        pxl::UserRecords::const_iterator us = m_TrigEvtView->getUserRecords().begin();
        for( ; us != m_TrigEvtView->getUserRecords().end(); ++us ) {
            if (
                //string::npos != (*us).first.find( "HLT_Ele27_CaloIdL_CaloIsoVL_TrkIdVL_TrkIsoVL_") or
                string::npos != (*us).first.find( "HLT_Ele27_WP80_v") or
                //string::npos != (*us).first.find( "HLT_Ele27_WP80_PFMET_MT50_") or
                //string::npos != (*us).first.find( "HLTEle30_CaloIdVT_TrkIdT_") or
                //string::npos != (*us).first.find( "HLT_Ele32_CaloIdL_CaloIsoVL_TrkIdVL_TrkIsoVL_") or
                string::npos != (*us).first.find( "HLT_Ele80_CaloIdVT_GsfTrkIdT_") or
                //string::npos != (*us).first.find( "HLTEle90_CaloIdVT_GsfTrkIdT_") or
                //string::npos != (*us).first.find( "HLTIsoMu20_WCandPt80_") or
                //string::npos != (*us).first.find( "HLTIsoMu20_eta2p1_") or
                //string::npos != (*us).first.find( "HLTIsoMu24_") or
                string::npos != (*us).first.find( "HLT_IsoMu24_eta2p1_") or
                //string::npos != (*us).first.find( "HLTIsoMu30_") or
                string::npos != (*us).first.find( "HLT_IsoMu30_eta2p1_") or
                //string::npos != (*us).first.find( "HLT_IsoMu34_eta2p1_") or
                //string::npos != (*us).first.find( "HLT_IsoMu40_eta2p1_") or
                //string::npos != (*us).first.find( "HLT_MET120_") or
                //string::npos != (*us).first.find( "HLT_MET120_HBHENoiseCleaned_") or
                //string::npos != (*us).first.find( "HLT_MET200_") or
                //string::npos != (*us).first.find( "HLT_MET200_HBHENoiseCleaned_") or
                string::npos != (*us).first.find( "HLT_Mu40_")
                //string::npos != (*us).first.find( "HLT_Mu40_eta2p1_")
                //string::npos != (*us).first.find( "HLT_PFMET150_") or
                //string::npos != (*us).first.find( "HLT_PFMET180_")
            ){

                if(runOnData){
                    if(Datastream.Contains("SingleEle") and  (string::npos != (*us).first.find( "Mu"))){
                        continue;
                    }
                    if(Datastream.Contains("SingleMu") and  (string::npos != (*us).first.find( "Ele"))){
                        continue;
                    }
                }

                if(usePdgNumber==11 and !string::npos != (*us).first.find( "Ele")){
                    continue;
                }
                if(usePdgNumber==13 and !string::npos != (*us).first.find( "Mu")){
                    continue;
                }
                if(usePdgNumber==15 and !(string::npos != (*us).first.find( "Tau") or string::npos != (*us).first.find( "PFMET") or string::npos != (*us).first.find( "PFJet")) ){
                    continue;
                }
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
            ////HistClass::FillStr("Tau_triggers",us->first.c_str(),1);
        //}
    //}



     return (triggered );

}

bool specialAna::triggerKinematics(){
    bool tiggerKinematics=false;
    if(sel_lepton && sel_met){
        if(sel_lepton->getName()==m_TauType){
            // Warning this disables all triggers
            //if(sel_lepton->getPt()>100 && sel_met->getPt()>150){
            //if(sel_lepton->getPt()>50 && sel_met->getPt()>50){
                tiggerKinematics=true;
            //}

        }
        if(sel_lepton->getName()=="Muon"){
            //&& fabs(sel_lepton->getEta())<2.1
            if(sel_lepton->getPt()>30){
                tiggerKinematics=true;
            }
        }
        if(sel_lepton->getName()=="Ele"){
            if(sel_lepton->getPt()>35 ){
                //&& fabs(sel_lepton->getEta())<2.1
                tiggerKinematics=true;
            }
        }
    }
    //same for qcd lepton candiadates
    if(qcd_lepton && sel_met){
        if(qcd_lepton->getName()==m_TauType){
            //if(qcd_lepton->getPt()>100 && sel_met->getPt()>150){
            //if(qcd_lepton->getPt()>50 && sel_met->getPt()>50){
                tiggerKinematics=true;
            //}

        }
        if(qcd_lepton->getName()=="Muon"){
            //&& fabs(qcd_lepton->getEta())<2.1
            if(qcd_lepton->getPt()>30){
                tiggerKinematics=true;
            }
        }
        if(qcd_lepton->getName()=="Ele"){
            //&& fabs(qcd_lepton->getEta())<2.1
            if(qcd_lepton->getPt()>35){
                tiggerKinematics=true;
            }
        }

    }
    return tiggerKinematics;
}
bool specialAna::TriggerSelector_highpt(const pxl::Event* event){
    bool triggered=false;


    //I dont understand the 8TeV triggers at the moment!!
    if( m_dataPeriod =="13TeV"){
        //this is 13 TeV
        //for( std::vector< string >::const_iterator it=m_trigger_string.begin(); it!= m_trigger_string.end();it++){
        pxl::UserRecords::const_iterator us = m_TrigEvtView->getUserRecords().begin();
        for( ; us != m_TrigEvtView->getUserRecords().end(); ++us ) {
            if (
                string::npos != (*us).first.find( "HLT_Ele115_CaloIdVT_GsfTrkIdT_v") or
                string::npos != (*us).first.find( "HLT_Ele105_CaloIdVT_GsfTrkIdT_v")
            ){
                triggered=(*us).second;
                if(triggered){
                    break;
                }
            }
        }
    }

     return (triggered );

}

void specialAna::QCDAnalyse() {
    //inverted isolation
    if(sel_lepton!=0){
        return;
    }
    int qcd_id=0;
    bool passed=false;
    bool passedPtMet=false;
    bool passedDeltaPhi=false;

    double m_leptonVetoPt=20;
    int numVetoMuo=vetoNumber(MuonList,m_leptonVetoPt);
    int numVetoTau=vetoNumberTau(TauList,m_leptonVetoPt);
    int numVetoEle=vetoNumber(EleList,m_leptonVetoPt);

    if( numVetoEle==0 && TauList->size()>=1 && numVetoMuo==0 ){
        int passedID=0;
        for( std::vector< pxl::Particle* >::iterator it = TauList->begin(); it != TauList->end(); ++it ) {
            if( Check_Tau_ID_no_iso(*it) ){
                passedID++;
                (*it)->setPdgNumber(15);
                QCDLeptonList.push_back(*it);
            }
        }
        if(passedID>=1){
            qcd_lepton=QCDLeptonList.at(0);
            m_pt_min_cut=m_pt_min_cut_tau;
            m_delta_phi_cut=m_delta_phi_cut_tau;
            m_pt_met_min_cut=m_pt_met_min_cut_tau;
            m_pt_met_max_cut=m_pt_met_max_cut_tau;

            m_pt_met_min_cut_funk_root=m_pt_met_min_cut_funk_root_tau;
            m_pt_met_max_cut_funk_root=m_pt_met_max_cut_funk_root_tau;
            m_delta_phi_cut_funk_root=m_delta_phi_cut_funk_root_tau;
            qcd_id=15;
        }
    }
    if( EleList->size()>=1 && numVetoTau==0 && numVetoMuo==0 ){
        int passedID=0;
        for( std::vector< pxl::Particle* >::iterator it = EleList->begin(); it != EleList->end(); ++it ) {

            if( (*it)->hasUserRecord("loosIDnoISO")){
                if ( (*it)->getUserRecord("loosIDnoISO").toBool() or (*it)->getUserRecord("loosIDandISO").toBool() ){
                    passedID++;
                    (*it)->setPdgNumber(11);
                    QCDLeptonList.push_back(*it);
                }
            }
            //we can not do the fr if we tag the electrons!!
            //else if (EleList->size()==1){
                //passedID++;
                //tmpEle=( pxl::Particle* ) EleList->at(0);
                //break;
            //}
        }
        //cout<<passedID<<endl;
        if(passedID>=1){
            qcd_lepton=QCDLeptonList.at(0);
            m_pt_min_cut=m_pt_min_cut_ele;
            m_delta_phi_cut=m_delta_phi_cut_ele;
            m_pt_met_min_cut=m_pt_met_min_cut_ele;
            m_pt_met_max_cut=m_pt_met_max_cut_ele;

            m_pt_met_min_cut_funk_root=m_pt_met_min_cut_funk_root_ele;
            m_pt_met_max_cut_funk_root=m_pt_met_max_cut_funk_root_ele;
            m_delta_phi_cut_funk_root=m_delta_phi_cut_funk_root_ele;
            qcd_id=11;
        }
    }
    if( numVetoEle==0 && numVetoTau==0 && MuonList->size()>=1 ){
        int passedID=0;
        for( std::vector< pxl::Particle* >::iterator it = MuonList->begin(); it != MuonList->end(); ++it ) {
            if( (*it)->hasUserRecord("ISOfailed")){
                if (  (*it)->getUserRecord("ISOfailed").toBool() ){
                    passedID++;
                    (*it)->setPdgNumber(13);
                    QCDLeptonList.push_back(*it);
                }
            }
            //we can not do the fr if we tag the muons!!
            //else if (MuonList->size()==1){
                //passedID++;
                //tmpMuo=( pxl::Particle* ) MuonList->at(0);
                //break;
            //}
        }
        if(passedID>=1){
            qcd_lepton=QCDLeptonList.at(0);
            m_pt_min_cut=m_pt_min_cut_muo;
            m_delta_phi_cut=m_delta_phi_cut_muo;
            m_pt_met_min_cut=m_pt_met_min_cut_muo;
            m_pt_met_max_cut=m_pt_met_max_cut_muo;

            m_pt_met_min_cut_funk_root=m_pt_met_min_cut_funk_root_muo;
            m_pt_met_max_cut_funk_root=m_pt_met_max_cut_funk_root_muo;
            m_delta_phi_cut_funk_root=m_delta_phi_cut_funk_root_muo;
            qcd_id=13;
        }
    }

    if(sel_met && qcd_lepton && qcd_lepton->getPt()>m_pt_min_cut){
        if(qcd_id==11){
            qcd_weight = max(0.,qcd_weight_ele_pt->GetBinContent(qcd_weight_ele_pt->FindBin(qcd_lepton->getPt())));
            for(unsigned int i=1;  i<QCDLeptonList.size();i++){
                qcd_weight*=(1-max(0.,qcd_weight_ele_pt->GetBinContent(qcd_weight_ele_pt->FindBin(QCDLeptonList[i]->getPt()))));
            }
            //for(unsigned int j=0;  j<QCDLeptonList.size();j++){
                //qcd_weight = max(0.,qcd_weight_ele_pt->GetBinContent(qcd_weight_ele_pt->FindBin(QCDLeptonList[j]->getPt())));
                //for(unsigned int i=0;  i<QCDLeptonList.size();i++){
                    //if(i==j){
                        //continue;
                    //}
                    //qcd_weight*=(1-max(0.,qcd_weight_ele_pt->GetBinContent(qcd_weight_ele_pt->FindBin(QCDLeptonList[i]->getPt()))));
                //}
            //}
        }

        if(qcd_id==13){
            qcd_weight = max(0.,qcd_weight_muo_pt->GetBinContent(qcd_weight_muo_pt->FindBin(qcd_lepton->getPt())));
        }
        if(qcd_id==15){
            qcd_weight = max(0.,qcd_weight_tau_pt->GetBinContent(qcd_weight_tau_pt->FindBin(qcd_lepton->getUserRecord("decayMode"))));
        }
        double mt=MT(qcd_lepton,sel_met);
        m_pt_met_min_cut=   m_pt_met_min_cut_funk_root.Eval(mt);
        m_pt_met_max_cut=   m_pt_met_max_cut_funk_root.Eval(mt);
        m_delta_phi_cut=    m_delta_phi_cut_funk_root.Eval(mt);

        if(qcd_lepton->getPt()/sel_met->getPt()>m_pt_met_min_cut && qcd_lepton->getPt()/sel_met->getPt()<m_pt_met_max_cut){
            passedPtMet=true;
        }
        if(DeltaPhi(qcd_lepton->getPhi(),sel_met->getPhi())>m_delta_phi_cut){
            passedDeltaPhi=true;
        }
        if (passedDeltaPhi && passedPtMet){
            passed=true;
        }
    }
    if(qcd_lepton){
        qcd_lepton->setUserRecord("passedPtMet",passedPtMet);
        qcd_lepton->setUserRecord("passedDeltaPhi",passedDeltaPhi);
        qcd_lepton->setUserRecord("passed",passed);
        qcd_lepton->setPdgNumber(qcd_id);
    }

}


void specialAna::ResponseAnalyse(){

    std::vector<pxl::Particle*> goodEle;
    std::vector<pxl::Particle*> goodMuo;

    bool passedDilepton=false;

    //make veto numbers
    //we don't need std::vectors, do we?
    double m_leptonVetoPt=20;
    int numVetoMuo=vetoNumber(MuonList,m_leptonVetoPt);
    int numVetoTau=vetoNumberTau(TauList,m_leptonVetoPt);
    int numVetoEle=vetoNumber(EleList,m_leptonVetoPt);

    if( EleList->size()>=2 && numVetoTau==0 && numVetoMuo==0 ){
        int passedID=0;
        for( std::vector< pxl::Particle* >::iterator it = EleList->begin(); it != EleList->end(); ++it ) {
            if( (*it)->hasUserRecord("IDpassed")){
                if ( (*it)->getUserRecord("IDpassed").toBool() ){
                    passedID++;
                    goodEle.push_back(*it);
                }
            }else if (EleList->size()==2){
                passedID=2;
                goodEle.push_back(( pxl::Particle* ) EleList->at(0));
                goodEle.push_back(( pxl::Particle* ) EleList->at(1));
                break;
            }
        }

        if(passedID==2){
            passedDilepton=true;
        }
    }
    if( numVetoEle==0 && numVetoTau==0 && MuonList->size()>=2 ){
        int passedID=0;
        for( std::vector< pxl::Particle* >::iterator it = MuonList->begin(); it != MuonList->end(); ++it ) {
            if( (*it)->hasUserRecord("IDpassed")){
                if (  (*it)->getUserRecord("IDpassed").toBool() ){
                    passedID++;
                    goodMuo.push_back(*it);
                }
            }else if (MuonList->size()==2){
                passedID=2;
                goodMuo.push_back(( pxl::Particle* ) MuonList->at(0));
                goodMuo.push_back(( pxl::Particle* ) MuonList->at(1));
                break;
            }
        }
        if(passedID==2){
            passedDilepton=true;
        }
    }

    if(passedDilepton){
        TVector3 qT=TVector3(0,0,0);
        TLorentzVector Zcandidate=TLorentzVector();
        TVector3 METvec=TVector3(sel_met->getPx(),sel_met->getPy(),0);


        if(goodEle.size()==2){
            qT.SetXYZ(
                        goodEle.at(0)->getPx()+goodEle.at(1)->getPx(),
                        goodEle.at(0)->getPy()+goodEle.at(1)->getPy(),
                        0
                        );
            Zcandidate.SetPxPyPzE(   goodEle.at(0)->getPx()+goodEle.at(1)->getPx(),
                                    goodEle.at(0)->getPy()+goodEle.at(1)->getPy(),
                                    goodEle.at(0)->getPz()+goodEle.at(1)->getPz(),
                                    goodEle.at(0)->getE()+goodEle.at(1)->getE()
                                );
        }
        else if(goodMuo.size()==2){
            qT.SetXYZ(
                        goodMuo.at(0)->getPx()+goodMuo.at(1)->getPx(),
                        goodMuo.at(0)->getPy()+goodMuo.at(1)->getPy(),
                        0
                        );
            Zcandidate.SetPxPyPzE(   goodMuo.at(0)->getPx()+goodMuo.at(1)->getPx(),
                                    goodMuo.at(0)->getPy()+goodMuo.at(1)->getPy(),
                                    goodMuo.at(0)->getPz()+goodMuo.at(1)->getPz(),
                                    goodMuo.at(0)->getE()+goodMuo.at(1)->getE()
                                );
        }


        TVector3 utot = (-1.) * ( qT + METvec );
        double u_par   = utot.Dot( qT.Unit() );
        //double u_trans = utot.Dot( qT.Orthogonal().Unit() );
        double R=      -u_par/qT.Perp();



        if(Zcandidate.M()>80 && Zcandidate.M()<100){
            if(goodEle.size()==2){
                mZtree["lep_type"]=11;
                mZtree["lep1_pt"]=goodEle.at(0)->getPt();
                mZtree["lep1_eta"]=goodEle.at(0)->getEta();
                mZtree["lep1_phi"]=goodEle.at(0)->getPhi();
                mZtree["lep1_E"]=goodEle.at(0)->getE();
                mZtree["lep1_pt"]=goodEle.at(1)->getPt();
                mZtree["lep1_eta"]=goodEle.at(1)->getEta();
                mZtree["lep1_phi"]=goodEle.at(1)->getPhi();
                mZtree["lep1_E"]=goodEle.at(1)->getE();

            }else{
                mZtree["lep_type"]=13;
                mZtree["lep1_pt"]=goodMuo.at(0)->getPt();
                mZtree["lep1_eta"]=goodMuo.at(0)->getEta();
                mZtree["lep1_phi"]=goodMuo.at(0)->getPhi();
                mZtree["lep1_E"]=goodMuo.at(0)->getE();
                mZtree["lep1_pt"]=goodMuo.at(1)->getPt();
                mZtree["lep1_eta"]=goodMuo.at(1)->getEta();
                mZtree["lep1_phi"]=goodMuo.at(1)->getPhi();
                mZtree["lep1_E"]=goodMuo.at(1)->getE();
            }


            mZtree["z_pt"]=Zcandidate.Pt();
            mZtree["z_eta"]=Zcandidate.Eta();
            mZtree["z_phi"]=Zcandidate.Phi();
            mZtree["z_E"]=Zcandidate.E();
            mZtree["met_pt"]=sel_met->getPt();
            mZtree["met_phi"]=sel_met->getPhi();
            HistClass::FillTree("ztree");

            HistClass::Fill("general_boson_qt",qT.Perp(),weight);
            HistClass::Profile("response",qT.Perp(),R);
            if(goodEle.size()==2){
                HistClass::Fill("general_boson_qt_ele",qT.Perp(),weight);
                HistClass::Profile("response_ele",qT.Perp(),R);
            }else if(goodMuo.size()==2){
                HistClass::Fill("general_boson_qt_muo",qT.Perp(),weight);
                HistClass::Profile("response_muo",qT.Perp(),R);
            }
        }
    }
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
    if(abs(sel_lepton->getPdgNumber())==11){
        mLeptonTree["highEtEleTrig"]=highEtTriggStored;
    }else{
        mLeptonTree["highEtEleTrig"]=0;
    }

    HistClass::FillTree("slimtree");

}
void specialAna::Fill_QCD_Tree(bool iso){

    if(iso){
        mQCDTree["lepton_n"]=0;
        mQCDTree["iso"]=iso;
        mQCDTree["ThisWeight"]=weight;
        mQCDTree["QCDWeight"]=qcd_weight;
        mQCDTree["met"]=sel_met->getPt();
        mQCDTree["pt"]=sel_lepton->getPt();
        mQCDTree["eta"]=sel_lepton->getEta();
        mQCDTree["delta_phi"]=DeltaPhi(sel_lepton,sel_met);
        mQCDTree["lepton_type"]=sel_lepton->getPdgNumber();
        if(sel_lepton->hasUserRecord("decayMode")){
            mQCDTree["decay_mode"]=sel_lepton->getUserRecord("decayMode").toDouble();
        }else{
            mQCDTree["decay_mode"]=-1;
        }
        HistClass::FillTree("qcdtree");
    }else{
        int i=0;
        for(auto thisQCDlepton : QCDLeptonList){
            mQCDTree["lepton_n"]=i;
            i++;
            mQCDTree["iso"]=iso;
            mQCDTree["ThisWeight"]=weight;
            mQCDTree["QCDWeight"]=qcd_weight;
            mQCDTree["met"]=sel_met->getPt();
            mQCDTree["pt"]=thisQCDlepton->getPt();
            mQCDTree["eta"]=thisQCDlepton->getEta();
            mQCDTree["delta_phi"]=DeltaPhi(thisQCDlepton,sel_met);
            mQCDTree["lepton_type"]=thisQCDlepton->getPdgNumber();
            if(thisQCDlepton->hasUserRecord("decayMode")){
                mQCDTree["decay_mode"]=thisQCDlepton->getUserRecord("decayMode").toDouble();
            }else{
                mQCDTree["decay_mode"]=-1;
            }
            HistClass::FillTree("qcdtree");
        }
    }


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

    if( m_dataPeriod=="13TeV" ){
        /// W tail fitting
        if(Datastream.Contains("WTo") && Datastream.Contains("Nu_Tune4C_13TeV")) {
                if(getWmass() > 200) return true;
        }
         if(Datastream.Contains("WTo") && (Datastream.Contains("Nu_M_200") || Datastream.Contains("Nu_M-200"))) {
                if(getWmass() > 500) return true;
        }
        if(Datastream.Contains("WTo") && (Datastream.Contains("Nu_M_500") || Datastream.Contains("Nu_M-500"))) {
                if(getWmass() > 1000) return true;
        }
        if(Datastream.Contains("WTo") && (Datastream.Contains("Nu_M_1000_13TeV") || Datastream.Contains("Nu_M-1000"))) {
                if(getWmass() > 3000) return true;
        }
        if(Datastream.Contains("WJetsToLNu_")) {
            if(getWmass() > 200) return true;
            //if(Datastream.Contains("WJetsToLNu_13TeV")){
                //if (m_GenEvtView->getUserRecord("genHT").toDouble() > 100) return true;
            //}
            if(getWmass()<0){
                cout<<"-----------------------------------------"<<endl;
                for(uint i = 0; i < S3ListGen->size(); i++){
                    cout<<S3ListGen->at(i)->getPdgNumber()<<"  "<<S3ListGen->at(i)->getPt() <<endl;
                }

            }

            //if(Datastream.Contains("FXFX") && !Datastream.Contains("HT")){
                //if(m_GenEvtView->hasUserRecord("genHT")){
                    //if(m_GenEvtView->getUserRecord("genHT").toDouble()>100){
                        //return true;
                    //}
                //}
            //}

        }
        if(Datastream.Contains("TT") and not Datastream.Contains("TT_Mtt-")) {
            if(getInvMtt() > 700) return true;
        }
        if(Datastream.Contains("TT_Mtt-700to1000")) {
            if(getInvMtt() < 700 and getInvMtt() > 1000) return true;
        }
        if(Datastream.Contains("TT_Mtt-1000toInf")) {
            if(getInvMtt() < 1000) return true;
        }

        if(Datastream.Contains("DYJetsToLL_M-50_")) {
            if(getZmass() > 100) return true;
        }


    }else if( m_dataPeriod=="8TeV" ){
        for(uint i = 0; i < S3ListGen->size(); i++){
            S3ListGen->at(i)->setPdgNumber(S3ListGen->at(i)->getUserRecord("id").toInt32());
        }
        /// W tail fitting
        //if(Datastream.Contains("WJetsToLNu_TuneZ2Star_8TeV")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
                //if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    //if(S3ListGen->at(i)->getPt() > 55)return true;
                //}
            //}
        //}
        if(Datastream.Contains("WJetsToLNu_PtW")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    if(S3ListGen->at(i)->getPt() <= 60) return true;
                }
            }

        }
        //if(Datastream.Contains("WJetsToLNu_HT")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
                //if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    //if(S3ListGen->at(i)->getPt() <= 55) return true;
                //}
            //}
        //}
        /// W mass tail fitting
        if(Datastream.Contains("WJetsToLNu")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            if (getPtHat() > 120) {
                return true;
            }
            if(!Datastream.Contains("_PtW")){
                for(uint i = 0; i < S3ListGen->size(); i++){
                    if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                        if(S3ListGen->at(i)->getPt() > 60) return true;
                    }
                }
            }
                //if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    //if(S3ListGen->at(i)->getMass() > 300)return true;
                //}
            //}
        }
        if(Datastream.Contains("WTo") and Datastream.Contains("ptmin")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            if (getPtHat() <= 120) return true;
                //if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    //if(S3ListGen->at(i)->getMass() < 300)return true;
                //}
            //}
        }
        if(Datastream.Contains("WTo") and !Datastream.Contains("ptmin") and !Datastream.Contains("toAnything")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            if (getPtHat() > 120) return true;
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24){
                    if(S3ListGen->at(i)->getPt() > 60) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-20_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 120) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-120_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 200) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-200_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 400) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-400_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 500) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-500_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 700) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-700_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 800) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-800_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 1000) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-1000_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 1500) return true;
                }
            }
        }
        if(Datastream.Contains("DYTo") and Datastream.Contains("M-1500_")) {
            //for(uint i = 0; i < S3ListGen->size(); i++){
            for(uint i = 0; i < S3ListGen->size(); i++){
                if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
                    if(S3ListGen->at(i)->getMass() > 2000) return true;
                }
            }
        }

        if(Datastream.Contains("WWJets") or Datastream.Contains("WZJets") or Datastream.Contains("ZZJets")) {
            for(uint i = 0; i < S3ListGen->size(); i++){
                int part_id = TMath::Abs(S3ListGen->at(i)->getPdgNumber());
                if(part_id == 23 || part_id == 24 ){
                    if (getPtHatDi() > 500) return true;
                }
            }
        }



    }

    /// Diboson tail fitting
//     if(Datastream.Contains("WW_") || Datastream.Contains("WZ_") || Datastream.Contains("ZZ_")) {
//         for(uint i = 0; i < S3ListGen->size(); i++){
//                 int part_id = TMath::Abs(S3ListGen->at(i)->getPdgNumber());
//           if(part_id == 23 || part_id == 22){
//                     if(S3ListGen->at(i)->getPt() > 500)return true;
//           }
//         }
//     }


    return false;
}


void specialAna::Fill_Gen_Controll_histo() {

    HistClass::Fill("MC_W_m_Gen",getWmass(),m_GenEvtView->getUserRecord( "Weight" ));
    int muon_gen_num=0;
    int ele_gen_num=0;
    int tau_gen_num=0;
    for(uint i = 0; i < S3ListGen->size(); i++){
        if (S3ListGen->at(i)->getPt()<10 && not (TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24)   ){
            continue;
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
        }else if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24 or TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23){
            if( (( pxl::Particle*)  S3ListGen->at(i)->getMother())!=0){
                if(TMath::Abs( (( pxl::Particle*)  S3ListGen->at(i)->getMother())->getPdgNumber()) == 15){
                    continue;
                }
            }

            HistClass::Fill("MC_W_pt_Gen",S3ListGen->at(i)->getPt(),m_GenEvtView->getUserRecord( "Weight" ));


            HistClass::FillSparse( "W_pt_m_Gen",2,S3ListGen->at(i)->getPt(),S3ListGen->at(i)->getMass());
            HistClass::FillSparse( "W_pt_m_Gen",2,S3ListGen->at(i)->getPt(),S3ListGen->at(i)->getMass());

        }



    }
    for(uint i = 0; i < TauVisListGen->size(); i++){
        HistClass::Fill(1,"Tau_pt_Gen",TauVisListGen->at(i)->getPt(),m_GenEvtView->getUserRecord( "Weight" ));
        HistClass::Fill(1,"Tau_eta_Gen",TauVisListGen->at(i)->getEta(),m_GenEvtView->getUserRecord( "Weight" ));
        HistClass::Fill(1,"Tau_phi_Gen",TauVisListGen->at(i)->getPhi(),m_GenEvtView->getUserRecord( "Weight" ));
        HistClass::Fill(0,"Tau_gen_decayMode",TauVisListGen->at(i)->getUserRecord("decay_mode_id").toDouble(),m_GenEvtView->getUserRecord( "Weight" ));

    }
    //double ht=0;
    //for(uint i = 0; i < JetListGen->size(); i++){

        //ht+=JetListGen->at(i)->getPt();
    //}

    //ht+=METListGen->at(0)->getPt();
    //cout<<"jet ht: "<<ht<<"   "<<getGenHT()<<endl;
    HistClass::Fill("MC_W_pthat_Gen",getPtHat(),m_GenEvtView->getUserRecord( "Weight" ));

    if(m_GenEvtView->hasUserRecord( "binScale" )){
        HistClass::Fill("MC_W_pthat2_Gen", m_GenEvtView->getUserRecord( "binScale" ).toDouble()  ,m_GenEvtView->getUserRecord( "Weight" ));
    }
    HistClass::Fill("MC_HT_constructed_Gen",getGenHT(),m_GenEvtView->getUserRecord( "Weight" ));

    HistClass::Fill(0,"Tau_num_Gen",tau_gen_num,m_GenEvtView->getUserRecord( "Weight" ));
    HistClass::Fill(0,"Muon_num_Gen",muon_gen_num,m_GenEvtView->getUserRecord( "Weight" ));
    HistClass::Fill(0,"Ele_num_Gen",ele_gen_num,m_GenEvtView->getUserRecord( "Weight" ));



    //resolution plots:
    //for( std::vector< pxl::Particle* >::const_iterator part_it = MuonList->begin(); part_it != MuonList->end(); ++part_it ) {
        //pxl::Particle *part = *part_it;
        //if(part->getSoftRelations().hasType("priv-gen-rec")){
            //pxl::Particle *genPart = (pxl::Particle*) part->getSoftRelations().getFirst (m_GenEvtView->getObjectOwner(), "priv-gen-rec");
            //Fill_Gen_Rec_histos(genPart,part);
        //}
    //}
    //resolution plots:
    for( std::vector< pxl::Particle* >::const_iterator part_it = TauList->begin(); part_it != TauList->end(); ++part_it ) {
        pxl::Particle *part = *part_it;
        if(part->getSoftRelations().hasType("priv-gen-rec")){
            //pxl::Particle *genPart = (pxl::Particle*) part->getSoftRelations().getFirst (m_GenEvtView->getObjectOwner(), "priv-gen-rec");

            //cout<<"gentau"<<genPart<<endl;
            //pxl::Particle *genPart2 = (pxl::Particle*) genPart->getSoftRelations().getFirst (m_GenEvtView->getObjectOwner(), "genParticle");
            pxl::Particle *genPart = Get_Truth_match("Tau",part);
            if (genPart!=0){
                pxl::Particle *genVisTau =Get_tau_truth_decay_mode( genPart);
                Fill_Gen_Rec_histos(genVisTau,part);
            }
        }
    }

    for(std::vector<TString>::iterator it = d_mydisc.begin(); it != d_mydisc.end(); it++){
        for(auto itau = TauVisListGen->begin(); itau != TauVisListGen->end(); itau++){
            pxl::Particle* matched_reco_particle = 0;
            //if (part_i->getUserRecord("decay_mode_id") == 0 or part_i->getUserRecord("decay_mode_id") == 1) continue;
            double delta_r_max = 0.25;
            for (std::vector< pxl::Particle* >::const_iterator part_jt = TauList->begin(); part_jt != TauList->end(); ++part_jt) {
                if (DeltaR(*part_jt, *itau) < delta_r_max) {
                    delta_r_max = DeltaR(*part_jt, *itau);
                    matched_reco_particle = *part_jt;
                }
            }

            if (matched_reco_particle){

                if(matched_reco_particle->getUserRecord(it->Data()).toDouble()>0.5) {
                    HistClass::FillEff(Form("Tau_%s_RECO_vs_pT", it->Data()), (*itau)->getPt(), true);
                }else{
                    HistClass::FillEff(Form("Tau_%s_RECO_vs_pT", it->Data()), (*itau)->getPt(), false);
                }
            }
            if (matched_reco_particle != 0 && matched_reco_particle->getUserRecord("decayModeFindingNewDMs").toDouble()>0.5) {
                if( matched_reco_particle->getUserRecord(it->Data()).toDouble()>0.5){
                    HistClass::FillEff(Form("Tau_%s_ID_vs_pT", it->Data()), (*itau)->getPt(), true);
                }else{
                    HistClass::FillEff(Form("Tau_%s_ID_vs_pT", it->Data()), (*itau)->getPt(), false);
                }
            }
            //if(matched_reco_particle==0){
                //cout<<(*itau)->getPt()<<"   "<<(*itau)->getEta() <<endl;
            //}

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


    //HistClass::Fill(hist_number,"Muon_LHits",lepton->getUserRecord("LHits"),weight);
    //HistClass::Fill(hist_number,"Muon_VHits",lepton->getUserRecord("VHits"),weight);
    //HistClass::Fill(hist_number,"Muon_VHitsPixel",lepton->getUserRecord("VHitsPixel"),weight);
    //HistClass::Fill(hist_number,"Muon_VHitsTracker",lepton->getUserRecord("VHitsTracker"),weight);
    //HistClass::Fill(hist_number,"Muon_VHitsMuonSys",lepton->getUserRecord("VHitsMuonSys"),weight);
    //HistClass::Fill(hist_number,"Muon_TrackerLayersWithMeas",lepton->getUserRecord("TrackerLayersWithMeas"),weight);
    //HistClass::Fill(hist_number,"Muon_PixelLayersWithMeas",lepton->getUserRecord("PixelLayersWithMeas"),weight);
    //HistClass::Fill(hist_number,"Muon_NMatchedStations",lepton->getUserRecord("NMatchedStations"),weight);

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

    HistClass::Fill(hist_number,"Muon_Dxy",lepton->getUserRecord("Dxy").toDouble() ,weight);
    HistClass::Fill(hist_number,"Muon_Dz",lepton->getUserRecord("Dz").toDouble() ,weight);
    if(lepton->hasUserRecord("ndof")){
        HistClass::Fill(hist_number,"Muon_Ndof",lepton->getUserRecord("ndof"),weight);
        HistClass::Fill(hist_number,"Muon_Chi2",lepton->getUserRecord("chi2"),weight);
        HistClass::Fill(hist_number,"Muon_qoverp",lepton->getUserRecord("qoverp"),weight);
        HistClass::Fill(hist_number,"Muon_qoverpError",lepton->getUserRecord("qoverpError"),weight);
        HistClass::Fill(hist_number,"Muon_ptError",lepton->getUserRecord("ptError"),weight);
        HistClass::Fill(hist_number,"Muon_ptErroroverpt",lepton->getUserRecord("ptError").toDouble()/lepton->getPt(),weight);
        HistClass::Fill(hist_number,"Muon_dpt_over_pt",(lepton->getUserRecord("ptError").toDouble())/lepton->getPt(),weight);
    }
    if(lepton->hasUserRecord("DxyBT")){
        HistClass::Fill(hist_number,"Muon_DxyBT",lepton->getUserRecord("DxyBT").toDouble() ,weight);
        HistClass::Fill(hist_number,"Muon_DzBT",lepton->getUserRecord("DzBT").toDouble() ,weight);
        HistClass::Fill(hist_number,"Muon_DxyBS",lepton->getUserRecord("DxyBS").toDouble() ,weight);
        HistClass::Fill(hist_number,"Muon_DzBS",lepton->getUserRecord("DzBS").toDouble() ,weight);
    }

    HistClass::Fill(hist_number,"Muon_pt_reciprocal",1/lepton->getPt(),weight);
    if(sel_met)
        HistClass::Fill(hist_number,"Muon_mt_reciprocal",1/MT(lepton,sel_met),weight);

}
void specialAna::Fill_Controll_Ele_histo(int hist_number, pxl::Particle* lepton){
    Fill_Particle_hisos(hist_number,lepton);


    if(lepton->hasUserRecord("Ele_27")){
        if(lepton->getUserRecord("Ele_27").toBool() && !lepton->getUserRecord("Ele105").toBool() && !lepton->getUserRecord("Ele115").toBool() ){
            HistClass::Fill(hist_number, "Ele_mt_Ele_27",MT(lepton,sel_met),weight);
            HistClass::Fill(hist_number, "Ele_pt_Ele_27",lepton->getPt(),weight);
            HistClass::Fill(hist_number,"Ele_met_Ele_27",sel_met->getPt(),weight);
        }else if(lepton->getUserRecord("Ele105").toBool() && !lepton->getUserRecord("Ele115").toBool()){
            HistClass::Fill(hist_number, "Ele_mt_Ele_105",MT(lepton,sel_met),weight);
            HistClass::Fill(hist_number, "Ele_pt_Ele_105",lepton->getPt(),weight);
            HistClass::Fill(hist_number,"Ele_met_Ele_105",sel_met->getPt(),weight);
        }else if(lepton->getUserRecord("Ele115").toBool()){
            HistClass::Fill(hist_number, "Ele_mt_Ele_115",MT(lepton,sel_met),weight);
            HistClass::Fill(hist_number, "Ele_pt_Ele_115",lepton->getPt(),weight);
            HistClass::Fill(hist_number,"Ele_met_Ele_115",sel_met->getPt(),weight);
        }
    }




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
    HistClass::Fill(hist_number,"Ele_ECALIso",lepton->getUserRecord ("ECALIso").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_ECALIso03",lepton->getUserRecord ("ECALIso03").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_ECALIso04",lepton->getUserRecord ("ECALIso04").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_ESCOverPout",lepton->getUserRecord ("ESCOverPout"),weight);
    HistClass::Fill(hist_number,"Ele_ESCSeedOverP",lepton->getUserRecord ("ESCSeedOverP"),weight);
    HistClass::Fill(hist_number,"Ele_ESCSeedPout",lepton->getUserRecord ("ESCSeedPout"),weight);
    HistClass::Fill(hist_number,"Ele_EoP",lepton->getUserRecord ("EoP"),weight);
    HistClass::Fill(hist_number,"Ele_GSFNormChi2",lepton->getUserRecord ("GSFNormChi2"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso",lepton->getUserRecord ("HCALIso").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso03",lepton->getUserRecord ("HCALIso03"),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso03d1",lepton->getUserRecord ("HCALIso03d1").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso03d2",lepton->getUserRecord ("HCALIso03d2").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso04",lepton->getUserRecord ("HCALIso04").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso04d1",lepton->getUserRecord ("HCALIso04d1").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_HCALIso04d2",lepton->getUserRecord ("HCALIso04d2").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_HCALIsoConeDR03_2012",lepton->getUserRecord ("HCALIsoConeDR03_2012").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_HCALIsoConeDR04_2012",lepton->getUserRecord ("HCALIsoConeDR04_2012").toDouble()/lepton->getPt(),weight);
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
    HistClass::Fill(hist_number,"Ele_TrkIso",lepton->getUserRecord ("TrkIso").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_TrkIso03",lepton->getUserRecord ("TrkIso03").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_TrkIso04",lepton->getUserRecord ("TrkIso04").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_Vtx_X",lepton->getUserRecord ("Vtx_X"),weight);
    HistClass::Fill(hist_number,"Ele_Vtx_Y",lepton->getUserRecord ("Vtx_Y"),weight);
    HistClass::Fill(hist_number,"Ele_Vtx_Z",lepton->getUserRecord ("Vtx_Z"),weight);
    HistClass::Fill(hist_number,"Ele_chargedHadronIso",lepton->getUserRecord ("chargedHadronIso").toDouble()/lepton->getPt(),weight);
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
    HistClass::Fill(hist_number,"Ele_neutralHadronIso",lepton->getUserRecord ("neutralHadronIso").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_photonIso",lepton->getUserRecord ("photonIso").toDouble()/lepton->getPt(),weight);
    HistClass::Fill(hist_number,"Ele_pin",lepton->getUserRecord ("pin"),weight);
    HistClass::Fill(hist_number,"Ele_pout",lepton->getUserRecord ("pout"),weight);
    if(lepton->hasUserRecord("puChargedHadronIso")){
        HistClass::Fill(hist_number,"Ele_puChargedHadronIso",lepton->getUserRecord ("puChargedHadronIso").toDouble()/lepton->getPt(),weight);
    }
    if(lepton->hasUserRecord("Ele_Match")){
        HistClass::Fill(hist_number,"Ele_Match",lepton->getUserRecord ("Match"),weight);
    }
    HistClass::Fill(hist_number,"Ele_sigmaIetaIeta",lepton->getUserRecord ("sigmaIetaIeta"),weight);





    //cout<<"-----------------------------------------"<<endl;
    //pxl::UserRecord::const_iterator us = lepton->getUserRecord().begin();
    //for( ; us != lepton->getUserRecord().end(); ++us ) {
        //try{
            //lepton->getUserRecord(us->first);
            ////cout<<"HistClass::Fill(hist_number,\"Ele_"<<us->first<<"\",lepton->getUserRecord (\""<<us->first<<"\"),weight);"<<endl;
            ////HistClass::CreateHisto(7,"Muon_VHits","Muon_VHits", 100, 0, 100,"N_{valid hits}");
            //cout<<"HistClass::CreateHisto(7,\"Ele_"<< us->first<<"\",\"Ele_"<< us->first<<"\", 100, 0, 100,\""<<  us->first<<"\");"<<endl;
        //}catch( std::runtime_error &exc ) {
        //}
        //try{
            //lepton->getUserRecord(us->first);
            ////cout<<"HistClass::Fill(hist_number,\"Ele_"<<us->first<<"\",lepton->getUserRecord (\""<<us->first<<"\"),weight);"<<endl;
            //cout<<"HistClass::CreateHisto(7,\"Ele_"<< us->first<<"\",\"Ele_"<< us->first<<"\", 100, 0, 100,\""<<  us->first<<"\");"<<endl;
        //}catch( std::runtime_error &exc ) {
        //}
        //try{
            //lepton->getUserRecord(us->first);
            ////cout<<"HistClass::Fill(hist_number,\"Ele_"<<us->first<<"\",lepton->getUserRecord (\""<<us->first<<"\"),weight);"<<endl;
            //cout<<"HistClass::CreateHisto(7,\"Ele_"<< us->first<<"\",\"Ele_"<< us->first<<"\", 100, 0, 100,\""<<  us->first<<"\");"<<endl;
        //}catch( std::runtime_error &exc ) {
        //}
    //}

}
void specialAna::Fill_Controll_Tau_histo(int hist_number, pxl::Particle* lepton) {
    Fill_Particle_hisos(hist_number,lepton);
    //for(uint j = 0; j < 67; j++) {
        //HistClass::Fill(hist_number,"Tau_discriminator",j+1,lepton->getUserRecord( (string)d_mydisc[j] ));
    //}

    for(std::vector<TString>::iterator it = d_mydisc.begin(); it != d_mydisc.end(); it++){
        HistClass::FillStr(hist_number,"Tau_discriminator",*it,lepton->getUserRecord( (string)(*it) ));
    }
    //if(hist_number==0){
        //pxl::UserRecords::const_iterator us = lepton->getUserRecords().begin();
        //for( ; us != lepton->getUserRecords().end(); ++us ) {
            ////if ( string::npos == (*us).first.find( *it )){
            ////if ( string::npos != (*us).first.find( "against")){
                //triggers.insert(us->first);
            ////}
        //}
    //}
    if(lepton->hasUserRecord("decayMode")){
        HistClass::Fill(hist_number,"Tau_decayMode",lepton->getUserRecord("decayMode"),weight);
        HistClass::Fill(hist_number,"Tau_Vtx_X",lepton->getUserRecord("Vtx_X"),weight);
        HistClass::Fill(hist_number,"Tau_Vtx_Y",lepton->getUserRecord("Vtx_Y"),weight);
        HistClass::Fill(hist_number,"Tau_Vtx_Z",lepton->getUserRecord("Vtx_Z"),weight);
        HistClass::Fill(hist_number,"Tau_NumPFChargedHadrCands",lepton->getUserRecord("NumPFChargedHadrCands"),weight);
        HistClass::Fill(hist_number,"Tau_NumPFGammaCands",lepton->getUserRecord("NumPFGammaCands"),weight);
        HistClass::Fill(hist_number,"Tau_NumPFNeutralHadrCands",lepton->getUserRecord("NumPFNeutralHadrCands"),weight);
        HistClass::Fill(hist_number,"Tau_LeadingHadronPt",lepton->getUserRecord("LeadingHadronPt"),weight);
        //HistClass::Fill(hist_number,"tauJetpx",lepton->getUserRecord("tauJetpx"),weight);
        //HistClass::Fill(hist_number,"tauJetpy",lepton->getUserRecord("tauJetpy"),weight);
        //HistClass::Fill(hist_number,"tauJetpz",lepton->getUserRecord("tauJetpz"),weight);
        HistClass::Fill(hist_number,"Tau_Jet_pt",lepton->getUserRecord("tauJetpt"),weight);
        HistClass::Fill(hist_number,"Tau_Jet_eta",lepton->getUserRecord("tauJeteta"),weight);
        HistClass::Fill(hist_number,"Tau_Jet_phi",lepton->getUserRecord("tauJetphi"),weight);
        //delete jet;
        HistClass::Fill(hist_number,"Tau_dxy",lepton->getUserRecord("dxy"),weight);
        HistClass::Fill(hist_number,"Tau_dxy_error",lepton->getUserRecord("dxy_error"),weight);
        HistClass::Fill(hist_number,"Tau_dxy_Sig",lepton->getUserRecord("dxy_Sig"),weight);

    }
    HistClass::Fill(hist_number,"Tau_Mass",lepton->getMass(),weight);

    if(hist_number==3){
        if(not runOnData){
            pxl::Particle* tmp_part=Get_Truth_match_all_flavor(lepton);
            if(tmp_part){
                HistClass::Fill(abs(tmp_part->getPdgNumber()),"Tau_mt_genmatch",MT(lepton,sel_met),weight);
                if(tmp_part->getPdgNumber()==15){
                    pxl::Particle* tmp_part2=Get_tau_truth_decay_mode(tmp_part);
                    HistClass::Fill(tmp_part2->getUserRecord("decay_mode"),"Tau_mt_decaymode",MT(lepton,sel_met),weight);
                }
            }else{
                HistClass::Fill(24,"Tau_mt_genmatch",MT(lepton,sel_met),weight);
            }
        }
    }
}

void specialAna::Fill_Particle_hisos(int hist_number, pxl::Particle* lepton , string syst){
    string name=lepton->getName();
    if(lepton->getName()==m_TauType){
        name="Tau";
    }
    //without systematics:
    if(syst==""){

        HistClass::Fill(hist_number,str(boost::format("%s_nPrimaryVertices")%name ),m_RecEvtView->getUserRecord("NumVertices"),weight);
        HistClass::Fill(str(boost::format("%s_cutflow")%name).c_str() ,hist_number+1,weight);
        if (lepton->hasUserRecord("IDFailValue")){
            HistClass::Fill(hist_number,str(boost::format("%s_IDFail")%name ),lepton->getUserRecord("IDFailValue"),weight);
        }
        HistClass::Fill(hist_number,str(boost::format("%s_charge")%name ),lepton->getCharge(),weight);
        if(JetList->size()>0){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_pt")%name ),JetList->at(0)->getPt(),weight);
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_eta")%name ),JetList->at(0)->getEta(),weight);
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_phi")%name ),JetList->at(0)->getPhi(),weight);
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_DeltaPhi")%name ),DeltaPhi(lepton,JetList->at(0)),weight);
            HistClass::Fill(hist_number,str(boost::format("%s_jet_1_lep_Minv")%name ),Mass(JetList->at(0),lepton),weight);
        }
        if(JetList->size()>1){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_2_pt")%name ),JetList->at(1)->getPt(),weight);
        }
        if(JetList->size()>2){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_3_pt")%name ),JetList->at(2)->getPt(),weight);
        }
        if(JetList->size()>3){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_4_pt")%name ),JetList->at(3)->getPt(),weight);
        }
        if(JetList->size()>4){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_5_pt")%name ),JetList->at(4)->getPt(),weight);
        }
        if(JetList->size()>5){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_6_pt")%name ),JetList->at(5)->getPt(),weight);
        }
        if(JetList->size()>6){
            HistClass::Fill(hist_number,str(boost::format("%s_jet_7_pt")%name ),JetList->at(6)->getPt(),weight);
        }
        double st=0;
        for(auto it = JetList->begin();it!=JetList->end();it++){
            st+=(*it)->getPt();
        }
        HistClass::Fill(hist_number,str(boost::format("%s_jet_ht")%name ),st,weight);
    } else {
        syst="_syst_"+syst;
    }
    HistClass::Fill(hist_number,str(boost::format("%s_pt%s")%name %syst ),lepton->getPt(),weight);
    HistClass::Fill(hist_number,str(boost::format("%s_eta%s")%name %syst ),lepton->getEta(),weight);
    HistClass::Fill(hist_number,str(boost::format("%s_phi%s")%name %syst ),lepton->getPhi(),weight);


    if(sel_met){
        HistClass::Fill(hist_number,str(boost::format("%s_DeltaPhi%s")%name %syst ),DeltaPhi(lepton,sel_met),weight);
        HistClass::Fill(hist_number,str(boost::format("%s_mt%s")%name %syst ),MT(lepton,sel_met),weight);
        HistClass::Fill(hist_number,str(boost::format("%s_ET_MET%s")%name %syst ),lepton->getPt()/sel_met->getPt(),weight);
        HistClass::Fill(hist_number,str(boost::format("%s_met%s")%name %syst ),sel_met->getPt(),weight);
        HistClass::Fill(hist_number,str(boost::format("%s_met_phi%s")%name %syst ),sel_met->getPhi(),weight);
        if(syst==""){
            if(m_FilterEvtView){
                if(m_FilterEvtView->hasUserRecord("Flag_CSCTightHaloFilter")){
                    if( m_FilterEvtView->getUserRecord("Flag_CSCTightHaloFilter").toBool()){
                        HistClass::Fill(hist_number,str(boost::format("%s_mt%s_Flag_CSCTightHaloFilter")%name %syst ),MT(lepton,sel_met),weight);
                    }
                    if( m_FilterEvtView->getUserRecord("Flag_goodVertices").toBool()){
                        HistClass::Fill(hist_number,str(boost::format("%s_mt%s_Flag_goodVertices")%name %syst ),MT(lepton,sel_met),weight);
                    }
                    if( m_FilterEvtView->getUserRecord("Flag_eeBadScFilter").toBool()){
                        HistClass::Fill(hist_number,str(boost::format("%s_mt%s_Flag_eeBadScFilter")%name %syst ),MT(lepton,sel_met),weight);
                    }
                    if( m_FilterEvtView->getUserRecord("Flag_eeBadScFilter").toBool() &&m_FilterEvtView->getUserRecord("Flag_goodVertices").toBool() &&m_FilterEvtView->getUserRecord("Flag_CSCTightHaloFilter").toBool()){
                        HistClass::Fill(hist_number,str(boost::format("%s_mt%s_allFilter")%name %syst ),MT(lepton,sel_met),weight);
                    }
                }else{
                   HistClass::Fill(hist_number,str(boost::format("%s_mt%s_noFilterRun")%name %syst ),MT(lepton,sel_met),weight);
                }
            }
            if(JetList->size()>0){
                HistClass::Fill(hist_number,str(boost::format("%s_jet_1_DeltaPhiMET")%name ),DeltaPhi(sel_met,JetList->at(0)),weight);
                calc_stuff.setP4(sel_met->getVector());
                calc_stuff.addP4(lepton->getVector());
                HistClass::Fill(hist_number,str(boost::format("%s_jet_1_DeltaPhiMETlep")%name ),DeltaPhi(calc_stuff.getPhi(),JetList->at(0)->getPhi()),weight);
            }
        }
    }
}

void specialAna::Fill_Controll_histo(int hist_number, pxl::Particle* lepton) {

    //if(lepton->getPdgNumber()==11){
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

void specialAna::Create_RECO_effs() {
    Create_RECO_object_effs("Muon");
    Create_RECO_object_effs("Ele");
    Create_RECO_object_effs("Tau");
    Create_RECO_object_effs("TauVis");
    TString x_bins_names[13] = {"1Pi0Pi0",
                                "1Pi1Pi0",
                                "1Pi2Pi0",
                                "1Pi>2Pi0",
                                "3Pi0Pi0",
                                "3Pi1Pi0",
                                "3Pi2Pi0",
                                "3Pi>2Pi0",
                                ">3Pi0Pi0",
                                ">3Pi1Pi0",
                                ">3Pi2Pi0",
                                ">3Pi>2Pi0",
                                "else"};
    TString y_bins_names[16] = {"tauDecay1ChargedPion0PiZero",
                                "tauDecay1ChargedPion1PiZero",
                                "tauDecay1ChargedPion2PiZero",
                                "tauDecay1ChargedPion3PiZero",
                                "tauDecay1ChargedPion4PiZero",
                                "tauDecay2ChargedPion0PiZero",
                                "tauDecay2ChargedPion1PiZero",
                                "tauDecay2ChargedPion2PiZero",
                                "tauDecay2ChargedPion3PiZero",
                                "tauDecay2ChargedPion4PiZero",
                                "tauDecay3ChargedPion0PiZero",
                                "tauDecay3ChargedPion1PiZero",
                                "tauDecay3ChargedPion2PiZero",
                                "tauDecay3ChargedPion3PiZero",
                                "tauDecay3ChargedPion4PiZero",
                                "tauDecayOther"};

    HistClass::CreateHisto("Tau_RECO_vs_gendm_vs_recodm_0_500", 13, 0, 13, 16, 0, 16, "DM(gen)", "DM(reco)");
    HistClass::NameBins("Tau_RECO_vs_gendm_vs_recodm_0_500", 13, x_bins_names, 16, y_bins_names);
    HistClass::CreateHisto("Tau_RECO_vs_gendm_vs_recodm_500_1000", 13, 0, 13, 16, 0, 16, "DM(gen)", "DM(reco)");
    HistClass::NameBins("Tau_RECO_vs_gendm_vs_recodm_500_1000", 13, x_bins_names, 16, y_bins_names);
    HistClass::CreateHisto("Tau_RECO_vs_gendm_vs_recodm_1000_1500", 13, 0, 13, 16, 0, 16, "DM(gen)", "DM(reco)");
    HistClass::NameBins("Tau_RECO_vs_gendm_vs_recodm_1000_1500", 13, x_bins_names, 16, y_bins_names);
    HistClass::CreateHisto("Tau_RECO_vs_gendm_vs_recodm_1500_2000", 13, 0, 13, 16, 0, 16, "DM(gen)", "DM(reco)");
    HistClass::NameBins("Tau_RECO_vs_gendm_vs_recodm_1500_2000", 13, x_bins_names, 16, y_bins_names);
    HistClass::CreateHisto("Tau_RECO_vs_gendm_vs_recodm_2000", 13, 0, 13, 16, 0, 16, "DM(gen)", "DM(reco)");
    HistClass::NameBins("Tau_RECO_vs_gendm_vs_recodm_2000", 13, x_bins_names, 16, y_bins_names);
    HistClass::CreateHisto("Tau_RECO_vs_genpt_vs_recopt", 100, 0, 1000, 100, 0, 1000, "p_{T}^{gen}", "p_{T}^{reco}");
    for(auto it = d_mydisc.begin(); it != d_mydisc.end(); it++){
        HistClass::CreateEff(TString::Format("Tau_%s_RECO_vs_pT", it->Data()),         500, 0, 5000,"p_{T}^{(gen)} (GeV)");
        HistClass::CreateEff(TString::Format("Tau_%s_ID_vs_pT", it->Data()),         500, 0, 5000,"p_{T}^{(gen)} (GeV)");
    }

    Create_RECO_object_effs("MET");
}

void specialAna::Create_RECO_object_effs(std::string object) {

    HistClass::CreateEff(TString::Format("%s_RECO_vs_pT", object.c_str()),         500, 0, 5000,TString::Format("p_{T}^{%s(gen)} (GeV)", object.c_str()));
    HistClass::CreateEff(TString::Format("%s_RECO_vs_leadcharged_pt", object.c_str()),         500, 0, 5000,TString::Format("p_{T}^{%s leadcharged(gen)} (GeV)", object.c_str()));
    HistClass::CreateEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()),       70, 0, 70,
                         "n_{vtx}");
    HistClass::CreateEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), 150, -3, 3, 500, 0, 3.5,
                         TString::Format("#eta(%s(gen))", object.c_str()), TString::Format("#phi(%s(gen)) (rad)", object.c_str()));
    if (object != "MET") {
        HistClass::CreateEff(TString::Format("%s_RECO_vs_pT_in_Acc", object.c_str()),         500, 0, 5000,
                             TString::Format("p_{T}^{%s(gen)} (GeV)", object.c_str()));
        HistClass::CreateEff(TString::Format("%s_RECO_vs_Nvtx_in_Acc", object.c_str()),       70, 0, 70,
                             "n_{vtx}");
        HistClass::CreateEff(TString::Format("%s_RECO_vs_eta_vs_phi_in_Acc", object.c_str()), 150, -3, 3, 500, 0, 3.5,
                             TString::Format("#eta(%s(gen))", object.c_str()), TString::Format("#phi(%s(gen)) (rad)", object.c_str()));
    }
    HistClass::CreateProf(TString::Format("%s_RECO_vs_pT", object.c_str()),         500, 0, 5000,TString::Format("p_{T}^{%s(gen)} (GeV)", object.c_str()),"(reco-gen)/gen");
    if (object == "TauVis") {
        HistClass::CreateProf(TString::Format("%s_RECO_vs_jetpT", object.c_str()),         500, 0, 5000,TString::Format("p_{T}^{%s(gen)} (GeV)", object.c_str()),"(reco-gen)/gen");
        HistClass::CreateProf(TString::Format("%s_RECO_tau_vs_jet_pT", object.c_str()),         500, 0, 5000,TString::Format("p_{T}^{%s} (GeV)", object.c_str()),"(tau-jet)/tau");
        for(int i=0;i<60;i++){
            HistClass::CreateProf(TString::Format("%s_vtx_%i_RECO_vs_pT", object.c_str(),i),         500, 0, 5000,TString::Format("p_{T}^{%s(gen)} (GeV)", object.c_str()),"(reco-gen)/gen");
        }
        for(int i=0;i<15;i++){
            HistClass::CreateProf(TString::Format("%s_decay_%i_RECO_vs_pT", object.c_str(),i),         500, 0, 5000,TString::Format("p_{T}^{%s(gen)} (GeV)", object.c_str()),"(reco-gen)/gen");
            HistClass::CreateEff(TString::Format("%s_decay_%i_RECO_vs_pT", object.c_str(),i),         500, 0, 5000,TString::Format("p_{T}^{%s(gen)} (GeV)", object.c_str()));


        }
    }

}

void specialAna::Fill_RECO_effs() {
    Fill_RECO_object_effs("Muon", 13, *MuonList);
    Fill_RECO_object_effs("Ele", 11, *EleList);
    Fill_RECO_object_effs("Tau", 15, *TauList);
    Fill_RECO_object_effs("TauVis", 15, *TauList);
    Fill_RECO_object_effs("MET", 12, *METList);
}

void specialAna::Fill_RECO_object_effs(std::string object, int id, std::vector< pxl::Particle* > part_list) {
    if (object == "MET") {
        pxl::Particle* gen_met = 0;
        for (std::vector< pxl::Particle* >::const_iterator part_it = S3ListGen->begin(); part_it != S3ListGen->end(); ++part_it) {
            if (TMath::Abs((*part_it)->getPdgNumber()) != 12 and TMath::Abs((*part_it)->getPdgNumber()) != 14 and TMath::Abs((*part_it)->getPdgNumber()) != 16 ) continue;
            if (gen_met == 0) {
                gen_met = (pxl::Particle*) (*part_it)->clone();
            } else {
                gen_met->addP4((*part_it));
            }
        }
        if (gen_met != 0) {
            if (part_list.size() > 0 and DeltaPhi(part_list[0], gen_met) < 0.25) {
                HistClass::Profile(TString::Format("%s_RECO_vs_pT", object.c_str()), gen_met->getPt(),  (part_list[0]->getPt()-gen_met->getPt())/gen_met->getPt());
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), gen_met->getPt(), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), gen_met->getEta(), gen_met->getPhi(), true);
            } else {
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), gen_met->getPt(), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), gen_met->getEta(), gen_met->getPhi(), false);
            }
        }
        delete gen_met;
    } else if (object == "TauVis") {
        for (std::vector< pxl::Particle* >::const_iterator part_it = TauVisListGen->begin(); part_it != TauVisListGen->end(); ++part_it) {
            pxl::Particle* matched_reco_particle = 0;
            if ((*part_it)->getUserRecord("decay_mode_id") == 0 or (*part_it)->getUserRecord("decay_mode_id") == 1) continue;
            double delta_r_max = 0.25;
            for (std::vector< pxl::Particle* >::const_iterator part_jt = part_list.begin(); part_jt != part_list.end(); ++part_jt) {
                pxl::Particle *part_j = *part_jt;
                if (DeltaR(part_j, (*part_it)) < delta_r_max) {
                    delta_r_max = DeltaR(part_j, (*part_it));
                    matched_reco_particle = (pxl::Particle*) part_j->clone();
                }
            }

            if (matched_reco_particle != 0) {
                //cout<<"found match"<<endl;
                HistClass::Profile(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(),  (matched_reco_particle->getPt()-(*part_it)->getPt())/(*part_it)->getPt());
                HistClass::Profile(TString::Format("%s_RECO_tau_vs_jet_pT", object.c_str()), matched_reco_particle->getPt(),  (matched_reco_particle->getPt()-matched_reco_particle->getUserRecord("tauJetpt").toDouble())/matched_reco_particle->getPt());
                HistClass::Profile(TString::Format("%s_vtx_%i_RECO_vs_pT", object.c_str(), m_RecEvtView->getUserRecord("NumVertices").toInt32() ), (*part_it)->getPt(),  (matched_reco_particle->getPt()-(*part_it)->getPt())/(*part_it)->getPt());
                HistClass::Profile(TString::Format("%s_decay_%i_RECO_vs_pT", object.c_str(),(*part_it)->getUserRecord("decay_mode_id").toInt32() ), (*part_it)->getPt(),  (matched_reco_particle->getPt()-(*part_it)->getPt())/(*part_it)->getPt());


                HistClass::Profile(TString::Format("%s_RECO_vs_jetpT", object.c_str()), (*part_it)->getPt(),  (matched_reco_particle->getUserRecord("tauJetpt").toDouble()-(*part_it)->getPt())/(*part_it)->getPt());
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(), true);
                HistClass::FillEff(TString::Format("%s_decay_%i_RECO_vs_pT", object.c_str(),(*part_it)->getUserRecord("decay_mode_id").toInt32()), (*part_it)->getPt(), true);

                HistClass::FillEff(TString::Format("%s_RECO_vs_leadcharged_pt", object.c_str()), Get_highest_pt_dauter((*part_it))->getPt(), true);

                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()),m_RecEvtView->getUserRecord("NumVertices"), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), true);
                HistClass::Fill("Tau_RECO_vs_genpt_vs_recopt",(*part_it)->getPt(),matched_reco_particle->getPt(), 1.);


                if (matched_reco_particle->getUserRecord("decayMode") == 17) {
                    if ((*part_it)->getPt() < 500) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_0_500",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         15.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 500 and (*part_it)->getPt() < 1000) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_500_1000",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         15.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 1000 and (*part_it)->getPt() < 1500) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_1000_1500",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         15.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 1500 and (*part_it)->getPt() < 2000) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_1500_2000",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         15.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 2000) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_2000",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         15.5,
                                         1.);
                    }
                } else {
                    if ((*part_it)->getPt() < 500) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_0_500",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         (double)matched_reco_particle->getUserRecord("decayMode")+0.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 500 and (*part_it)->getPt() < 1000) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_500_1000",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         (double)matched_reco_particle->getUserRecord("decayMode")+0.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 1000 and (*part_it)->getPt() < 1500) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_1000_1500",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         (double)matched_reco_particle->getUserRecord("decayMode")+0.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 1500 and (*part_it)->getPt() < 2000) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_1500_2000",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         (double)matched_reco_particle->getUserRecord("decayMode")+0.5,
                                         1.);
                    } else if ((*part_it)->getPt() > 2000) {
                        HistClass::Fill("Tau_RECO_vs_gendm_vs_recodm_2000",
                                         (double)(*part_it)->getUserRecord("decay_mode_id")-1.5,
                                         (double)matched_reco_particle->getUserRecord("decayMode")+0.5,
                                         1.);
                    }
                }
            } else {
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(), false);
                HistClass::FillEff(TString::Format("%s_decay_%i_RECO_vs_pT", object.c_str(),(*part_it)->getUserRecord("decay_mode_id").toInt32()), (*part_it)->getPt(), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_leadcharged_pt", object.c_str()), Get_highest_pt_dauter((*part_it))->getPt(), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), false);
            }
            if (TMath::Abs((*part_it) -> getEta()) < 2.5) {
                if (matched_reco_particle != 0) {
                    HistClass::FillEff(TString::Format("%s_RECO_vs_pT_in_Acc", object.c_str()), (*part_it)->getPt(), true);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx_in_Acc", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), true);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi_in_Acc", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), true);
                } else {
                    HistClass::FillEff(TString::Format("%s_RECO_vs_pT_in_Acc", object.c_str()), (*part_it)->getPt(), false);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx_in_Acc", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), false);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi_in_Acc", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), false);
                }
            }
            delete matched_reco_particle;
        }
    } else if (object == "Tau") {
        for (std::vector< pxl::Particle* >::const_iterator part_it = S3ListGen->begin(); part_it != S3ListGen->end(); ++part_it) {
            pxl::Particle* matched_reco_particle = 0;
            if (TMath::Abs((*part_it)->getPdgNumber()) != id) continue;
            double delta_r_max = 0.25;
            for (std::vector< pxl::Particle* >::const_iterator part_jt = part_list.begin(); part_jt != part_list.end(); ++part_jt) {
                pxl::Particle *part_j = *part_jt;
                if (DeltaR(part_j, (*part_it)) < delta_r_max) {
                    delta_r_max = DeltaR(part_j, (*part_it));
                    matched_reco_particle = (pxl::Particle*) part_j->clone();
                }
            }
            if (matched_reco_particle != 0) {
                HistClass::Profile(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(),  (matched_reco_particle->getPt()-(*part_it)->getPt())/(*part_it)->getPt());
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_leadcharged_pt", object.c_str()), Get_highest_pt_dauter((*part_it))->getPt(), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), true);
            } else {
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_leadcharged_pt", object.c_str()), Get_highest_pt_dauter((*part_it))->getPt(), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), false);
            }
            if (TMath::Abs((*part_it) -> getEta()) < 2.5) {
                if (matched_reco_particle != 0) {
                    HistClass::FillEff(TString::Format("%s_RECO_vs_pT_in_Acc", object.c_str()), (*part_it)->getPt(), true);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx_in_Acc", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), true);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi_in_Acc", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), true);
                } else {
                    HistClass::FillEff(TString::Format("%s_RECO_vs_pT_in_Acc", object.c_str()), (*part_it)->getPt(), false);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx_in_Acc", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), false);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi_in_Acc", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), false);
                }
            }
            delete matched_reco_particle;
        }
    } else {
        for (std::vector< pxl::Particle* >::const_iterator part_it = S3ListGen->begin(); part_it != S3ListGen->end(); ++part_it) {
            pxl::Particle* matched_reco_particle = 0;
            if (TMath::Abs((*part_it)->getPdgNumber()) != id) continue;
            double delta_r_max = 0.25;
            for (std::vector< pxl::Particle* >::const_iterator part_jt = part_list.begin(); part_jt != part_list.end(); ++part_jt) {
                pxl::Particle *part_j = *part_jt;
                if( (*part_jt)->hasUserRecord("IDpassed")){
                    if ( !(*part_jt)->getUserRecord("IDpassed").toBool() ){
                        continue;
                    }
                }
                if (DeltaR(part_j, (*part_it)) < delta_r_max) {
                    delta_r_max = DeltaR(part_j, (*part_it));
                    matched_reco_particle = (pxl::Particle*) part_j->clone();
                }
            }
            if (matched_reco_particle != 0) {
                HistClass::Profile(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(),  (matched_reco_particle->getPt()-(*part_it)->getPt())/(*part_it)->getPt());
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), true);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), true);
            } else {
                HistClass::FillEff(TString::Format("%s_RECO_vs_pT", object.c_str()), (*part_it)->getPt(), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), false);
                HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), false);
            }
            if (TMath::Abs((*part_it) -> getEta()) < 2.5) {
                if (matched_reco_particle != 0) {
                    HistClass::FillEff(TString::Format("%s_RECO_vs_pT_in_Acc", object.c_str()), (*part_it)->getPt(), true);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx_in_Acc", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), true);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi_in_Acc", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), true);
                } else {
                    HistClass::FillEff(TString::Format("%s_RECO_vs_pT_in_Acc", object.c_str()), (*part_it)->getPt(), false);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_Nvtx_in_Acc", object.c_str()), m_RecEvtView->getUserRecord("NumVertices"), false);
                    HistClass::FillEff(TString::Format("%s_RECO_vs_eta_vs_phi_in_Acc", object.c_str()), (*part_it)->getEta(), (*part_it)->getPhi(), false);
                }
            }
            delete matched_reco_particle;
        }
    }
}

pxl::Particle* specialAna::Get_Truth_match(std::string name, pxl::Particle* lepton) {
    int part_temp_id = 0;
    if (name == "Tau") {
        part_temp_id = 15;
    } else if (name == "Muon") {
        part_temp_id = 13;
    } else if (name == "Ele") {
        part_temp_id = 11;
    }
    double temp_delta_r = 10;
    pxl::Particle* gen_match = 0;
    for (std::vector< pxl::Particle* >::const_iterator part_it = S3ListGen->begin(); part_it != S3ListGen->end(); ++part_it) {
        pxl::Particle *part_i = *part_it;
        int part_temp_truth_id = 0;
        if (m_dataPeriod =="8TeV") {
            part_temp_truth_id = TMath::Abs(part_i->getUserRecord("id").asInt32());
        } else if (m_dataPeriod =="13TeV") {
            part_temp_truth_id = TMath::Abs(part_i->getPdgNumber());
        }
        if (part_temp_id != part_temp_truth_id) continue;
        double test_delta_r = lepton->getVector().deltaR(part_i->getVector());
        if (test_delta_r < temp_delta_r) {
            temp_delta_r = test_delta_r;
            gen_match = part_i;
        }
    }
    return gen_match;
}


pxl::Particle* specialAna::Get_Truth_match_all_flavor(pxl::Particle* lepton) {
    double temp_delta_r = 0.5;
    pxl::Particle* gen_match = 0;
    for (std::vector< pxl::Particle* >::const_iterator part_it = S3ListGen->begin(); part_it != S3ListGen->end(); ++part_it) {
        pxl::Particle *part_i = *part_it;
        double test_delta_r = lepton->getVector().deltaR(part_i->getVector());
        if (test_delta_r < temp_delta_r && fabs(lepton->getPt()-part_i->getPt())/part_i->getPt()<0.5 && abs(part_i->getPdgNumber())<23) {
            temp_delta_r = test_delta_r;
            gen_match = part_i;
        }
    }
    return gen_match;
}
//pxl::Particle* specialAna::Get_tau_truth_decay_mode( pxl::Particle* truth_tau) {
    //int n_prong = 0;
    //std::vector<pxl::Particle*>* final_state_part_list = new std::vector< pxl::Particle* >;
    //std::vector<pxl::Particle*>* temp_part = new std::vector< pxl::Particle* >;
    //temp_part->push_back(truth_tau);
    //std::vector<pxl::Particle*>* new_temp_part = new std::vector< pxl::Particle* >;
    //pxl::Particle* vis_tau_decay = new pxl::Particle();
    //vis_tau_decay->setName("Tau_visible_decay");
    //vis_tau_decay->setP4(0, 0, 0, 0);
    //while (true) {
        //bool continue_loop = false;
        //for (uint i = 0; i < temp_part->size(); i++) {
            //pxl::Particle* temp_part_dummy = temp_part->at(i);
            //if (temp_part_dummy->getDaughters().size() == 0) {
                //if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 11) {  /// electrons
                    //final_state_part_list->push_back(temp_part_dummy);
                    //n_prong++;
                    //vis_tau_decay->addP4(temp_part_dummy);
                //} else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 13) {  /// muons
                    //final_state_part_list->push_back(temp_part_dummy);
                    //n_prong++;
                    //vis_tau_decay->addP4(temp_part_dummy);
                //} else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 111) {  /// pi 0
                    //final_state_part_list->push_back(temp_part_dummy);
                    //vis_tau_decay->addP4(temp_part_dummy);
                //} else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 211) {  /// pi +
                    //final_state_part_list->push_back(temp_part_dummy);
                    //n_prong++;
                    //vis_tau_decay->addP4(temp_part_dummy);
                //} else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 130 or
                    //TMath::Abs(temp_part_dummy->getPdgNumber()) == 310 or
                    //TMath::Abs(temp_part_dummy->getPdgNumber()) == 311) {  /// K 0
                    //final_state_part_list->push_back(temp_part_dummy);
                    //vis_tau_decay->addP4(temp_part_dummy);
                //} else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 321) {  /// K +
                    //final_state_part_list->push_back(temp_part_dummy);
                    //n_prong++;
                    //vis_tau_decay->addP4(temp_part_dummy);
                //}
            //} else {
                //for (std::set< pxl::Relative* >::const_iterator part_it = temp_part_dummy->getDaughters().begin(); part_it != temp_part_dummy->getDaughters().end(); ++part_it) {
                    //pxl::Relative *part_i = *part_it;
                    //pxl::Particle* part = (pxl::Particle*)part_i;
                    //if (TMath::Abs(part->getPdgNumber()) == 16) {  /// tau neutrino
                        //continue;
                    //} else if (TMath::Abs(part->getPdgNumber()) == 14) {  /// muon neutrino
                        //continue;
                    //} else if (TMath::Abs(part->getPdgNumber()) == 12) {  /// electron neutrino
                        //continue;
                    //} else if (TMath::Abs(part->getPdgNumber()) == 22) {  /// photon
                        //vis_tau_decay->addP4(part);
                        //continue;
                    //} else if (TMath::Abs(part->getPdgNumber()) == 11) {  /// electrons
                        //final_state_part_list->push_back(part);
                        //n_prong++;
                        //vis_tau_decay->addP4(part);
                    //} else if (TMath::Abs(part->getPdgNumber()) == 13) {  /// muons
                        //final_state_part_list->push_back(part);
                        //n_prong++;
                        //vis_tau_decay->addP4(part);
                    //} else if (TMath::Abs(part->getPdgNumber()) == 111) {  /// pi 0
                        //final_state_part_list->push_back(part);
                        //vis_tau_decay->addP4(part);
                    //} else if (TMath::Abs(part->getPdgNumber()) == 211) {  /// pi +
                        //final_state_part_list->push_back(part);
                        //n_prong++;
                        //vis_tau_decay->addP4(part);
                    //} else if (TMath::Abs(part->getPdgNumber()) == 130 or
                        //TMath::Abs(part->getPdgNumber()) == 310 or
                        //TMath::Abs(part->getPdgNumber()) == 311) {  /// K 0
                        //final_state_part_list->push_back(part);
                        //vis_tau_decay->addP4(part);
                    //} else if (TMath::Abs(part->getPdgNumber()) == 321) {  /// K +
                        //final_state_part_list->push_back(part);
                        //n_prong++;
                        //vis_tau_decay->addP4(part);
                    //} else {  /// others like W +
                        //continue_loop = true;
                        //new_temp_part->push_back(part);
                    //}
                //}
            //}
        //}
        //if (not continue_loop) {
            //break;
        //} else {
            //delete temp_part;
            //temp_part = new std::vector< pxl::Particle* >;
            //*temp_part = *new_temp_part;
            //delete new_temp_part;
            //new_temp_part = new std::vector< pxl::Particle* >;
        //}
    //}

    //int n_piplus = 0;
    //int n_pizero = 0;
    //int n_Kplus = 0;
    //int n_Kzero = 0;
    //int n_ele = 0;
    //int n_muo = 0;
    //int charge = 0;
    //for (std::vector< pxl::Particle* >::const_iterator part_it = final_state_part_list->begin(); part_it != final_state_part_list->end(); ++part_it) {
        //pxl::Particle *part_i = *part_it;
        //if (part_i->getPdgNumber() == 211) {
            //n_piplus++;
            //charge++;
        //} else if (part_i->getPdgNumber() == -211) {
            //n_piplus++;
            //charge -= 1;
        //} else if (part_i->getPdgNumber() == 111) {
            //n_pizero++;
        //} else if (part_i->getPdgNumber() == 130 or
                   //part_i->getPdgNumber() == 310 or
                   //part_i->getPdgNumber() == 311 or
                   //part_i->getPdgNumber() == -311) {
            //n_Kzero++;
        //} else if (part_i->getPdgNumber() == 321) {
            //n_Kplus++;
            //charge++;
        //} else if (part_i->getPdgNumber() == -321) {
            //n_Kplus++;
            //charge -= 1;
        //} else if (part_i->getPdgNumber() == 11) {
            //n_ele++;
            //charge -= 1;
        //} else if (part_i->getPdgNumber() == -11) {
            //n_ele++;
            //charge++;
        //} else if (part_i->getPdgNumber() == 13) {
            //n_muo++;
            //charge -= 1;
        //} else if (part_i->getPdgNumber() == -13) {
            //n_muo++;
            //charge++;
        //} else {
            //std::cerr << "Found a particle that should not be here" << std::endl;
            //std::cerr << "ID: " << part_i->getPdgNumber() << std::endl;
        //}
    //}

    //TString decay_mode = "";
    //int decay_mode_id = -1;
    //if (n_ele > 0 and (n_piplus > 0 or n_Kplus > 0)) {
        //if (n_ele == 2) {
            //n_pizero++;
            //n_ele = 0;
        //} else {
            //decay_mode = TString::Format("%iEleX", n_ele);
            //decay_mode_id = 14;
        //}
    //} else if (n_ele > 0 and n_muo == 0) {
        //decay_mode = TString::Format("%iEle", n_ele);
        //decay_mode_id = 0;
    //} else if (n_muo > 0 and (n_piplus > 0 or n_Kplus > 0)) {
        //decay_mode = TString::Format("%iMuoX", n_muo);
        //decay_mode_id = 14;
    //} else if (n_muo > 0 and n_ele == 0) {
        //decay_mode = TString::Format("%iMuo", n_muo);
        //decay_mode_id = 1;
    //} else if (n_muo > 0 and n_ele > 0) {
        //decay_mode = TString::Format("%iEle%iMuo", n_ele, n_muo);
        //decay_mode_id = 14;
    //} else {
        //TString pi_zero_part = "";
        //if (n_pizero > 0) {
            //pi_zero_part = TString::Format("%iPi0", n_pizero);
        //}
        //TString pi_plus_part = "";
        //if (n_piplus > 0) {
            //pi_plus_part = TString::Format("%iPi", n_piplus);
        //}
        //TString K_zero_part = "";
        //if (n_Kzero > 0) {
            //K_zero_part = TString::Format("%iK0", n_Kzero);
        //}
        //TString K_plus_part = "";
        //if (n_Kplus > 0) {
            //K_plus_part = TString::Format("%iK", n_Kplus);
        //}
        //decay_mode = (pi_plus_part + pi_zero_part + K_zero_part + K_plus_part);
        //if (n_piplus + n_Kplus == 1) {
            //if (n_pizero + n_Kzero == 0) {
                //decay_mode_id = 2;
            //} else if (n_pizero + n_Kzero == 1) {
                //decay_mode_id = 3;
            //} else if (n_pizero + n_Kzero == 2) {
                //decay_mode_id = 4;
            //} else {
                //decay_mode_id = 5;
            //}
        //} else if (n_piplus + n_Kplus == 3) {
            //if (n_pizero + n_Kzero == 0) {
                //decay_mode_id = 6;
            //} else if (n_pizero + n_Kzero == 1) {
                //decay_mode_id = 7;
            //} else if (n_pizero + n_Kzero == 2) {
                //decay_mode_id = 8;
            //} else {
                //decay_mode_id = 9;
            //}
        //} else {
            //if (n_pizero + n_Kzero == 0) {
                //decay_mode_id = 10;
            //} else if (n_pizero + n_Kzero == 1) {
                //decay_mode_id = 11;
            //} else if (n_pizero + n_Kzero == 2) {
                //decay_mode_id = 12;
            //} else {
                //decay_mode_id = 13;
            //}
        //}
    //}

    //delete final_state_part_list;
    //delete new_temp_part;
    //delete temp_part;

    //vis_tau_decay->setUserRecord("n_prong",  n_prong);
    //vis_tau_decay->setUserRecord("decay_mode",  (std::string)decay_mode);
    //vis_tau_decay->setUserRecord("decay_mode_id",  decay_mode_id);
    //cout<<(std::string)decay_mode<< "  "<<decay_mode_id<<   "  : "<<n_prong<<"  : "<<vis_tau_decay->getPt()<<"  "<<truth_tau->getPt()<<" : "<< n_pizero <<"  "<< n_Kzero<<"  "<<  n_pizero+n_Kzero <<endl;

    //return vis_tau_decay;
//}
pxl::Particle* specialAna::Get_tau_truth_decay_mode( pxl::Particle* truth_tau) {
    if (tau_gen_visMap.end()  !=tau_gen_visMap.find(truth_tau)){
        return tau_gen_visMap[truth_tau];
    }
    int n_prong = 0;
    std::vector<pxl::Particle*>* final_state_part_list = new std::vector< pxl::Particle* >;
    std::vector<pxl::Particle*>* temp_part = new std::vector< pxl::Particle* >;
    temp_part->push_back(truth_tau);
    std::vector<pxl::Particle*>* new_temp_part = new std::vector< pxl::Particle* >;
    pxl::Particle* vis_tau_decay = new pxl::Particle();
    vis_tau_decay->setName("Tau_visible_decay");
    vis_tau_decay->setP4(0,0,0,0);
    while (true) {
        bool continue_loop = false;
        for (unsigned int i = 0; i < temp_part->size(); i++) {
            pxl::Particle* temp_part_dummy = temp_part->at(i);
            if (temp_part_dummy->getDaughters().size() == 0) {
                if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 11) { // electrons
                    final_state_part_list->push_back(temp_part_dummy);
                    n_prong++;
                    vis_tau_decay->addP4(temp_part_dummy);
                } else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 13) { // muons
                    final_state_part_list->push_back(temp_part_dummy);
                    n_prong++;
                    vis_tau_decay->addP4(temp_part_dummy);
                } else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 15) { // taus
                    cout<<" not clean tau"<<endl;
                } else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 111) { // pi 0
                    final_state_part_list->push_back(temp_part_dummy);
                    vis_tau_decay->addP4(temp_part_dummy);
                } else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 211) { // pi +
                    final_state_part_list->push_back(temp_part_dummy);
                    n_prong++;
                    vis_tau_decay->addP4(temp_part_dummy);
                } else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 130 or
                    TMath::Abs(temp_part_dummy->getPdgNumber()) == 310 or
                    TMath::Abs(temp_part_dummy->getPdgNumber()) == 311) { // K 0
                    final_state_part_list->push_back(temp_part_dummy);
                    vis_tau_decay->addP4(temp_part_dummy);
                } else if (TMath::Abs(temp_part_dummy->getPdgNumber()) == 321) { // K +
                    final_state_part_list->push_back(temp_part_dummy);
                    n_prong++;
                    vis_tau_decay->addP4(temp_part_dummy);
                }
                else{
                    final_state_part_list->push_back(temp_part_dummy);
                    //n_prong++;
                    vis_tau_decay->addP4(temp_part_dummy);
                    //cout<<"ssdusdsdb "<<temp_part_dummy->getPdgNumber()<<"    "<< temp_part_dummy->getDaughters().size() <<endl;
                }
            } else {
                for (std::set< pxl::Relative* >::const_iterator part_it = temp_part_dummy->getDaughters().begin(); part_it != temp_part_dummy->getDaughters().end(); ++part_it) {
                    pxl::Relative *part_i = *part_it;
                    pxl::Particle* part = (pxl::Particle*)part_i;
                    if (TMath::Abs(part->getPdgNumber()) == 16) { // tau neutrino
                        continue;
                    } else if (TMath::Abs(part->getPdgNumber()) == 14) { // muon neutrino
                        continue;
                    } else if (TMath::Abs(part->getPdgNumber()) == 12) { // electron neutrino
                        continue;
                    } else if (TMath::Abs(part->getPdgNumber()) == 15) { // grrr tau
                        cout<<" not clean tau"<<endl;
                    } else if (TMath::Abs(part->getPdgNumber()) == 22) { // photon
                        vis_tau_decay->addP4(part);
                        continue;
                    } else if (TMath::Abs(part->getPdgNumber()) == 11) { // electrons
                        final_state_part_list->push_back(part);
                        n_prong++;
                        vis_tau_decay->addP4(part);
                    } else if (TMath::Abs(part->getPdgNumber()) == 13) { // muons
                        final_state_part_list->push_back(part);
                        n_prong++;
                        vis_tau_decay->addP4(part);
                    } else if (TMath::Abs(part->getPdgNumber()) == 111) { // pi 0
                        final_state_part_list->push_back(part);
                        vis_tau_decay->addP4(part);
                    } else if (TMath::Abs(part->getPdgNumber()) == 211) { // pi +
                        final_state_part_list->push_back(part);
                        n_prong++;
                        vis_tau_decay->addP4(part);
                    } else if (TMath::Abs(part->getPdgNumber()) == 130 or
                        TMath::Abs(part->getPdgNumber()) == 310 or
                        TMath::Abs(part->getPdgNumber()) == 311) { // K 0
                        final_state_part_list->push_back(part);
                        vis_tau_decay->addP4(part);
                    } else if (TMath::Abs(part->getPdgNumber()) == 321) { // K +
                        final_state_part_list->push_back(part);
                        n_prong++;
                        vis_tau_decay->addP4(part);
                    } else { // others like W +
                        continue_loop = true;
                        new_temp_part->push_back(part);
                    }
                }
            }
        }
        if (not continue_loop) {
            break;
        } else {
            temp_part->clear();
            //new_temp_part->clear();
            delete temp_part;
            temp_part = new std::vector< pxl::Particle* >;
            *temp_part = *new_temp_part;
            delete new_temp_part;
            new_temp_part = new std::vector< pxl::Particle* >;
        }
    }

    if(final_state_part_list->size()==0){
        cout<<"---------------------begin---------------------------"<<endl;
        for (std::set< pxl::Relative* >::const_iterator part_it = truth_tau->getDaughters().begin(); part_it != truth_tau->getDaughters().end(); ++part_it) {
            pxl::Relative *part_i = *part_it;
            pxl::Particle* part = (pxl::Particle*)part_i;

            cout<<part->getPdgNumber()<<"    "<< part->getDaughters().size() <<endl;
            for (std::set< pxl::Relative* >::const_iterator part_it2 = part->getDaughters().begin(); part_it2 != part->getDaughters().end(); ++part_it2) {
                pxl::Relative *part_i2 = *part_it2;
                pxl::Particle* part2 = (pxl::Particle*)part_i2;

                cout<<"sub "<<part2->getPdgNumber()<<"    "<< part2->getDaughters().size() <<endl;
            }
        }
        cout<<"----------------------end--------------------------"<<endl;
    }

    int n_piplus = 0;
    int n_pizero = 0;
    int n_Kplus = 0;
    int n_Kzero = 0;
    int n_ele = 0;
    int n_muo = 0;
    int charge = 0;
    int n_other=0;
    for (std::vector< pxl::Particle* >::const_iterator part_it = final_state_part_list->begin(); part_it != final_state_part_list->end(); ++part_it) {
        pxl::Particle *part_i = *part_it;
        if (abs(part_i->getPdgNumber()) == 211) {
            n_piplus++;
            charge+=part_i->getPdgNumber()/abs(part_i->getPdgNumber());
        } else if (part_i->getPdgNumber() == 111) {
            n_pizero++;
        } else if (abs(part_i->getPdgNumber()) == 130 or
                   abs(part_i->getPdgNumber()) == 310 or
                   abs(part_i->getPdgNumber()) == 311) {
            n_Kzero++;
        } else if (abs(part_i->getPdgNumber()) == 321) {
            n_Kplus++;
            charge+=part_i->getPdgNumber()/abs(part_i->getPdgNumber());
        } else if (abs(part_i->getPdgNumber()) == 11) {
            n_ele++;
            charge+=part_i->getPdgNumber()/abs(part_i->getPdgNumber());
        } else if (abs(part_i->getPdgNumber()) == 13) {
            n_muo++;
            charge+=part_i->getPdgNumber()/abs(part_i->getPdgNumber());
        } else {
            n_other++;
            //std::cerr << "Found a particle that should not be here" << std::endl;
            //std::cerr << "ID: " << part_i->getPdgNumber() << std::endl;
            //std::cerr << << std::endl;
        }
    }

    TString decay_mode = "";
    int decay_mode_id = 13;

    if (n_ele > 0 and (n_piplus > 0 or n_Kplus > 0)) {
        if(n_ele==2){
            //n_pizero++;
            n_ele=0;
        }else{
            decay_mode = TString::Format("%iEle%iPi%iK", n_ele,n_piplus,n_Kplus);
            decay_mode_id = 14;
            //std::cerr << "Sanity check failed!" << std::endl;
            //std::cerr << "Found electron and hadrons as tau decay products" << std::endl;
            //std::cerr << "ele: "<<n_ele<<"  pi+ "<<n_piplus<<"  K+"<<n_Kplus << std::endl;
        }
        //raise(SIGINT);
    } else if (n_ele > 0 and n_muo == 0) {
        decay_mode = TString::Format("%iEle", n_ele);
        decay_mode_id = 0;
    } else if (n_muo > 0 and (n_piplus > 0 or n_Kplus > 0)) {
        if(n_muo==2){
            //found a Z!!
            n_muo=0;
        }
        decay_mode = TString::Format("%iMuo%iPi%iK", n_muo,n_piplus,n_Kplus);
        decay_mode_id = 14;
        //std::cerr << "Sanity check failed!" << std::endl;
        //std::cerr << "Found muon and hadrons as tau decay products" << std::endl;
        //std::cerr << "muon: "<<n_muo<<"  pi+ "<<n_piplus<<"  K+"<<n_Kplus << std::endl;


        ////if(final_state_part_list->size()==0){
        //printEvent();
        //cout<<"---------------------begin---------------------------"<<endl;
        //for (std::set< pxl::Relative* >::const_iterator part_it = truth_tau->getDaughters().begin(); part_it != truth_tau->getDaughters().end(); ++part_it) {
            //pxl::Relative *part_i = *part_it;
            //pxl::Particle* part = (pxl::Particle*)part_i;

            //cout<<part->getPdgNumber()<<"    "<< part->getDaughters().size() <<endl;
            //for (std::set< pxl::Relative* >::const_iterator part_it2 = part->getDaughters().begin(); part_it2 != part->getDaughters().end(); ++part_it2) {
                //pxl::Relative *part_i2 = *part_it2;
                //pxl::Particle* part2 = (pxl::Particle*)part_i2;

                //cout<<"sub "<<part2->getPdgNumber()<<"    "<< part2->getDaughters().size() <<endl;

            //}

        //}
        //cout<<"----------------------end--------------------------"<<endl;
    //}
    } else if (n_muo > 0 and n_ele == 0) {
        decay_mode = TString::Format("%iMuo", n_muo);
        decay_mode_id = 1;
    } else if (n_muo > 0 and n_ele > 0) {
        decay_mode = TString::Format("%iEle%iMuo", n_ele, n_muo);
        decay_mode_id = 14;
    } else {
        TString pi_zero_part = "";
        if (n_pizero > 0) {
            pi_zero_part = TString::Format("%iPi0", n_pizero);
        }
        TString pi_plus_part = "";
        if (n_piplus > 0) {
            pi_plus_part = TString::Format("%iPi", n_piplus);
        }

        TString K_zero_part = "";
        if (n_Kzero > 0) {
            K_zero_part = TString::Format("%iK0", n_Kzero);
        }
        TString K_plus_part = "";
        if (n_Kplus > 0) {
            K_plus_part = TString::Format("%iK", n_Kplus);
        }
        decay_mode = pi_plus_part + pi_zero_part + K_zero_part + K_plus_part;

        int strips=n_pizero + n_Kzero;
        if (n_piplus + n_Kplus == 1) {
            if(strips==0) decay_mode_id = 2;
            else if(strips==1) decay_mode_id = 3;
            else if(strips==2)  decay_mode_id = 4;
            else decay_mode_id = 5;
        }
        else if (n_piplus + n_Kplus == 3) {
            if(strips==0) decay_mode_id = 6;
            else if(strips==1) decay_mode_id = 7;
            else if(strips==2) decay_mode_id = 8;
            else decay_mode_id = 9;
        }
        else {
            if(strips==0) decay_mode_id = 10;
            else if(strips==1) decay_mode_id = 11;
            else if(strips==2) decay_mode_id = 12;
            else decay_mode_id = 13;
        }



    }

    /// decay mode ids:
    /// 0 xEle
    /// 1 xMuo
    /// 2 1Pi0Pi0
    /// 3 1Pi1Pi0
    /// 4 1Pi2Pi0
    /// 5 1Pi>2Pi0
    /// 6 3Pi0Pi0
    /// 7 3Pi1Pi0
    /// 8 3Pi2Pi0
    /// 9 3Pi>2Pi0
    /// 10 >3Pi0Pi0
    /// 11 >3Pi1Pi0
    /// 12 >3Pi2Pi0
    /// 13 >3Pi>2Pi0
    /// else

    delete final_state_part_list;
    delete new_temp_part;
    delete temp_part;

    vis_tau_decay->setUserRecord("n_prong",  n_prong);
    vis_tau_decay->setUserRecord("decay_mode",  (std::string)decay_mode);
    vis_tau_decay->setUserRecord("decay_mode_id",  decay_mode_id);
    //if (n_muo > 0 and (n_piplus > 0 or n_Kplus > 0)){
    //if (n_piplus + n_Kplus == 1){
        //cout<<(std::string)decay_mode<< "  "<<decay_mode_id<<   "  : "<<n_prong<<"  : "<<vis_tau_decay->getPt()<<"  "<<truth_tau->getPt()<<" : "<< n_pizero <<"  "<< n_Kzero<<"  "<<  n_pizero+n_Kzero <<"   "<<n_piplus <<"   "<< n_Kplus<<endl;

    //}
    tau_gen_visMap[truth_tau]=vis_tau_decay;
    return vis_tau_decay;
}

pxl::Particle* specialAna::Get_highest_pt_dauter( pxl::Particle* truth_tau) {
    pxl::Particle* temp_part_dummy=truth_tau;
    double highestpt=0;

    for (std::set< pxl::Relative* >::const_iterator part_it = truth_tau->getDaughters().begin(); part_it != truth_tau->getDaughters().end(); ++part_it) {
        pxl::Relative *part_i = *part_it;
        pxl::Particle* part = (pxl::Particle*)part_i;
        if(part->getDaughters().size()>0 ){
            part=Get_highest_pt_dauter(part);
        }
        if(part->getPt()>highestpt && part->getPdgNumber() != 111
                                && abs(part->getPdgNumber()) != 130
                                && abs(part->getPdgNumber()) != 310
                                && abs(part->getPdgNumber()) != 311
                                && abs(part->getPdgNumber()) != 22
                                ){
            temp_part_dummy=part;
            highestpt=part->getPt();
        }
    }
    return temp_part_dummy;
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


double specialAna::DeltaR(pxl::Particle* part_j, pxl::Particle* part_i) {
    return part_j->getVector().deltaR(part_i->getVector());
}


double specialAna::MT(pxl::Particle* lepton, pxl::Particle* met) {
    double mm = 2 * lepton->getPt() * met->getPt() * ( 1. - cos(lepton->getPhi() - met->getPhi()) );
    return sqrt(mm);
}


double specialAna::Mass(pxl::Particle* p1, pxl::Particle* p2) {
    return (p1->getVector()+p2->getVector()).getMass();
}

double specialAna::getPtHat(){
    double pthat=0;
    pxl::Particle* w=0;
    pxl::Particle* lepton=0;
    for(uint i = 0; i < S3ListGen->size(); i++){
        //this method treats all Bosons as W
        if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24 or TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
            w=S3ListGen->at(i);
        }
        //take the neutrio to avoid showering and so on!!
        if((TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 11 || TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 13 || TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 15) && lepton==0){
            lepton=S3ListGen->at(i);
        }
        if(w!=0 && lepton!=0){
            break;
        }
    }

    if(w!=0 && lepton!=0){
        //boost in the w restframe
        lepton->boost( -(w->getBoostVector()) );
        pthat=lepton->getPt();
        // get the original lepton back!!
        lepton->boost(w->getBoostVector());
    }else{
        pthat=-1;
    }
    return pthat;
}

double specialAna::getPtHatDi(){
    double pthat=0;
    pxl::Particle* w1=0;
    pxl::Particle* w2=0;
    for(uint i = 0; i < S3ListGen->size(); i++){
        //this method treats all Bosons as W
        if(TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 24 or TMath::Abs(S3ListGen->at(i)->getPdgNumber()) == 23 ){
            if (w1==0){
                w1=S3ListGen->at(i);
            }else{
                w2=S3ListGen->at(i);
            }
        }

        if(w1!=0 && w2!=0){
            break;
        }
    }

    if(w1!=0 && w2!=0){
        //boost in the w restframe
        pxl::Particle* virtpart= dynamic_cast< pxl::Particle* >(w1->clone());
        virtpart->addP4(w2);
        w2->boost( -(virtpart->getBoostVector()) );
        pthat=w2->getPt();
        // get the original lepton back!!
        w2->boost(virtpart->getBoostVector());
        delete virtpart;
    }else{
        pthat=-1;
    }
    return pthat;
}

double specialAna::getGenHT(){
    double ht=0;
    //cout<<"------------------------------------------------"<<endl;
    for(uint i = 0; i < S3ListGen->size(); i++){
        if( S3ListGen->at(i)->numberOfDaughters()==0){
            ht+=S3ListGen->at(i)->getPt();
        }
    }
    //cout<<"------------------------------------------------"<<endl;
    //for(uint i = 0; i < JetListGen->size(); i++){

        //ht+=JetListGen->at(i)->getPt();
        ////if(S3ListGen->at(i)->hasUserRecord("id") &&S3ListGen->at(i)->getPdgNumber()==0){
            ////S3ListGen->at(i)->setPdgNumber(S3ListGen->at(i)->getUserRecord("id"));
        ////}
        ////if(abs(S3ListGen->at(i)->getPdgNumber())>16 and S3ListGen->at(i)->numberOfDaughters()==0){
            ////ht+=S3ListGen->at(i)->getPt();
        ////}
    //}
    return ht;
}

double specialAna::getWmass(){
    if(wmass_stored!=0){
        return wmass_stored;
    }
    pxl::Particle* lepton=0;
    pxl::Particle* neutrino=0;
    for(uint i = 0; i < S3ListGen->size(); i++){
        if (abs(S3ListGen->at(i)->getPdgNumber())==24 and S3ListGen->at(i)->getMass()>5){
            wmass_stored=S3ListGen->at(i)->getMass();
            //cout<<wmass_stored<<"   "<<endl;
            return wmass_stored;
        }
        int pdgCode= abs(S3ListGen->at(i)->getPdgNumber());
        if((pdgCode==11 or pdgCode==13 or pdgCode==15) and lepton==0){
            lepton=S3ListGen->at(i);
        }
        if((pdgCode==12 or pdgCode==14 or pdgCode==16) and neutrino==0){
            neutrino=S3ListGen->at(i);
        }
    }
    if(neutrino!=0 and lepton!=0){
        wmass_stored=Mass(neutrino,lepton);
        return wmass_stored;
    }else{
        wmass_stored=-1;
        return wmass_stored;
    }
    wmass_stored=-1;
    return wmass_stored;
}

double specialAna::getZmass(){
    if(zmass_stored!=0){
        return zmass_stored;
    }
    pxl::Particle* lepton=0;
    pxl::Particle* neutrino=0;
    for(uint i = 0; i < S3ListGen->size(); i++){
        if (abs(S3ListGen->at(i)->getPdgNumber())==23 and S3ListGen->at(i)->getMass()>5){
            zmass_stored=S3ListGen->at(i)->getMass();
            return zmass_stored;
        }
        int pdgCode= abs(S3ListGen->at(i)->getPdgNumber());
        if((pdgCode==11 or pdgCode==13 or pdgCode==15) and lepton==0){
            lepton=S3ListGen->at(i);
        }
        if((pdgCode==11 or pdgCode==13 or pdgCode==15) and neutrino==0 and lepton!=0){
            neutrino=S3ListGen->at(i);
        }
    }
    if(neutrino!=0 and lepton!=0){
        zmass_stored=Mass(neutrino,lepton);
        return zmass_stored;
    }else{
        zmass_stored=-1;
        return zmass_stored;
    }
    zmass_stored=-1;
    return zmass_stored;
}

double specialAna::getInvMtt(){
    // if present, get ttbar invariant mass of event
    if(mtt_stored != 0) {
        return mtt_stored;
    }

    pxl::Particle* sel_t    = 0;
    pxl::Particle* sel_tbar = 0;

    for(uint i = 0; i < S3ListGen->size(); i++){
        int pdgCode = S3ListGen->at(i)->getPdgNumber();
        if (pdgCode==6  and not sel_t)    sel_t =    S3ListGen->at(i);
        if (pdgCode==-6 and not sel_tbar) sel_tbar = S3ListGen->at(i);
    }

    if(sel_t and sel_tbar){
        // invariant mass of ttbar system
        mtt_stored = sqrt(pow(sel_t->getMass(),2) + pow(sel_tbar->getMass(),2)
                     + 2*sel_t->getE()*sel_tbar->getE()
                     - 2*(sel_t->getPx()*sel_tbar->getPx() + sel_t->getPy()*sel_tbar->getPy() + sel_t->getPz()*sel_tbar->getPz())
                     );
    }

    return mtt_stored;
}

int specialAna::vetoNumber(vector< pxl::Particle* > *list, double ptTreshold){
    //make veto numbers
    //we don't need std::vectors, do we?
    int numVeto=0;
    bool passedID=false;
    for( std::vector< pxl::Particle* >::const_iterator part_it = list->begin(); part_it != list->end(); ++part_it ) {
        passedID=false;
        if((*part_it)->hasUserRecord("IDpassed")){
            passedID=(*part_it)->getUserRecord("IDpassed");
        }else{
            passedID=true;
        }

        if( (*part_it)->getPt()>ptTreshold && passedID ){
            numVeto++;
        }else if((*part_it)->getPt()<ptTreshold) {
            //Lists are Pt sorted
            break;
        }
    }
    return numVeto;
}
int specialAna::vetoNumberTau(vector< pxl::Particle* > *list, double ptTreshold){
    //make veto numbers
    //we don't need std::vectors, do we?
    int numVeto=0;
    bool passedID=false;
    for( std::vector< pxl::Particle* >::const_iterator part_it = list->begin(); part_it != list->end(); ++part_it ) {
        passedID=false;
        passedID=Check_Tau_ID(*part_it);
        if( (*part_it)->getPt()>ptTreshold && passedID ){
            numVeto++;
        }else if((*part_it)->getPt()<ptTreshold) {
            //Lists are Pt sorted
            break;
        }
    }
    return numVeto;
}

void specialAna::printEvent(){
    map< string,int > pdgForPlotting;
    pdgForPlotting["Ele"]=11;
    pdgForPlotting["Muon"]=13;
    pdgForPlotting[m_TauType]=15;
    pdgForPlotting["Gamma"]=22;
    pdgForPlotting[m_METType]=0;
    pdgForPlotting[m_JetAlgo]=1;

    std::vector< pxl::Particle* > AllParticles;
    m_RecEvtView->getObjectsOfType< pxl::Particle >( AllParticles );
    pxl::sortParticles( AllParticles );
    eventsAfterCutsEvents<<"<event>\n";
    eventsAfterCutsEvents<<"#"<< temp_run << ":"
                            << temp_ls << ":"
                            << temp_event <<"\n";
    // push them into the corresponding std::vectors
    for( std::vector< pxl::Particle* >::const_iterator part_it = AllParticles.begin(); part_it != AllParticles.end(); ++part_it ) {
        pxl::Particle *part = *part_it;
        string Name = part->getName();
        // Only fill the collection if we want to use the particle!
        if(pdgForPlotting.find(Name)!=pdgForPlotting.end()){
            if (part->hasUserRecord("IDpassed")){
                if(! part->getUserRecord("IDpassed")){
                    continue;
                }
            }
            eventsAfterCutsEvents<<pdgForPlotting[Name]<<" "<<part->getPx()<<" "<<part->getPy()<<" "<<part->getPz()<<" "<<part->getE()<<"\n";
        }
    }
    eventsAfterCutsEvents<<"</event>\n";

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
    //        HistClass::CreateHisto(7,"ptoverMET",particleName[i].c_str(), 20, 0, 2,       TString::Format("#frac{p_{T}^{%s}}{MET}", particleLatex[i].c_str()) );

    cout << "n_allTaus:   " << numAllTaus << endl;
    cout << "n_lepton:   " << n_lepton << endl;
    cout << "h_counters: " << HistClass::ReturnHist("h_counters")->GetBinContent(1) << endl;
    cout << "efficiency: " << n_lepton / (HistClass::ReturnHist("h_counters")->GetBinContent(1)) << endl;

    cout << "h_counters: " << HistClass::ReturnHist("h_counters")->GetBinContent(1) << endl;
    file1->cd();
    HistClass::WriteAll("counters");
    //if(not runOnData){
    file1->mkdir("MC");
    file1->cd("MC/");
    HistClass::WriteAll("_Gen");
    //}
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
    file1->mkdir("Other");
    file1->cd("Other/");
    HistClass::WriteAll("_general_");
    file1->cd();
    file1->mkdir("Trees");
    file1->cd("Trees/");
    HistClass::WriteAllTrees();
    file1->cd();
    file1->mkdir("nDim");
    file1->cd("nDim");
    HistClass::WriteN();
    file1->cd();
    file1->mkdir("2Dim");
    file1->cd("2Dim");
    HistClass::WriteAll2();
    file1->cd();
    file1->mkdir("Effs");
    file1->cd("Effs/");
    HistClass::WriteAllEff();
    HistClass::WriteAllProf();
    file1->Close();

    std::string outputstring = eventsAfterCuts.str();
    eventDisplayFile << outputstring;
    outputstring = eventsAfterCutsEvents.str();
    eventDisplayFile << outputstring;
    eventDisplayFile.close();
    delete file1;
}

void specialAna::initEvent( const pxl::Event* event ){
    HistClass::Fill("h_counters", 1, 1); // increment number of events
    events_++;

    //no pu weight at the moment!!

    weight = 1;
    qcd_weight=1;
    wmass_stored=0;
    mtt_stored=0;
    highEtTriggStored=0;
    m_RecEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Rec" );
    m_GenEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Gen" );
    if(event->getObjectOwner().findObject< pxl::EventView >( "Trig" )){
        m_TrigEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Trig" );
    }else{
        m_TrigEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Rec" );
    }
    if(event->getObjectOwner().findObject< pxl::EventView >( "Filter" )){
        m_FilterEvtView = event->getObjectOwner().findObject< pxl::EventView >( "Filter" );
    }else{
        m_FilterEvtView=0;
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

    EleList   = new std::vector< pxl::Particle* >;
    MuonList  = new std::vector< pxl::Particle* >;
    GammaList = new std::vector< pxl::Particle* >;
    METList   = new std::vector< pxl::Particle* >;
    JetList   = new std::vector< pxl::Particle* >;
    TauList   = new std::vector< pxl::Particle* >;


    // get all particles
    std::vector< pxl::Particle* > AllParticles;
    m_RecEvtView->getObjectsOfType< pxl::Particle >( AllParticles );
    pxl::sortParticles( AllParticles );
    // push them into the corresponding std::vectors
    for( std::vector< pxl::Particle* >::const_iterator part_it = AllParticles.begin(); part_it != AllParticles.end(); ++part_it ) {
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


    if(METList->size()>0){
        sel_met=METList->at(0);
    }else{
        cout<<"size "<<METList->size()<<":"
                            << event->getUserRecord("Run") << ":"
                            << event->getUserRecord("LumiSection") << ":"
                            << event->getUserRecord("EventNum") << endl;
        for( std::vector< pxl::Particle* >::const_iterator part_it = AllParticles.begin(); part_it != AllParticles.end(); ++part_it ) {
            cout<<(*part_it)->getName()<<endl;
        }

        throw std::runtime_error("specialAna.cc: no MET '" + m_METType +"' found!");
        sel_met=0;
    }

    for(vector< pxl::Particle* >::const_iterator part_it = TauList->begin(); part_it != TauList->end(); ++part_it ) {
        TLorentzVector* jet = new TLorentzVector();
        jet->SetPxPyPzE((*part_it)->getUserRecord("tauJetpx"),(*part_it)->getUserRecord("tauJetpy"),(*part_it)->getUserRecord("tauJetpz"),(*part_it)->getUserRecord("tauJetE"));
        (*part_it)->setUserRecord("tauJetpt",jet->Pt());
        (*part_it)->setUserRecord("tauJeteta",jet->Eta());
        (*part_it)->setUserRecord("tauJetphi",jet->Phi());
        delete jet;

    }


    if(TauList->size()>0){
        numAllTaus++;
    }


    sel_lepton=0;
    qcd_lepton=0;

    //     h1_num_Taus.Fill(TauList->size());
    EleListGen     = new std::vector< pxl::Particle* >;
    MuonListGen    = new std::vector< pxl::Particle* >;
    GammaListGen   = new std::vector< pxl::Particle* >;
    METListGen     = new std::vector< pxl::Particle* >;
    JetListGen     = new std::vector< pxl::Particle* >;
    TauListGen     = new std::vector< pxl::Particle* >;
    TauVisListGen  = new std::vector< pxl::Particle* >;
    S3ListGen      = new std::vector< pxl::Particle* >;

    if( not runOnData ){

        double event_weight = m_GenEvtView->getUserRecord( "Weight" );
        //double varKfactor_weight = m_GenEvtView->getUserRecord_def( "kfacWeight",1. );
        double pileup_weight = m_GenEvtView->getUserRecord_def( "PUWeight",1.);

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
        std::vector< pxl::Particle* > AllParticlesGen;
        m_GenEvtView->getObjectsOfType< pxl::Particle >( AllParticlesGen );
        pxl::sortParticles( AllParticlesGen );
        // push them into the corresponding std::vectors
        string genCollection="gen";
        if(m_dataPeriod=="8TeV"){
            genCollection="S3";
        }
        for( std::vector< pxl::Particle* >::const_iterator part_it = AllParticlesGen.begin(); part_it != AllParticlesGen.end(); ++part_it ) {
            pxl::Particle *part = *part_it;
            string Name = part->getName();
            // Only fill the collection if we want to use the particle!
            if(      Name == "Muon"    ) MuonListGen->push_back( part );
            else if( Name == "Ele"     ) EleListGen->push_back( part );
            else if( Name == "Gamma"   ) GammaListGen->push_back( part );
            else if( Name == "Tau"     ) TauListGen->push_back( part );
            else if( Name == (m_METType+"_gen") ) METListGen->push_back( part );
            else if( Name == m_JetAlgo ) JetListGen->push_back( part );
            else if( Name == genCollection) {
                S3ListGen->push_back( part );
                if( abs(part->getPdgNumber())==15){
                        TauVisListGen->push_back(Get_tau_truth_decay_mode(part));
                }
            }
        }
        //for( std::vector< pxl::Particle* >::const_iterator part_it = TauListGen->begin(); part_it != TauListGen->end(); ++part_it ) {
                //pxl::Particle* tmp=(pxl::Particle*) (*part_it)->getSoftRelations().getFirst(m_GenEvtView->getObjectOwner(), "genParticle");
                //if(tmp){
                    //TauVisListGen->push_back(Get_tau_truth_decay_mode(tmp));
                //}else{
                    //cout<<(*part_it)->getSoftRelations().size()<<endl;
                //}
                //delete tmp;
        //}

    }
}




void specialAna::applyKfactor(const pxl::Event* event , int mode){
    if( not (mode==1 || mode==0) ){
        throw std::runtime_error("specialAna.cc: The k-faktor must be additive (mode=0) or multiplicative (mode=1) yours is "+std::to_string(mode));
    }
    string datastream = event->getUserRecord( "Dataset" );
    TString Datastream = datastream;

    if( (datastream.find("WTo")!=1) and (datastream.find("WJetsToLNu_")!=1) ){
        return;
    }


    if( m_dataPeriod=="13TeV" ){
        double mass=getWmass();
        double k_faktor=1.;
        if(mass>0){
            k_faktor=m_kfactorHist[mode]->GetBinContent(m_kfactorHist[mode]->FindBin(mass));
        }
        if (k_faktor<0){
            k_faktor=1.;
        }
        weight*=k_faktor;

    }else if(m_dataPeriod=="8TeV"){
        double mass=0.;
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
            mass=Mass(S3ListGen->at(l), S3ListGen->at(nu));
        }
        if(mass!=0){
            weight*=m_kfactorHist[mode]->GetBinContent(m_kfactorHist[mode]->FindBin(mass));
        }


        //if(Datastream.Contains("WJets") ) {
            ////mg additive
            //double par []={1.42167 ,0.00036744,6.77295e-8};
            ////mg multiply
            //if(mode==1){
                //par[0]= 1.402;
                //par[1]= -0.000435535;
                //par[2]= 7.43225e-8;
            //}

            //int l=-1;
            //int nu=-1;

            //for(uint i = 0; i < S3ListGen->size(); i++){
                //int tmpid= TMath::Abs(S3ListGen->at(i)->getPdgNumber());
                //if(tmpid == 11) l=i;
                //if(tmpid == 13) l=i;
                //if(tmpid == 15) l=i;

                //if(tmpid == 12) nu=i;
                //if(tmpid == 14) nu=i;
                //if(tmpid == 16) nu=i;
            //}
            //if(l>=0 && nu>=0){
                //mt=MT(S3ListGen->at(l), S3ListGen->at(nu));
            //}
            //if(mt!=0){
                //weight*=(par[0]+mt*par[1]+mt*mt*par[2]);
            //}
        //}
        //if(Datastream.Contains("WTo") ) {
            ////pythia additive
            //double par []={1.42167 ,0.00036744,6.77295e-8};
            ////pythia multiply
            //if(mode==1){
                //par[0]= 1.402;
                //par[1]= -0.000435535;
                //par[2]= 7.43225e-8;
            //}
            //int l=-1;
            //int nu=-1;
            //for(uint i = 0; i < S3ListGen->size(); i++){
                //int tmpid= TMath::Abs(S3ListGen->at(i)->getPdgNumber());
                //if(tmpid == 11) l=i;
                //if(tmpid == 13) l=i;
                //if(tmpid == 15) l=i;

                //if(tmpid == 12) nu=i;
                //if(tmpid == 14) nu=i;
                //if(tmpid == 16) nu=i;
            //}
            //if(l>=0 && nu>=0){
                //mt=MT(S3ListGen->at(l), S3ListGen->at(nu));
            //}
            //if(mt!=0){
                //weight*=(par[0]+mt*par[1]+mt*mt*par[2]);
            //}
        //}
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
        }else if(sel_lepton && sel_lepton->getPdgNumber()==11){
            /*
             * CB
             * https://twiki.cern.ch/twiki/bin/view/Main/EGammaScaleFactors2012?rev=28
            Tight
            pT 	10 - 15 	15 - 20 	20 - 30 	30 - 40 	40 - 50 	50 - 200
            0.0 < abs(η) < 0.8 	0.827 + 0.021- 0.021 	0.924 + 0.010- 0.010 	0.960 + 0.003- 0.003 	0.978 + 0.001- 0.001 	0.981 + 0.001- 0.001 	0.982 + 0.002- 0.002
            0.8 < abs(η) < 1.442 	0.948 + 0.024- 0.023 	0.932 + 0.012- 0.012 	0.936 + 0.004- 0.004 	0.958 + 0.002- 0.002 	0.969 + 0.001- 0.001 	0.969 + 0.002- 0.002
            1.442 < abs(η) < 1.556 	1.073 + 0.117- 0.107 	0.808 + 0.045- 0.042 	0.933 + 0.015- 0.017 	0.907 + 0.008- 0.008 	0.904 + 0.004- 0.004 	0.926 + 0.011- 0.011
            1.556 < abs(η) < 2.0 	0.854 + 0.048- 0.047 	0.853 + 0.022- 0.022 	0.879 + 0.007- 0.007 	0.909 + 0.003- 0.003 	0.942 + 0.002- 0.002 	0.957 + 0.004- 0.004
            2.0 < abs(η) < 2.5 	1.007 + 0.047- 0.046 	0.903 + 0.029- 0.029 	0.974 + 0.004- 0.004 	0.987 + 0.004- 0.004 	0.991 + 0.003- 0.003 	0.999 + 0.005- 0.005
            */
            /*
             * HEEP
             * https://twiki.cern.ch/twiki/bin/view/CMS/HEEPEfficiencies?rev=21
            Et > 35 	0.997 ± 0.000 (stat.) ± 0.007 (syst.) 	0.979 ± 0.000 (stat.) ± 0.006 (syst.)
            Et > 100 	0.985 ± 0.002 (stat.) ± 0.014 (syst.) 	0.981 ± 0.006 (stat.) ± 0.004 (syst.)
            */
            //HEEP
            if( sel_lepton->getPt()>100){
                if(sel_lepton->hasUserRecord("isBarrel")){
                    sf=0.985;
                }else{
                    sf=0.981;
                }
            }
            //CB
            else{
                if(fabs(sel_lepton->getEta())>=0.0  && fabs(sel_lepton->getEta())<0.8 ){
                    if(sel_lepton->getPt()>10 and sel_lepton->getPt()<=15){
                        sf=0.8;
                    }else if(sel_lepton->getPt()>15 and sel_lepton->getPt()<=20){
                        sf=	0.924;
                    }else if(sel_lepton->getPt()>20 and sel_lepton->getPt()<=30){
                        sf=	0.978;
                    }else if(sel_lepton->getPt()>30 and sel_lepton->getPt()<=40){
                        sf=	 0.978;
                    }else if(sel_lepton->getPt()>40 and sel_lepton->getPt()<=50){
                        sf=	 0.981;
                    }else if(sel_lepton->getPt()>50 and sel_lepton->getPt()<=200){
                        sf=	 0.982;
                    }
                }else if(fabs(sel_lepton->getEta())>=0.8  && fabs(sel_lepton->getEta())<1.442 ){
                    if(sel_lepton->getPt()>10 and sel_lepton->getPt()<=15){
                        sf=0.948;
                    }else if(sel_lepton->getPt()>15 and sel_lepton->getPt()<=20){
                        sf=	0.932;
                    }else if(sel_lepton->getPt()>20 and sel_lepton->getPt()<=30){
                        sf=	0.936;
                    }else if(sel_lepton->getPt()>30 and sel_lepton->getPt()<=40){
                        sf=	 0.958;
                    }else if(sel_lepton->getPt()>40 and sel_lepton->getPt()<=50){
                        sf=	 0.969;
                    }else if(sel_lepton->getPt()>50 and sel_lepton->getPt()<=200){
                        sf=	 0.969;
                    }
                }else if(fabs(sel_lepton->getEta())>=1.442  && fabs(sel_lepton->getEta())<1.556){
                    if(sel_lepton->getPt()>10 and sel_lepton->getPt()<=15){
                        sf=1.073;
                    }else if(sel_lepton->getPt()>15 and sel_lepton->getPt()<=20){
                        sf=	0.808;
                    }else if(sel_lepton->getPt()>20 and sel_lepton->getPt()<=30){
                        sf=	0.933;
                    }else if(sel_lepton->getPt()>30 and sel_lepton->getPt()<=40){
                        sf=	 0.907 ;
                    }else if(sel_lepton->getPt()>40 and sel_lepton->getPt()<=50){
                        sf=	 0.904;
                    }else if(sel_lepton->getPt()>50 and sel_lepton->getPt()<=200){
                        sf=	 0.926;
                    }
                }else if(fabs(sel_lepton->getEta())>=1.556  && fabs(sel_lepton->getEta())<2.0){
                    if(sel_lepton->getPt()>10 and sel_lepton->getPt()<=15){
                        sf=0.854;
                    }else if(sel_lepton->getPt()>15 and sel_lepton->getPt()<=20){
                        sf=	0.853;
                    }else if(sel_lepton->getPt()>20 and sel_lepton->getPt()<=30){
                        sf=	0.879;
                    }else if(sel_lepton->getPt()>30 and sel_lepton->getPt()<=40){
                        sf=	 0.909 ;
                    }else if(sel_lepton->getPt()>40 and sel_lepton->getPt()<=50){
                        sf=	 0.942;
                    }else if(sel_lepton->getPt()>50 and sel_lepton->getPt()<=200){
                        sf=	 0.957;
                    }
                }else if(fabs(sel_lepton->getEta())>=2.  && fabs(sel_lepton->getEta())<2.5){
                    if(sel_lepton->getPt()>10 and sel_lepton->getPt()<=15){
                        sf=1.007;
                    }else if(sel_lepton->getPt()>15 and sel_lepton->getPt()<=20){
                        sf=	0.903;
                    }else if(sel_lepton->getPt()>20 and sel_lepton->getPt()<=30){
                        sf=	0.974;
                    }else if(sel_lepton->getPt()>30 and sel_lepton->getPt()<=40){
                        sf=	0.987 ;
                    }else if(sel_lepton->getPt()>40 and sel_lepton->getPt()<=50){
                        sf=	 0.991;
                    }else if(sel_lepton->getPt()>50 and sel_lepton->getPt()<=200){
                        sf=	 0.999;
                    }
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
    QCDLeptonList.clear();

    if( not runOnData ){
        for(auto it = TauVisListGen->begin();it!=TauVisListGen->end();){
                delete *it;
                it = TauVisListGen->erase(it);
        }
        tau_gen_visMap.clear();

        delete EleListGen;
        delete MuonListGen;
        delete GammaListGen;
        delete METListGen;
        delete JetListGen;
        delete TauListGen;
        delete TauVisListGen;
        delete S3ListGen;

        EleListGen = 0;
        MuonListGen = 0;
        GammaListGen = 0;
        METListGen = 0;
        JetListGen = 0;
        TauListGen = 0;
        TauVisListGen = 0;
        S3ListGen = 0;
    }
}

//~ void specialAna::SetEvents(int e){
    //~ events_=e;
//~ }

