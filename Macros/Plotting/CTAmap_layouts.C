//
// It creates a plot of every single layout contained in a Candidate list
// starting from a star or stereo file containining ALL the telescopes in the candidate list.
// If a candidate list is not provided is going to plot all the telescopes in the file in a same Canvas
//
// If the option pdf is activated and more than 1 layout has to be plot, they will all be saved
// in the same file, on different pages. Otherwise, one png file per layout wil be created
//
// Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

#include <fstream>
#include <iostream>

#include <TChain.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TEllipse.h>
#include <TH1F.h>

#include "MMcCorsikaRunHeader.h"
#include "MMcRunHeader.hxx"
#include "MGeomCam.h"
#include "MGeomCorsikaCT.h"


using namespace std;

void CTAmap_layouts(TString filename ="gamma_20deg_180deg_run111___cta-prod3-merged_desert-2150m--subarray-3-nosct.root",
  TString outputtype = "png", // type of the output: can be png or pdf
  TString Subarray = "Placeholder_candidates.txt", // is the candidate list is not provided all the telescopes will be plotted
  TString opt = "")
{
  //Suggested 500 for La Palma and 1500 for Paranal, the axis will go from -PlotHalfSize to PlotHalfSize
  Double_t PlotHalfSize = 500.;

  Bool_t candidate = true;

  // If no candidate list is provided, plot all the telescopes in the same graph
  if (Subarray=="Placeholder_candidates.txt"){
    candidate = false;
  }

  if (outputtype!="png" && outputtype!="pdf"){
    cout<<"The output file can be only png or pdf! Exiting!"<<endl;
    return;
  }

  ifstream fin(Subarray.Data());
  Int_t **TelId = new Int_t*[100];
  Char_t ch[1000];
  Int_t numberoflayouts = 0;
  TString LayoutName[100];
  Int_t Ntel[100];

  // Prod3's LSTs and NectarCam MSTs:
  const Int_t nselectedtels = 200;
  const Int_t nselectedarray = 50;
  Int_t seltelids[nselectedarray][nselectedtels];

  for (Int_t i = 0; i < nselectedarray; i++){
    for (Int_t j = 0; j < nselectedtels; j++){
      seltelids[i][j]=0;
    }
  }

  /*for (Int_t i = 0; i < 4; i++)
    seltelids[i] = i+1;  // LSTs have id's from 1 to 7
  for (Int_t i = 4; i < nselectedtels; i++)
    seltelids[i] = i;  // NectarCam MSTs have id's from 71 to 133*/

  Int_t ipoint = 0;
  for (;;){

	  fin.getline(ch, 1000);

	  if (fin.eof() || !candidate)
	    break;

	  if (ch[0] == '#' || ch[0] == 0) // ignore empty lines & lines with comments
	    continue;

	  TString str = ch;

	  TObjArray* arr;
	  arr = str.Tokenize(" ");

	  if (arr->GetEntries() == 1)  // Line contains just the candidate layout name
	  {
      TString layname = ((TObjString*)arr->At(0))->GetString();
	    if (layname == TString("\n")) continue;  // => this was an empty line (just spaces & \n)

      LayoutName[numberoflayouts] = layname;
	    continue;
	  }

	  Ntel[numberoflayouts] = arr->GetEntries();
	  TelId[numberoflayouts] = new Int_t[Ntel[numberoflayouts]];
	  cout << "Will analyze layout " << LayoutName[numberoflayouts] << " with " << Ntel[numberoflayouts]
		 << " telescopes with id's: ";

	  for (Int_t i = 0; i < Ntel[numberoflayouts]; i++){
		  seltelids[numberoflayouts][i] = ((TObjString*)arr->At(i))->GetString().Atoi();
		  cout << seltelids[numberoflayouts][i] << " ";
		}
	  cout << endl;
    numberoflayouts++;
  }

if (candidate==false) numberoflayouts=1;

for (Int_t layouts = 0; layouts < numberoflayouts; layouts++){
  const Int_t MaxNtelTypes = 6;
  Int_t col[MaxNtelTypes] = {1, 2, 4, 6, 3, 7};
  Float_t markersize[MaxNtelTypes] = {1., 1., 0.5, 0.5, 0.5, 0.5};
  Int_t markertype[MaxNtelTypes] = {24, 20, 25, 26, 32, 27};

  TChain c("RunHeaders");

  c.Add(filename.Data());
  c.SetBranchStatus("*",1);
  MMcCorsikaRunHeader *crh = 0;
  c.SetBranchAddress("MMcCorsikaRunHeader.",&crh);
  c.GetEvent(0);

  Int_t Ntel = 0;

  for (Int_t i = 0; i < crh->GetNumCT(); i++){
    if (c.FindBranch(Form("MMcRunHeader_%d.", i+1))){
      Ntel++;
      if (candidate==false) seltelids[layouts][i] = i+1;
    }
  }
  Int_t   *TelId      = new Int_t[Ntel];
  Int_t   *NumPixels  = new Int_t[Ntel];
  Float_t *PlateScale = new Float_t[Ntel];
  Int_t   *TelType    = new Int_t[Ntel];

  Int_t ict = 0;
  cout << "Found telescopes:" << endl;
  for (Int_t i = 0; i < crh->GetNumCT(); i++)
    if (c.FindBranch(Form("MMcRunHeader_%d.", i+1)))
      TelId[ict++] = i+1;

  // Now find how many types of telescopes
  MGeomCam** gcam = new MGeomCam*[Ntel];
  for (Int_t i = 0; i < Ntel; i++)
    {
      Int_t telid = TelId[i];
      gcam[i] = 0;
      c.SetBranchAddress(Form("MGeomCam_%d.", telid), &gcam[i]);
    }
  c.GetEvent(0);
  for (Int_t i = 0; i < Ntel; i++)
    {
      NumPixels[i] = gcam[i]->GetNumPixels();
      PlateScale[i] = gcam[i]->GetConvMm2Deg();
    }

  TelType[0] = 0;  // By definition!
  Int_t type = 0;
  Int_t NtelTypes = 1;
  for (Int_t i = 1; i < Ntel; i++)
    {
      Bool_t newtype = kTRUE;
      for (Int_t j = 0; j < i; j++)
	{
	  if (NumPixels[i] == NumPixels[j] &&
	      fabs(PlateScale[i]-PlateScale[j]) < 1.e-8)
	    {
	      newtype = kFALSE;
	      TelType[i] = TelType[j];
	      break;
	    }
	}
      if (!newtype)
	continue;

      NtelTypes++;
      type++;
      TelType[i] = type;
    }

  for (Int_t i = 0; i < Ntel; i++)
    {
      Int_t telid = TelId[i];
      cout << "telid = " << telid << ",  Num pixels = " << gcam[i]->GetNumPixels()
	   << " tel type = " << TelType[i] << endl;
    }


  TGraph** telpos = new TGraph*[NtelTypes];
  Int_t* npoints = new Int_t[NtelTypes];
  for (Int_t i = 0; i < NtelTypes; i++)
    {
      telpos[i] = new TGraph;
      npoints[i] = 0;
    }

  for (Int_t i = 0; i < Ntel; i++)
    {
      if (nselectedtels > 0)
	{
	  Int_t j;
	  for (j = 0; j < nselectedtels; j++){
	    if (seltelids[layouts][j] == TelId[i]){
	      break;
      }
    }
	  if (j == nselectedtels)  // No match found: telescope is not among selected ones.
	    continue;
	}

      MGeomCorsikaCT &ct = (*crh)[TelId[i]-1];
      telpos[TelType[i]]->SetPoint(npoints[TelType[i]]++, -ct.GetCTy()/100., ct.GetCTx()/100.);

      cout << "Tel #" << TelId[i] << ": " << ct.GetCTtheta() << "  " << ct.GetCTphi() << endl;
      cout << "    x: " << ct.GetCTx()/100. << "   y: " << ct.GetCTy()/100. << endl;
      cout << ct.GetCTdiam() << "  " << ct.GetCTfocal() << endl << endl;
    }


  for (Int_t itype = 0; itype < NtelTypes; itype++)
    {
      telpos[itype]->SetMarkerStyle(markertype[itype]);
      telpos[itype]->SetMarkerSize(markersize[itype]);
      telpos[itype]->SetMarkerColor(col[itype]);
    }

  //if (opt != "same")
    //{
      TCanvas *canv = new TCanvas("canv", "", 800, 800);

      TH1 *frame = canv->DrawFrame(-PlotHalfSize,-PlotHalfSize,PlotHalfSize,PlotHalfSize, LayoutName[layouts]);
      frame->GetXaxis()->SetTitle("-y [m]");
      frame->GetYaxis()->SetTitle("x [m]");
      canv->Update();
      canv->SetGridx();
      canv->SetGridy();
    //}

  for (Int_t itype = 0; itype < NtelTypes; itype++)
    {
      if (telpos[itype]->GetN() < 1)
	continue;

      telpos[itype]->Draw("p");
    }

  TLatex label;
  label.SetTextSize(0.02);

  for (Int_t i = 0; i < Ntel; i++)
    {
      if (nselectedtels > 0)
	{
	  Int_t j;
	  for (j = 0; j < nselectedtels; j++)
	    if (seltelids[layouts][j] == TelId[i])
	      break;
	  if (j == nselectedtels)  // No match found: telescope is not among selected ones.
	    continue;
	}

      MGeomCorsikaCT &ct = (*crh)[TelId[i]-1];

      //if (TelType[i] > 0)
      // 	continue;

      Int_t color;
      /*if (TelType[i] == 1)
	color = col[1];
      else if (TelType[i] == 2)
	color = col[2];
      else if (TelType[i] == 3)
	color = col[3];
      else
	color = col[0];*/
  color = col[TelType[i]];

      label.SetTextColor(color);
      label.DrawLatex(-ct.GetCTy()/100.+5*TelType[i], ct.GetCTx()/100.+5*TelType[i], Form("%d", TelId[i]));
    }

  TEllipse *c500 = new TEllipse(0.,0.,250, 250);
  TEllipse *c700 = new TEllipse(0.,0.,350, 350);

  c500->SetFillStyle(0);
  c700->SetFillStyle(0);

  //c500->Draw("same");
  //c700->Draw("same");
      //canv->Update();
    if (outputtype == "png"){
      if (candidate) canv->SaveAs(Form("/home/paolo/Layout_%s.png", LayoutName[layouts].Data()));
      else canv->SaveAs("/home/paolo/Layout.png");
    } else if (outputtype == "pdf"){
      if (numberoflayouts==1) canv->Print("/home/paolo/Layouts.pdf","pdf");
      else if (layouts == 0) canv->Print("/home/paolo/Layouts.pdf(","pdf");
      else if (layouts == numberoflayouts-1) canv->Print("/home/paolo/Layouts.pdf)","pdf");
      else canv->Print("/home/paolo/Layouts.pdf","pdf");
    }
  }//Layouts loop
}
