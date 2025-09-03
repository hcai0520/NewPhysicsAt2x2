#include "G4PrimaryGeneratorAction.hh"
#include "G4Constantes.hh"

#include "Randomize.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4RandomTools.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>  
// ============================================================================

G4PrimaryGeneratorAction::G4PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(), 
   particleGun(0)
{   
    // default
    flag_alpha=true;
    G4int n_particle = 1;
    particleGun = new G4ParticleGun(n_particle);
    // G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    // G4ParticleDefinition* particle = particleTable->FindParticle("alpha");   
}

// ============================================================================

G4PrimaryGeneratorAction::~G4PrimaryGeneratorAction() {
    
    delete particleGun;
    
}

// ============================================================================

void G4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) { 
    
    static const double pi  = 3.14159265358979323846;

    // Define energy range (in MeV)
    //G4double E_min = 50.*MeV;
    //G4double E_max = 1000.*MeV;

    // Sample energy uniformly in range
    //G4double kineticEnergy = E_min + G4UniformRand() * (E_max - E_min);

    G4double p = 1.0 * GeV;

    G4ParticleDefinition* mcp = G4ParticleTable::GetParticleTable()->FindParticle("millicharged");
    G4double mass = mcp->GetPDGMass();
    G4double ekin = std::sqrt(p*p + mass*mass) - mass;
    particleGun->SetParticleDefinition(mcp);
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0., -0.05, 1.));
    particleGun->SetParticleEnergy(ekin);
    particleGun->SetParticlePosition(G4ThreeVector(-40*cm,0*cm,-2.8*m));

    particleGun->GeneratePrimaryVertex(anEvent);
    

    flag_alpha = true;   
    
}


// ============================================================================

void G4PrimaryGeneratorAction::SetOptPhotonPolar() {
    
    G4double angle = G4UniformRand() * 360.0*deg;
    SetOptPhotonPolar(angle);
    
}

// ============================================================================

void G4PrimaryGeneratorAction::SetOptPhotonPolar(G4double angle) {
    
    if (particleGun->GetParticleDefinition()->GetParticleName() != "opticalphoton") {
        G4cout << "--> warning from PrimaryGeneratorAction::SetOptPhotonPolar() :"
                "the particleGun is not an opticalphoton" << G4endl;
        return;
    }

    G4ThreeVector normal (1., 0., 0.);
    G4ThreeVector kphoton = particleGun->GetParticleMomentumDirection();
    G4ThreeVector product = normal.cross(kphoton);
    G4double modul2       = product*product;

    G4ThreeVector e_perpend (0., 0., 1.);
    if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product;
    G4ThreeVector e_paralle    = e_perpend.cross(kphoton);

    G4ThreeVector polar = std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
    particleGun->SetParticlePolarization(polar);
    
}

// ========================================================================