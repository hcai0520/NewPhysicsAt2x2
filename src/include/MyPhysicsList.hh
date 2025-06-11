#ifndef MYPHYSICSLIST_HH
#define MYPHYSICSLIST_HH

#include "G4VModularPhysicsList.hh"

class MyPhysicsList : public G4VModularPhysicsList {
public:
    MyPhysicsList();
    ~MyPhysicsList() override = default;

    void ConstructParticle() override;
    void ConstructProcess() override;

private:
    void DefineMillichargedParticle();
};

#endif