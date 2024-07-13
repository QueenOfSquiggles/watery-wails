use std::{collections::VecDeque, fmt::Debug, rc::Rc};

use bevy::prelude::*;

use crate::{
    data::World,
    tasks::{Task, TaskComponent},
};

#[derive(Component, Default)]
pub struct HtnAgent {
    goals: Vec<Goal>,
    current_plan: Option<Plan>,
    available_tasks: Vec<Task>,
    possibilities: PossibilitySpace,
}

#[derive(Component)]
pub struct HtnAgentWorld {
    pub world: World,
}

#[derive(Default, Clone, Debug)]
pub struct Goal(World);

#[derive(Default)]
pub struct Plan(VecDeque<TaskComponent>);

#[derive(Default, Debug)]
struct PossibilitySpace(Vec<(Plan, Vec<Goal>)>);

impl Debug for Plan {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_tuple("Plan")
            .field(&format_args!("Steps: {}", self.0.len()))
            .finish()
    }
}

impl HtnAgent {
    pub fn new() -> Self {
        Self::default()
    }
    pub fn add_task(&mut self, task: Task) {
        self.available_tasks.push(task);
    }
    pub fn has_plan(&self) -> bool {
        self.current_plan.is_some()
    }

    pub fn rebuild_possibility_space(&mut self, world: World) {
        // TODO: does this method need to be broken up more?
        self.possibilities.0.clear();
        let mut plan_indexer: usize = 0;
        let mut planning = Vec::<(usize, Vec<Task>, Vec<World>, Option<Goal>)>::new();
        // load initial available tasks into planning set
        let initial_tasks = self.possible_tasks(&world);
        for t in initial_tasks {
            let new_world = world.concat(&t.postconditions());
            plan_indexer += 1;
            planning.push((plan_indexer, vec![t], vec![new_world], None));
        }
        // find all valid paths, prune dead ends, extract goal-meeting paths into possibility space
        while !planning.is_empty() {
            // iteration caching
            let mut invalid_paths = Vec::<usize>::new();
            let mut n_plans = Vec::<(usize, Vec<Task>, Vec<World>, Option<Goal>)>::new();

            // for each current path, find valid next step and check that the world remains valid
            for (id, tasks, world_stack, goal_met) in planning.iter() {
                // get the top of the world stack, prune path if none
                let Some(world) = world_stack.last() else {
                    // no world means no tasks
                    invalid_paths.push(*id);
                    continue;
                };

                // if the goal is met, extract into possibility space, along with all goals that are met by this plan.
                if goal_met.is_some() {
                    invalid_paths.push(*id); // still purge since we already reached an end
                    let mut plan_items = VecDeque::<TaskComponent>::new();
                    for t in tasks {
                        plan_items.append(&mut t.decompose().clone().into())
                    }

                    let goals = self
                        .goals
                        .clone()
                        .into_iter()
                        .filter(|p| world.validate(&p.0))
                        .collect();

                    self.possibilities.0.push((Plan(plan_items), goals))
                }

                let available_tasks = self.possible_tasks(world);
                if available_tasks.is_empty() {
                    // no available tasks means no progress for that plan
                    invalid_paths.push(*id);
                    continue;
                }

                // create a virtual plan for each valid next task. Append new plans to the cache with updated stack values.
                for t in &available_tasks {
                    eprintln!("Considering task {:?}, depth={}", t, tasks.len());
                    // virtual paths for all valid task combinations
                    let v_world = world.concat(&t.postconditions());
                    let v_goal = self
                        .goals
                        .clone()
                        .into_iter()
                        .find(|p| v_world.validate(&p.0));
                    let mut v_tasks = tasks.clone();

                    plan_indexer += 1;
                    let mut v_world_stack = world_stack.clone();
                    v_tasks.push(t.clone());
                    v_world_stack.push(v_world);
                    n_plans.push((plan_indexer, v_tasks, v_world_stack, v_goal))
                }
            }
            // add cached new plans to plan set
            planning.append(&mut n_plans);

            // remove dead plans
            planning = planning
                .into_iter()
                .filter(|(id, _, _, _)| invalid_paths.contains(id))
                .collect();
        }
        println!("Possibility Space: {:#?}", self.possibilities);
    }

    fn possible_tasks(&self, world: &World) -> Vec<Task> {
        self.available_tasks
            .clone()
            .into_iter()
            .filter(|p| world.validate(&p.preconditions()))
            .collect()
    }
}

#[cfg(test)]
mod tests {

    use bevy::prelude::*;

    use crate::{
        data::{Truth, World},
        tasks::{GlobalHtnTaskRegistry, Postconditions, Preconditions, Task, TaskComponent},
    };

    use super::HtnAgent;

    #[test]
    fn possibility_space() {
        let mut agent = HtnAgent::default();
        let mut world = crate::data::World::new();
        world.insert("hungry", Truth::Bool(true));
        world.insert("thirsty", Truth::Bool(false));

        let mut task_registry = GlobalHtnTaskRegistry::default();

        task_registry.task("test", TestingTask);
        agent.add_task(Task::Primitive(
            Preconditions(vec![("hungry", Truth::Bool(true))].into_iter().into()),
            TaskComponent("test".into()),
            Postconditions(vec![("hungry", Truth::Bool(true))].into_iter().into()),
        ));

        let app = App::new()
            .add_plugins(MinimalPlugins)
            .insert_resource(world)
            .insert_resource(task_registry)
            .add_systems(Startup, move |mut command: Commands| {
                command.spawn(agent);
            })
            .add_systems(
                Update,
                |agents: Query<&mut HtnAgent>, world: Res<crate::data::World>| {
                    let agent = agents
                        .get_single()
                        .expect("failed to find agent in bevy ecs world");
                    agent.rebuild_possibility_space(world);
                },
            );
        eprintln!("Test Possibility Space: {:#?}", agent.possibilities);
    }

    #[derive(Component, Default, Clone)]
    struct TestingTask;
}
