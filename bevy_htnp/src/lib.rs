#![feature(trivial_bounds)]

pub mod data;
pub mod execution;
pub mod state;
pub mod tasks;

pub mod prelude {
    use bevy::app::App;
    use bevy::app::Plugin;

    pub use crate::data::*;
    pub use crate::execution::*;
    pub use crate::state::*;
    pub use crate::tasks::*;

    pub struct HtnPlanningPlugin {
        initial_world: Option<Context>,
    }

    impl Plugin for HtnPlanningPlugin {
        fn build(&self, app: &mut App) {
            if let Some(world) = &self.initial_world {
                app.insert_resource(world.clone());
            }
            crate::data::plugin(app);
            crate::state::plugin(app);
            crate::tasks::plugin(app);
            crate::execution::plugin(app);
        }
    }

    impl HtnPlanningPlugin {
        pub fn new() -> Self {
            Self {
                initial_world: None,
            }
        }

        pub fn with(world: impl Into<Context>) -> Self {
            Self {
                initial_world: Some(world.into()),
            }
        }
    }
}
