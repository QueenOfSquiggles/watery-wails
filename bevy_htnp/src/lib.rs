#![feature(trivial_bounds)]

pub mod data;
pub mod state;
pub mod tasks;

pub mod prelude {
    use bevy::app::App;
    use bevy::app::Plugin;

    pub use crate::data::*;
    pub use crate::state::*;
    pub use crate::tasks::*;

    pub struct HtnPlanningPlugin {
        initial_world: Option<TruthSet>,
    }

    impl Plugin for HtnPlanningPlugin {
        fn build(&self, app: &mut App) {
            app.insert_resource(GlobalHtnTaskRegistry::default());

            if let Some(world) = &self.initial_world {
                app.insert_resource(world.clone());
            }
        }
    }

    impl HtnPlanningPlugin {
        pub fn new() -> Self {
            Self {
                initial_world: None,
            }
        }

        pub fn with(world: impl Into<TruthSet>) -> Self {
            Self {
                initial_world: Some(world.into()),
            }
        }
    }
}
