# Agent Instructions

These instructions apply to the CIHR development checkout. Read
`docs/DEVELOPMENT.md` and the private CIHR coordination repository's
`docs/engineering/RUNTIME_REGISTRY.md` before development or deployment.
The shared development tracker currently resides in the HDR checkout.

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
- Maintain one persistent source checkout per research stream. `eb_gui_hdr_dev`
  is the canonical HDR checkout and `eb_gui_dev` is the protected CIHR checkout.
- Develop and integrate reviewed CIHR work cumulatively on `cihr/all-fixes`.
  HDR development remains separate on `hdr-lung/development`.
  Keep commits clean enough for later selective upstream extraction.
- Do not create per-feature source directories, clones, or Git worktrees without
  explicit approval. Temporary worktrees are exceptional and must be removed
  when their approved purpose is complete.
- Prefer the smallest correct change. Avoid unrelated modernization or
  refactoring.
- Add focused synthetic regression tests. Run the narrow test first, then
  relevant builds, sanitizers, and integration checks.
- Prefer separate commits for production code, internal tests/test
  infrastructure, and development documentation. Record dependencies on
  unmerged corrections explicitly.
- Treat generated code as unvalidated until it has been reviewed and tested.
- Record source commit, executable SHA-256, runtime, and validation status for
  every deployed build.
- Record CIHR development and validation status in the private coordination
  repository and flag updates needed in the shared development tracker.
  Do not modify the HDR checkout or duplicate its tracker without approval.
- Preserve concise PR-ready summaries of problem, correction, and validation.

## Upstream contributions

Create an upstream-facing branch only when preparing an actual contribution.
Start it from current CLRP `main` where feasible and selectively cherry-pick or
adapt only the necessary production correction. Do not include internal tests,
synthetic fixtures, test harnesses, test project files, research-specific
features, governance, or tracker changes. Do not remove existing upstream tests.

Keep each PR coherent and technically justified; do not impose arbitrary line
limits. Describe the defect, correction, and verified observations concisely.
Delete temporary contribution branches after their purpose is complete, and
keep the fork's `main` aligned with CLRP `main` wherever practical.

Before any push, review tracked and staged content, including previously
tracked files that `.gitignore` cannot protect.
