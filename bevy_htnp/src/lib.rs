#![feature(trivial_bounds)]

pub mod data;
#[cfg(feature = "default_tasks")]
pub mod default_tasks;
pub mod execution;
pub mod planning;
pub mod tasks;

pub mod prelude {
    use bevy::app::App;
    use bevy::app::Plugin;

    pub use crate::data::*;
    #[cfg(feature = "default_tasks")]
    pub use crate::default_tasks::*;
    pub use crate::execution::*;
    pub use crate::planning::*;
    pub use crate::tasks::*;

    pub struct HtnPlanningPlugin {
        initial_world: Option<WorldState>,
    }

    impl Plugin for HtnPlanningPlugin {
        fn build(&self, app: &mut App) {
            app.insert_resource(self.initial_world.as_ref().cloned().unwrap_or_default());
            crate::data::plugin(app);
            crate::planning::plugin(app);
            crate::tasks::plugin(app);
            crate::execution::plugin(app);
            #[cfg(feature = "default_tasks")]
            crate::default_tasks::plugin(app);
        }
    }

    impl HtnPlanningPlugin {
        pub fn new() -> Self {
            Self {
                initial_world: None,
            }
        }

        pub fn with(world: impl Into<WorldState>) -> Self {
            Self {
                initial_world: Some(world.into()),
            }
        }
    }
}

#[cfg(test)]
mod tests {}
