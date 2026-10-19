# Junk-to-Gold (AzerothCore)

Automatically converts eligible newly looted gray (poor-quality) items into their vendor value and sends a chat receipt.

## Conversion rules

- Preserves items that start quests, as well as items required or referenced by accepted quests that are incomplete or complete but not yet turned in.
- Removes the same quantity as was just looted, even when it merges with existing stacks; it does not clear a whole stack merely because one item was looted.
- Checks that enough inventory items can be removed without touching stacks reserved for trading. Loot that cannot be safely converted stays in inventory.
- Skips conversion when the payout is too large or the resulting gold balance would **reach or exceed** AzerothCore's maximum, including zero-value gray items looted while already at the cap.
- Leaves non-gray items untouched.

The module uses AzerothCore's `OnPlayerLootItem` hook and the built-in inventory and money APIs. It adds no database tables or runtime configuration settings.

## Server scope and testing

This fork is configured for a server that does not permit trial accounts; it does not enforce a separate trial-account money cap. Reassess that restriction before allowing trial accounts.

See [the regression test plan](docs/regression-test-plan.md) for quest-item, stack, trade, payout-boundary, and receipt checks. Compilation alone does not establish in-game correctness. Full build/link verification and in-game tests are required before deployment.

Changes should be reviewed in development branches and pull requests. Do not merge or deploy unverified changes without authorization.
