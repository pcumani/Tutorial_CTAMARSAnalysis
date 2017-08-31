//
// This macro creates a summary plot with all the IRFs starting from the file Subarray%s_IFAE_average_%shours_v1.root
// created by CTAsensi
//
// Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

#include <Rtypes.h>
#include <TArrayD.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TString.h>
#include <TText.h>
#include <TVectorDfwd.h>
#include <cstdio>
#include <fstream>

#include "./reqcurve.C"
#include "./ctareqangres.C"
#include "./ctareqeres.C"


using namespace std;

TString ref_filename;

TString findArrayName(TString filename){

	TString Subarray = "/home/thassan/Paquetes/lastVersion/Mars/mcta/Prod3_candidates.txt";
	ifstream fin(Subarray.Data());
	if (!fin.is_open())
	{
		// Look for the file in the standard location, $MARSSYS/mcta
		TString str = gSystem->Getenv("MARSSYS");
		str += "/mcta/";
		str += Subarray;
		fin.open(str.Data());

		if (!fin.is_open())
		{
			cout << "Cannot open file " << Subarray << " containing layout candidates" << endl;
			return -1;
		}
	}

	TString arrayName;
	TString lastArrayName;

	Char_t ch[1000];

	for (;;)
	{
		fin.getline(ch, 1000);
		if (ch[0] == '#')
			continue;

		if (fin.eof())
			break;

		TString str = ch;

		TObjArray* arr;
		arr = str.Tokenize(" ");

		if (arr->GetEntries() == 1)  // Line contains just the candidate layout name
		{
			arrayName = (TString)((TObjString*)arr->At(0))->GetString();
			if (filename.Contains(arrayName)) {
				lastArrayName = arrayName;
			}

		}
	}


	return lastArrayName;
}



TH1F* CTAgetHistogram(TString filename, TString what_to_plot) {

	//open file
	TFile* f = NULL;
	if (!(f = TFile::Open(filename))) {
		cout << "ERROR: could not open root performance file " << filename
				<< endl;
		return NULL;
	}

	if (what_to_plot == "EffArea") {

		TH1F* effColArea = NULL;

		if (!(effColArea = (TH1F*) f->Get("EffectiveArea"))) {
			cout
			<< "ERROR: did not find histogram EffectiveArea in the provided root performance file "
			<< filename << endl;
			return NULL;
		} else {
			effColArea->SetDirectory(0);
			return effColArea;
		}

	} else if (what_to_plot == "DiffSens") {

		TH1F* DiffSens = NULL;
		if (!(DiffSens = (TH1F*) f->Get("DiffSens"))) {
			cout
			<< "ERROR: did not find histogram DiffSens in the provided root performance file "
			<< filename << endl;
			return NULL;
		} else {
			DiffSens->SetDirectory(0);
			return DiffSens;
		}

	} else if (what_to_plot == "AngRes") {

		TH1F* AngRes = NULL;
		if (!(AngRes = (TH1F*) f->Get("AngRes"))) {
			cout
			<< "ERROR: did not find histogram DiffSens in the provided root performance file "
			<< filename << endl;
			return NULL;
		} else {
			AngRes->SetDirectory(0);
			return AngRes;
		}

	}

	f->Close();
	delete f;

	return NULL;

}


