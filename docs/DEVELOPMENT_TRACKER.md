# Development Tracker

Last reviewed: 2026-11-04

This is the authoritative public-safe record of meaningful development in the
`matthewjmuscat/eb_gui` fork. It records why work exists, where it is available,
and what remains before research or upstream use. Git history remains the
authority for source content; this tracker records intent and lifecycle state.

This tracker covers both CIHR and HDR development. Its intended long-term home
is a shared internal governance branch rather than either research stream; that
migration remains deferred pending an explicit repository-governance decision.

## Status model

These states are distinct and must not be collapsed:

1. **Implemented**: source exists in a commit or explicitly identified working
   tree.
2. **Independently tested**: focused validation has passed on its feature branch.
3. **Integrated**: the commit is present in a named research branch.
4. **Deployed**: a named binary was built from a verified commit. Deployment is
   not proof of scientific validation.
5. **Merged upstream**: CLRP accepted the change into its `main` branch.

Update this file whenever one of those states changes. Never treat an older
pinned binary as containing a later upstream merge merely because the source fix
is equivalent.

## Completed corrections

### FIX-001: HU-to-density upper-range extrapolation

- **Type:** Bug fix.
- **Motivation:** Prevent an out-of-bounds calibration lookup and preserve linear
  extrapolation when CT values reach or exceed the final calibration point.
- **Branch/commits:** `fix/hu-density-extrapolation-warning` at
  `21b18ca4d6311a13a34c2261a13dd430cd963623`; CIHR integration equivalent
  `c7a2727f7019261d81ec0c8911b304f6337c2309`.
- **Fork:** Pushed.
- **Upstream:** Merged, PR #11; upstream merge
  `889a11810b0a3e63b87d10f515a8497728c4bc80`.
- **CIHR:** Present in `cihr/all-fixes`; deployed in `eb_gui_dev_v5`.
- **HDR:** Present through the upstream base of `hdr-lung/development`; no HDR
  executable is deployed.
- **Validation:** Build and targeted VPM density comparison recorded in PR #11.
- **Next action:** Preserve in future LDR and HDR regression suites.

### FIX-002: Dcc requests beyond structure volume

- **Type:** Bug fix.
- **Motivation:** Return `n/a` instead of the minimum voxel dose when the
  requested absolute volume exceeds the evaluated structure volume.
- **Branch/commits:** `fix/dxcc-volume-boundary` at
  `de62c0eeb76faf484ee62cdaca6defef4e6515d0`; CIHR integration equivalent
  `d4e4c6c5033d6799ec64cf8e02782be89fe98d04`.
- **Fork:** Pushed.
- **Upstream:** Merged, PR #12; upstream merge
  `afcd2f773d4907c67534fe45abc338ea7e9fa978`.
- **CIHR:** Present in `cihr/all-fixes`; deployed in `eb_gui_dev_v5`.
- **HDR:** Present through the upstream base of `hdr-lung/development`; no HDR
  executable is deployed.
- **Validation:** Build and targeted TG43/TG186 metric checks recorded in PR #12.
- **Next action:** Preserve valid-Dcc and out-of-volume cases in regression tests.

### FIX-003: Vx volume accumulation and boundary cases

- **Type:** Bug fix.
- **Motivation:** Implement direct qualifying-voxel volume accumulation so Vx
  handles empty sets, exact thresholds, full-volume thresholds, multiple
  thresholds, and nonuniform voxels correctly.
- **Branch/commits:** `fix/vx-metric-calculation` at
  `94325a940ae83f138f1dbafa5b82d2f6f5fafc26`; CIHR integration equivalent
  `cf2eaaf6d8ad63b68dbbdaf03168bc7dd75be73a`.
- **Fork:** Pushed.
- **Upstream:** Merged, PR #13; upstream merge
  `18d059fafb5abb298b4b218afd712f83d11f19ce`.
- **CIHR:** Present in `cihr/all-fixes`; deployed in `eb_gui_dev_v5`.
- **HDR:** Present through the upstream base of `hdr-lung/development`; no HDR
  executable is deployed.
