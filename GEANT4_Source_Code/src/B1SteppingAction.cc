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
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "B1Analysis.hh"
#include "G4SystemOfUnits.hh"
#include "G4Alpha.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(B1EventAction *eventAction) :
		G4UserSteppingAction(), fEventAction(eventAction), fScoringVolume(0)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step *step)
{
	auto particle = step->GetTrack()->GetParticleDefinition();
	auto analysisManager = G4AnalysisManager::Instance();

	if (particle == G4Alpha::Alpha()&&step->GetTrack()->GetParentID()==1)
	{
		fEventAction->SetIsFission(false);
		if(step->GetPreStepPoint()->GetPosition()==step->GetTrack()->GetVertexPosition())
		{
			fEventAction->SetDecayTime(step->GetTrack()->GetGlobalTime());
		}
		//G4RunManager::GetRunManager()->AbortEvent();
	}

	//Save Prompt Gammas/ Neutorns after 100 seconds of decay
	if (! fEventAction->GetIsFission() && (particle == G4Gamma::Gamma()))
	{
		if ((step->GetPreStepPoint()->GetGlobalTime()/s-fEventAction->GetDecayTime()/s)<100 && (step->IsFirstStepInVolume()))
		{
			analysisManager->FillNtupleIColumn(6, 0,
									G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
			analysisManager->FillNtupleDColumn(6, 1,
									step->GetPreStepPoint()->GetKineticEnergy() / MeV);
			analysisManager->FillNtupleDColumn(6, 2,
												step->GetTrack()->GetParentID());
							analysisManager->AddNtupleRow(6);
		}
	}

	if (step->IsFirstStepInVolume())
	{
		if (!fScoringVolume)
		{
			const B1DetectorConstruction *detectorConstruction =
					static_cast<const B1DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
			fScoringVolume = detectorConstruction->GetScoringVolume();
		}
		G4LogicalVolume *volume =
				step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

		if (volume == fScoringVolume)
		{

			if (particle == G4Neutron::Neutron())
			{
				fEventAction->AddNeutron();
				analysisManager->FillNtupleDColumn(4, 0,
						step->GetPreStepPoint()->GetKineticEnergy() / MeV);
				analysisManager->AddNtupleRow(4);

				analysisManager->FillNtupleIColumn(5, 0,
						G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
				analysisManager->FillNtupleDColumn(5, 1,
						step->GetPreStepPoint()->GetKineticEnergy() / MeV);
				analysisManager->AddNtupleRow(5);
			}
			if (particle == G4Gamma::Gamma())
			{
				fEventAction->AddGamma();
				analysisManager->FillNtupleDColumn(3, 0,
						step->GetPreStepPoint()->GetKineticEnergy() / MeV);
				analysisManager->AddNtupleRow(3);
			}

		}
	}
	/*

	 }

	 // get volume of the current step
	 G4LogicalVolume* volume
	 = step->GetPreStepPoint()->GetTouchableHandle()
	 ->GetVolume()->GetLogicalVolume();

	 // check if we are in scoring volume
	 if (volume != fScoringVolume) return;

	 // collect energy deposited in this step
	 G4double edepStep = step->GetTotalEnergyDeposit();
	 fEventAction->AddEdep(edepStep);  */

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

