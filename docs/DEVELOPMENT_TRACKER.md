# Development Tracker

Last reviewed: 2026-11-05

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
- **Fork/upstream:** Integrated and pushed on `hdr-lung/development`; no upstream
  branch or PR exists. Plan a separate general simulation lifecycle PR; do not
  combine it with HDR calibration or EGSnrc infrastructure.
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
- **Production commit:** `168bc2e9c7cd5f2263daf76139aa1e1f0e0c61f7`.
- **Internal test commit:** `4b4478e42b7dcf00f998eeb63d30688c66d61e23`.
- **Correction:** Temporary treatments always generate `superposition` run mode,
  independently of TG-43 water selection. Permanent treatments preserve the
  selected heterogeneous mode, while TG-43 continues to force superposition.
- **Fork/upstream:** Integrated and pushed on `hdr-lung/development`; no upstream
  branch or PR exists. Plan an HDR run-mode production contribution separate from
  calibration and lifecycle handling.
- **CIHR/HDR/deployment:** Not synchronized to CIHR and not deployed.
- **Validation:** Focused temporary, TG-43, and permanent-LDR mode cases passed;
  the complete Qt application compiled and linked.
- **Batching limitation:** The input writer enforces this invariant when a run is
  correctly configured as temporary, but the existing manifest generator still
  assumes the CIHR permanent/prostate workflow and does not establish HDR dwell
  state. HDR-compatible CLI batch generation remains separate work.
- **Next action:** Preserve this invariant when defining an HDR manifest contract;
  do not reuse the existing CIHR batch assumptions unchanged.

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
- **Fork/upstream:** Integrated and pushed on `hdr-lung/development`; no upstream
  branch or PR exists. Group as a general source-normalization fail-fast
  contribution.
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
- **Production commit:** `d508414b763fe0f3e7d3b759b5156670ce2e1cfe`.
- **Internal test commit:** `9af2aa1d39405fb89aaffbaec7d5010262f3bcd2`.
- **Correction:** Temporary RTPLAN import now decodes binary FL Control Point
  Orientation `(300A,0412)` using the plan transfer syntax, validates finite
  nonzero and consistent vectors across each active dwell pair, normalizes one
  vector per dwell, and writes it as an EGSnrc `rotation vector`. The DICOM vector
  is the source long axis in insertion direction in patient coordinates; the
  EGSnrc transformation maps the model's local positive z-axis to that vector.
- **Cohort preflight:** All ten reviewed anonymized HDR plans omit optional
  Control Point Orientation. They are single-channel STEPWISE plans with complete,
  monotonic Control Point Relative Position and Control Point 3D Position pairs.
  Their ordered dwell centerlines are nondegenerate, but local curvature is
  sufficient that a single global direction is not defensible.
- **Current failure behavior:** Missing, malformed, zero, nonfinite, or
  inconsistent temporary-source orientation aborts the import with atomic
  rollback. Permanent LDR remains translation-only to preserve established
  behavior pending a separately justified LDR orientation change.
- **Fork/upstream:** Integrated and pushed on `hdr-lung/development`; no upstream
  branch or PR exists. Plan an HDR RTPLAN orientation contribution separate from
  source calibration and general lifecycle handling.
- **CIHR/HDR/deployment:** Not synchronized to CIHR and not deployed.
- **Validation:** Synthetic signed and non-unit vectors were normalized and
  retained in dwell order. Missing and inconsistent orientations were rejected;
  five RTPLAN import groups and eight timing groups passed, including permanent
  LDR compatibility. The complete Qt application compiled and linked.
- **Proposed surgical refinement:** Keep explicit DICOM orientation as the
  preferred path. When it is absent, show a blocking GUI warning that explains
  the missing anisotropy information and offers: abort (default); supply an
  externally established direction; or explicitly approve a local catheter-
  tangent approximation when the channel has complete monotonic relative
  positions and at least two distinct dwell centers. For that approximation,
  use one-sided endpoint tangents and centered interior tangents, with sign from
  larger toward smaller relative position to represent insertion direction.
  Record the assumption in the transformation header/log and generated-input
  provenance. Hard-fail degenerate, incomplete, nonmonotonic, or unsupported
  channel geometry. Never infer or apply a fallback silently.
- **Next action:** Review the proposed user-choice contract before changing code,
  then include orientation provenance in generated-input review and compare
  anisotropy-sensitive dose against an independent source-model reference.

### HDR-006: Referenced-source validation

- **Type:** Scientific prerequisite.
- **Problem:** Plan import uses one global source while channel-level source
  references are not enforced. Unsupported multi-source plans could therefore be
  interpreted silently.
