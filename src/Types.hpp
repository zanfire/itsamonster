#pragma once

#include <cstdint>
#include <string_view>

namespace itsamonster {

enum class Stat : uint32_t {
    Strength     = 0x01,
    Dexterity    = 0x02,
    Constitution = 0x04,
    Intelligence = 0x08,
    Wisdom       = 0x10,
    Charisma     = 0x20
};

enum class DamageType : uint32_t {
    Acid        = 0x0001,
    Bludgeoning = 0x0002,
    Cold        = 0x0004,
    Fire        = 0x0008,
    Force       = 0x0010,
    Lightning   = 0x0020,
    Necrotic    = 0x0040,
    Piercing    = 0x0080,
    Poison      = 0x0100,
    Psychic     = 0x0200,
    Radiant     = 0x0400,
    Slashing    = 0x0800,
    Thunder     = 0x1000
};

enum class Condition : uint32_t {
    Blinded      = 0x0001,
    Charmed      = 0x0002,
    Deafened     = 0x0004,
    Frightened   = 0x0008,
    Grappled     = 0x0010,
    Incapacitated= 0x0020,
    Invisible    = 0x0040,
    Paralyzed    = 0x0080,
    Petrified    = 0x0100,
    Poisoned     = 0x0200,
    Prone        = 0x0400,
    Restrained   = 0x0800
};

inline std::string_view to_string(Stat stat) {
    switch (stat) {
        case Stat::Strength:     return "Strength";
        case Stat::Dexterity:    return "Dexterity";
        case Stat::Constitution: return "Constitution";
        case Stat::Intelligence: return "Intelligence";
        case Stat::Wisdom:       return "Wisdom";
        case Stat::Charisma:     return "Charisma";
        default:                 return "Unknown";
    }
}

inline std::string_view to_string(DamageType type) {
    switch (type) {
        case DamageType::Acid:        return "Acid";
        case DamageType::Bludgeoning: return "Bludgeoning";
        case DamageType::Cold:        return "Cold";
        case DamageType::Fire:        return "Fire";
        case DamageType::Force:       return "Force";
        case DamageType::Lightning:   return "Lightning";
        case DamageType::Necrotic:    return "Necrotic";
        case DamageType::Piercing:    return "Piercing";
        case DamageType::Poison:      return "Poison";
        case DamageType::Psychic:     return "Psychic";
        case DamageType::Radiant:     return "Radiant";
        case DamageType::Slashing:    return "Slashing";
        case DamageType::Thunder:     return "Thunder";
        default:                      return "Unknown";
    }
}

inline std::string_view to_string(Condition cond) {
    switch (cond) {
        case Condition::Blinded:       return "Blinded";
        case Condition::Charmed:       return "Charmed";
        case Condition::Deafened:      return "Deafened";
        case Condition::Frightened:    return "Frightened";
        case Condition::Grappled:      return "Grappled";
        case Condition::Incapacitated: return "Incapacitated";
        case Condition::Invisible:     return "Invisible";
        case Condition::Paralyzed:     return "Paralyzed";
        case Condition::Petrified:     return "Petrified";
        case Condition::Poisoned:      return "Poisoned";
        case Condition::Prone:         return "Prone";
        case Condition::Restrained:    return "Restrained";
        default:                       return "Unknown";
    }
}

} // namespace itsamonster