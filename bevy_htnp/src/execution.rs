use bevy::{ecs::system::EntityCommands, prelude::*};

use crate::{data::WorldState, planning::HtnAgent, tasks::TaskRegistry};

pub(crate) fn plugin(app: &mut App) {
    app.add_systems(
        Update,
        (
            create_plans_for_unplanned_agents,
            handle_agent_state_changes,
        ),
    );
}

#[derive(Component)]
pub struct HtnAgentWorld(pub WorldState);

#[derive(Component, Debug)]
pub struct HtnAgentPlan {
    pub plan_stack: Vec<String>,
}

#[derive(Component)]
pub struct HtnAgentCurrentTask(pub String);

#[derive(Component, PartialEq)]
pub enum HtnAgentState {
    // TODO: should this be constructed in a way that allows observers?
    Running,
    Success,
    Failure,
}

fn create_plans_for_unplanned_agents(
    query: Query<(Entity, &HtnAgent, Option<&HtnAgentWorld>), Without<HtnAgentPlan>>,
    world: Res<WorldState>,
    registry: Res<TaskRegistry>,
    mut command: Commands,
) {
    for (entity, agent, ctx) in query.iter() {
        let mut agent_context = world.clone();
        if let Some(w) = ctx {
            agent_context.append(&w.0);
        }
        let plan = agent.create_plan(agent_context, &registry);
        let Ok(plan_data) = plan else {
            continue;
        };
        command.entity(entity).insert(HtnAgentPlan {
            plan_stack: plan_data.decompose_tasks(),
        });
    }
}

fn handle_agent_state_changes(
    mut query: Query<(
        Entity,
        &mut HtnAgentPlan,
        Option<&HtnAgentState>,
        Option<&HtnAgentCurrentTask>,
    )>,
    task_registry: Res<TaskRegistry>,
    mut command: Commands,
) {
    for (entity, mut plan, state, task) in query.iter_mut() {
        if let Some(agent_state) = state {
            match agent_state {
                // running states process as handled by that task ( user defined system(s) )
                HtnAgentState::Running => continue,
                // when a task succeeds, push this state. Old task removed and next task injected
                HtnAgentState::Success => {
                    if let Some(next_task) = plan.plan_stack.pop() {
                        if let Some(prev_task) = task {
                            try_remove_previous_task(
                                &mut command.entity(entity),
                                &task_registry,
                                prev_task,
                            );
                        }
                        push_task_to_agent(next_task, &mut command.entity(entity), &task_registry);
                    } else {
                        command
                            .entity(entity)
                            .remove::<(HtnAgentCurrentTask, HtnAgentState, HtnAgentPlan)>();
                    }
                }
                // When a task fails for some reason we push this state, which purges existing execution data
                HtnAgentState::Failure => {
                    command
                        .entity(entity)
                        .remove::<(HtnAgentCurrentTask, HtnAgentState, HtnAgentPlan)>();
                }
            }
        } else {
            if let Some(next_task) = plan.plan_stack.pop() {
                push_task_to_agent(next_task, &mut command.entity(entity), &task_registry);
            } else {
                command
                    .entity(entity)
                    .remove::<(HtnAgentCurrentTask, HtnAgentState, HtnAgentPlan)>();
                warn!("Failed to initialize a plan for entity {}", entity);
            }
        }
    }
}

fn push_task_to_agent(
    task: String,
    mut entity: &mut EntityCommands,
    task_registry: &Res<TaskRegistry>,
) {
    let Some(task_data) = task_registry.get_named(&task) else {
        return;
    };
    task_data.add(&mut entity);
    entity.insert((HtnAgentCurrentTask(task), HtnAgentState::Running));
}

fn try_remove_previous_task(
    mut entity: &mut EntityCommands,
    task_registry: &Res<TaskRegistry>,
    previous: &HtnAgentCurrentTask,
) {
    let Some(task) = task_registry.get_named(&previous.0) else {
        return;
    };
    task.remove(&mut entity);
}
