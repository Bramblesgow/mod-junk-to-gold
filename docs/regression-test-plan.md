# Junk-to-Gold — PR #1 regression plan

## Build verification

- [x] 2026-10-18: Downloaded updated review commit `3a2dfb9abe8aad0148e1a6551852c08e1a368294` to `/tmp/junk-to-gold-pr-review-20261018/` on the authorized server.
- [x] Re-ran `clang++ -fsyntax-only` against the exact compile flags from `/root/brambletown-upgrade-work/build-production/compile_commands.json` (AzerothCore commit `2a2211cd8`).
- [x] New module source commit `3a2dfb9` compiled against AzerothCore commit `2a2211cd8`, exit code 0 with no diagnostics (2026-10-18). No live source, binary, or configuration modified.
- [x] 2026-10-18: Simplified source commit `849f22d` passed `clang++ -fsyntax-only` against the installed AzerothCore `2a2211cd8` compile flags; exit code 0 and no diagnostics. The temporary file was outside the live module.
- [x] 2026-10-18: Trade-aware source commit `7cf7c19` passed `clang++ -fsyntax-only` with the same installed AzerothCore `2a2211cd8` production compile flags (exit 0, no diagnostics; temporary copy outside live module).
- [x] Compiled review source `7cf7c19` to an isolated C++ object file using the actual module `compile_commands.json` entry, exit 0 with no diagnostics. This confirms translation-unit compilation, not a successful full-worldserver link or runtime test.
- [ ] Full core/module link build from the proposed branch.
- [ ] In-game runtime regression tests below.

## In-game regression cases (not yet executed)

| Case | Setup | Expected |
| --- | --- | --- |
| Quest starter | Loot gray-quality item with `StartQuest` | Item remains; no payout |
| Active collection | Loot gray-quality required item before final objective | Item remains; quest progress increments |
| Final required item | Loot the last gray-quality required item (quest transitions to COMPLETE) | Item remains in inventory for hand-in |
| Completed, not rewarded | Loot another relevant gray item while quest is COMPLETE | Preserved until the quest is rewarded or abandoned |
| Quest source/drop | Loot gray item explicitly listed in an accepted quest's `ItemDrop` fields | Item remains |
| Unrelated gray item | Accept or complete an unrelated quest; loot a gray item | Item is converted, despite quest's empty objective slots |
| No active quest | Loot gray item that is otherwise associated with a quest | Ordinary junk behavior unless it starts a quest |
| Preexisting stack | Have 19 of a stackable gray item, then loot 1 | 19 remain; payout for 1 |
| Traded stack only | Mark the only stack of gray items as in trade, then loot more of that item so it merges into the traded stack | Conversion is skipped; no gold granted and no stack destroyed |
| Traded plus non-traded, insufficient | Hold a traded gray stack and fewer than the newly looted quantity in non-traded stacks | Conversion is skipped; no gold granted |
| Traded plus non-traded, sufficient | Hold a traded gray stack and enough non-traded quantity to cover the loot | Exactly the loot quantity is removed from eligible non-traded stacks; traded items remain; payout matches removed quantity |
| Multi-stack loot | Loot enough stackable gray items to merge into and split between multiple stacks | Inventory count decreases by exactly the amount looted; payout matches |
| Ordinary junk | Loot 1 and several gray items at varying sell prices | Item count and copper payout match precisely |
| Non-gray loot | Loot normal/green items | Items remain; no payout |
| Gold cap (equal boundary) | Loot an item whose full reward would bring money to exactly `MAX_MONEY_AMOUNT` | Item remains; no sale receipt or payout |
| Gold cap (above boundary) | Loot gray item with a reward that would exceed maximum money | Item remains; no partial payout |
| Zero-price gray item | Loot a zero-sell-price gray item even when at money cap | Item is discarded normally, no extra money |
| Overflow guard | Exercise quantity/price combination over signed 32-bit payout maximum | Item remains; no overflow or negative payout |
| Zero count | Invoke handler with zero count in a controlled test | No payout, no destruction |
| Repeated looting | Loot a series of gray and quest items | No double-pay, item loss, or quest completion regression |

## Trade preflight rationale

On installed AzerothCore `2a2211cd8`, `Player::HasItemCount(itemId, count)` and `Player::DestroyItemCount(itemId, count, true)` both exclude stacks with `IsInTrade()`; `GetItemCount` does not. Preflighting `HasItemCount` before invoking the core removal routine guarantees that the single-threaded inventory operation has sufficient eligible quantity when it begins. This avoids introducing manual per-slot removal and an error-prone rollback implementation. Runtime testing is still required, especially for loot that merges into a traded stack; this is source-level verification, not a claim that gameplay tests passed.

## Account scope

The server does not admit trial accounts, by owner decision. The module intentionally checks the core's normal gold maximum only, not `CONFIG_TRIAL_MONEY_CAP`. Do not assume trial-account payout safety without separate changes if the account policy changes.

## Release gate

Do not merge for deployment until CodeRabbit's follow-up review findings are resolved, the module is fully built, and the in-game runtime regressions are recorded. No server restart or deployment is authorized by this PR.
