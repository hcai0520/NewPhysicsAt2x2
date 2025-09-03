#include "G4SteppingAction.hh"
#include "G4Constantes.hh"
#include "G4PrimaryGeneratorAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"

#include "G4SystemOfUnits.hh"

/* Alteracao 5 5 17 */
#include "G4ios.hh"
#include "G4GeometryTolerance.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4PhysicalConstants.hh"
#include "G4AnalysisManager.hh"
// */
#include "MyGlobals.hh"


//Output TXT file
#include <fstream>
using namespace std;
// Creates an output file stream to write to the TXT file

// Conversion from radians to degrees.
#define RAD_TO_DEG 180/pi
//================================================================================
//eventAction: A pointer to the event-level action handler.
//runAction: A pointer to the run-level action handler.
//PrimaryGeneratorAction: A pointer to the primary particle generator.
G4SteppingAction::G4SteppingAction( G4EventAction *eventAction, G4RunAction* runAction, G4PrimaryGeneratorAction* PrimaryGeneratorAction)
: G4UserSteppingAction (), fEventAction (eventAction), fRunAction (runAction), fPrimaryGeneratorAction (PrimaryGeneratorAction) {

    fEventNumber = -1;//Initialize the event number -1, no event has been processed yet.

}

G4SteppingAction::~G4SteppingAction() { ; }

//================================================================================

void G4SteppingAction::UserSteppingAction (const G4Step* aStep) {

    fEventNumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    //============================================================================
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    
    // Step
    G4Track* track = aStep->GetTrack();
    G4String parti = track->GetDefinition()->GetParticleName();
    G4String materia = track->GetMaterial()->GetName();

    G4double step_x = aStep->GetTrack()->GetPosition().x();
    G4double step_y = aStep->GetTrack()->GetPosition().y();
    G4double step_z = aStep->GetTrack()->GetPosition().z();
    
    const G4VProcess* creator = track->GetCreatorProcess();

    G4double momentum = track->GetMomentum().mag();

    const G4DynamicParticle* dynParticle = track -> GetDynamicParticle();
    G4double kinEnergy = dynParticle -> GetKineticEnergy(); 
  
    //PreStep Info
    G4StepPoint * aPrePoint = aStep->GetPreStepPoint();
    G4VPhysicalVolume * aPrePV = aPrePoint->GetPhysicalVolume();
    G4String PreVolName = "";
    if (aPrePV) PreVolName = aPrePV->GetName();
    G4int PreCopyNo = aPrePoint->GetTouchableHandle()->GetCopyNumber();

    //PostStep Info
    G4StepPoint * aPostPoint = aStep->GetPostStepPoint();
    G4VPhysicalVolume * aPostPV = aPostPoint->GetPhysicalVolume();
    G4String PostVolName = "";
    if (aPostPV) PostVolName= aPostPV->GetName();
    G4int PostCopyNo = aPostPoint->GetTouchableHandle()->GetCopyNumber();

    
    G4int evtNb = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

    
    G4ThreeVector   Normal          =   G4ThreeVector(0 * m, 0 * m, 1 * m);
    G4ThreeVector   direction       =   track->GetMomentumDirection();

    G4double        CosAngle        =   direction.dot(Normal)/(direction.mag()*Normal.mag());

    G4double        direction_x     =   direction.getX();
    G4double        direction_y     =   direction.getY();
    G4double        direction_z     =   direction.getZ();
    G4double        stepLength = aStep->GetStepLength();
    //energy deposite in current step
    G4double        EDep            =   aStep-> GetTotalEnergyDeposit();
    //if (EDep == 0.) return;
    // skip world


    if (!aPrePV  || PreVolName == MUNDO_NOME) return; 
    if (!aPostPV || PostVolName == MUNDO_NOME) return;

    G4ThreeVector centerPos  = aPrePoint->GetTouchableHandle()->GetHistory()->GetTransform(0).TransformPoint(aPrePoint->GetPosition());
    
    accumulatedEnergy[PreVolName][PreCopyNo] += EDep;
    positionY[PreVolName][PreCopyNo] = centerPos.getY();
    positionZ[PreVolName][PreCopyNo] = centerPos.getZ();
    if (track->GetParentID() == 0){
        accumulatedEnergy_Primary[PreVolName][PreCopyNo] += EDep;

    } else{
        accumulatedEnergy_Secondary[PreVolName][PreCopyNo] += EDep;

    }
    if (stepLength <= 0) return; 

    G4double dedx = EDep / (stepLength / cm);
    
    if (PostVolName == "")
    {
        track->SetTrackStatus(fStopAndKill);
    }

    //G4int parentID = track->GetParentID();
    //G4String particleName = track->GetDefinition()->GetParticleName();

    //if ((parti == "mu+" || parti == "millicharged") && track->GetParentID() == 0)
    //if (PreCopyNo != PostCopyNo && PostVolName != ""&& PreVolName != MUNDO_NOME)
    //{   //void FillNtupleDColumn(G4int ntupleId, G4int columnId, G4double value);
        analysisManager->FillNtupleIColumn(1,0,fEventNumber);
        analysisManager->FillNtupleDColumn(1,1,stepLength/cm);
        analysisManager->FillNtupleDColumn(1,2,step_y/cm);
        analysisManager->FillNtupleDColumn(1,3,step_z/cm);
        analysisManager->FillNtupleDColumn(1,4,track->GetGlobalTime()/ns);
        analysisManager->FillNtupleDColumn(1,5,dedx);
        analysisManager->FillNtupleDColumn(1,6,acos(CosAngle)*180/3.1415);
        if (PreVolName == "Prisms_M0" )
        {
            analysisManager->FillNtupleIColumn(1, 7, 2);
        }
        if (PreVolName == "Prisms_M1" )
        {
            analysisManager->FillNtupleIColumn(1, 7, 0);
        }
        if (PreVolName == "Prisms_M2" )
        {
            analysisManager->FillNtupleIColumn(1, 7, 3);
        }
        if (PreVolName == "Prisms_M3" )
        {
            analysisManager->FillNtupleIColumn(1, 7, 1);
        }
        analysisManager->FillNtupleDColumn(1,8,direction_y);
        analysisManager->FillNtupleDColumn(1,9,direction_z);      
        analysisManager->FillNtupleDColumn(1,10,momentum/GeV);
        analysisManager->AddNtupleRow(1);
        
    //}
       
}

//================================================================================
//each step check if the step in the interface or same volume (across the volume ?)
//energy deposition in that vloume
//implement the changes to the stepping action to get the energy deposited in the previous volume
//write a quick root script to display it as a 2d histogram (or other options?)

