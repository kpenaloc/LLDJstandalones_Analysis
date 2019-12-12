#include <iostream>
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TLegend.h"
#include "TPad.h"
#include "TStyle.h"
#include "TText.h"
#include <fstream>
#include "TFile.h"
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <cstdlib> /* mkdir */
#include <stdlib.h>     /* getenv */

using namespace std;

void taggerSlim(Double_t c_ip, Double_t c_ta, TString year, TString sample/*, Int_t ntags, TString lifetime, TString mass*/){

  Double_t c_al;
  Double_t c_Zpt;
  Double_t Z_Sf;
  //cout <<"(c_Zpt, c_ip, c_ta, c_al)=  "<<"("<<c_Zpt<<","<<c_ip<<","<<c_ta<<","<<c_al<<","<<")"<<endl;
  //cout <<"sample: "<<year+"_"+sample<<endl;
  
  TString nt, s_c_ip, s_c_ta, s_c_al, s_c_Zpt;
  float tags;
 
  s_c_ip.Form("%1.2f",c_ip);
  s_c_ta.Form("%1.2f",c_ta);
  double a_c_al[11] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
  double a_c_Zpt[6] = {50.0, 60.0, 70.0, 80.0, 90.0, 100.0};
  
  double MCSF = 1.0;

  // lumi 2016: 16226.2
  // lumi 2017: 42500.
  // lumi 2018: 58670.

  // DYSF 2016: 0.8546545
  // DYSF 2017: 0.780398
  // DYSF 2018: .7323

  // TTJets_amx SF(2018): 1.12778
 
  if (sample.Contains("HToSSTobbbb") ) MCSF *= 117396.2/16226.2; // Extend to Full Lumi
  if (year == "2016" ) MCSF *= 1./0.8546545; // Remove overallDY SF 2016
  if (year == "2017" ) MCSF *= 1./0.780398; // Remove overallDY SF 2017
  if (year == "2018" ) MCSF *= 1./.7323; // Remove overallDY SF 2018
  if (year == "2018" && sample.Contains("TTJets") ) MCSF *= 1.12778; // apply TTJets SF 2018
  if (year == "2016" && sample.Contains("DYJetsToLL") ) MCSF *= 0.8546545;    // Re-apply DY SF 2016
  if (year == "2017" && sample.Contains("DYJetsToLL") ) MCSF *= 0.780398;     // Re-apply DY SF 2017
  if (year == "2018" && sample.Contains("DYJetsToLL") ) MCSF *= .7323;        // Re-apply DY SF 2018

  cout <<"samples: "<<sample <<"   MCSF: "<<MCSF <<endl;

  int Nbins = 6;
  TH1F* h_ntags      = new TH1F("h_ntags"      , "h_ntags"      , Nbins, -0.5, (float)(Nbins)-0.5);
  TH1F* h_ntags_base = new TH1F("h_ntags_base" , "h_ntags_base" , Nbins, -0.5, (float)(Nbins)-0.5);
  h_ntags->Sumw2();

  TString Path;
  if     (year=="2016") Path = "root://cmsxrootd.fnal.gov///store/user/ddiaz/OPT/2016/";
  else if(year=="2017") Path = "root://cmsxrootd.fnal.gov///store/user/ddiaz/OPT/2017/";
  else if(year=="2018") Path = "root://cmsxrootd.fnal.gov///store/user/ddiaz/OPT/2018/";
  else cout <<"Bad Year"<<endl;

  TString sampleName= sample;
  if(sampleName.Length()>10) sampleName.Remove(sampleName.Length()-5, sampleName.Length());
  ofstream ResFile;
  //ResFile.open(year+"_"+sampleName+"_"+s_c_ip+"_"+s_c_ta+"_Results.txt",fstream::app);
  ResFile.open(year+"_"+sampleName+"_"+s_c_ip+"_"+s_c_ta+"_Results.txt");
  
  //----------
  //high_z_pt
  //----------
  vector<float> NTags          (Nbins,0);
  vector<float> NTagsError     (Nbins,0);
  vector<float> NTags_bare     (Nbins,0);
  vector<float> NTags_base     (Nbins,0);
  vector<float> NTags_baseError(Nbins,0);
  vector<float> NTags_ele      (Nbins,0);
  vector<float> NTags_eleError (Nbins,0);
  vector<float> NTags_PU       (Nbins,0);
  vector<float> NTags_PUError  (Nbins,0);
  //----------
  //low_z_pt
  //----------
  vector<float> NTags_low          (Nbins,0);
  vector<float> NTagsError_low     (Nbins,0);
  vector<float> NTags_bare_low     (Nbins,0);
  vector<float> NTags_base_low     (Nbins,0);
  vector<float> NTags_baseError_low(Nbins,0);
  vector<float> NTags_ele_low      (Nbins,0);
  vector<float> NTags_eleError_low (Nbins,0);
  vector<float> NTags_PU_low       (Nbins,0);
  vector<float> NTags_PUError_low  (Nbins,0);
      

  TString FileName = Path+sampleName+".root";
  TFile file;
  TFile* f2 = TFile::Open(FileName);
  
  for(int zz=0; zz<6; zz++){//loop Zpt
    c_Zpt = a_c_Zpt[zz]; 
    for(int jj=0; jj<11; jj++){//loopAlpha
      c_al = a_c_al[jj];
      
      double base_2p = 0.0, baseE_2p = 0.0, ele_2p = 0.0, eleE_2p = 0.0, PU_2p = 0.0, PUE_2p = 0.0, EW_2p = 0.0, EW_E2p = 0.0;
      double base_2p_low = 0.0, baseE_2p_low = 0.0, ele_2p_low = 0.0, eleE_2p_low = 0.0, EW_2p_low = 0.0, EW_E2p_low = 0.0;
      NTags          .assign(Nbins,0.0);
      NTagsError     .assign(Nbins,0.0);
      NTags_bare     .assign(Nbins,0.0);
      NTags_base     .assign(Nbins,0.0);
      NTags_baseError.assign(Nbins,0.0);
      NTags_ele      .assign(Nbins,0.0);
      NTags_eleError .assign(Nbins,0.0);
      NTags_PU       .assign(Nbins,0.0);
      NTags_PUError  .assign(Nbins,0.0);
      //low-z-pt
      NTags_low          .assign(Nbins,0.0);
      NTagsError_low     .assign(Nbins,0.0);
      NTags_bare_low     .assign(Nbins,0.0);
      NTags_base_low     .assign(Nbins,0.0);
      NTags_baseError_low.assign(Nbins,0.0);
      NTags_ele_low      .assign(Nbins,0.0);
      NTags_eleError_low .assign(Nbins,0.0);
      NTags_PU_low       .assign(Nbins,0.0);
      NTags_PUError_low  .assign(Nbins,0.0);
  
      h_ntags->Reset();
      h_ntags_base->Reset();
     
//      for(int ii=0; ii<NTags.size(); ii++){
//      cout <<"i= "<<ii<<endl;
//      cout << NTags          [ii]<<endl;
//      cout << NTagsError     [ii]<<endl;
//      cout << NTags_bare     [ii]<<endl;
//      cout << NTags_base     [ii]<<endl;
//      cout << NTags_baseError[ii]<<endl;
//      cout << NTags_ele      [ii]<<endl;
//      cout << NTags_eleError [ii]<<endl;
//      cout << NTags_PU       [ii]<<endl;
//      cout << NTags_PUError  [ii]<<endl;
//
//      }
      
      TTreeReader reader("OPTtree",f2);
      TTreeReaderValue<vector<int>>    Event(reader,       "OPT_Event"); // template type must match datatype
      TTreeReaderValue<vector<float>>  EventWeight(reader, "OPT_EventWeight"); // name must match branchname
      TTreeReaderValue<vector<float>>  ZPt(reader,         "OPT_ZPt"); 
      TTreeReaderValue<vector<float>>  ZSf(reader,         "OPT_ZSf"); 
      TTreeReaderValue<vector<float>>  base_weight(reader, "OPT_base_weight"); 
      TTreeReaderValue<vector<float>>  ele_weight(reader,  "OPT_ele_weight"); 
      TTreeReaderValue<vector<float>>  mu_weight(reader,   "OPT_mu_weight"); 
      TTreeReaderValue<vector<float>>  PU_weight(reader,   "OPT_PU_weight"); 
      TTreeReaderValue<vector<float>>  Pt(reader,          "OPT_AODCaloJetPt");
      TTreeReaderValue<vector<float>>  IP(reader,          "OPT_AODCaloJetMedianLog10IPSig");
      TTreeReaderValue<vector<float>>  TA(reader,          "OPT_AODCaloJetMedianLog10TrackAngle");
      TTreeReaderValue<vector<float>>  Alpha(reader,       "OPT_AODCaloJetAlphaMax");
      TTreeReaderValue<vector<float>>  Eta(reader,         "OPT_AODCaloJetEta");
      //std::cout << FileName << std::endl;
      int count = 0;
      int ntag_low_zpt;
      double sample_weight = 0.0;
      while (reader.Next()) {
        for(int i = 0; i<EventWeight->size(); i++){
          Z_Sf = ZSf->at(i);
          tags =0;
	  ntag_low_zpt = 0;
          for (int j=0; j<TA->size(); j++)
	    {
	      if(Alpha->at(j)<=c_al && IP->at(j)>=c_ip && TA->at(j)>=c_ta && Alpha->at(j)>=0.0 && ZPt->at(i)>=c_Zpt ) tags = tags + 1;
	      if(Alpha->at(j)<=c_al && IP->at(j)>=c_ip && TA->at(j)>=c_ta && Alpha->at(j)>=0.0 && ZPt->at(i) < c_Zpt ) ntag_low_zpt = ntag_low_zpt+1;
	    }
	  double weight          = MCSF*base_weight->at(i);//xsec weight;
	  double weight_eleSF    = weight*ele_weight->at(i);//xsec weight*eleSF;
	  double full_weight     = weight*ele_weight->at(i)*PU_weight->at(i);//full-weight;
	  if( i == 0 ) sample_weight = weight;

          if(tags<Nbins)
	    {
	      NTags_bare      [tags]   = NTags_bare[tags]      + 1; //count raw mc events
	      NTags           [tags]   = NTags[tags]           + full_weight;//full weight
	      NTagsError      [tags]   = NTagsError[tags]      + pow(full_weight,2.0);//full weight
	      NTags_base      [tags]   = NTags_base[tags]      + weight;//only xsec weight
	      NTags_baseError [tags]   = NTags_baseError[tags] + pow(weight,2.0);//only xsec weight
	      NTags_ele       [tags]   = NTags_ele[tags]       + weight_eleSF;//xsec*eleSF
	      NTags_eleError  [tags]   = NTags_eleError[tags]  + pow(weight_eleSF, 2.0);//xsec*eleSF
	      h_ntags_base->Fill(tags, MCSF*base_weight->at(i)); 
	    }

	  if(ntag_low_zpt<Nbins)
	    {
	      NTags_bare_low      [ntag_low_zpt]   = NTags_bare_low[ntag_low_zpt]      + 1; //count raw mc events
              NTags_low           [ntag_low_zpt]   = NTags_low[ntag_low_zpt]           + full_weight;//full weight
              NTagsError_low      [ntag_low_zpt]   = NTagsError_low[ntag_low_zpt]      + pow(full_weight,2.0);//full weight
              NTags_base_low      [ntag_low_zpt]   = NTags_base_low[ntag_low_zpt]      + weight;//only xsec weight
              NTags_baseError_low [ntag_low_zpt]   = NTags_baseError_low[ntag_low_zpt] + pow(weight,2.0);//only xsec weight
              NTags_ele_low       [ntag_low_zpt]   = NTags_ele_low[ntag_low_zpt]       + weight_eleSF;//xsec*eleSF
              NTags_eleError_low  [ntag_low_zpt]   = NTags_eleError_low[ntag_low_zpt]  + pow(weight_eleSF, 2.0);//xsec*eleSF
	    }
//      for(int ii=0; ii<1 /*NTags.size()*/; ii++){
//      cout <<"i= "<<ii<<endl;
//      cout <<"NTags            "<< NTags          [ii]<<"  __  " << MCSF*(base_weight->at(i)*ele_weight->at(i)*PU_weight->at(i))  <<endl;
//      cout <<"NTagsError       "<< NTagsError     [ii]<<"  __  " << sqrt((MCSF*MCSF*base_weight->at(i)*base_weight->at(i)) + (ele_weight->at(i)*ele_weight->at(i)) + (PU_weight->at(i)*PU_weight->at(i)))<<endl;
//      cout <<"NTags_bare       "<< NTags_bare     [ii]<<"  __  " << 1 <<endl;
//      cout <<"NTags_base       "<< NTags_base     [ii]<<"  __  " << MCSF*base_weight->at(i)                    <<endl;
//      cout <<"NTags_baseError  "<< NTags_baseError[ii]<<"  __  " << MCSF*MCSF*base_weight->at(i)*base_weight->at(i)  <<endl;
//      cout <<"NTags_ele        "<< NTags_ele      [ii]<<"  __  " << ele_weight->at(i)                     <<endl;
//      cout <<"NTags_eleError   "<< NTags_eleError [ii]<<"  __  " << ele_weight->at(i)*ele_weight->at(i)    <<endl;
//      cout <<"NTags_PU         "<< NTags_PU       [ii]<<"  __  " << PU_weight->at(i)                      <<endl;
//      cout <<"NTags_PUError    "<< NTags_PUError  [ii]<<"  __  " << PU_weight->at(i)*PU_weight->at(i)     <<endl;
//      }
//      cout << "count: "<< count<< endl;
//      cout<<FileName<<"  "<<c_Zpt<<"  "<<Z_Sf<<" "<<c_ip<<" "<<c_ta<<" "<<c_al<<" "<<NTags_bare[0]<<" "<<NTags_base[0]<<" "<<sqrt(NTags_baseError[0])<<" "
//                                                                                                                <<NTags_ele [0]<<" "<<sqrt(NTags_eleError [0])<<" "
//                                                                                                                <<NTags_PU  [0]<<" "<<sqrt(NTags_PUError  [0])<<" "
//                                                                                                                <<NTags     [0]<<" "<<NTagsError          [0] <<" "
//                                                                                                                <<NTags_base[1]<<" "<<sqrt(NTags_baseError[1])<<" "
//                                                                                                                <<NTags_ele [1]<<" "<<sqrt(NTags_eleError [1])<<" "
//                                                                                                                <<NTags_PU  [1]<<" "<<sqrt(NTags_PUError  [1])<<" "
//                                                                                                                <<NTags     [1]<<" "<<NTagsError          [1] <<" "
//                                                                                                                <<base_2p      <<" "<<sqrt(baseE_2p)          <<" "
//                                                                                                                <<ele_2p       <<" "<<sqrt(eleE_2p)           <<" "  
//                                                                                                                <<PU_2p        <<" "<<sqrt(PUE_2p)            <<" "
//                                                                                                                <<EW_2p        <<" "<<EW_E2p                  <<" "
//                                                                                                                <<endl;
        }
        
      count = count +1;
      //      if (count ==10) break;
      }
      for(int nn =0; nn<NTags_bare.size(); nn++){
	h_ntags->SetBinContent(nn+1,NTags_bare[nn]);
      }
      for (int l = 2; l<Nbins; l++){
        base_2p         += NTags_base[l];//xsec_only
        baseE_2p        += NTags_baseError[l];//xsec only
	base_2p_low     += NTags_base_low[l];//xsec_only
        baseE_2p_low    += NTags_baseError_low[l];//xsec only
	
  
        ele_2p          += NTags_ele[l];//xsec*eleSF
        eleE_2p         += NTags_eleError[l];//xsec*eleSF
	ele_2p_low      += NTags_ele[l];//xsec*eleSF
        eleE_2p_low     += NTags_eleError[l];//xsec*eleSF

        //PU_2p       += NTags_PU[l];    
        //PUE_2p      += (NTags_PUError[l]*NTags_PUError[l]);     

        EW_2p           += NTags[l];//full weight
        EW_E2p          += NTagsError[l];//full weight
	EW_2p_low       += NTags_low[l];//full weight
        EW_E2p_low      += NTagsError_low[l];//full weight
     }
  
      s_c_al.Form("%1.2f",c_al);
      s_c_Zpt.Form("%1.2f",c_Zpt);
     
  
      //-----------------
      //comput generic TF
      //-----------------
      double tf[3];
      double tf_err[3];
      for(int i_tf = 0; i_tf < 3; i_tf++)
	{
	  tf[i_tf] = NTags[i_tf]/NTags_low[i_tf];
	  tf_err[i_tf]   = sqrt(pow(tf[i_tf],2.0)*(NTagsError[i_tf]/pow(NTags[i_tf],2.0)+NTagsError_low[i_tf]/pow(NTags_low[i_tf],2.0)));
	  if( i_tf == 2 )
	    {
	      tf[i_tf] = EW_2p/EW_2p_low;
	      tf_err[i_tf]   = sqrt(pow(tf[i_tf],2.0)*(EW_E2p/pow(EW_2p,2.0)+EW_E2p_low/pow(EW_2p_low,2.0)));
	    }
	}


      ResFile <<FileName<<"  "<<c_Zpt<<"  "<<Z_Sf<<" "<<c_ip<<" "<<c_ta<<" "<<c_al<<" "<< NTags_bare[0] << " " 
	//<<NTags_base[0]<<" "<<sqrt(NTags_baseError[0])  <<" "
	//    <<NTags_ele [0]<<" "<<sqrt(NTags_eleError[0])   <<" "
	      <<NTags     [0]<<" "<<sqrt(NTagsError[0])       <<" "
	      <<NTags_low [0]<<" "<<sqrt(NTagsError_low[0])       <<" "
	//<<NTags_base[1]<<" "<<sqrt(NTags_baseError[1])  <<" "
	//<<NTags_ele [1]<<" "<<sqrt(NTags_eleError[1])   <<" "
	      <<NTags     [1]<<" "<<sqrt(NTagsError[1])       <<" "
	      <<NTags_low [1]<<" "<<sqrt(NTagsError_low[1])       <<" "
	//<<base_2p      <<" "<<sqrt(baseE_2p)            <<" "
	//<<ele_2p       <<" "<<sqrt(eleE_2p)             <<" "  
	      <<EW_2p        <<" "<<sqrt(EW_E2p)              <<" "
	      <<EW_2p_low    <<" "<<sqrt(EW_E2p_low)              <<" "
	      <<tf [0]       <<" "<< tf_err[0]                <<" "
	      <<tf [1]       <<" "<< tf_err[1]                <<" "
	      <<tf [2]       <<" "<< tf_err[2]                <<" "
	      <<endl;
    }//c_al loop
  }//c_ZPt loop
  
  f2->Close();
  ResFile.close();
}