- **Validation:** Build and targeted Vx checks recorded in PR #13; a later CIHR
  numerical audit found only expected Vx/Dcc changes in paired metrics.
- **Next action:** Add small-grid synthetic boundary cases to the maintained test
  suite when a general metric-test harness is established.

### FIX-004: DICOM top-level tag memory safety

- **Type:** Bug fix.
- **Motivation:** Correct ownership on malformed top-level delimiter handling to
  avoid double-free, use-after-free, and leaked temporary attributes.
- **Branch/commits:** `fix/dicom-parser-tag-memory-safety` at
  `2b016796ee04c2e66b6076bac460c18aa590af37`; integration commit
  `696c414e75cabaa2d3bc43108bf6acaa2458517d`.
- **Fork:** Pushed.
- **Upstream:** Planned as the focused parser ownership/memory-safety PR; not
  submitted.
- **CIHR:** Not present in the pinned CIHR v5 source or binary.
- **HDR:** Integrated in `hdr-lung/development`.
- **Deployment:** Present in parser-reference binary
  `eb_gui_dev_dicom_integrated`; not deployed to the HDR runtime.
- **Validation:** Synthetic parser test passed normally and with sanitizers.
- **Next action:** Review and submit independently after approval.

### FIX-005: Bounded DICOM sequence suffix reads

- **Type:** Bug fix.
- **Motivation:** Replace unbounded integer dereferences with bounded byte-suffix
  comparisons in both sequence readers.
- **Branch/commits:** `fix/dicom-sequence-safe-suffix-reads` at
  `805e87b8be401359a5c9acb544691c920c62a43d`; integration commit
  `ea8f0f230ede85aa9ec257feed22b41651d9920e`.
- **Fork:** Pushed.
- **Upstream:** Planned for the sequence-parsing PR; not submitted.
- **CIHR:** Not present in the pinned CIHR v5 source or binary.
- **HDR:** Integrated in `hdr-lung/development`.
- **Deployment:** Present in `eb_gui_dev_dicom_integrated`; not deployed to the
  HDR runtime.
- **Validation:** Synthetic short-buffer and delimiter tests passed normally and
  with sanitizers.
- **Next action:** Combine with FIX-006 and FIX-007 for upstream review.

### FIX-006: Explicit-VR nested sequence recognition

- **Type:** Bug fix.
- **Motivation:** Recognize nested `SQ` fields through a byte-safe Explicit-VR
  comparison rather than an alignment- and endian-sensitive integer cast.
- **Branch/commits:** `fix/dicom-explicit-vr-nested-sequences` at
  `f6d136c020d9d3d5bbefbbe79ad474431dad1c77`; integration commit
  `0ab5bee0eacfb025876f56a0c3defb27ba197c4c`.
- **Fork:** Pushed.
- **Upstream:** Planned for the sequence-parsing PR; not submitted.
- **CIHR:** Not present in the pinned CIHR v5 source or binary.
- **HDR:** Integrated in `hdr-lung/development`.
- **Deployment:** Present in `eb_gui_dev_dicom_integrated`; not deployed to the
  HDR runtime.
- **Validation:** Synthetic Explicit-VR nested-sequence tests passed normally and
  with sanitizers.
- **Next action:** Combine with FIX-005 and FIX-007 for upstream review.

### FIX-007: Defined-sequence item-length accounting

- **Type:** Bug fix.
- **Motivation:** Account for the eight-byte undefined-item delimiter when an
  undefined-length item occurs inside a defined-length sequence.
- **Branch/commits:** `fix/dicom-defined-sequence-item-length` at
  `33960d60edd47fe0aeceacd8daa2f0a1f083d666`; integration commit
  `3d3ea21099f6c2af7426ee88d31459ec41ebfaae`.
- **Fork:** Pushed.
- **Upstream:** Planned for the sequence-parsing PR; not submitted.
- **CIHR:** Not present in the pinned CIHR v5 source or binary.
- **HDR:** Integrated in `hdr-lung/development`.
- **Deployment:** Present in `eb_gui_dev_dicom_integrated`; not deployed to the
  HDR runtime.
