# Workspace Artifact Placement

Apply this policy whenever EMULATORS work creates plans, specifications, contracts, workbooks, source registries, validation matrices, generated inputs, scripts, state, or other support material.

## Final repository deliverables

- Reserve `docs/` exclusively for final, maintained documentation about the emulated system, its architecture, operation, use, testing, or supported behavior.
- Do not use `docs/`, including directories named `*Data`, as an agent workspace, staging area, scratch directory, or store for code-generation inputs.
- Publish material from `.codex/` into `docs/` only when it has become an explicitly requested final documentation deliverable. Use `$emulators-documentation-authoring` when its trigger applies.

## Maintained working material

- Store code-generation support and maintained intermediate artifacts under a descriptive hierarchy rooted at `.codex/work/`.
- For new machines, prefer `.codex/work/emulators/<machine>/<model-or-variant>/<scope>/`. Add a milestone or subsystem level when it makes ownership clearer, for example `.codex/work/emulators/gameboy/dmg-01/milestones/hito-0/`.
- Keep related specifications, decision ledgers, CPU workbooks, chip and I/O contracts, source registries, validation matrices, risks, and milestone definitions together in that scoped directory unless one artifact is intentionally shared by several scopes.
- Put reusable skill instructions, references, scripts, templates, and assets inside the owning `.codex/skills/<skill>/` directory rather than the task workspace.
- Put maintained generators, structured sources, and state outside a skill under a purpose-specific `.codex/<purpose>/` hierarchy, separating `sources/`, `scripts/`, and `state/` when those categories exist.

## Temporary material and implementation outputs

- Store disposable QA output, renderings, logs, and experiments under `.codex/tmp/<task>/`; remove them after verification when they are no longer needed.
- Place authorized implementation code, tests, project metadata, ROM declarations, and runtime resources in their normal repository destinations. The `.codex/work/` rule applies to supporting and intermediate material, not to the requested product code itself.
- Before creating an artifact, classify it as final documentation, maintained working material, reusable skill material, temporary material, or implementation output and choose its path accordingly.
