# Agent Instructions

These instructions apply to the entire repository. Read
`docs/DEVELOPMENT.md` and `docs/DEVELOPMENT_TRACKER.md` before changing code,
building, deploying, or cleaning files.

## Required preflight

Before development work, report and verify:

- the repository and checkout;
- the current branch and full `HEAD` commit;
- the intended runtime/database root;
- the executable path and SHA-256, when a binary is involved;
- whether the work is general-purpose, CIHR-specific, or HDR-specific.

Do not assume a Git branch selects a runtime database. Deploy binaries only to
the explicitly approved isolated runtime.

## Protected boundaries

- Do not modify, replace, move, clean, or rebuild production databases, pinned
  executables, research outputs, or another project's runtime without explicit
  approval.
- Do not inspect or process patient data unless a specifically approved local
  protocol permits it. Never expose identifiers, private paths, raw metadata,
  credentials, or patient-derived content in commits, logs, prompts, or PRs.
- Do not change Monte Carlo transport/kernel code as part of a GUI fix without
  explicitly identifying and approving that scope first.
- Do not force-push, rewrite shared history, delete branches, clean worktrees,
  deploy binaries, or open pull requests without explicit approval.

## Development policy

- Keep `cihr/all-fixes` and `hdr-lung/development` independent. Neither stream
  automatically inherits the other's project-specific changes.
- Base independent general-purpose fixes on current CLRP upstream `main` where
  feasible. Base project work on its corresponding development stream.
- Prefer the smallest correct change. Avoid unrelated modernization or
  refactoring.
- Add focused synthetic regression tests. Run the narrow test first, then
  relevant builds, sanitizers, and integration checks.
- Treat generated code as unvalidated until it has been reviewed and tested.
- Record source commit, executable SHA-256, runtime, and validation status for
  every deployed build.
- Update `docs/DEVELOPMENT_TRACKER.md` on `hdr-lung/development` whenever a
  feature or correction changes implementation, test, integration, deployment,
  or upstream status. Do not add the tracker to an upstream-facing fix branch.

## Upstream contributions

Push clean feature branches to the existing fork. Upstream PRs are opened from
those fork branches into CLRP `main` after approval. Keep project-specific
changes, private operational material, and fork governance commits out of the
upstream PR commit range.

Before any push, review tracked and staged content, including previously
tracked files that `.gitignore` cannot protect.