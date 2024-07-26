---
created: 2024-07-25T21:27:22.865Z
updated: 2024-07-25T21:27:37.584Z
assigned: ""
progress: 0
tags:
  - HTN
---

# Rewrite Predicates to support dynamic comparisons

Predicates should support value comparison values such that a `Context` stores a series of variant values and a predicate stores comparison such as equality, inequality, less/greater than, etc... This will allow for more versatile use of tasks such that a single entry can be made for dynamic information such as the current room the agent occupies or their hunger meter.

## Sub-tasks

- [ ] Create `Predicate` enum with various comparison values (optionally a custom function comparison?)
