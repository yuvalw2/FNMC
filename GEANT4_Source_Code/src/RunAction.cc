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
/// \file B1RunAction.cc
/// \brief Implementation of the B1RunAction class

#include <RunAction.hh>
#include "B1PrimaryGeneratorAction.hh"
#include "B1DetectorConstruction.hh"
// #include "B1Run.hh"
#include "B1Analysis.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "B1EventAction.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1RunAction::B1RunAction(B1EventAction *eventAction) :
		G4UserRunAction(), fEdep(0.), fEdep2(0.), fEventAction(eventAction)
{
	// add new units for dose
	//
	const G4double milligray = 1.e-3 * gray;
	const G4double microgray = 1.e-6 * gray;
	const G4double nanogray = 1.e-9 * gray;
	const G4double picogray = 1.e-12 * gray;

	new G4UnitDefinition("milligray", "milliGy", "Dose", milligray);
	new G4UnitDefinition("microgray", "microGy", "Dose", microgray);
	new G4UnitDefinition("nanogray", "nanoGy", "Dose", nanogray);
	new G4UnitDefinition("picogray", "picoGy", "Dose", picogray);

	// Register accumulable to the accumulable manager
	G4AccumulableManager *accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->RegisterAccumulable(fEdep);
	accumulableManager->RegisterAccumulable(fEdep2);

	auto analysisManager = G4AnalysisManager::Instance();
	//analysisManager->SetNtupleMerging(true);
	// Note: merging ntuples is available only with Root output
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetFileName("rossi");

	// Creating ntuple
	//0
	analysisManager->CreateNtuple("rossi", "Hits");

	std::vector<std::string> names =
	{ "Time", "Energy", "Id", "Track ID" };
	for (int i = 0; i < names.size(); i++)
	{
		analysisManager->CreateNtupleDColumn(names[i] + " Gamma", eventAction->GetGammaVectors(i));
	}
	for (int i = 0; i < names.size(); i++)
	{
		analysisManager->CreateNtupleDColumn(names[i] + " Neutron",
				eventAction->GetNeutronVectors(i));
	}
	analysisManager->FinishNtuple();
	//1
	analysisManager->CreateNtuple("Neutron Mul", "NeutronM");
	analysisManager->CreateNtupleDColumn("M");
	analysisManager->FinishNtuple();
	//2
	analysisManager->CreateNtuple("Gamma Mul", "GammaM");
	analysisManager->CreateNtupleDColumn("M");
	analysisManager->FinishNtuple();
	//3
	analysisManager->CreateNtuple("Gamma E", "Eg");
	analysisManager->CreateNtupleDColumn("E");
	analysisManager->FinishNtuple();
//4
	analysisManager->CreateNtuple("Neutron E", "En");
	analysisManager->CreateNtupleDColumn("E");
	analysisManager->FinishNtuple();

//5
	analysisManager->CreateNtuple("Neutron Occ", "En");
	analysisManager->CreateNtupleIColumn("Event ID");
	analysisManager->CreateNtupleDColumn("E");
	analysisManager->FinishNtuple();
	// 6
	analysisManager->CreateNtuple("Prompt decay gamma", "gamma");
	analysisManager->CreateNtupleIColumn("Event ID");
	analysisManager->CreateNtupleDColumn("E");
	analysisManager->CreateNtupleDColumn("ParentID");
	analysisManager->FinishNtuple();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1RunAction::~B1RunAction()
{
	delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::BeginOfRunAction(const G4Run*)
{
	// inform the runManager to save random number seed
	G4RunManager::GetRunManager()->SetRandomNumberStore(false);

	// reset accumulables to their initial values
	G4AccumulableManager *accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Reset();

	auto analysisManager = G4AnalysisManager::Instance();

	// Open an output file
	// it can be overwritten in a macro
	analysisManager->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::EndOfRunAction(const G4Run *run)
{
	G4int nofEvents = run->GetNumberOfEvent();
	if (nofEvents == 0)
		return;

	// Merge accumulables
	G4AccumulableManager *accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Merge();

	// Compute dose = total energy deposit in a run and its variance
	//
	G4double edep = fEdep.GetValue();
	G4double edep2 = fEdep2.GetValue();

	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::AddEdep(G4double edep)
{
	fEdep += edep;
	fEdep2 += edep * edep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

