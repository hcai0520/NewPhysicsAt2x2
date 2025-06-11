#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoE.hh"
#include "G4OpticalPhoton.hh"
#include "G4GenericIon.hh"


#include "MyPhysicsList.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "G4ProcessManager.hh"
#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4MuIonisation.hh"
#include "G4hIonisation.hh"
#include "G4CoulombScattering.hh"


#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4AntiProton.hh"


MyPhysicsList::MyPhysicsList() {
    RegisterPhysics(new G4EmStandardPhysics_option4());
    RegisterPhysics(new G4OpticalPhysics());
}

void MyPhysicsList::DefineMillichargedParticle() {
    if (!G4ParticleTable::GetParticleTable()->FindParticle("millicharged")) {
        new G4ParticleDefinition(
            "millicharged",    // name
            105.7*MeV,           // mass
            0.0*MeV,           // width
            1 * eplus,         // charge
            1,                 // 2*spin
            0, 0,              // parity, C-conjugation
            0, 0, 0,           // isospin, isospin3, G-parity
            "lepton",          // type
            1,                 // lepton number
            0,                 // baryon number
            9999,              // PDG encoding
            true,              // stable
            -1,                // lifetime
            nullptr,           // decay table
            false,             // shortlived
            "teste"            // subType
        );
    }
}


void MyPhysicsList::ConstructParticle() {

    G4Gamma::GammaDefinition();
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4Proton::ProtonDefinition();
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4NeutrinoE::NeutrinoEDefinition();
    G4OpticalPhoton::OpticalPhotonDefinition();
    G4GenericIon::GenericIonDefinition();


    ////
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();
    G4AntiProton::AntiProtonDefinition();
    //////

    DefineMillichargedParticle();
}

void MyPhysicsList::ConstructProcess() {
    G4VModularPhysicsList::ConstructProcess();

    auto particleIterator = GetParticleIterator();
    particleIterator->reset();

    while ((*particleIterator)()) {
        G4ParticleDefinition* particle = particleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();

        if (particle->GetParticleName() == "millicharged") {
            pmanager->AddProcess(new G4eMultipleScattering(), -1, 1, 1);
            pmanager->AddProcess(new G4hIonisation(),        -1, 2, 2);
            pmanager->AddProcess(new G4eBremsstrahlung(),    -1, -1, 3);

            // Add Coulomb Scattering
            auto coulombProcess = new G4CoulombScattering();
            pmanager->AddDiscreteProcess(coulombProcess);
        }
    }

    auto particleTable = G4ParticleTable::GetParticleTable();    
    auto mcp = particleTable->FindParticle("millicharged");
    mcp->DumpTable();
    mcp->GetProcessManager()->DumpInfo();
    if (!mcp) {
    G4Exception("MyPhysicsList::ConstructProcess",
                "MissingMillicharged", FatalException,
                "'millicharged' particle not found in particle table.");
    }
    if (!mcp->GetProcessManager()) {
    G4Exception("MyPhysicsList::ConstructProcess",
                "MissingProcessManager", FatalException,
                "'millicharged' has no process manager.");
    }
}