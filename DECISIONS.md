# Repo Decisions

A dated log of decisions made for this repository. Standing rules that agents
must follow live in `AGENTS.md`; this file records when and what was decided.

| Date | Decision |
|------|----------|
| 2026-10-04 | ZCode may autonomously commit and push this repo without asking for confirmation. (Also recorded as a standing rule in `AGENTS.md`.) |
| 2026-10-04 | `.gitignore` targets macOS (`.DS_Store`, resource-fork files, etc.) and C++ development (object files, libraries, executables, CMake builds, IDE folders). |
| 2026-10-04 | Split agent guidance across two files: `AGENTS.md` holds live operating rules agents should follow each session; `DECISIONS.md` (this file) stays a dated historical log. |
