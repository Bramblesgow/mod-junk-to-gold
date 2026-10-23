# Gold-cap incident — October 19, 2026

## Observations, not conclusions

- Live updated worldserver SHA-256: `3f7563ea9d0f869bf44ab796edee2a455afdcd56eeb2c7439f171a5fb6900666`, built from merged module `8c35a9fbd8ae6fbefa4e86482298153fb6a5a5c2` and AzerothCore `2a2211cd8f3d157da432ec0175ddd4b8a191931f`.
- During a 4-copper gray loot transaction with 2,147,483,644 copper already held, the game stopped responding, worldserver was unreachable, and guest SSH/ping failed. The VirtualBox snapshot is preserved externally. No kernel panic, OOM, or worldserver core dump was identified. Cause is **unconfirmed**.
- After restoring the predeployment binary SHA-256 `a09690ddaf26d73e0cd1b6d59c2a143a521956cae17fb8803ef625f70975e4b0`, the same boundary produced an “At gold limit” warning without a freeze; the old handler falsely reported a sale and destroyed gray loot without awarding payment.
- AzerothCore `Player::ModifyMoney` rejects positive payouts when `GetMoney() >= MAX_MONEY_AMOUNT - amount`; this core defines `MAX_MONEY_AMOUNT = 0x7FFFFFFF-1`. The updated preflight correctly rejects this specific 4-copper case before any removal or money mutation.
- The 26 existing mock-core regression tests pass, but they cannot establish the behavior of a real running worldserver.

## Review and diagnostic change

The handler's money rejection and trade-availability checks are separated without changing the conditions or transaction ordering. A DEBUG-level message under the existing `module` logger records the rejected item ID, count and money amounts. It does not mutate player data or enable logging in production.

This is **diagnostic instrumentation, not a proven bug fix**. A successful CodeRabbit review or test compilation is not grounds for production deployment.

## Isolated verification gate

1. Build from this PR branch against exactly the installed core revision, using all 11 installed modules, outside production source/build/output paths.
2. Verify the built binary, linked modules, and hash; run existing 26 mock tests, including an exact 2,147,483,644-copper balance and 4-copper loot case.
3. Use separate auth/character/world/playerbots databases and separate realm, server ports, configurations, logs, and process sessions. Never point the test server at live `acore_*` database schemas; never replace or restart production executables.
4. Verify that test startup causes no production modifications and that production ports 3724/8085 remain serviced by the original PIDs.
5. Perform the boundary using real creature loot, not `.additem`. Record chat warning, amount, inventory item retention, debug line, and test-server response.
6. If the isolated instance hangs, preserve its logs and process state before any restart. Do not repeat the test against production without separate authorization.

## Current disposition

**Updated October 23, 2026:** PR #3 was tested in the isolated combined realm, merged and deployed to production after explicit owner approval. The earlier review-only restrictions above describe the historical investigation phase and are not the current release status.

The owner reported J02 and the main conversion/boundary cases Pass; archived real-loot logs show exact-cap, above-cap, at-cap priced-gray and at-cap zero-value rejection with retained items. All 27 existing mock-integrated checks passed. The original J02 starting balance was not independently logged because DEBUG was initially off; several gameplay edge cases remain deferred.

The VM-wide freeze did not recur. Its root cause remains **unconfirmed**, and this diagnostic change is **not** presented as proof of a VM-crash fix.

See [October 23 test and release results](TEST_RESULTS_2026-10-23.md) and the [open coverage/evidence follow-up](https://github.com/Bramblesgow/mod-brambletown-services/issues/9).
