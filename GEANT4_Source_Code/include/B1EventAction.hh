//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1EventAction.hh
/// \brief Definition of the B1EventAction class

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>
#include <array>
class B1RunAction;

/// Event action class
///
const G4int kDim = 4;
class B1EventAction : public G4UserEventAction
{
  public:
    B1EventAction();
    virtual ~B1EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddEdep(G4double edep) { fEdep += edep; }

    void AddNeutron(){fNneutrons+=1;}
    void AddGamma(){fNgammas+=1;}

    void SetIsFission(bool b){fIsFission = b;}
    bool GetIsFission(){return fIsFission;}

    void SetDecayTime(G4double time) { fissionTime =time; }
    G4double GetDecayTime() { return fissionTime; }

    std::vector<G4double>& GetGammaVectors(int i){return fGamma[i];}
    std::vector<G4double>& GetNeutronVectors(int i){return fNeutron[i];}
  private:
    G4double     fEdep;

    std::array<std::vector<G4double>,4> fNeutron;
    std::array<std::vector<G4double>,4> fGamma;

    // hit collections Ids
	std::array<G4int, kDim> fScintHCID;
	G4int fNneutrons;
	G4int fNgammas;
	bool fIsFission;
	G4double fissionTime;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
