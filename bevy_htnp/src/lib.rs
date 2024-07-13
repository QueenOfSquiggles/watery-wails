#![feature(trivial_bounds)]

use bevy::prelude::*;
use data::World;
use tasks::GlobalHtnTaskRegistry;

pub mod data;
pub mod state;
pub mod tasks;

pub struct HtnPlanningPlugin {
    initial_world: Option<World>,
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

    pub fn with(world: impl Into<World>) -> Self {
        Self {
            initial_world: Some(world.into()),
        }
    }
}
