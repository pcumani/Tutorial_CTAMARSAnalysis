//
// Angular resolution requirement (used in CTAplot_Summary.C)
//
// Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

#include <Rtypes.h>
#include <TGraph.h>
#include <TLegend.h>

void ctareqangres(TLegend * leg)
{
  Float_t loge[10] = {  -1.64983, -1.49475, -1.32191, -1.05307, -0.522689, 0.139036, 0.949169, 1.67254,  2.20447, 2.49232};

  Float_t angres[10] = {0.453339 , 0.295111 , 0.203515 , 0.138619 , 0.0858772, 0.0569610, 0.0372988, 0.0274391, 0.0232297, 0.0216182};

  TGraph* gang = new TGraph(10, loge, angres);

  gang->SetLineColor(9);
  gang->SetLineStyle(2);

  gang->Draw("samec");
  leg->AddEntry(gang, "Requirement", "l");

}