- **Validation:** Synthetic mixed-length sequence test passed normally and with
  sanitizers. Combined parser expectations are recorded at
  `c294f792c238892673fe0f7b617c008a413a9c04`.
- **Next action:** Combine with FIX-005 and FIX-006 for upstream review.

### FIX-008: Simulation completion and output validation

- **Type:** General-purpose bug fix and execution safeguard.
- **Problem:** GUI completion handling treated only exit code 1 as failure,
  ignored `CrashExit` and other nonzero codes, and described a zero-exit run with
  no dose output as successful. Existing raw dose files could also be mistaken
  for output from a new attempt, and failed copies were followed by source-file
  removal.
- **Production commit:** `42542d7f6cf596f9618edf76e36f659e79198d3d`.
- **Internal test commit:** `262d8ce523451259ffbc9cab048a8da82e37ce71`.
- **Correction:** A simulation succeeds only after `NormalExit`, exit code zero,
  and at least one nonempty matching dose file. Runs are blocked when matching
  raw dose output already exists. Dose sources are removed only after all dose
  files copy successfully; copy failure rolls back newly copied destinations and
  retains source output. The obsolete one-argument process-finished connection
  was removed.
- **Fork/upstream:** Integrated locally on `hdr-lung/development`; push pending
  completion of the approved repair sequence. Plan a separate general simulation
  lifecycle PR; do not combine it with HDR calibration or EGSnrc infrastructure.
- **CIHR/HDR/deployment:** Not synchronized to CIHR and not deployed.
- **Validation:** Focused tests passed for nonzero normal exit, crash exit, zero
  exit without output, empty output, stale-output discovery, and zero exit with
  nonempty dose output. The complete Qt application compiled and linked.
- **Parallel investigation:** The local `egs-parallel-cpu` wrapper can time out
  waiting for a lock and use a bare `exit`, masking submission failure with status
  zero. The reason the worker did not create its lock remains unresolved. No
  EGSnrc script or transport code was modified.
- **Next action:** Keep the lock-file failure under investigation. Establish the
  worker failure mechanism before separately approving any EGSnrc wrapper change.

## Fork features and workflow enhancements

### GUI-001: Plaque position and rotation controls

- **Type:** Workflow enhancement.
- **Motivation:** Improve interactive plaque positioning and rotation.
- **Branch/commit:** Fork `main` at
  `7ca6cd15e97344054420d8059b418282cb7fcf62`.
- **Fork:** Pushed.
- **Upstream:** Present in upstream ancestry; original PR provenance is
  unverified.
- **CIHR/HDR:** Present through their shared historical base.
- **Deployment:** Exact named-binary introduction is unverified.
- **Validation:** No focused evidence found during the 2026-11-01 audit.
- **Next action:** Preserve as historical functionality; add a GUI regression
  only if this control is modified.

### CIHR-001: Prostate VPM auto-configuration

- **Type:** Workflow enhancement.
- **Motivation:** Reduce repetitive GUI selection while making contour/media
  priority and resource ordering deterministic for the CIHR prostate workflow.
- **Branch/commits:** `mjm-vpm-workflow` at
  `2e55401ec3b7b7db75de3f97c85b544192000268`,
  `c6c1612a23f1bcf9d5356cfd5b060197d763d008`, and
  `84985dc8015d25ab4015e3e6b61f965dc7500a0f`.
- **Fork:** Pushed.
- **Upstream:** Under consideration; generality outside the CIHR naming and
  workflow assumptions has not been established.
- **CIHR:** Integrated in `cihr/all-fixes`; included in `eb_gui_dev_v5`.
- **HDR:** Not integrated and not a dependency of HDR work.
- **Validation:** Interactive workflow checks and historical-output comparisons
  were recorded privately; no maintained public synthetic suite exists.
- **Next action:** Separate reusable GUI behavior from CIHR policy before any
  upstream proposal.

### CIHR-002: Source and air-kerma workflow automation

- **Type:** Workflow enhancement.
- **Motivation:** Resolve source-related GUI selections and air-kerma values
  through existing eb_gui plan/database behavior instead of repeated manual
  entry.
- **Branch/commit:** `mjm-vpm-workflow` at
  `09e55d93a793f2074756dd31184d64fc077714fe`.
