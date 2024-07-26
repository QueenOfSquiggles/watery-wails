#![cfg(feature = "default_tasks")]

use bevy::{
    app::{App, Update},
    log::debug,
    math::Vec3,
    prelude::{Commands, Component, Entity, Query, Res, Transform},
    time::Time,
};

use crate::prelude::HtnAgentState;

pub(crate) fn plugin(app: &mut App) {
    app.add_systems(Update, (system_goto, system_debug));
}

#[derive(Component)]
/// This is the most incredibly brain dead version of a GoTo task. Highly recommmend only using this as a learning reasource to make an implementation that works in your game.
pub struct HtnTaskSimpleGoTo(pub Vec3);

#[derive(Component)]
/// An exceptionally basic task that will simply debug print a message upon completion. Useful for task stubs but not much else.
pub struct HtnTaskDebug(pub String);

fn system_goto(
    mut query: Query<(Entity, &mut Transform, &HtnTaskSimpleGoTo, &HtnAgentState)>,
    time: Res<Time>,
    mut commands: Commands,
) {
    for (e, mut trans, target, state) in query.iter_mut() {
        if *state != HtnAgentState::Running {
            continue;
        }
        let delta_pos = target.0 + trans.translation;
        if delta_pos.length_squared() < 0.1 {
            commands.entity(e).insert(HtnAgentState::Success);
            continue;
        }
        trans.translation += delta_pos * time.delta_seconds();
    }
}

fn system_debug(query: Query<(Entity, &HtnTaskDebug, &HtnAgentState)>, mut commands: Commands) {
    for (e, msg, state) in query.iter() {
        if *state != HtnAgentState::Running {
            continue;
        }
        debug!("HTN DEBUG {}", msg.0);
        commands.entity(e).insert(HtnAgentState::Success);
    }
}
