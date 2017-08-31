//
// This macro creates a canvas containining the camera and the phe or the timing
// and save it either in a png or a pdf file
//
// If the option pdf is activated and more than 1 event has to be plot, they will all be saved
// in the same file, on different pages. Otherwise, one png file per event wil be created
//
// Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo


#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TChain.h>
#include <TCanvas.h>
#include "TH1F.h"
#include "MArrayI.h"
#include "TArrayD.h"
#include "TArrayF.h"
#include "MArrivalTime.h"
#include "MHillasSrc.h"
#include "MGeomCam.h"
#include "MStereoShowerPar.h"
#include "MHadronness.h"
#include "MEnergyEst.h"
#include "MMcEvt.hxx"
#include "MMcCorsikaRunHeader.h"
#include "MCerPhotEvt.h"
#include "MHCamera.h"
#include "TImage.h"
#include "TPaveText.h"


using namespace std;

void CTAplotEvent(TString FileName, //Input file name
	Int_t first_event_to_draw, // Number of the entry in the TFile to draw (not the event number from CORSIKA)
	Int_t last_event_to_draw, //Last event NOT included!!!
	Int_t tel_id, // Tel id to draw (e.g: 1 or 2 or 3 or 4 or 560 or...)
	TString outputtype = "png", // type of the output: can be png or pdf
	Bool_t cleaned=false, Float_t min_energy=0., Float_t max_impact=1000000.)  {

		const UInt_t NtelTypes = 1;

		Int_t MaxNtel;

		if (outputtype!="png" && outputtype!="pdf"){
			cout<<"The output file can be only png or pdf! Exiting!"<<endl;
			return;
		}

		TChain rh("RunHeaders");
		rh.Add(FileName.Data());

		rh.SetBranchStatus("*", 1);
		MMcCorsikaRunHeader* rhcor = 0;
		rh.SetBranchAddress("MMcCorsikaRunHeader.", &rhcor);
		rh.GetEvent(0);

		MaxNtel = 1;

		cout << "Found telescopes: " << endl;
		for (Int_t i = 0; i < rhcor->GetNumCT(); i++)
		if (rh.FindBranch(Form("MMcRunHeader_%d.", i+1))){
			cout<<i+1<<" ";
			MaxNtel++;
		}
		cout << endl;

		MaxNtel -= 1;
		cout << "Found a total of " << MaxNtel << " telescopes in the file." << endl;


		const Int_t Ntel = MaxNtel;
		MArrayI Number_of_Pixels;


		MGeomCam* geom = new MGeomCam;

		geom = 0;
		rh.SetBranchAddress(Form("MGeomCam_%d.", tel_id), &(geom));


		rh.GetEvent(0);

		// Identify telescope types by number of pixels (and camera dist if needed):
		Int_t TelType = -1;
		Int_t npixels = geom->GetNumPixels();
		Float_t fdist = geom->GetCameraDist();
		if (npixels == 1855)
		{
			if (fabs(fdist-28.) < 1.e-3)
			TelType = 0;  // LST
			else
			TelType = 1;  // Nectar
		}
		else if (npixels == 1764)
		TelType = 2;  // FlashCam
		else if (npixels == 1296)
		TelType = 3;  // SST-1M
		else if (npixels == 2048)
		TelType = 4;  // GCT-CHEC
		else if (npixels == 2368)
		TelType = 5;  // ASTRI
		else if (npixels == 11328)
		TelType = 6;  // SCT

		cout << "I'll plot telescope id: " << tel_id <<" Of type: "<<TelType<< endl;

		TChain c("Events");
		c.Add(FileName.Data());

		c.SetBranchStatus("*", 1);

		MCerPhotEvt* cphot = new MCerPhotEvt;
		MArrivalTime* arrtime = new MArrivalTime;
		MMcEvt* mcevent = new MMcEvt;

		cphot = 0;
		c.SetBranchAddress(Form("MCerPhotEvt_%d.", tel_id), &cphot);
		arrtime = 0;
		c.SetBranchAddress(Form("MArrivalTime_%d.", tel_id), &arrtime);
		mcevent = 0;
		c.SetBranchAddress(Form("MMcEvt_%d.", tel_id), &mcevent);

		TArrayD pixels,times;
		TArrayF times_raw;

		Int_t eventdrawn = 0;
		TCanvas* c1 = new TCanvas;
		c1->Divide(2,1);


		for (Int_t ievt = first_event_to_draw; ievt < c.GetEntries(); ievt++) {
			c.GetEvent(ievt);

			if (ievt>last_event_to_draw) break;

			if (ievt % 10000 == 0 || ievt == first_event_to_draw)
			cout << ievt << " / " << c.GetEntries() << endl;


			if (cphot->GetNumPixels() < 1)
			continue;

			pixels.Reset();
			times.Reset();
			times_raw.Reset();

			if ( mcevent->GetEnergy() > min_energy && mcevent->GetImpact() < max_impact) {
				cout<<"Event id: "<<ievt<<endl;

				pixels.Set(geom->GetNumPixels());
				times.Set(arrtime->GetSize());
				times_raw.Set(arrtime->GetSize());

				times_raw = arrtime->GetData();


				for (Int_t ipix = 0; ipix < cphot->GetNumPixels(); ipix++) {
					MCerPhotPix &cpix = (*cphot)[ipix];
					Double_t signal = cpix.GetNumPhotons();
					if (signal <= 0.)
					continue;

					if (cleaned) {
						if (cpix.IsPixelUsed()){
							pixels[cpix.GetPixId()] = (signal);
							times[cpix.GetPixId()]=times_raw[cpix.GetPixId()];
						}
						else {
							pixels[cpix.GetPixId()] = 0;
							times[cpix.GetPixId()] = 0;
						}
					} else {
						pixels[cpix.GetPixId()] = (signal);
						times[cpix.GetPixId()] = times_raw[cpix.GetPixId()];
					}
				}

				Bool_t lastevent = false;
				if (ievt==last_event_to_draw-1 || first_event_to_draw==last_event_to_draw) lastevent = true;

				c1->cd(1);

				MHCamera *cam = new MHCamera(*geom);
				cam->SetBit(TH1::kNoStats | kCanDelete );
				cam->SetCamContent(pixels);
				cam->Draw("pixelindex");
				TString telidevt = Form("Event %i on telescope id %i ",ievt, tel_id);
				TString energy = Form("Energy = %.2f GeV",mcevent->GetEnergy());
				TString impact = Form("Impact = %.2f m",mcevent->GetImpact()/100.);
				TString heightfi = Form("Height First Interaction = %.2f m",mcevent->GetZFirstInteraction()/100.);

				c1->cd(2);
				MHCamera *camt = new MHCamera(*geom);
				camt->SetBit(TH1::kNoStats | kCanDelete );
				camt->SetCamContent(times);
				camt->Draw("pixelindex");

				TPaveText *t=new TPaveText(0.5,0.81,1,1,"brNDC");
				t->AddText(telidevt);
				t->AddText(energy);
				t->AddText(impact);
				t->AddText(heightfi);
				t->SetFillColor(0);
				t->Draw();

				if (outputtype == "png"){
					c1->SaveAs(Form("/home/paolo/Evt%i_tel%i.png", ievt, tel_id));
				} else if (outputtype == "pdf"){
					if ((eventdrawn == 0 && lastevent) || (eventdrawn != 0)) c1->Print(Form("/home/paolo/Events_tel%i.pdf", tel_id),"pdf");
					else if (eventdrawn == 0 && !lastevent) c1->Print(Form("/home/paolo/Events_tel%i.pdf(", tel_id),"pdf");
				}

				cout << "Energy = " << mcevent->GetEnergy() << " Impact = " << mcevent->GetImpact() <<" First Interaction = "<<mcevent->GetZFirstInteraction()<< endl;

				eventdrawn++;
				delete cam, camt;
				if (ievt==last_event_to_draw-1 || first_event_to_draw==last_event_to_draw) break;
				c1->Clear("D");
			}
		}
		if (outputtype == "pdf"){
			c1->Print(Form("/home/paolo/Events_tel%i.pdf]", tel_id),"pdf");
		}
		return;
	}