- **Fork:** Pushed.
- **Upstream:** Under consideration; current behavior is CIHR-oriented and is not
  evidence of general HDR source-model or calibration support.
- **CIHR:** Integrated in `cihr/all-fixes`; included in `eb_gui_dev_v5`.
- **HDR:** Not integrated.
- **Validation:** CIHR preflight and generated-input comparisons were recorded
  privately.
- **Next action:** Keep separate from HDR source-selection and calibration work.

### CIHR-003: Manifest-driven egsinp preflight and generation

- **Type:** Feature.
- **Motivation:** Provide auditable, fail-fast, repeatable batch input generation
  while retaining eb_gui as the scientific input writer.
- **Branch/commits:** `mjm-batch-egsinp` at
  `c24ee1740608d3b4e6ce491d3d7960f75ff2481d`,
  `03a06be226b85cc5fe342ea981fbf8c70dcb27ba`,
  `198922d04cda88e444a209f14e10e2c5d7dc7bc6`, and
  `9619173e5b647f78def433e8f463abb8275fb995`.
- **Fork:** Pushed.
- **Upstream:** Under consideration; manifest concepts may be reusable, but the
  current contract was developed for CIHR operations.
- **CIHR:** Integrated in `cihr/all-fixes`; deployed as frozen generator
  `eb_gui_dev_v3` from `9619173e5b647f78def433e8f463abb8275fb995`.
- **HDR:** Not integrated.
- **Validation:** Invalid-manifest, no-overwrite, write-failure, source/air-kerma,
  and historical-output checks are recorded in the private binary registry.
- **Next action:** Preserve the frozen CIHR generator; review API generality and
  add maintained synthetic fixtures before proposing upstream.

### CIHR-004: Manifest-driven headless result export

- **Type:** Feature and workflow safeguard.
- **Motivation:** Reuse eb_gui dose, contour, DVH, metric, and output logic for
  auditable batch export without manual GUI repetition.
- **Branch/commits:** `cihr/export-manifest` at
  `6f8637e82ff73f265774f0dd413d28c4de2f6315`; missing-dose hard failure at
  `3e1e74d8f927e4108b9e31cf8116caad3144ca4c`.
- **Fork:** Pushed.
- **Upstream:** Under consideration; current orchestration contract is
  CIHR-oriented.
- **CIHR:** Integrated in `cihr/all-fixes`; `eb_gui_dev_v4` is the retained
  pre-v5 export sidecar, and `eb_gui_dev_v5` is the current corrected exporter.
- **HDR:** Not integrated.
- **Validation:** Manual-versus-headless exports matched in the initial
  validation set; later campaign preflight, completeness, and numerical-audit
  evidence is recorded privately.
- **Next action:** Preserve fail-fast behavior and isolate a public synthetic
  export fixture before any upstream proposal.

## Active HDR corrections and prerequisites

The following items were confirmed by the 2026-10-27 Phase 1 source audit. They
remain separate changes; implementing one does not validate the others.

### HDR-001: Physical RTPLAN channel timing

- **Type:** Bug fix and scientific prerequisite.
- **Problem:** Non-permanent plans currently treat cumulative time-weight
  differences as seconds. Each channel instead requires
  `ChannelTotalTime * delta_weight / final_weight`.
- **Implementation:** Original correction `e69ee28`; reviewed production
  refinement `872155a`; both integrated in `hdr-lung/development`.
- **Internal tests:** Helper and synthetic RTPLAN import coverage at `5104481`
  and `f13fec9`; whitespace-only cleanup at `a0a93ad`.
- **Fork/upstream:** Integrated on the fork's HDR development branch. No
  upstream PR has been submitted. The temporary implementation branch was
  retired locally and from the fork after its patches and tests were verified on
  the cumulative branch. A future production-only branch should be prepared
  from current CLRP `main`; internal test files must remain internal.
- **CIHR:** Not applicable to the permanent-LDR workflow; no integration planned
  without regression evidence.