void CTAdrawHistogramTGraph(TString filename, TString legend, TString what_to_plot, TString drawOpt, Int_t color_in, TLegend * leg) {


	//	if (!checkCanvas(c1)){
	//		TPad *pad1 = new TPad("DiffSensPad", "DiffSensPad", 0, 0.3, 1, 1);
	//	//	pad1->SetBottomMargin(5);
	//		if (what_to_plot == "DiffSens" || what_to_plot == "EffArea")
	//			pad1->SetLogy();
	//		pad1->Draw();
	//		pad1->cd();
	//	}
	if (what_to_plot == "DiffSens")gPad->SetLogy();


	//	if (what_to_plot=="AngRes"){
	//		h_0->Sumw2();
	//		h_1->Sumw2();
	//	}

	//	h_0->GetXaxis()->SetTitle("log_{10}( energy ) (GeV)");
	//	h_0->GetYaxis()->SetTitle("# events");
	//	h_1->SetLineColor(2);
	//	h_1->SetLineStyle(2);


	//	h_0->GetYaxis()->SetRangeUser(5E-14, 1E-09);
	//	h_0->GetXaxis()->SetRangeUser(-1.7, 2.3);
	//	h_1->GetXaxis()->SetRangeUser(-1.7, 2.3);
	//	h_0->GetXaxis()->SetTitle("");
	//	h_0->GetXaxis()->SetLabelSize(0);
	////	h_0->GetXaxis()->SetRangeUser(log10(0.01), log10(300.));
	//	h_0->GetYaxis()->CenterTitle(kTRUE);
	//	h_0->GetYaxis()->SetTitleSize(0.05);
	//	h_0->GetYaxis()->SetTitleOffset(0.88);

	//	c1->cd();

	//open file
	TFile* f = NULL;
	if (!(f = TFile::Open(filename))) {
		cout << "ERROR: could not open root performance file " << filename
				<< endl;
		return;
	}

	if (what_to_plot == "EffArea") {

		TH1F* effColArea_ = NULL;

		if (!(effColArea_ = (TH1F*) f->Get("EffectiveArea"))) {
			cout
			<< "ERROR: did not find histogram EffectiveArea in the provided root performance file "
			<< filename << endl;
			return;
		} else {
			TGraphErrors* effColArea = new TGraphErrors(effColArea_);
			Int_t totPoints = effColArea->GetN();
			for (Int_t i = 0; i < totPoints; i++) {
				Double_t xi = -1.;
				Double_t yi = -1.;
				effColArea->GetPoint(i, xi, yi);
				if (yi == 0) {
					effColArea->RemovePoint(i);
					i--;
					totPoints--;
				}
			}
			effColArea->SetLineColor(color_in);
			effColArea->SetLineWidth(2);
			effColArea->GetYaxis()->SetTitle("Effective Area / m^{2}");
			effColArea->GetXaxis()->SetTitle("log_{10}(Energy/TeV)");
			effColArea->GetYaxis()->SetRangeUser(1E2,1E7);
			effColArea->GetXaxis()->SetLimits(-1.9, 2.4);
			if (drawOpt == "AL" || drawOpt == "Lsame") effColArea->SetLineStyle(color_in+1);
			//if (drawOpt != "AL" && drawOpt != "Lsame")leg->AddEntry(effColArea, legend, "ple");
			effColArea->Draw(drawOpt);
			gPad->SetLogy();
			return;

		}

	} else if (what_to_plot == "BGRate") {

		TH1F* bgrate_ = NULL;

		if (!(bgrate_ = (TH1F*) f->Get("BGRate"))) {
			cout
			<< "ERROR: did not find histogram BGRate in the provided root performance file "
			<< filename << endl;
			return;
		} else {
			TGraphErrors* bgrate = new TGraphErrors(bgrate_);
			Int_t totPoints = bgrate->GetN();
			for (Int_t i = 0; i < totPoints; i++) {
				Double_t xi = -1.;
				Double_t yi = -1.;
				bgrate->GetPoint(i, xi, yi);
				if (yi == 0) {
					bgrate->RemovePoint(i);
					i--;
					totPoints--;
				}
			}
			//bgrate->Print("ALL");
			bgrate->SetLineColor(color_in);
			bgrate->SetLineWidth(2);
			bgrate->GetYaxis()->SetTitle("Background rate / Hz");
			bgrate->GetXaxis()->SetTitle("log_{10}(Energy/TeV)");
			bgrate->GetYaxis()->SetRangeUser(1E-08,1E1);
			bgrate->GetXaxis()->SetLimits(-1.9, 2.4);
			if (drawOpt == "AL" || drawOpt == "Lsame") bgrate->SetLineStyle(color_in+1);
			//if (drawOpt != "AL" && drawOpt != "Lsame")leg->AddEntry(bgrate, legend, "ple");
			bgrate->Draw(drawOpt);
			gPad->SetLogy();
			return;

		}

	}else if (what_to_plot == "DiffSens") {

		TH1F* DiffSens_ = NULL;
		if (!(DiffSens_ = (TH1F*) f->Get("DiffSens"))) {
			cout
			<< "ERROR: did not find histogram DiffSens in the provided root performance file "
			<< filename << endl;
			return;
		} else {
			TGraphErrors* DiffSens = new TGraphErrors(DiffSens_);
			const int size = DiffSens->GetN();
			Double_t* Yaxis = DiffSens->GetY();
			int points_to_remove_left=0;
			int points_to_remove_right=0;
			bool leftOrRight=1;

			for (int i=0;i<size;i++){
				if (Yaxis[i] < 1E-28 && leftOrRight == 1) points_to_remove_left++;
				else if (leftOrRight == 1) leftOrRight=0;
				if (Yaxis[i] < 1E-28 && leftOrRight==0) points_to_remove_right++;
			}
			for (int i=0;i<points_to_remove_left;i++){
				DiffSens->RemovePoint(0);
			}
			for (int i=0;i<points_to_remove_right;i++){
				DiffSens->RemovePoint(DiffSens->GetN()-1);
			}
			DiffSens->SetTitle("Differential Sensitivity");
			DiffSens->GetYaxis()->SetTitle(DiffSens_->GetYaxis()->GetTitle());
			DiffSens->GetXaxis()->SetTitle(DiffSens_->GetXaxis()->GetTitle());
			DiffSens->GetYaxis()->SetRangeUser(2E-14, 1E-09);
			DiffSens->GetXaxis()->SetLimits(-1.9, 2.4);
			//			DiffSens->GetXaxis()->SetTitle("");
			//			DiffSens->GetXaxis()->SetLabelSize(0);
			DiffSens->GetYaxis()->CenterTitle(kTRUE);
			DiffSens->GetYaxis()->SetTitleSize(0.05);
						//DiffSens->Print("ALL");
			DiffSens->GetYaxis()->SetTitleOffset(0.88);
			DiffSens->GetYaxis()->SetLabelFont(42);
			DiffSens->GetYaxis()->SetTitleFont(42);

			DiffSens->SetLineColor(color_in);
			DiffSens->SetLineWidth(2);
			if (drawOpt == "AL" || drawOpt == "Lsame") DiffSens->SetName("ref_sensi");
			if (drawOpt == "AL" || drawOpt == "Lsame") DiffSens->SetLineStyle(color_in+1);
			DiffSens->Draw(drawOpt);
			//			DiffSens->SetDirectory(0);
			if (drawOpt != "AL" && drawOpt != "Lsame")leg->AddEntry(DiffSens, legend, "ple");

			return;
		}

	} else if (what_to_plot == "AngRes") {

		TH1F* AngRes_ = NULL;
		if (!(AngRes_ = (TH1F*) f->Get("AngRes"))) {
			cout
			<< "ERROR: did not find histogram AngRes in the provided root performance file "
			<< filename << endl;
			return;
		} else {
			TGraphErrors* AngRes = new TGraphErrors(AngRes_);
			Int_t totPoints = AngRes->GetN();
			for (Int_t i = 0; i < totPoints; i++) {
				Double_t xi = -1.;
				Double_t yi = -1.;
				AngRes->GetPoint(i, xi, yi);
				if (yi == 0) {
					AngRes->RemovePoint(i);
					i--;
					totPoints--;
				}
			}
			AngRes->SetLineColor(color_in);
			//			AngRes->Sumw2();
			AngRes->Draw(drawOpt);
			//			AngRes->SetDirectory(0);
			AngRes->SetLineWidth(2);
			AngRes->GetYaxis()->SetTitle("Angular Resolution #sigma_{68} / deg");
			AngRes->GetXaxis()->SetTitle("log_{10}(Energy/TeV)");
			AngRes->GetXaxis()->SetLimits(-1.9, 2.4);
			if (drawOpt == "AL" || drawOpt == "Lsame") AngRes->SetLineStyle(color_in+1);
			AngRes->Draw(drawOpt);
			//			DiffSens->SetDirectory(0);
			//if (drawOpt != "AL" && drawOpt != "Lsame")leg->AddEntry(AngRes, legend, "ple");

			return;
		}

	} else if (what_to_plot == "ERes") {

		TH1F* ERes_ = NULL;
		if (!(ERes_ = (TH1F*) f->Get("ERes"))) {
			cout
			<< "ERROR: did not find histogram ERes in the provided root performance file "
			<< filename << endl;
			return;
		} else {
			TGraph* ERes = new TGraph(ERes_);
			Int_t totPoints = ERes->GetN();
			for (Int_t i = 0; i < totPoints; i++) {
				Double_t xi = -1.;
				Double_t yi = -1.;
				ERes->GetPoint(i, xi, yi);
				if (yi == 0) {
					ERes->RemovePoint(i);
					i--;
					totPoints--;
				}
			}
			ERes->SetLineColor(color_in);
			ERes->GetYaxis()->SetTitle("Relative Energy resolution (68% Erec/Etrue=1)");
			ERes->GetXaxis()->SetTitle("log_{10}(Energy/TeV)");

			ERes->GetYaxis()->SetRangeUser(0,0.35);
			ERes->GetXaxis()->SetLimits(-1.9, 2.4);
			//			ERes->Sumw2();
			//ERes->Draw(drawOpt);
			//			ERes->SetDirectory(0);
			ERes->SetLineWidth(3);


			//if (drawOpt == "AL" || drawOpt == "Lsame") ERes->SetLineStyle(color_in+1);
			ERes->Draw(drawOpt);
			//			DiffSens->SetDirectory(0);
			//if (drawOpt != "AL" && drawOpt != "Lsame")leg->AddEntry(ERes, legend, "ple");

			return;
		}

	}

	f->Close();
	delete f;

	return;

}


