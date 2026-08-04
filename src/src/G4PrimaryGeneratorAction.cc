#include "G4PrimaryGeneratorAction.hh"
#include "G4Constantes.hh"

#include "G4Event.hh"
#include "G4Exception.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace {
const char* kGeometryPoolPath =
    "/Users/hongc/MCP_Analysis/Notebook/production_muon_geometry_pool.csv";

std::vector<std::string> SplitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) fields.push_back(field);
    return fields;
}
}

G4PrimaryGeneratorAction::G4PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      particleGun(new G4ParticleGun(1)),
      selectedTrackId(-1),
      selectedDataEventIndex(-1),
      selectedDataGroupId(-1) {
    flag_alpha = true;
    LoadGeometryPool(kGeometryPoolPath);

    G4cout << "Loaded muon geometry pool: " << geometryPool.size()
           << " tracks from " << kGeometryPoolPath << G4endl;
}

G4PrimaryGeneratorAction::~G4PrimaryGeneratorAction() {
    delete particleGun;
}

void G4PrimaryGeneratorAction::LoadGeometryPool(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        G4ExceptionDescription message;
        message << "Cannot open muon geometry pool: " << path;
        G4Exception("G4PrimaryGeneratorAction::LoadGeometryPool",
                    "MuonPool001", FatalException, message);
        return;
    }

    std::string line;
    if (!std::getline(input, line)) {
        G4Exception("G4PrimaryGeneratorAction::LoadGeometryPool",
                    "MuonPool002", FatalException,
                    "Muon geometry pool is empty.");
        return;
    }

    const auto header = SplitCsvLine(line);
    std::unordered_map<std::string, std::size_t> column;
    for (std::size_t i = 0; i < header.size(); ++i) column[header[i]] = i;

    const std::vector<std::string> required = {
        "track_id", "event_index", "group_id",
        "start_x_cm", "start_y_cm", "start_z_cm",
        "direction_x", "direction_y", "direction_z"
    };
    for (const auto& name : required) {
        if (column.find(name) == column.end()) {
            G4ExceptionDescription message;
            message << "Missing column in muon geometry pool: " << name;
            G4Exception("G4PrimaryGeneratorAction::LoadGeometryPool",
                        "MuonPool003", FatalException, message);
            return;
        }
    }

    G4long lineNumber = 1;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (line.empty()) continue;

        try {
            const auto fields = SplitCsvLine(line);
            auto value = [&](const std::string& name) -> const std::string& {
                const auto index = column.at(name);
                if (index >= fields.size()) throw std::runtime_error("short row");
                return fields[index];
            };

            TrackGeometry track;
            track.trackId = std::stoll(value("track_id"));
            track.eventIndex = std::stoll(value("event_index"));
            track.groupId = std::stoll(value("group_id"));
            track.start = G4ThreeVector(
                std::stod(value("start_x_cm")) * cm,
                std::stod(value("start_y_cm")) * cm,
                std::stod(value("start_z_cm")) * cm);
            track.direction = G4ThreeVector(
                std::stod(value("direction_x")),
                std::stod(value("direction_y")),
                std::stod(value("direction_z")));

            const G4double norm = track.direction.mag();
            if (!std::isfinite(norm) || norm <= 0.0)
                throw std::runtime_error("invalid direction");
            track.direction /= norm;
            geometryPool.push_back(track);
        } catch (const std::exception& error) {
            G4ExceptionDescription message;
            message << "Invalid geometry-pool row " << lineNumber
                    << ": " << error.what();
            G4Exception("G4PrimaryGeneratorAction::LoadGeometryPool",
                        "MuonPool004", FatalException, message);
            return;
        }
    }

    if (geometryPool.empty()) {
        G4Exception("G4PrimaryGeneratorAction::LoadGeometryPool",
                    "MuonPool005", FatalException,
                    "Muon geometry pool contains no tracks.");
    }
}

void G4PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    // Sample one complete row with replacement. Position and direction
    // therefore always come from the same measured data track.
    const std::size_t index = std::min(
        static_cast<std::size_t>(G4UniformRand() * geometryPool.size()),
        geometryPool.size() - 1);
    const auto& track = geometryPool[index];

    selectedTrackId = track.trackId;
    selectedDataEventIndex = track.eventIndex;
    selectedDataGroupId = track.groupId;

    auto* muon = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    if (!muon) {
        G4Exception("G4PrimaryGeneratorAction::GeneratePrimaries",
                    "MuonPool006", FatalException,
                    "Could not find the mu- particle definition.");
        return;
    }

    const G4double momentum = 10.0 * GeV;
    const G4double mass = muon->GetPDGMass();
    const G4double kineticEnergy =
        std::sqrt(momentum * momentum + mass * mass) - mass;

    particleGun->SetParticleDefinition(muon);
    particleGun->SetParticlePosition(track.start);
    particleGun->SetParticleMomentumDirection(track.direction);
    particleGun->SetParticleEnergy(kineticEnergy);
    particleGun->GeneratePrimaryVertex(event);

    flag_alpha = true;
}

void G4PrimaryGeneratorAction::SetOptPhotonPolar() {
    SetOptPhotonPolar(G4UniformRand() * 360.0 * deg);
}

void G4PrimaryGeneratorAction::SetOptPhotonPolar(G4double angle) {
    if (particleGun->GetParticleDefinition()->GetParticleName()
        != "opticalphoton") {
        G4cout << "--> warning from PrimaryGeneratorAction::SetOptPhotonPolar(): "
               << "the particleGun is not an opticalphoton" << G4endl;
        return;
    }

    const G4ThreeVector normal(1., 0., 0.);
    const G4ThreeVector direction =
        particleGun->GetParticleMomentumDirection();
    const G4ThreeVector product = normal.cross(direction);
    const G4double modulus2 = product * product;

    G4ThreeVector perpendicular(0., 0., 1.);
    if (modulus2 > 0.) perpendicular = product / std::sqrt(modulus2);
    const G4ThreeVector parallel = perpendicular.cross(direction);

    particleGun->SetParticlePolarization(
        std::cos(angle) * parallel + std::sin(angle) * perpendicular);
}
