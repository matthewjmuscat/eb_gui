# Development Strategy

This repository uses one fork and one canonical upstream for multiple research
development streams:

- fork: `matthewjmuscat/eb_gui`;
- upstream: `clrp-code/eb_gui`;
- CIHR integration: `cihr/all-fixes`;
- HDR lung integration: `hdr-lung/development`.

The two project streams share reviewed general-purpose fixes selectively. They
do not automatically inherit one another's project-specific functionality.

Maintain one persistent source checkout per stream: `eb_gui_hdr_dev` for HDR
and the protected `eb_gui_dev` checkout for CIHR. Source checkouts are not build
directories or runtime databases. Do not create per-feature clones, directories,
or worktrees without explicit approval. An approved temporary worktree is an
exception and must be removed when its purpose is complete.

`DEVELOPMENT_TRACKER.md` is the authoritative public-safe record of what has
been implemented, tested, integrated, deployed, proposed upstream, or merged.
Consult it before starting work and update it whenever one of those states
changes.

## Change classification

### General-purpose fixes

Develop, test, and integrate reviewed corrections on the applicable cumulative
project branch. Keep the implementation and commit boundaries narrow enough for
a general-purpose correction to be selectively extracted later. Everyday
development does not require an upstream-based branch or another filesystem
workspace.

### CIHR development

CIHR batch generation, export automation, and research workflow conveniences
belong in the CIHR stream unless separately reviewed for general use. They are
not dependencies of HDR development by default.

### HDR development

HDR source interpretation, dwell timing, calibration safeguards, run-mode
validation, and image-geometry compatibility belong in the HDR stream unless a
change is deliberately implemented as a general-purpose correction.

## Branch and review workflow

The normal HDR workflow is cumulative:

1. Work in the canonical `eb_gui_hdr_dev` checkout on
   `hdr-lung/development`.
2. Demonstrate the defect with a focused synthetic test or reproducible check.
3. Implement the smallest technically correct change.
4. Run focused tests, relevant regression tests, sanitizers, and a clean build.
5. Integrate the reviewed correction into the cumulative branch and update the
   development tracker.
6. Build and deploy only to an explicitly approved isolated runtime.

Prefer three logically distinct commits for each correction:

- **Production:** the surgical correction only, independently compilable against
  its intended upstream base wherever feasible.
- **Internal validation:** synthetic/regression tests and test infrastructure.
- **Governance:** tracker and development-policy updates.

If a correction depends on another unmerged change, document and validate that
dependency explicitly. Existing published history does not need rewriting to
retrofit this structure.

When an upstream contribution is actually ready, update remote metadata, create
a temporary branch from current CLRP `main`, and selectively cherry-pick or
adapt only the relevant production commit or commits. Verify the resulting
production-only diff and dependencies before opening a PR. Delete the temporary
branch after its PR purpose is complete. The fork's `main` should remain aligned
with CLRP `main` wherever practical.

## Runtime isolation

Source history and runtime state are separate concerns. The workspace maintains
three independent runtime classes:

- a protected CIHR operational runtime;
- a shared development and validation runtime;
- a dedicated HDR lung research runtime.

The CIHR and HDR source checkouts are not additional runtime databases. A build
must be copied deliberately into its approved runtime, and its executable must
resolve only that runtime's writable database. Validation and HDR runtimes must
not contain writable links to operational research data.

Record the following for every deployment in the private runtime registry:

- runtime purpose and database root;
- source branch and full commit;
- executable name and SHA-256;
- build and validation status;
- approved use and rollback version.

Never replace a pinned executable in place. Deploy a new versioned binary,
verify its provenance and runtime boundary, validate it, and promote it only
after approval.

## Scientific validation

A successful build, import, or generated input is not evidence of scientific
correctness. Changes that affect geometry, timing, source strength, material
assignment, dose scaling, or reported metrics require:

- unit and dimensional checks;
- focused synthetic tests with independently calculated expectations;
- regression checks for unaffected workflows;
- review of generated scientific inputs;
- comparison against an appropriate independent reference before research use.

Preserve valid permanent LDR behavior when changing HDR workflows. Do not infer
a source model from isotope alone, guess source orientation, derive radioactive
decay from RT Plan modification timestamps, or silently accept unsupported
multi-source or image geometry.

Changes to the underlying Monte Carlo transport algorithm require separate,
explicit scope and validation.

## DICOM parser contribution plan

The existing parser corrections remain independently recoverable and are
integrated for HDR development. Their intended upstream organization is:

1. a focused DICOM parser ownership and memory-safety PR;
2. a coherent DICOM sequence-parsing PR covering bounded suffix reads, Explicit
   VR nested sequence recognition, defined-length delimiter accounting, and the
   final combined test expectations.

Strengthen synthetic coverage internally before proposing these PRs. Upstream
PRs should contain only the necessary production changes, not new internal test
files or test-project infrastructure. Do not combine parser changes with HDR
calculation changes or broad parser modernization.

## Responsible agent use

Agents must follow `AGENTS.md`, identify the active checkout and runtime before
work, and state assumptions that affect scientific behavior. AI-assisted code
requires the same review, tests, provenance, and scientific validation as any
other contribution.

Public branches and PRs must not contain patient identifiers, patient-derived
treatment information, absolute workstation paths, institutional data
locations, credentials, private database inventories, internal archival
procedures, or personal contact information. Keep operational details in the
private coordination repository or ignored local records.

Close each feature task with a separate tracker update on
`hdr-lung/development`. Record exact commits and distinguish implementation,
independent testing, research-branch integration, binary deployment, and
upstream merge status. Do not place governance or tracker commits in an
upstream-facing fix branch.

The tracker covers both CIHR and HDR development; it is not permanently
HDR-specific. The intended long-term home is a shared internal governance
branch separate from either research stream. Until that migration is approved,
maintain the canonical copy here and do not duplicate divergent trackers.

Upstream PRs must be surgical: include only technically necessary production
code, preserve existing upstream tests, and exclude new internal regression
files, fixtures, harnesses, test project configurations, research-specific
features, governance, and development history. Keep detailed validation
evidence internally and keep PR descriptions concise.

Filesystem cleanup, Git history changes, deployments, and upstream PR creation
are explicit approval gates.