void draw_crab(Float_t crabunits = 0.1, TLegend * leg) {
	TString specstrDef =
			"[0]*(6.0e-10)*pow(((10**x)/.3),(-2.31+(-0.26*TMath::Log10((10**x)/.3))))"; //Crab Nebula (MAGIC paper) with [0]=1.

	TF1 *origSpec = new TF1("origSpec", specstrDef, -1.9, 2.7);
	origSpec->SetNpx(100);
	origSpec->FixParameter(0, crabunits);
	origSpec->SetLineWidth(1);
	origSpec->SetLineColor(1);

	TF1 *origSpecE2 = new TF1("origSpecE2",
			"origSpec*(10**x)*(10**x)*1.602176487", -1.9, 2.7);
	origSpecE2->SetNpx(100);
	origSpecE2->SetLineWidth(1);
	origSpecE2->SetLineColor(1);
	origSpecE2->SetLineStyle(3);
	if (crabunits == 1)
		leg->AddEntry(origSpecE2, "Crab 100%, 10%, 1%", "l");
	origSpecE2->Draw("same");

}

void draw_req_curve(TString hemisphere = "S", TLegend * leg, TString time) {

	Int_t Npoints = 100;

	TString legendString;

	TH1F* h_0 = new TH1F("Requirements", "Req", 100, -1.9, 2.5);

	if (hemisphere == "S")
		for (int i = 0; i < Npoints; i++){
			if (time == "50"){
				h_0->SetBinContent(i + 1, Flux_req50_E2erg_south(pow(10, h_0->GetBinCenter(i + 1))));
			} else if(time == "5"){
				h_0->SetBinContent(i + 1, Flux_req5_E2erg_south(pow(10, h_0->GetBinCenter(i + 1))));
			} else if (time == "0.5"){
				h_0->SetBinContent(i + 1, Flux_req05_E2erg_south(pow(10, h_0->GetBinCenter(i + 1))));
			}
		}
	else if (hemisphere == "N")
		for (int i = 0; i < Npoints; i++){
			if (time == "50"){
				h_0->SetBinContent(i + 1, Flux_req50_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
			} else if(time == "5"){
				h_0->SetBinContent(i + 1, Flux_req5_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
			} else if (time == "0.5"){
				h_0->SetBinContent(i + 1, Flux_req05_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
			}
			//Flux_req50_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
		}
	legendString = Form("CTA-%s Requirement (%sh)", hemisphere.Data(), time.Data());

	h_0->SetLineStyle(9);
	leg->AddEntry(h_0, legendString, "l");
	h_0->Draw("samel");

}

void draw_goal_curve(TString hemisphere = "S", TLegend * leg, TString time) {

	Int_t Npoints = 100;

	TString legendString;

	TH1F* h_0 = new TH1F("Goal", "goal", 100, -1.9, 2.5);

	if (hemisphere == "S")
		for (int i = 0; i < Npoints; i++){
			if (time == "50"){
				h_0->SetBinContent(i + 1, Flux_goal50_E2erg_south(pow(10, h_0->GetBinCenter(i + 1))));
			} else if(time == "5"){
				h_0->SetBinContent(i + 1, Flux_goal5_E2erg_south(pow(10, h_0->GetBinCenter(i + 1))));
			} else if (time == "0.5"){
				h_0->SetBinContent(i + 1, Flux_goal05_E2erg_south(pow(10, h_0->GetBinCenter(i + 1))));
			}
		}

	else if (hemisphere == "N")
		for (int i = 0; i < Npoints; i++){
			if (time == "50"){
				h_0->SetBinContent(i + 1, Flux_goal50_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
			} /*else if(time == "5"){
				h_0->SetBinContent(i + 1, Flux_goal5_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
			} else if (time == "0.5"){
				h_0->SetBinContent(i + 1, Flux_goal05_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
			}*/
			//Flux_goal50_E2erg_north(pow(10, h_0->GetBinCenter(i + 1))));
		}


	legendString = Form("CTA-%s Goal (%sh)", hemisphere.Data(), time.Data());

	h_0->SetLineStyle(9);
	leg->AddEntry(h_0, legendString, "l");
	h_0->Draw("samel");

}

void CTAplot_Summary_file(TString filename = "", TString drawOption, Int_t colorIn, TCanvas *c1, TString layout){

	TPad *pad1 = (TPad*)c1->GetPrimitive("pad1");
	TPad *pad2 = (TPad*)c1->GetPrimitive("pad2");
	TPad *pad3 = (TPad*)c1->GetPrimitive("pad3");
	TPad *pad4 = (TPad*)c1->GetPrimitive("pad4");
	TPad *pad5 = (TPad*)c1->GetPrimitive("pad5");
	TPad *pad6 = (TPad*)c1->GetPrimitive("pad6");

	//	TH1F* h_ref = (TH1F*)c1->GetPrimitive("h_ref");

	TString arrayName = layout;

	//	if (colorIn==1) arrayName = arrayName + " Mars";
	//	else arrayName = arrayName + " evndisplay";


	/*

	 PAD 1 - Differential sensitivity

	 */

	pad1->cd();

	TLegend *leg1 = (TLegend*)c1->GetPrimitive("leg1");

	if (drawOption != "same" && drawOption != "SAME"){
		ref_filename = filename;
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "AL", colorIn,leg1);
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "PEsame", colorIn,leg1);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "Lsame", colorIn,leg1);
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "PEsame", colorIn,leg1);
	}


	/*

	 PAD 2 - Differential sensitivity ratio

	 */
	pad2->cd();
	TLegend *leg2 = (TLegend*)c1->GetPrimitive("leg2");


	if (drawOption != "same" && drawOption != "SAME"){
		TH1F * th1 = CTAgetHistogram(filename,"DiffSens");
		th1->SetTitle("Differential sensitivity ratios");
		th1->Divide(th1);
		th1->GetYaxis()->SetTitle("");
		th1->GetYaxis()->SetRangeUser(0,2);
		th1->SetLineWidth(2);
		th1->SetLineColor(colorIn);
		leg2->AddEntry(th1,arrayName,"le");
		th1->Draw();
	} else {
		TH1F * th_ref = CTAgetHistogram(ref_filename,"DiffSens");
		//    	th_ref->Print("ALL");
		TH1F * th1 = CTAgetHistogram(filename,"DiffSens");
		//th1->Divide(th_ref);
		th1->Divide(th_ref,th1);
		th1->SetName("ref_sensi");
		th1->GetYaxis()->SetTitle("");
		th1->GetYaxis()->SetRangeUser(0,2);
		th1->SetLineWidth(2);
		th1->SetLineColor(colorIn);
		leg2->AddEntry(th1,arrayName,"le");
		th1->Draw("same");

	}


	/*

	 PAD 3 - Angular resolution

	 */

	pad3->cd();
	TLegend *leg3 = (TLegend*)c1->GetPrimitive("leg3");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "AL", colorIn,leg3);
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "PEsame", colorIn,leg3);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "Lsame", colorIn,leg3);
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "PEsame", colorIn,leg3);
	}

	/*

	 PAD 4 - Effective Area

	 */

	pad4->cd();
	TLegend *leg4 = (TLegend*)c1->GetPrimitive("leg4");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "AL", colorIn,leg4);
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "PEsame", colorIn,leg4);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "Lsame", colorIn,leg4);
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "PEsame", colorIn,leg4);
	}

	/*

	 PAD 5 - Background Rate

	 */

	pad5->cd();
	TLegend *leg5 = (TLegend*)c1->GetPrimitive("leg5");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "AL", colorIn,leg5);
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "PEsame", colorIn,leg5);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "Lsame", colorIn,leg5);
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "PEsame", colorIn,leg5);
	}


	/*

	 PAD 6 - Energy Resolution

	 */

	pad6->cd();
	TLegend *leg6 = (TLegend*)c1->GetPrimitive("leg6");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "AL", colorIn,leg6);
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "PEsame", colorIn,leg6);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "Lsame", colorIn,leg6);
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "PEsame", colorIn,leg6);
	}



}


