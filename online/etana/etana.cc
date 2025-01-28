/*
 *  etana.cc (c++)
 *  - The code to obtain the ET offline historgrams.
 *  - Please use the CSI ET run to get the result.
 *
 *   @author: Chieh Lin ( clin@gm.ncue.edu.tw )
 *   @date:   2025.01.28
 *
 */
#include <iostream>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

// parameter setting //
const Double_t T0 = 28.6;
const Double_t Tsigma = 2.5;
const Double_t Emin = 3.;
const Double_t Emax = 3.e3;

int main( int argc, char** argv )
{

   /// read arg ///
   if( argc!=3 ){
      std::cout<<"arg err: " << argv[0] <<" (input) (output)" << std::endl;
      return 0; 
   }

   const std::string ifname = argv[1];
   const std::string ofname = argv[2];
   std::cout<<" input file  = " << ifname << std::endl;
   std::cout<<" output file = " << ofname << std::endl;
  
   /// input files
   TChain *itree = new TChain("T");
   itree->Add( ifname.c_str() );
   const Long64_t nentries = itree->GetEntries();
   std::cout<<" #entries = " << nentries << std::endl;
   if( nentries==0 ){
      return 0;
   }

   Int_t ncsi;
   UInt_t trigbit;
   Float_t ene[3000], time[3000];

   itree->SetBranchAddress("ScaledTrigBit", &trigbit);
   itree->SetBranchAddress("CSINumber", &ncsi);
   itree->SetBranchAddress("CSIEne", ene);
   itree->SetBranchAddress("CSITime", time);

   /// output files 
   TFile *ofile = new TFile( ofname.c_str(), "recreate");
   TTree *otree = new TTree("tr","ET calculation");

   Double_t et = 0.;
   otree->Branch("et", &et, "et/D");

   /// Loop analysis
   for( Long64_t ientry=0; ientry<nentries; ++ientry )
   {
      if( nentries>100 && ientry%(nentries/10)==0 )
         std::cout<< 10*ientry/(nentries/10) << "\% proceesed." << std::endl;

      itree->GetEntry(ientry);

      /// skip non-internal trigger events 
      if( trigbit==0 ) continue;

      /// loop CSI channels
      et = 0.;
      for( int icsi=0; icsi<ncsi; ++icsi )
      {
         if(    time[icsi] > T0 - 2*Tsigma && time[icsi] < T0 + 2*Tsigma 
             && ene[icsi] > Emin && ene[icsi] < Emax )
         {
            et += ene[icsi];
         } 
      }

      otree->Fill();
   }

   /// Write out the file
   otree->Write();
   ofile->Close();

   std::cout<<"End of the program. " << std::endl;

}
