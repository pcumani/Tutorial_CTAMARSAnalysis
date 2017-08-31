//
// This macro check if a root file has been made a Zombie
//
// Author: Paolo Cumani, Tarek Hassan, Abelardo Moralejo

#include<TString.h>
#include<TFile.h>

using namespace std;

int CheckFileZombie(TString filename){
	TFile f(filename);
	if (f.IsZombie()) return 1;
  else if (f.TestBit(TFile::kRecovered)) return 1;
	else return 0;
}
