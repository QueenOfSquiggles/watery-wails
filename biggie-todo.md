# Todo

Todo lists for each crate in this workspace, which will ultimately be a game ***someday***

## Watery Wails

## Bevy HTNP

- Implement Random Weighted priority for goals using goal utility factor
- Integrate `bevy_trait_query` to enable:
  - Implement `TaskProvider` trait
  - Implement `GoalProvider` trait
  - Integrate trait queries into change detection for available tasks/goals.
  - Build static struct implementors of `TaskProvider` and `GoalProvider`
- Create Event & Observer system for detecting replanning requests, task invalidation, etc...
- Convert frequently repeated `String`s to `Arc<str>` in a `UniqueName` struct that handles references to byte sequences
  - > This makes all string sequences longer than ~4 characters more efficient due to sizing of pointers versus `Vec<u8>` which is how `String` is contained. Every unique string would only be stored in memory once, allowing for a significantly larger set of names and/or longer/descriptive names while retaining the same memory budget

## Bevy HTNP Scripting

- Decide if this crate would be worth the effort
- Pick a scripting language (Luau? For ease of access and robustness?)
  - How to provide functions to scripting language while also providing APIs to rust-side for Watery Wails???
- Custom implementations of each trait
- Single coherent `Component` for handling scripted tasks (because bevy does not currently support dynamic queries like that)
- Implement `Asset` type for the script
  - Figure out how to support hot reloading? Especially with HTNP's replanning request system
