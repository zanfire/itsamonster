# Project TODOs — itsamonster (DnD 2024 Monster vs Monster)

This is a living roadmap for implementing a deterministic, testable fight simulator of two monsters using the 2024 D&D ruleset (at a high level; no proprietary text).

Legend: [ ] Planned | [x] Done | [~] In progress

## 1) Core Combat Engine
- [ ] Align turn/round structure with 2024 order of operations
  - Start-of-combat setup → initiative → round loop: start-of-turn effects → movement → action(s) → bonus action (if any) → reactions (timing windows) → end-of-turn effects
  - Acceptance: two monsters complete a full combat without undefined steps.
- [ ] Initiative and tie-breaking
  - Roll initiative per creature; resolve ties (e.g., compare Dex mod, reroll if needed). Persist order per round.
- [ ] Deterministic RNG seeding
  - Single seed controlling substreams (initiative, attack, saves, recharge). Acceptance: same seed reproduces a full transcript.
- [ ] Reactions framework + Opportunity Attacks
  - Hook windows (move out of reach, on hit, on being targeted, etc.), enforce once-per-round and resource limits.
- [ ] Movement & positioning
  - Speed, difficult terrain, forced movement, prone stand-up cost. OA only on voluntary movement leaving reach.
- [ ] Battlefield: range, reach, line of sight, cover
  - Apply cover bonuses to AC/save DCs; handle reach for melee.

## 2) Rules Mechanics
- [ ] Advantage/Disadvantage consolidation
  - Non-stacking rule; sources collapse to single advantage/disadvantage; interacts with conditions.
- [ ] Critical hits (2024 policy)
  - Define which attack types can crit; double only dice, not flat modifiers.
- [ ] Damage types and R/I/V
  - Model types; apply resistances, immunities, vulnerabilities before HP reduction; log pre/post.
- [ ] Conditions system
  - Frightened, restrained, prone, poisoned, stunned, etc.; durations and expiration at right phase.
- [ ] Saving throws & DCs
  - STR/DEX/CON/INT/WIS/CHA with proficiency, advantage/disadvantage; action DCs (spells/breath/abilities).
- [ ] Grapple/Shove special attacks
  - Contested checks, escape DCs, movement restrictions; restrained from grapple upgrades if applicable.
- [ ] Recharge mechanics
  - "Recharge X–Y": check start of turn, persist state; expose in `RechargeAction`.
- [ ] Multiattack & action economy
  - Encode sequences per stat block; ensure ordering and on-hit effects work across multiple hits.
- [ ] Legendary actions & legendary resistances
  - Legendary budget between turns; LR to convert failed saves; logging and per-day caps.
- [ ] Lair actions (optional)
  - Fire at initiative 20 (losing ties) once/round; configurable effects.
- [ ] Spellcasting & concentration
  - Casting checks, spell save DCs, concentration saves (DC 10 or half damage), break on incapacitation.

## 3) Data Model & Content
- [ ] Data-driven monster definitions
  - Introduce JSON/YAML schema → loader → runtime `Monster` objects (avoid hardcoding). Validate at load.
- [ ] Provide SRD-safe sample monsters
  - At least 6 generic/stat-block-safe creatures to avoid copyright issues; cover different archetypes.
- [ ] Tactics/AI layer
  - Pluggable strategies: target selection, use of AOEs/recharge, Dodge/Disengage decisions, advantage leverage.

## 4) CLI, Logging, and Metrics
- [ ] CLI for simulations
  - Example: `itsamonster.exe --m1 X --m2 Y -n 1000 --seed 42 --log-level info --out results.csv`
- [ ] Structured logging
  - JSON/CSV outputs per simulation plus human-readable logs; per-round transcript, optional replay.
- [ ] Metrics suite
  - Win rate, avg rounds, DPR, damage taken, ability usage rates; CSV export.

## 5) Testing & Quality
- [ ] Expand GoogleTest coverage
  - Unit tests for every rule: advantage, crits, damage types, conditions, saves, OA, grapple, recharge, multiattack.
- [ ] Property-based tests for dice
  - Statistical sanity checks on distributions; tolerance-based assertions.
- [ ] Performance guardrails
  - Benchmark 10k simulations; maintain a perf budget in CI.
- [ ] CI setup (GitHub Actions)
  - Build matrix (Windows MSVC, Linux GCC/Clang), run tests, upload artifacts; status badges.
- [ ] Documentation updates
  - README: usage, CLI, data schema, examples; mention `CODESTYLE.md`; contribution guidance.

---

### Suggested Implementation Order (High to Low Priority)
1) Core Combat Loop + Initiative + Deterministic RNG
2) Advantage/Disadvantage + Attack/Save/Damage pipeline (incl. crits, R/I/V)
3) Conditions + Movement/Positioning + Battlefield basics
4) Reactions (OA) + Multiattack + Recharge
5) Data-driven Monsters + SRD-safe samples
6) CLI + Logging + Metrics
7) Legendary/Lair Actions + Spellcasting/Concentration
8) Tests coverage expansion + Property tests + Performance + CI + Docs
