#pragma once

#include <cstdint>
#include <string_view>
#include <random>

namespace itsamonster {

enum class Stat : uint32_t {
    Strength     = 0,
    Dexterity    ,
    Constitution ,
    Intelligence ,
    Wisdom       ,
    Charisma     
};

enum class DamageType : uint32_t {
    Acid        = 0,
    Bludgeoning ,
    Cold        ,
    Fire        ,
    Force       ,
    Lightning   ,
    Necrotic    ,
    Piercing    ,
    Poison      ,
    Psychic     ,
    Radiant     ,
    Slashing    ,
    Thunder     ,
    Count
};

enum class Condition : uint32_t {
    Blinded      = 0,
    Charmed      ,
    Deafened     ,
    Frightened   ,
    Grappled     ,
    Incapacitated,
    Invisible    ,
    Paralyzed    ,
    Petrified    ,
    Poisoned     ,
    Prone        ,
    Restrained   ,
    Count
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

enum class Advantage : uint32_t {
    Normal = 0,
    Advantage,
    Disadvantage
};

inline std::string_view to_string(Advantage adv) {
    switch (adv) {
        case Advantage::Normal:    return "Normal";
        case Advantage::Advantage: return "Advantage";
        case Advantage::Disadvantage: return "Disadvantage";
        default:                  return "Unknown";
    }
}

inline Advantage ResolveAdvantage(Advantage current, Advantage target) {
    if (current == Advantage::Normal || target == Advantage::Normal) {
        return target;
    }
    if (target != current) {
        return Advantage::Normal;
    }
    return target;
}

inline int D20Test(std::mt19937 &rng, Advantage advantage) {
    std::uniform_int_distribution<int> dist(1, 20);
    if (advantage == Advantage::Advantage) {
        return std::max(dist(rng), dist(rng));
    } else if (advantage == Advantage::Disadvantage) {
        return std::min(dist(rng), dist(rng));
    }
    return dist(rng);
}

} // namespace itsamonster