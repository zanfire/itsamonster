# itsamonster Code Style & Conventions

This document captures lightweight guidelines for contributing to the codebase. Keep it short, pragmatic, and easy to apply.

## 1. Language / Toolchain
- C++20 (no compiler extensions assumed).
- Exceptions disabled (`_HAS_EXCEPTIONS=0`) and RTTI disabled in production build (tests re‑enable RTTI when needed).
- Prefer standard library facilities; avoid adding dependencies without discussion.

## 2. Randomness / Dice
- All in-simulation randomness flows through the thread‑local dice accessor `GetDice()`.
- Initialize RNG per worker thread with `InitThreadDice(seed)` before invoking any combat logic.
- Do NOT pass RNG objects down call stacks; rely on thread‑local storage.
- For tests, override RNG deterministically using `ScopedDiceOverride` with a mock `IDice`.

## 3. Naming
- Types: `PascalCase` (e.g., `Monster`, `AttackAction`).
- Methods & functions: `PascalCase` for class methods to match existing style (`TakeAction`, `StartTurn`).
- Members: `m_snakeCase` or concise `m_name` already established—follow local file convention (do not churn).
- Enums: `PascalCase` enumerators (e.g., `Blinded`, `Incapacitated`).
- Constants (if added): `ALL_CAPS` or `kPascalCase`—choose whichever already dominates in the immediate file; prefer `kPascalCase` for new code.

## 4. Headers & Includes
- Use `#pragma once` at the top of every header.
- Include only what you use; forward declare where practical for pointer/reference members.
- Order: local header first, then related module headers, then standard library headers.
- Avoid cyclic dependencies—prefer minimal interfaces or callbacks if coupling grows.

## 5. Ownership & Memory
- Prefer value semantics where cheap (stat containers, small PODs).
- Use `std::unique_ptr` for exclusive ownership when dynamic allocation is required.
- Avoid raw `new`/`delete` in user code; wrap in smart pointers or factory helpers.

## 6. Threading
- Each simulation thread must call `InitThreadDice(seed + offset)` exactly once before any combat operations.
- Shared counters use `std::atomic` with relaxed ordering unless stronger guarantees are required.
- Avoid sharing mutable non-atomic state across threads; prefer thread‑local or copy-per-thread objects.

## 7. Logging
- Use the `LOG(...)` macro for combat narration and debug flow.
- Keep log messages concise and structured (`<Actor> does X ...`).
- Avoid logging in tight inner loops unless guarded by a verbosity flag.

## 8. Error Handling
- With exceptions disabled, use early returns and explicit validity checks.
- Invariant violations in development code may use `assert` (optional—none present yet). Avoid leaving asserts in hot paths if they become noisy.

## 9. Conditions & Status Effects
- Use `SetCondition` / `IsCondition` consistently instead of manipulating condition arrays directly.
- When adding new conditions, update `Condition` enum and `to_string(Condition)`.

## 10. Actions & Combat Flow
- New actions derive from `Action` (or specialized helpers) and implement `Execute(Monster&, Monster&)`.
- Distance/range checks belong in the action, not in the caller.
- Apply advantage/disadvantage resolution with `ResolveAdvantage`—don’t duplicate logic.

## 11. Testing Guidelines
- Use GoogleTest / GoogleMock (already integrated).
- Mock randomness via `ScopedDiceOverride`.
- Keep tests deterministic; never rely on wall clock time or real seeds unless explicitly testing variance.

## 12. Performance Notes
- Favor simple arithmetic and branch-light code in core loops (attack resolution, movement).
- Reserve containers if size is known ahead.
- Avoid virtual dispatch in inner loops if a static alternative provides clear benefit—defer refactors until profiling shows a hotspot.

## 13. Adding New Monsters / Actions Checklist
1. Create new monster header under `src/monsters/`.
2. Derive from `Monster`, fill stats, override `TakeAction` (and special reactions if needed).
3. Use existing action types (`AttackAction`, `RechargeAction`) before making new ones.
4. Ensure any recharge abilities call `TryRecharge()` in `StartTurn`.
5. Add resistances/immunities by overriding `IsResistant` / `IsImmune` etc.
6. Add tests for at least one representative behavior if non-trivial.

## 14. Formatting / Style Lite
- Indent with 4 spaces (maintain existing indentation of surrounding code).
- Brace style: open brace on same line for functions, classes, and control blocks.
- One statement per line; keep lines <= ~120 chars where reasonable.
- Avoid trailing whitespace; end files with a newline.