- **Production commit:** `35f32f84e6db7169805dd3436d641c89b9d0c676`.
- **Internal test commit:** `b4bdbdc0458dcc11048006aa282ef233c67efd31`.
- **Implemented correction:** RTPLAN import currently requires exactly one
  positive Source Number and exactly one matching positive Referenced Source
  Number in every channel. Missing, malformed, unknown, duplicate-reference, and
  multi-source states fail before transformations are committed.
- **Cohort preflight defect:** All ten reviewed plans use Source Number zero and
  matching Referenced Source Number zero. DICOM requires a unique integer
  identifier but does not require positivity. The current implementation's
  positive-only check rejects every plan with status 311 before orientation is
  evaluated; therefore this safeguard is not release-ready.
- **Fork/upstream:** Integrated and pushed on `hdr-lung/development`; no upstream
  branch or PR exists. Plan an HDR RTPLAN source validation contribution;
  multi-source support is not implied.
- **CIHR/HDR/deployment:** Not synchronized to CIHR and not deployed.
- **Validation:** Synthetic multi-channel HDR and permanent-LDR imports passed
  with valid references. Missing channel references, unknown references, and
  multiple declared sources were rejected with atomic rollback. The complete Qt
  application compiled and linked.
- **Next action:** Surgically accept any valid integer Source Number, including
  zero, while retaining exact reference matching and single-source enforcement.
  Add focused zero-valued and mismatched-reference tests. Preserve explicit
  multi-source failure until separately designed and validated.

### HDR-007: CT image-plane geometry preflight

- **Type:** General-purpose geometry correction and scientific prerequisite.
- **Problem:** CT loading ignored Image Orientation Patient, confused DICOM
  rows/columns with patient x/y, and relied on Slice Thickness for z boundaries.
  Reversed, swapped, rectangular, descending, or empty-thickness series could be
  mapped incorrectly.
- **Production commits:** Orientation-aware mapping
  `226b65b706a0b3b1dd13547accac688ec35588fb`; IPP-derived z boundaries
  `6ef533defbf8ba0a55b4a3a5cf00acaf87e91cba`.
- **Internal tests:** Synthetic geometry matrix
  `4ad1db24f23a3b800c63fa74f4ab9ae728e3dcbe`; cumulative test linkage
  `55e747fe53078b10a230fdc5dc98f06427cbc759`.
- **Correction:** Decode pixels in DICOM row-major order, map them through the
  DICOM affine into increasing patient x/y axes, sort slices by physical IPP z,
  derive multi-slice boundaries from center midpoints, support irregular spacing,
  and explicitly reject oblique, duplicate, malformed, or inconsistent geometry.
  Single-slice series still require finite positive Slice Thickness.
- **Fork/upstream:** Integrated and pushed on `hdr-lung/development`; no upstream
  branch or PR exists. Plan a production-only general CT geometry contribution.
- **CIHR/HDR/deployment:** Not synchronized to CIHR and not deployed.
- **Validation:** Eleven synthetic groups passed normally and under sanitizers,
  including conventional, reversed, swapped, rotated, rectangular, descending,
  irregular-z, empty-thickness, density/media, mask, and source/MAR checks. The
  ten reviewed HDR CT series each contain 512 consistent axis-aligned slices with
  complete geometry attributes, no duplicate centers, and uniform spacing; all
  are within the implemented acceptance envelope. Their CT, RTSTRUCT, and HDR
  RTPLAN Frame of Reference UIDs agree.
- **Remaining gaps:** The mapper intentionally rejects rather than supports
  oblique/gantry-tilted data. Cross-object Frame of Reference agreement was
  verified in this cohort but is not yet enforced by the GUI. Enhanced/multiframe
  CT remains outside the validated classic single-frame-series path.
- **Next action:** Reclassify HDR-007 as implemented and cohort-compatible. Add a
  small cross-object Frame of Reference preflight before research use; do not
  broaden to oblique or enhanced CT without separate requirements and tests.

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
- **Status:** File-level model qualification complete; clinical model identity
  and absolute-dose validation remain incomplete.
- **Purpose:** Establish the clinical source model, geometry, spectrum, and
  source-model-specific air-kerma strength per primary history before absolute
  HDR scaling. Establish the source-strength reference time and an approved
  decay-to-treatment-time policy using authoritative treatment timing; RT Plan
  modification timestamps are not treatment delivery time.
- **Integration/deployment:** No source selection or HDR binary is approved by
  this record.
- **Authoritative evidence:** Upstream egs_brachy commit
  `851ae045e82eb61c595a13cf9ffab3e2fe2e5b66` added the audited Flexisource
  `air kerma: 1.16267E-13` header. Comment-free normalized geometry tokens are
  identical before and after that commit, and no later upstream commit changes
  Flexisource. The installed geometry is byte-identical to the pre-header file;
  its source shape, boundary shape, and Ir-192 spectrum are byte-identical to the
  audited/current upstream files. The installed material definitions are
  semantically identical to the pinned source; current upstream only adds
  unrelated `AIR_PHANT`, while Flexisource media remain unchanged.