- **HDR/deployment:** Integrated through `f13fec9` in
  `hdr-lung/development`. Validation executable
  `eb_gui_hdr_validation_8ef54b8` was built from cumulative commit
  `8ef54b807413c2666d85faf8db0baa63a447096d` and deployed only to the isolated
  validation runtime; its SHA-256 is
  `13601c09a8746156f9ee7fa882bf83c6ba130a0ea5d78b795a9064f81f070411`.
- **Validation:** Eight helper-level groups and three parser-path RTPLAN import
  cases passed normally and under AddressSanitizer, leak detection, and
  UndefinedBehaviorSanitizer. Coverage includes independent per-channel
  normalization, stationary-position tolerance, inactive zero-duration pairs,
  malformed timing rollback, and permanent-LDR weighted-position compatibility.
  The cumulative parser and timing suite passed and the full Qt application
  built successfully. Local import of an approved anonymized HDR RTPLAN in the
  isolated validation runtime matched independently established dwell count,
  spatial positions, individual dwell times, and total planned duration. No
  simulation or absolute-dose validation was performed.
- **Upstream PR summary:**
  - **Problem:** RTPLAN cumulative time-weight differences were used directly as
    seconds, producing incorrect HDR dwell durations when channel weights were
    normalized to an arbitrary final value.
  - **Correction:** Convert each stationary dwell interval independently to
    physical seconds using its channel total time and final cumulative weight;
    validate malformed timing and unsupported transit intervals while
    preserving permanent-LDR positioning.
  - **Validation:** Synthetic single- and multi-channel timing, malformed-input
    rollback, permanent-LDR compatibility, parser integration, sanitizers, and
    an approved local anonymized-plan import all passed. Dose simulation,
    source calibration, and absolute-Gy interpretation remain unvalidated.
- **Next action:** Prepare a new production-only branch only when an upstream PR
  is approved.

### HDR-002: Atomic dwell/activity loading and scaling state

- **Type:** Bug fix and scientific prerequisite.
- **Problem:** Manual and automatic loading could leave file path, type, ordered
  values, and maximum dwell/activity inconsistent or stale. Automatic loading
  could loop or miscount, and input generation could silently fall back to equal
  weights while retaining stale scaling state.
- **Production commit:** `f44ca7c1e3fc6e6df67644d5829c3835c60049fb`.
- **Internal tests:** Application-state coverage at
  `f02dcee913f833f986825a71b8636d1163e3a29f`; test-backend cleanup at
  `dfbe4557a230b098f06d1633adc2f00d955d4a8c`.
- **Fork/upstream:** Integrated in `hdr-lung/development`; no upstream PR or
  production-only contribution branch has been created.
- **CIHR/HDR/deployment:** Not integrated into CIHR and not deployed. Present in
  cumulative HDR development only.
- **Validation:** Synthetic application-state tests cover `.dwell` loading,
  `.activity` fallback, multiline ordered weights in generated input,
  independently calculated maximum-dwell scaling, valid zero activity weights,
  missing/malformed/nonfinite/negative/all-zero/count-mismatched files,
  post-selection file mutation, stale-state clearing, temporary-treatment hard
  failure, and permanent-LDR compatibility. The test passed normally and under
  AddressSanitizer and UndefinedBehaviorSanitizer. LeakSanitizer was limited by
  unrelated pre-existing Qt offscreen and GUI-object lifetime allocations; no
  reported leak stack involved HDR-002 functions. All existing parser/timing
  regressions passed normally and with sanitizers, and the full application
  built successfully.
- **Upstream PR summary:**
  - **Problem:** Dwell/activity loading updated path, value count, and maximum
    scaling state inconsistently. Missing or invalid files could retain stale
    state or silently generate equal source weights, compromising HDR input
    weighting and scaling.
  - **Correction:** Parse and validate the complete file and transformation
    count before atomically committing path, file type, ordered values, and
    maximum. Revalidate before input generation and abort temporary-treatment
    generation on missing or incompatible state.
  - **Validation:** Synthetic manual-state, automatic `.dwell`/`.activity`,
    malformed-input, stale-transition, generated-source-weight, scaling, and
    permanent-LDR compatibility cases passed. Full application build and
    existing parser/timing regressions passed. Source calibration, reference
    time/decay, run mode, orientation, CT geometry, and absolute-Gy validation
    remain separate prerequisites.
