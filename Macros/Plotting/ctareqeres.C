//
// Energy resolution requirement (used in CTAplot_Summary.C)
//
// Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

void ctareqeres(TLegend * leg)
{

  Float_t loge[9] = {-1.50968, -1.18280, -0.898925, -0.623656, -0.374194, -0.0731183, 0.468817, 1.87957, 2.48172};

  Float_t eres[9] = {0.409753 , 0.279136 , 0.208766 , 0.159410 , 0.128203 , 0.0999461, 0.0999461, 0.0999461, 0.0999461};

  TGraph *ger = new TGraph(9, loge, eres);

  ger->SetLineColor(9);
  ger->SetLineStyle(2);
  ger->Draw("samel");

  leg->AddEntry(ger, "Requirement", "l");

  return;
}
