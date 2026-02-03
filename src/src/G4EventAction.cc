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

            G4int volname = -1;
            if      (volumeName == "Prisms_M0") volname = 2;
            else if (volumeName == "Prisms_M1") volname = 0;
            else if (volumeName == "Prisms_M2") volname = 3;
            else if (volumeName == "Prisms_M3") volname = 1;
            
            //primary pixel energy
            //secondary energy
            G4double ePrim = 0.0, eSec = 0.0;
            if (auto vp = accumulatedEnergy_Primary.find(volumeName); vp != accumulatedEnergy_Primary.end()) {
                if (auto ip = vp->second.find(pixelID); ip != vp->second.end()) ePrim = ip->second;
            }
            if (auto vs = accumulatedEnergy_Secondary.find(volumeName); vs != accumulatedEnergy_Secondary.end()) {
                if (auto is = vs->second.find(pixelID); is != vs->second.end()) eSec = is->second;
            }


            G4double eSecElectron = 0.0;
            if (auto ve = accumulatedEnergy_e.find(volumeName); ve != accumulatedEnergy_e.end()) {
                if (auto ie = ve->second.find(pixelID); ie != ve->second.end()) eSecElectron = ie->second;
            }
            
            G4double eSecGamma = 0.0; 
            if (auto vg = accumulatedEnergy_gamma.find(volumeName); vg != accumulatedEnergy_gamma.end()) {
                if (auto ig = vg->second.find(pixelID); ig != vg->second.end()) eSecGamma = ig->second;
            }


            const bool hasPrim = (ePrim > 0.0);
            const bool hasSec  = (eSec  > 0.0);

            const G4double primaryPixelEnergy   = hasPrim ? energyDeposit : 0.0;
            const G4double secondaryOnlyEnergy  = (!hasPrim && hasSec) ? energyDeposit : 0.0;
            const G4double primaryOnlyEnergy  = (hasPrim && !hasSec) ? energyDeposit : 0.0;

            //position y and z
            G4double y = 0.0, z = 0.0;
            if (auto vy = positionY.find(volumeName); vy != positionY.end()) {
                if (auto iy = vy->second.find(pixelID); iy != vy->second.end()) y = iy->second;
            }
            if (auto vz = positionZ.find(volumeName); vz != positionZ.end()) {
                if (auto iz = vz->second.find(pixelID); iz != vz->second.end()) z = iz->second;
            }
            analysisManager->FillNtupleIColumn(0, 0, eventID);
            analysisManager->FillNtupleIColumn(0, 1, pixelID);
            analysisManager->FillNtupleIColumn(0, 2, volname);
            analysisManager->FillNtupleDColumn(0, 3, energyDeposit / CLHEP::MeV);
            analysisManager->FillNtupleDColumn(0, 4, primaryPixelEnergy/ CLHEP::MeV);
            analysisManager->FillNtupleDColumn(0, 5, primaryOnlyEnergy/ CLHEP::MeV);
            analysisManager->FillNtupleDColumn(0, 6, secondaryOnlyEnergy/ CLHEP::MeV);
            analysisManager->FillNtupleDColumn(0, 7, eSecElectron / CLHEP::MeV); 
            analysisManager->FillNtupleDColumn(0, 8, eSecGamma / CLHEP::MeV );  
            analysisManager->FillNtupleDColumn(0, 9, y/CLHEP::cm);
            analysisManager->FillNtupleDColumn(0, 10, z/CLHEP::cm);
            
            
            analysisManager->AddNtupleRow(0);
        }
    }

    accumulatedEnergy_Primary.clear();
    accumulatedEnergy_Secondary.clear();
    accumulatedEnergy.clear();
    accumulatedEnergy_e.clear();       
    accumulatedEnergy_gamma.clear();
    positionY.clear();
    positionZ.clear();
}

//================================================================================
