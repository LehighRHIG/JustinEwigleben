#include "TSystem.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TClonesArray.h"
#include "TBranch.h"
#include "TLorentzVector.h"
#include "TParameter.h"
#include <iostream>
#include <cmath>
#include <exception>

using namespace std;

int main(int argc, const char** argv)
{
    const char *defaults[6] = {"readPythiaTree","pytest15.root","in"};
    if ( argc==1 ) {
        argv=defaults;
        argc=3;
    }
    TString InFileName = argv[1];
    cout<<"In file is "<<InFileName.Data()<<endl;
    
    TString OutFileName = argv[2];
    cout<<"Out file is "<<OutFileName.Data()<<endl;
    
    TFile *_file0 = TFile::Open(InFileName); //read tree
    TTree* tree = (TTree*)gROOT->FindObject("tree");
    
    //read collections
    TClonesArray *HardPartons = new TClonesArray("TLorentzVector");
    TBranch* bHardPartons = tree->GetBranch("HardPartons");
    bHardPartons->SetAddress(&HardPartons);
    TClonesArray *HardPartonNames = new TClonesArray("TParameter<int>");
    TBranch* bHardPartonNames = tree->GetBranch("HardPartonNames");
    bHardPartonNames->SetAddress(&HardPartonNames);
    TClonesArray *particles = new TClonesArray("TLorentzVector");
    TBranch* bparticles = tree->GetBranch("Particles");
    bparticles->SetAddress(&particles);
    
    //First create the histograms that will be filled with >> command
    TString histname("hHardPartonspT");
    TH1F* hHardPartonspT = new TH1F(histname,histname,100,0,100);
    histname.Prepend("sqrt(HardPartons.fP.fX*HardPartons.fP.fX+HardPartons.fP.fY*HardPartons.fP.fY)>>");
    tree->Draw(histname);
    
    histname = "hHardPartonsName";
    TH1F* hHardPartonsName = new TH1F(histname,histname,40,-9.5,30.5);
    histname.Prepend("HardPartonNames.fVal>>");
    tree->Draw(histname);
    
    //Next book the histograms that will be filled event by event
    histname = "hParticlepT";
    TH1F* hParticlepT = new TH1F(histname,histname,1000,0,100);
    histname = "hParticleEtaPhi";
    TH2F* hParticleEtaPhi = new TH2F(histname,histname,100,-1,1,100,-6.2,6.2);
    
    Int_t entries = tree->GetEntries();
    for (Int_t iev = 0;iev<entries;iev++){
        //Looping over number of events
        if (iev % 1000 == 0)
            cout<<"Event "<<iev<<endl;
        
        //Get all the particles in the event
        tree->GetEntry(iev);
        bHardPartons->GetEntry(iev);
        bHardPartonNames->GetEntry(iev);
        bparticles->GetEntry(iev);
        
        //loop over final state particles
         Int_t nparticles = particles->GetEntriesFast();
        for (int ip = 0; ip<nparticles;ip++){
            //Find each particle
            TLorentzVector* particle = (TLorentzVector*)particles->At(ip);
            
            //Manipulate particles and fill histograms
            hParticlepT->Fill(particle->Pt());
            hParticleEtaPhi->Fill(particle->Eta(),particle->Phi());
        }
        
    } //end of event loop
    
    //Open output file and save the histograms
    TFile *file1 = TFile::Open(OutFileName.Data(),"RECREATE");

    hHardPartonspT->Write();
    hHardPartonsName->Write();
    hParticlepT->Write();
    hParticleEtaPhi->Write();

}
