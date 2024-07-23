use std::{collections::VecDeque, fmt::Debug};

use bevy::prelude::*;
use rand::{seq::IteratorRandom, thread_rng};

use crate::{data::Context, tasks::Task};

#[derive(Component, Default)]
pub struct HtnAgent {
    goals: Vec<Goal>,
    current_plan: Option<Plan>,
    available_tasks: Vec<Task>,
    evaluator: GoalEvaluation,
}

#[derive(Default)]
pub enum GoalEvaluation {
    Random,
    // TODO: how to handle weighted random? Or just rely on custom function?
    // RandomWeighted,
    #[default]
    Top,
    Custom(fn(&Vec<Goal>) -> Option<Goal>),
}

impl GoalEvaluation {
    pub fn next_goal(&self, goals: &Vec<Goal>) -> Option<Goal> {
        match *self {
            GoalEvaluation::Top => goals.first().cloned(),
            GoalEvaluation::Custom(f) => f(goals),
            GoalEvaluation::Random => goals.iter().choose(&mut thread_rng()).cloned(),
        }
    }
}

#[derive(Component)]
pub struct HtnAgentWorld {
    pub world: Context,
}

#[derive(Default, Clone, Debug)]
pub struct Goal(Context);

#[derive(Default)]
pub struct Plan(VecDeque<Task>);

impl Debug for Plan {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_tuple("Plan")
            .field(&format_args!("Steps: {}", self.0.len()))
            .finish()
    }
}
#[derive(Clone, Debug)]
pub struct PlanningPath {
    tasks: Vec<Task>,
    world_stack: Vec<Context>,
    accomplishes_goal: bool,
}
impl HtnAgent {
    pub fn new() -> Self {
        Self::default()
    }
    pub fn add_task(&mut self, task: Task) -> &mut Self {
        self.available_tasks.push(task);
        self
    }

    pub fn add_goal(&mut self, goal: Goal) -> &mut Self {
        self.goals.push(goal);
        self
    }
    pub fn has_plan(&self) -> bool {
        self.current_plan.is_some()
    }

    pub fn get_next_goal(&self) -> Option<Goal> {
        self.evaluator.next_goal(&self.goals)
    }

    pub fn create_plan(&mut self, world: Context) -> Result<PlanningPath, ()> {
        let Some(goal) = self.get_next_goal() else {
            eprintln!("Failed to create goal");
            return Err(());
        };
        let mut paths = Vec::<PlanningPath>::new();
        for t in self.possible_tasks(&world) {
            let virtual_world = world.concat(&t.postconditions());
            paths.push(PlanningPath {
                tasks: vec![t],
                world_stack: vec![virtual_world.clone()],
                accomplishes_goal: virtual_world.validate(&goal.0),
            });
        }
        let mut valid_paths = Vec::new();
        while !paths.is_empty() {
            let mut new_paths = Vec::new();
            let mut remove_paths = Vec::new();
            for (index, path) in paths.iter().enumerate() {
                remove_paths.push(index); // since we are basically decomposing each path into zero or more branches, we can purge each non-leaf path

                if path.accomplishes_goal {
                    valid_paths.push(path.clone());
                    eprintln!("Found valid leaf path: {:#?}", path);
                    continue;
                }

                let path_world = path.world_stack.last().unwrap();
                let tasks = self.possible_tasks(&path_world);
                if tasks.is_empty() {
                    eprintln!("Found dead end leaf");
                    continue;
                }

                for t in tasks {
                    let mut new_path = path.clone();
                    let new_world = path_world.clone().concat(&t.postconditions());

                    new_path.accomplishes_goal = new_world.validate(&goal.0);
                    new_path.tasks.push(t);
                    new_path.world_stack.push(new_world);
                    new_paths.push(new_path);
                }
            }

            paths = paths
                .into_iter()
                .enumerate()
                .filter(|(index, _)| !remove_paths.contains(index))
                .map(|v| v.1)
                .collect();
            paths.append(&mut new_paths);
        }
        if valid_paths.is_empty() {
            eprintln!("Failed to generate a valid plan");
            return Err(());
        }
        valid_paths.sort_by(|a, b| a.tasks.len().cmp(&b.tasks.len()));
        eprintln!("Created a valid plan?: {:#?}", valid_paths.first());

        Ok(valid_paths.first().unwrap().clone())
    }

    fn possible_tasks(&self, world: &Context) -> Vec<Task> {
        self.available_tasks
            .clone()
            .into_iter()
            .filter(|p| world.validate(&p.preconditions()))
            .collect()
    }
}

#[cfg(test)]
mod tests {

    use crate::tasks::Task;

    use super::*;

    #[test]
    fn goal_picking_planning() {
        let mut agent = HtnAgent::default();
        agent.evaluator = GoalEvaluation::Top;
        let goal_a = Goal(vec![("A", true.into())].into_iter().into());
        let goal_b = Goal(vec![("B", true.into())].into_iter().into());
        let goal_c = Goal(vec![("C", true.into())].into_iter().into());

        agent.add_goal(goal_a.clone());
        agent.add_goal(goal_b.clone());
        agent.add_goal(goal_c.clone());

        let next_goal = agent.get_next_goal();
        assert!(next_goal.is_some());
        let next_goal = next_goal.unwrap();
        assert!(goal_a.0.validate(&next_goal.0));
        assert!(!goal_b.0.validate(&next_goal.0));
        assert!(!goal_c.0.validate(&next_goal.0));
    }

    #[test]
    fn single_task_planning() {
        let precon = Context::new().add("hungry", true).build();
        let postcon = Context::new().add("hungry", true).build();

        let mut agent = HtnAgent::default();
        agent.add_task(Task::Primitive {
            precon: precon.clone(),
            postcon: postcon.clone(),
            name: "test".into(),
        });
        agent.add_goal(Goal(postcon));

        let result = agent.create_plan(precon);

        eprintln!("{:#?}", result);

        assert!(result.is_ok());
        let plan = result.unwrap();
        assert_eq!(plan.tasks.len(), 1);
        assert!(plan.accomplishes_goal);
        eprintln!("Test output");
    }

    #[test]
    fn multi_task_planning() {
        let mut agent = HtnAgent::default();
        agent.add_task(Task::primitive(
            "goto_door",
            Context::new()
                .add("in_room_a", true)
                .add("near_door", false)
                .build(),
            Context::new().add("near_door", true).build(),
        ));
        agent.add_task(Task::primitive(
            "open_door",
            Context::new()
                .add("near_door", true)
                .add("door_open", false)
                .build(),
            Context::new().add("door_open", true).build(),
        ));
        agent.add_task(Task::primitive(
            "walk_thru_door",
            Context::new()
                .add("in_room_a", true)
                .add("door_open", true)
                .add("near_door", true)
                .build(),
            Context::new().add("in_room_a", false).build(),
        ));
        agent.add_goal(Goal(Context::new().add("in_room_a", false).build()));

        let result = agent.create_plan(
            Context::new()
                .add("in_room_a", true)
                .add("near_door", false)
                .add("door_open", false)
                .build(),
        );

        assert!(result.is_ok());
        let plan = result.unwrap();
        assert_eq!(plan.tasks.len(), 3);
        assert!(plan.accomplishes_goal);
        eprintln!("Test output");
    }
}