void CTAplot_Summary_file_legend(TString filename = "", TString drawOption, Int_t colorIn, TString legend, TCanvas *c1, TString layout){

	TPad *pad1 = (TPad*)c1->GetPrimitive("pad1");
	TPad *pad2 = (TPad*)c1->GetPrimitive("pad2");
	TPad *pad3 = (TPad*)c1->GetPrimitive("pad3");
	TPad *pad4 = (TPad*)c1->GetPrimitive("pad4");
	TPad *pad5 = (TPad*)c1->GetPrimitive("pad5");
	TPad *pad6 = (TPad*)c1->GetPrimitive("pad6");

	TString arrayName = layout;//findArrayName(filename);

	/*

	 PAD 1 - Differential sensitivity

	 */

	pad1->cd();
	arrayName=legend;

	TLegend *leg1 = (TLegend*)c1->GetPrimitive("leg1");

	if (drawOption != "same" && drawOption != "SAME"){
		ref_filename = filename;
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "AL", colorIn,leg1);
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "PEsame", colorIn,leg1);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "Lsame", colorIn,leg1);
		CTAdrawHistogramTGraph(filename, arrayName,"DiffSens", "PEsame", colorIn,leg1);
	}


	/*

	 PAD 2 - Differential sensitivity ratio

	 */
	pad2->cd();
	TLegend *leg2 = (TLegend*)c1->GetPrimitive("leg2");

	if (drawOption != "same" && drawOption != "SAME"){
		TH1F * th1 = CTAgetHistogram(filename,"DiffSens");
		th1->SetTitle("Differential sensitivity ratios");
		th1->GetYaxis()->SetTitle("");
		th1->Divide(th1);
		th1->GetYaxis()->SetRangeUser(0,2);
		th1->SetLineWidth(2);
		th1->SetLineColor(colorIn);
		leg2->AddEntry(th1,arrayName,"le");
		th1->Draw();
	} else {
		TH1F * th_ref = CTAgetHistogram(ref_filename,"DiffSens");
		TH1F * th1 = CTAgetHistogram(filename,"DiffSens");
		th1->Divide(th_ref);
		th1->SetName("ref_sensi");
		th1->GetYaxis()->SetRangeUser(0,2);
		th1->SetLineWidth(2);
		th1->SetLineColor(colorIn);
		leg2->AddEntry(th1,arrayName,"le");
		th1->Draw("same");

	}


	/*

	 PAD 3 - Angular resolution

	 */

	pad3->cd();
	TLegend *leg3 = (TLegend*)c1->GetPrimitive("leg3");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "AL", colorIn,leg3);
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "PEsame", colorIn,leg3);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "Lsame", colorIn,leg3);
		CTAdrawHistogramTGraph(filename, arrayName,"AngRes", "PEsame", colorIn,leg3);
	}

	/*

	 PAD 4 - Effective Area

	 */

	pad4->cd();
	TLegend *leg4 = (TLegend*)c1->GetPrimitive("leg4");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "AL", colorIn,leg4);
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "PEsame", colorIn,leg4);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "Lsame", colorIn,leg4);
		CTAdrawHistogramTGraph(filename, arrayName,"EffArea", "PEsame", colorIn,leg4);
	}

	/*

	 PAD 5 - Background Rate

	 */

	pad5->cd();
	TLegend *leg5 = (TLegend*)c1->GetPrimitive("leg5");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "AL", colorIn,leg5);
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "PEsame", colorIn,leg5);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "Lsame", colorIn,leg5);
		CTAdrawHistogramTGraph(filename, arrayName,"BGRate", "PEsame", colorIn,leg5);
	}


	/*

	 PAD 6 - Energy Resolution

	 */

	pad6->cd();
	TLegend *leg6 = (TLegend*)c1->GetPrimitive("leg6");


	if (drawOption != "same" && drawOption != "SAME"){
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "AL", colorIn,leg6);
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "PEsame", colorIn,leg6);
	} else {
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "Lsame", colorIn,leg6);
		CTAdrawHistogramTGraph(filename, arrayName,"ERes", "PEsame", colorIn,leg6);
	}



}