- **Interpretation:** The published header applies to the installed Flexisource
  file set under the same transport/calibration convention; it is not an
  invented local constant. However, all ten reviewed plans omit Source Model ID,
  manufacturer, description, and serial provenance. They establish only an
  Ir-192 line source, so DICOM alone does not establish that Flexisource was the
  clinical model. Current GUI source matching correctly cannot resolve a unique
  model from isotope-only metadata.
- **Update procedure:** Do not edit the shared installed library in place. Stage
  the exact authoritative Flexisource geometry from `851ae045` (or current main,
  which is identical) in a versioned isolated HDR egs_brachy library; verify the
  normalized geometry and dependency hashes; record commit and file hashes; and
  point only an approved HDR validation binary/runtime at that library. The
  header-only update does not require transport recompilation. Preserve the
  protected CIHR library/runtime unchanged.
- **Next action:** Obtain an authoritative external clinical record confirming
  the source model, then stage the audited header in isolation and perform
  generated-input dimensional checks plus independent absolute-dose validation.

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

### AUDIT-002: Installed egs_brachy and EGSnrc upstream delta

- **Type:** Runtime provenance and upgrade planning.
- **Status:** Read-only audit complete; no update performed.
- **Installed baseline:** EGSnrc_CLRP
  `e5f9ab6db3714f4aa41bd2fee79ad29117070c13` pins egs_brachy
  `5551a44110ffe57a42788379c0437362fe0e5b76`. The installed executable was built
  in July 2024. Its runtime library matches the pinned egs_brachy source by
  checksum; the runtime also contains active research inputs/outputs and must not
  be updated in place.
- **Current upstream:** The coherent current EGSnrc_CLRP reference is
  `5fb40723c23928eaf8c6aabe7db4c4ecbd9f33a9`, 95 superproject commits ahead,
  pinning egs_brachy `cb9ed4b1aa54e2714eddd091546c3c1fb2d0afd9`. Direct
  egs_brachy main is `2f5dd4ca92c690065e51cec8c90fd3a69b303ad2`, 38 commits
  ahead of the installed pin; its seven commits after the coherent pin add Iris
  eye plaques and documentation, not HDR-lung executable behavior.
- **Direct egs_brachy significance:** Net executable changes add an optional
  source-coordinate transform, expand environment variables in muen paths, fix
  stale parallel egsdat cleanup in run-anywhere output directories, and clean log
  output. No missing direct commit changes egs_brachy dose scoring or transport
  mathematics. The source-coordinate transform may support future local-to-world
  workflows but is not required for current patient-space transformations.
- **Library significance:** Upstream includes audited air-kerma headers and broad
  source-library maintenance, plus eye plaques/examples/phantoms. Some source
  models were added, removed, renamed, or otherwise changed, so wholesale library
  replacement is not a metadata-only operation. Flexisource is the separately
  qualified header-only case under VAL-001.
- **Parent EGSnrc significance:** The 95-commit delta includes EGSnrc v2025a,
  run-anywhere/path behavior, and fixes for phase-space partitioning, source-
  collection fluence, specialized fluence/phase-space scoring, egs_kerma,
  BEAMnrc splitting, and geometry helpers. The reviewed HDR/CIHR inputs use an
  isotropic photon source and ordinary phantom dose scoring rather than those
  specialized paths, so no audited commit directly explains their dose scale.
  A parent-framework update can still alter build/runtime behavior and requires
  deterministic numerical regression before research use.
- **Parallel relevance:** Current upstream changes the `egs-parallel` front end
  to resolve input/output directories explicitly, and egs_brachy cleans stale
  worker data from the output directory. The lower-level `egs-parallel-cpu`
  script is unchanged and still has the lock-wait bare-exit behavior; therefore
  upstream does not by itself resolve the observed missing lock or false-zero
  timeout. Worker startup remains a separate investigation.
- **Prioritized upgrade plan:** (1) stage only the qualified Flexisource header in
  an isolated HDR library after clinical model confirmation; (2) build a separate
  candidate from coherent EGSnrc_CLRP `5fb4072` with its pinned egs_brachy
  `cb9ed4b`, without touching protected CIHR assets; (3) run upstream tests plus
  focused single/parallel completion, TG-43/TG-186, timing, scaling, geometry, and
  permanent-LDR regressions; (4) compare deterministic numerical outputs against
  the pinned baseline and independent references; (5) promote only through the
  existing versioned-runtime approval process. Do not mix the full framework
  upgrade with GUI scientific fixes or selective source-library metadata.

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