- **Next action:** Review before deployment or preparation of any
  production-only upstream branch.

### HDR-003: Stepping-source run-mode safeguards

- **Type:** Bug fix and scientific prerequisite.
- **Problem:** A stepping HDR source can be generated without enforcing
  superposition mode, leaving all dwell geometries active together.
- **Branch/commit:** Proposed `fix/hdr-stepping-source-run-mode`; not implemented.
- **Fork/upstream:** Not pushed; upstream contribution planned if the correction
  remains general-purpose.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** HDR mode enforcement without forcing TG-43 water, plus
  unchanged permanent-LDR behavior.
- **Next action:** Implement independently after timing and loader review.

### HDR-004: Air-kerma calibration hard failure

- **Type:** Bug fix and scientific prerequisite.
- **Problem:** Missing or invalid source-model air-kerma calibration can retain a
  clinical scale numerically after warning of a fallback, yielding output that
  must not be interpreted as Gy.
- **Production commit:** `bfd63877b28fec4ff4bbb26f0c6d7f59f25eab77`.
- **Internal test commit:** `5d3702a62ef694a8b759c98428d6b0618ec194a7`.
- **Correction:** Air-kerma scaling now requires finite positive clinical
  strength, exposure time, and source-model air-kerma per primary history. Input
  generation aborts when the source geometry is unavailable or its calibration
  is missing, malformed, nonfinite, zero, or negative. No calibration constant
  or source-library file was added or modified.
- **Fork/upstream:** Integrated locally on `hdr-lung/development`; push pending
  completion of the approved repair sequence. Group as a general
  source-normalization fail-fast contribution; no upstream branch or PR exists.
- **CIHR/HDR/deployment:** Not synchronized to CIHR and not deployed.
- **Validation:** A focused Qt Core test independently checks the scaling formula
  and missing-file, missing-header, malformed, nonfinite, zero, and invalid-input
  failures. It passed, and the full application compiled and linked. The existing
  HDR dwell application-state target rebuilt with the new helper, but its
  offscreen execution stopped at a pre-existing modal dialog and was terminated;
  no pass is claimed for that execution.
- **Next action:** Establish model-specific calibration provenance under VAL-001,
  then repeat end-to-end generated-input and dose validation. Missing calibration
  must remain a hard failure.

### HDR-005: Source orientation support

- **Type:** Scientific prerequisite.
- **Problem:** Plan import writes translations but does not preserve control-point
  source orientation needed for anisotropic source models.
- **Branch/commit:** Proposed `fix/hdr-source-orientation`; not implemented.
- **Fork/upstream:** Not pushed; upstream status under consideration.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** Defined source-axis convention, synthetic rotations,
  model-specific checks, and an independently reviewed orientation reference.
- **Next action:** Confirm orientation metadata and selected source model before
  choosing a mapping or fallback.

### HDR-006: Referenced-source validation

- **Type:** Scientific prerequisite.
- **Problem:** Plan import uses one global source while channel-level source
  references are not enforced. Unsupported multi-source plans could therefore be
  interpreted silently.
- **Branch/commit:** Not assigned; not implemented.
- **Fork/upstream:** Not pushed; upstream status under consideration.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** Single-source acceptance and explicit rejection of
  unsupported missing, unknown, or multiple references.
- **Next action:** Add a fail-fast preflight before general multi-source support.

### HDR-007: CT image-plane geometry preflight

- **Type:** Scientific prerequisite.
- **Problem:** Current CT loading is reliable only under restricted standard-axis,
  matrix, ordering, and spacing assumptions; unsupported geometry is not fully
  rejected.
- **Branch/commit:** Proposed `fix/ct-image-plane-preflight`; not implemented.
- **Fork/upstream:** Not pushed; upstream status under consideration.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** Synthetic standard, oblique, rectangular, reversed,
  irregular-spacing, and frame-mismatch series.
- **Next action:** Add explicit rejection first; generalize geometry only when
  requirements and references justify it.

### HDR-008: RT Plan timestamp semantics

