# Junk-to-Gold - October 23, 2026 test results and live release

Tested PR #3 head: `39a0213d3f29a6d66de4e9cab5bac4b8cb346c59`; merge/release commit `93f2d48bf578f4a5ca6cdcade4e7d5f9e3d23ccd`.

## Observed results

User marked J02, J04-J13, J16 and J19 Pass. J14, J15, J17, J18 and J21 remain **Deferred**. All **27 existing mock-integrated checks passed**, including overflow, zero-count and duplicate callback suppression. The harness module source was verified to match the staged module source.

Archived real corpse-loot rejection diagnostics include:

```text
item 7074, count 1, balance 2147483642, payout 4, proposed 2147483646
item 4865, count 1, balance 2147483642, payout 5, proposed 2147483647
item 7074, count 1, balance 2147483646, payout 4, proposed 2147483650
item 10839, count 1, balance 2147483646, payout 0, proposed 2147483646
```

Owner confirmed gray loot retained with no payout at the cap. J07 exact-cap preparation was performed on an offline copied test character with original-balance evidence saved. No corresponding test money edit was applied to a live character.

The original J02 2147483644 starting balance was not independently captured because DEBUG logging was initially off. J02 remains user-reported Pass; later exact/above-cap logs do not retroactively establish that missing starting balance. J12 split-stack and J19 quest-state variations retain evidence limitations described in the follow-up.

## Open follow-up and incident disposition

[Coverage/evidence follow-up](https://github.com/Bramblesgow/mod-brambletown-services/issues/9) tracks deferred gameplay cases and the exact original-incident diagnostic gap. The issue is on the NPC Services board because this fork's issue tracker is disabled.

The October 19 VM-wide freeze **did not recur** in this test session. Its root cause remains **unconfirmed**. PR #3 adds diagnostics and does not establish why the VM froze or prove that the instrumentation itself fixes the cause. Read [the historical incident report](gold-cap-incident-20261019.md) with the October 23 disposition below.

No hosted workflow runs were available for this PR head. Validation consisted of the local full build, mock checks and isolated in-game tests; hosted CI success is not claimed.

Evidence: `evidence/J-closeout-20261023T145716Z/`, `evidence/J20-final-harness-20261023.log`, and the J07 offline-balance record. Config reloads can truncate the active Server.log; the archived log copy preserves cap diagnostics.

## Shared session and deployment record

Owner-completed sheet: `Brambletown_Combined_Test_Sheet_2026-10-23(1).pdf`. Corrected user marks: **65 Pass, 7 Deferred, 2 Blocked, 23 Not run** across 97 cases. This is the user-form tally, not a claim that all operator/restoration cases were unexecuted: operator evidence and subsequent deployment are reconciled separately. S03 and E06 were explicitly corrected to Pass after submission.

Combined isolated test binary SHA-256: `8d2fa8f0c0593d02b455b34a471450fff4fdb4b6b7c1af53f8393fcc6964bcdf`.
Production build from the same tested gameplay source, rebuilt with production configuration paths: `ac4d29bd37f436f495f8211ae4d3eb291ef181a8c43926a3e995497eb2f3f483`.

Released October 23, 2026 after explicit owner approval. Production controller recorded success at **11:48 AM America/Chicago (CDT)**. Auth/world ports responded, world console responded, and the inspected error log was empty. All **35 production configuration files remained byte-for-byte unchanged**; original authserver retained. Production characters/starter receipts and normal loot rates preserved; no cloned test data, guaranteed-drop fixtures or test configuration were imported.

Production rollback binaries, database dumps, configuration backup, migration output and deployment manifest are retained on the authorized server under `rollout-20261023/`. Test evidence is retained under `combined-runtime-20261023/evidence/`. Live startup checks are operational checks, not a substitute for later human gameplay confirmation.
