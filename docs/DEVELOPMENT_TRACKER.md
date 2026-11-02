# Development Tracker

Last reviewed: 2026-11-01

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
  upstream PR has been submitted. A future production-only branch should be
  prepared from current CLRP `main`; internal test files must remain internal.
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
  built successfully.
- **Next action:** Validate the approved anonymized HDR plan in the isolated
  validation runtime. Retire the temporary development branch after verifying
  the cumulative branch and fork contain all implementation and test history;
  prepare a new production-only branch only when an upstream PR is approved.

### HDR-002: Atomic dwell/activity loading and scaling state

- **Type:** Bug fix and scientific prerequisite.
- **Problem:** Automatic loading can leave the path, maximum dwell, file type, or
  scaling state incomplete or stale.
- **Branch/commit:** Proposed `fix/dwell-loader-scaling-state`; not implemented.
- **Fork/upstream:** Not pushed; upstream status under consideration.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** Synthetic dwell and activity files, count mismatch,
  stale-state, zero/negative value, and scaling tests.
- **Next action:** Start only after HDR-001 review.

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
- **Branch/commit:** Proposed `fix/source-air-kerma-calibration-failure`; not
  implemented.
- **Fork/upstream:** Not pushed; upstream contribution planned if model-agnostic.
- **CIHR/HDR/deployment:** Not integrated or deployed.
- **Validation required:** Known calibration, missing metadata, zero/invalid
  calibration, unit checks, and explicit generation failure.
- **Next action:** Establish model-specific calibration evidence before absolute
  HDR dose interpretation.

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

## Deferred validation and engineering

### VAL-001: Flexisource model and calibration qualification

- **Type:** Validation.
- **Status:** Required; not complete.
- **Purpose:** Establish the clinical source model, geometry, spectrum, and
  source-model-specific air-kerma strength per primary history before absolute
  HDR scaling.
- **Integration/deployment:** No source selection or HDR binary is approved by
  this record.
- **Next action:** Obtain authoritative model evidence and compare against an
  independent reference.

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
