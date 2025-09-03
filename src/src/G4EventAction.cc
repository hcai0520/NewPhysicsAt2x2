/// \file G4EventAction.cc
/// \brief Implementation of the G4EventAction class

#include "G4EventAction.hh"
#include "G4RunAction.hh"
#include "G4Constantes.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "MyGlobals.hh"

//================================================================================

G4EventAction::G4EventAction() : G4UserEventAction() {}


//================================================================================

G4EventAction::~G4EventAction() {}

//================================================================================

void G4EventAction::BeginOfEventAction(const G4Event* /*event*/) {
    
    nOfReflections = 0;
    nOfDetections = 0;
}

//================================================================================

void G4EventAction::EndOfEventAction(const G4Event* event) {
    
    // Print number of reflections
    if (IMPRIMIR_EVENTOS == true) {
        G4cout << "Number of reflections [Event] = " << nOfReflections << G4endl;
        G4cout << "Number of detections [Event] = " << nOfDetections << G4endl;
    }
    auto analysisManager = G4AnalysisManager::Instance();
    // Print per event (modulo n)
    //
    G4int eventID = event->GetEventID();
    G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
        G4cout << "---> End of event: " << eventID << G4endl;
    }
    for (const auto& volumeEntry : accumulatedEnergy) {
        const G4String& volumeName = volumeEntry.first;
        const std::map<G4int, G4double>& pixelMap = volumeEntry.second;

        for (const auto& entry : pixelMap) {
            G4int pixelID = entry.first;
            G4double energyDeposit = entry.second; // In MeV

     
            analysisManager->FillNtupleIColumn(0, 0, eventID);
            analysisManager->FillNtupleIColumn(0, 1, pixelID);
            if (volumeName == "Prisms_M0" )
            {
                analysisManager->FillNtupleIColumn(0, 2, 2);
            }
            if (volumeName == "Prisms_M1" )
            {
                analysisManager->FillNtupleIColumn(0, 2, 0);
            }
             if (volumeName == "Prisms_M2" )
            {
                analysisManager->FillNtupleIColumn(0, 2, 3);
            }
             if (volumeName == "Prisms_M3" )
            {
                analysisManager->FillNtupleIColumn(0, 2, 1);
            }
            analysisManager->FillNtupleDColumn(0, 3, energyDeposit / CLHEP::MeV);

            if(accumulatedEnergy_Primary[volumeName][pixelID] > 0)
            {
                analysisManager->FillNtupleDColumn(0, 4, energyDeposit/ CLHEP::MeV);
            }
            if(accumulatedEnergy_Primary[volumeName][pixelID] == 0 and accumulatedEnergy_Secondary[volumeName][pixelID] >0){
                analysisManager->FillNtupleDColumn(0, 5, accumulatedEnergy_Secondary[volumeName][pixelID]/ CLHEP::MeV);
            }
            analysisManager->FillNtupleDColumn(0, 6, positionY[volumeName][pixelID]/CLHEP::cm);
            analysisManager->FillNtupleDColumn(0, 7, positionZ[volumeName][pixelID]/CLHEP::cm);
            
            analysisManager->AddNtupleRow(0);
        }
    }

    accumulatedEnergy_Primary.clear();
    accumulatedEnergy_Secondary.clear();
    accumulatedEnergy.clear();
    positionY.clear();
    positionZ.clear();
    //primaryVolumes.clear();
    //primaryCopyNos.clear();
    //secVolumes.clear();
    //secCopyNos.clear();
}

//================================================================================