- **Type:** Bug fix and validation prerequisite.
- **Problem:** RT Plan Date/Time is logged as treatment time even though it is a
  plan-instance timestamp and must not drive radioactive decay correction.
- **Branch/commit:** Not assigned; not implemented. Automatic decay remains
  disabled in current source.
- **Fork/upstream:** Not pushed; upstream status under consideration.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** Correct labeling and proof that no dose-scaling path
  uses plan modification timestamps.
- **Next action:** Correct semantics independently of calibration work.

### HDR-009: RTPLAN import duration-log formatting

- **Type:** Bug fix.
- **Problem:** The RTPLAN import log appends numeric `treatmentTime` directly to
  a `QString`, so the total duration can be omitted or displayed incorrectly.
- **Branch/commit:** Not assigned; not implemented.
- **Fork/upstream:** Not pushed; upstream status under consideration.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** Explicit numeric formatting, planned-duration
  labeling, units, and a regression check for temporary and permanent plans.
- **Next action:** Correct independently after HDR-002; do not combine with
  timestamp semantics or scaling changes.

## Deferred validation and engineering

### VAL-001: Flexisource model and calibration qualification

- **Type:** Validation.
- **Status:** Required; not complete.
- **Purpose:** Establish the clinical source model, geometry, spectrum, and
  source-model-specific air-kerma strength per primary history before absolute
  HDR scaling. Establish the source-strength reference time and an approved
  decay-to-treatment-time policy using authoritative treatment timing; RT Plan
  modification timestamps are not treatment delivery time.
- **Integration/deployment:** No source selection or HDR binary is approved by
  this record.
- **Current evidence:** The locally bundled Flexisource geometry searched during
  HDR-004 development contains no `air kerma:` calibration header. HDR-004 does
  not insert the candidate value `1.16267E-13` or otherwise alter the model.
- **Next action:** Establish why the local model differs from the authoritative
  CLRP version and verify that geometry, spectrum, calibration method, units, and
  source version are mutually consistent before adding qualified metadata or
  interpreting absolute dose.

### VAL-002: HDR end-to-end scientific validation

- **Type:** Validation.
- **Status:** Required; not complete.
- **Purpose:** Validate import, timing, source mapping/orientation, geometry,
  generated input, transport setup, scaling, and reported dose as one chain.
- **Integration/deployment:** Parser import success alone is insufficient; no HDR
  workflow is approved for research dose generation.
- **Next action:** Define synthetic and independent-reference acceptance criteria
  after the blocking corrections are reviewed.

### VAL-003: Historical permanent-LDR regression

- **Type:** Validation.
- **Status:** Required for HDR changes; not complete as a maintained suite.
- **Purpose:** Demonstrate that timing, loader, run-mode, and scaling corrections
  do not alter supported permanent-LDR behavior.
- **Integration/deployment:** Existing CIHR binaries remain pinned and protected.
- **Next action:** Build public-safe synthetic permanent-LDR fixtures and compare
  generated inputs through stable APIs.

### AUDIT-001: Comprehensive DICOM parser engineering audit

- **Type:** Validation and deferred engineering.
- **Status:** Deferred; the four focused parser corrections do not constitute a
  full parser audit.
- **Purpose:** Review pointer decoding, ownership, length accounting, nested
  sequences, transfer syntax, malformed input, and bounded fuzzing systematically.
- **Integration/deployment:** No additional defects are claimed by this entry.
- **Next action:** Begin only under a separately approved scope; retain synthetic
  data and scientific-behavior boundaries.

## Maintenance rule

Every feature task must close by updating the relevant entry with exact commits
and one or more of: implemented, independently tested, integrated, deployed, or
merged upstream. Upstream-facing branches must not contain this governance file;
record their status on `hdr-lung/development` in a separate documentation commit.

For every general-purpose correction, preserve a concise **Upstream PR summary**
inside its existing tracker entry with three fields: **Problem**, **Correction**,
and **Validation**. State observed outcomes and material limitations without
patient-derived details, private paths, or internal development narration. Keep
production commits, internal test commits, integration/deployment state, and
upstream submission state recorded separately. Do not create separate PR-draft
files or tracking systems.
