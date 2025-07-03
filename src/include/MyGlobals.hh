#ifndef MY_GLOBALS_HH
#define MY_GLOBALS_HH

#include "globals.hh"
#include <map>

extern std::map<G4String, std::map<G4int, G4double>> accumulatedEnergy;
extern std::map<G4String, std::map<G4int, G4double>> positionY;
extern std::map<G4String, std::map<G4int, G4double>> positionZ;


#endif
