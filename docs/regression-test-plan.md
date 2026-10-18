# Junk-to-Gold — PR #1 regression plan

## Build verification

- [ ] 2026-10-18: Re-download updated review commit `3a2dfb9abe8aad0148e1a6551852c08e1a368294` to `/tmp/junk-to-gold-pr-review-20261018/` on the authorized server.
- [ ] Re-run `clang++ -fsyntax-only` against the exact compile flags from `/root/brambletown-upgrade-work/build-production/compile_commands.json` (AzerothCore commit `2a2211cd8`).
- [ ] Record compiler diagnostics and exit status of latest reviewed commit. Previous code at `f9183c5` compiled successfully; the latest commit still requires verification.
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
| Multi-stack loot | Loot enough stackable gray items to merge into and split between multiple stacks | Inventory count decreases by exactly the amount looted; payout matches |
| Ordinary junk | Loot 1 and several gray items at varying sell prices | Item count and copper payout match precisely |
| Non-gray loot | Loot normal/green items | Items remain; no payout |
| Gold cap (equal boundary) | Loot an item whose full reward would bring money to exactly `MAX_MONEY_AMOUNT` | Item remains; no sale receipt or payout |
| Gold cap (above boundary) | Loot gray item with a reward that would exceed maximum money | Item remains; no partial payout |
| Trial-account cap | On a trial account, loot gray item that would exceed configured `Trial.MoneyCap` | Item remains; no sale receipt or payout |
| Zero-price gray item | Loot a zero-sell-price gray item even when at money cap | Item is discarded normally, no extra money |
| Overflow guard | Exercise quantity/price combination over signed 32-bit payout maximum | Item remains; no overflow or negative payout |
| Zero count | Invoke handler with zero count in a controlled test | No payout, no destruction |
| Repeated looting | Loot a series of gray and quest items | No double-pay, item loss, or quest completion regression |

## Release gate

Do not merge for deployment until CodeRabbit's follow-up review findings are resolved, the module is fully built, and the in-game runtime regressions are recorded. No server restart or deployment is authorized by this PR.
