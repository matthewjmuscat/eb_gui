# Development Strategy

This repository uses one fork and one canonical upstream for multiple research
development streams:

- fork: `matthewjmuscat/eb_gui`;
- upstream: `clrp-code/eb_gui`;
- CIHR integration: `cihr/all-fixes`;
- HDR lung integration: `hdr-lung/development`.

The two project streams share reviewed general-purpose fixes selectively. They
do not automatically inherit one another's project-specific functionality.

`DEVELOPMENT_TRACKER.md` is the authoritative public-safe record of what has
been implemented, tested, integrated, deployed, proposed upstream, or merged.
Consult it before starting work and update it whenever one of those states
changes.

## Change classification

### General-purpose fixes

Corrections that apply to eb_gui independently of a research project should
normally start on a clean `fix/...` branch based on current upstream `main`.
Keep the production change narrow, add synthetic regression coverage, and
validate the fix independently before integrating it into a project stream.

### CIHR development

CIHR batch generation, export automation, and research workflow conveniences
belong in the CIHR stream unless separately reviewed for general use. They are
not dependencies of HDR development by default.

### HDR development

HDR source interpretation, dwell timing, calibration safeguards, run-mode
validation, and image-geometry compatibility belong in the HDR stream unless a
change is deliberately implemented as a general-purpose correction.

## Branch and review workflow

For an independent fix:

1. Update remote metadata and identify the exact upstream `main` commit.
2. Create one focused `fix/...` branch from that commit.
3. Demonstrate the defect with a synthetic fixture or focused test.
4. Implement the smallest technically correct change.
5. Run the focused test, relevant build, and sanitizer checks where applicable.
6. Review the production diff, test diff, and commit range for unrelated or
   sensitive files.
7. Commit and push the feature branch to the fork after approval.
8. Open an upstream PR from the fork branch to CLRP `main` only after explicit
   approval.
9. Integrate the reviewed commit into each project stream that needs it.

An internal PR targeting the fork is optional. It is not a prerequisite for an
upstream PR. A dependent fix must document its dependency instead of silently
including unrelated commits.

Fork governance files, private operations, and project-only changes must not be
included in upstream PR commit ranges. Use selective integration rather than
merging an entire project development stream upstream.

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

Strengthen synthetic coverage before proposing these PRs. Do not combine them
with HDR calculation changes or broad parser modernization.

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

Filesystem cleanup, Git history changes, deployments, and upstream PR creation
are explicit approval gates.