void CTAplot_Summary_rate(TString direction="average", Int_t layouts=2, TString obsTime="50", TString array1, TString array2, TString array3, TString array4){


	TCanvas *c1 = new TCanvas ("c1", "c1", 1366,700);

	TPad *pad1 = new TPad("pad1", "pad1",0.0,0.4,0.66,1.0,0);			// DiffSens
	pad1->Draw();
	if (obsTime == "0.5") TLegend *leg1 = new TLegend(0.28,0.54,0.68,0.89);
	else if (obsTime == "5") TLegend *leg1 = new TLegend(0.442976,0.52,0.8431229,0.88);
	else TLegend *leg1 = new TLegend(0.442976,0.5088454,0.8431229,0.8649907);
	leg1->SetName("leg1");
	leg1->SetFillColor(10);
	leg1->SetBorderSize(1);
	leg1->SetTextFont(131);
	leg1->SetTextSize(0.05135388);
	leg1->Draw();
	TPad *pad2 = new TPad("pad2", "pad2",0.,0.,0.33,0.4,0);			// DiffSens ratios
	pad2->Draw();
	TLegend *leg2 = new TLegend(0.3043478,0.4661191,0.7851171,0.6723966);
	leg2->SetName("leg2");
	leg2->SetFillColor(10);
	leg2->SetBorderSize(1);
	leg2->SetTextFont(131);
	leg2->SetTextSize(0.03813435);
	leg2->Draw();
	TPad *pad3 = new TPad("pad3", "pad3",0.33,0.0,0.66,0.4,0);				// ang res
	pad3->Draw();
	TLegend *leg3 = new TLegend(0.63,0.77,0.88,0.88);
	leg3->SetName("leg3");
	leg3->SetFillColor(10);
	leg3->SetBorderSize(1);
	leg3->SetTextFont(131);
	leg3->SetTextSize(0.065);
	leg3->Draw();
	TPad *pad4 = new TPad("pad4", "pad4",0.66,0.6666666,1.,1.,0);			// eff area
	pad4->Draw();
	TLegend *leg4 = new TLegend(0.452401,0.2268909,0.8104465,0.5420169);
	leg4->SetName("leg4");
	leg4->SetFillColor(10);
	leg4->SetBorderSize(1);
	leg4->SetTextFont(131);
	leg4->SetTextSize(0.07563024);
	//leg4->Draw();
	TPad *pad5 = new TPad("pad5", "pad5",0.66,0.3333333,1.,0.6666666,0);			// background rate
	pad5->Draw();
	TLegend *leg5 = new TLegend(0.5,0.55,0.85,0.89);
	leg5->SetName("leg5");
	leg5->SetFillColor(10);
	leg5->SetBorderSize(1);
	leg5->SetTextFont(131);
	leg5->SetTextSize(0.07563024);
	//leg5->Draw();
	TPad *pad6 = new TPad("pad6", "pad6",0.66,0.0000000,1.,0.3333333,0);				// en. res
	pad6->Draw();
	TLegend *leg6 = new TLegend(0.63,0.77,0.88,0.88);
	leg6->SetName("leg6");
	leg6->SetFillColor(10);
	leg6->SetBorderSize(1);
	leg6->SetTextFont(131);
	leg6->SetTextSize(0.07563024);
	leg6->Draw();


	TString layout = array1;
	TString filename = Form("/nfs/cta-ifae/thassan/Prod3/IRFs/Paranal/Prod3b/Subarray%s_IFAE_average_%shours_v1.root", layout.Data(), obsTime.Data());
	CTAplot_Summary_file(filename, "",1, c1, layout);

	layout = array2;
	filename = "/nfs/cta-ifae/thassan/Prod3/IRFs/Paranal/Prod3b/Subarray3HB9-LST_IFAE_average_50hours_v1.root";
	CTAplot_Summary_file(filename, "same",2, c1, layout);


	layout = array3;
	filename ="/nfs/cta-ifae/thassan/Prod3/IRFs/Paranal/Subarray3HB89-TS-BB2L-ND_IFAE_average_0.5hours_version2.root";
	if(layouts>2) CTAplot_Summary_file(filename, "same",3, c1, layout);

	layout = array4;
	filename = Form("/nfs/cta-ifae/thassan/Prod3/IRFs/Paranal/Prod3b/Subarray%s_IFAE_average_%shours_v1.root", layout.Data(), obsTime.Data());
	if(layouts>3) CTAplot_Summary_file(filename, "same",6, c1, layout);



	pad1->cd();
	draw_crab(1.0, leg1);
	draw_crab(0.1, leg1);
	draw_crab(0.01, leg1);
	draw_req_curve("S", leg1, obsTime);
	if (obsTime == "50") draw_goal_curve("S", leg1, obsTime);
	leg1->Draw();

	pad3->cd();
	ctareqangres(leg3);
	leg3->Draw();

	pad4->cd();

	pad5->cd();

	pad6->cd();
	ctareqeres(leg6);
	leg6->Draw();

	TString outputfilename = Form("/nfs/cta-ifae/pcumani/%s_%s_average_%s.png", array1.Data(), array2.Data(),obsTime.Data());
	if(layouts == 3) outputfilename = Form("/nfs/cta-ifae/pcumani/%s_%s_%s_average_%s.png", array1.Data(), array2.Data(), array3.Data(),obsTime.Data());
	if(layouts == 4) outputfilename = Form("/nfs/cta-ifae/pcumani/%s_%s_%s_%s_average_%s.png", array1.Data(), array2.Data(), array3.Data(), array4.Data(), obsTime.Data());
	c1->SaveAs(outputfilename);
}

void CTAplot_Summary(){

	CTAplot_Summary_rate("average", 2, "50", "3AL4M00 South (20.9 uT)", "3HB89-LST Av. (20.1 uT)", "3HB89-TS-BB2L-ND", "3HB9-ND");
	CTAplot_Summary_rate("average", 2, "0.5", "3AL4M00", "3HB9-LST", "3HB9-SST-G", "3HB89-SST-D");
	//CTAplot_Summary_rate("average", 2, "50", "3HB9-SST-D", "3HB9-SST-ASTRI", "3HB9-SST-G", "3HB89-SST-D");
	CTAplot_Summary_rate("average", 4, "50", "3HB9-ND", "3HB9-TS-AA-ND", "3HB9-TS-BA-ND", "3HB9-TS-CA-ND");
	CTAplot_Summary_rate("average", 4, "50", "3HB9-ND", "3HB9-TS-AB-ND", "3HB9-TS-BB-ND", "3HB9-TS-CB-ND");
	//CTAplot_Summary_rate("average", 3, "50", "3HB9-ND", "3HB9-TS-AB-ND", "3HB9-TS-AA-ND", "3HB9-TS-CB-ND");

}
