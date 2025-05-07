#include "G4MTRunManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4DetectorConstruction.hh"
#include "G4ActionInitialization.hh"
#include "G4PhysicsList.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


// ============================================================================

int main(int argc,char** argv) {
    
    // Seed the random number generator manually
//    G4long myseed = 345354;
    // G4long myseed = 3453540;
    // CLHEP::HepRandom::setTheSeed(myseed);

   
    G4double RIndex = 1;
    if (argc==2) {
      RIndex = atof(argv [1]);
    }
    if (argc==3) {
      RIndex = atof(argv [2]);
    }

    // ============================================================================
    // Run manager
    // this first step sets the number of cores available.
    G4MTRunManager* runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(4);
    runManager->SetVerboseLevel(0);

    // declares the geometry of the simulation
    G4DetectorConstruction* detConstruction = new G4DetectorConstruction(RIndex);
    runManager->SetUserInitialization(detConstruction);

    // declares the physics of the simulation
    G4VModularPhysicsList* physicsList = new FTFP_BERT();
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option1());
    G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();

    physicsList->RegisterPhysics(opticalPhysics);
    runManager->SetUserInitialization(physicsList);

    // declares the first action of the simulation
    G4ActionInitialization* actionInitialization = new G4ActionInitialization(detConstruction);
    runManager->SetUserInitialization(actionInitialization);
    
    // visualization manager
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
    
    // Initialize G4 kernel
    runManager->Initialize();
    
    // ============================================================================
    // User Interface
    
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    
    // Define UI session for interactive mode
    if (argc < 3) {

      G4UIExecutive * ui = new G4UIExecutive(argc,argv);

      UImanager->ApplyCommand("/control/execute vis.mac");

      ui->SessionStart();
      delete ui;

    } else if (argc == 3) {
      
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
      
    }
    
    // ============================================================================
    // Delete

    delete visManager;

    delete runManager;

    return 0;
}

// ============================================================================
