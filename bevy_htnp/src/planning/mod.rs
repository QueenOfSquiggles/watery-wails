use std::{collections::VecDeque, fmt::Debug, sync::Arc};

use bevy::{
    app::{App, Update},
    log::{debug, error},
    prelude::Component,
};
use goals::{Goal, GoalEvaluation};
use tree::Node;

use crate::{
    data::{Requirements, WorldState},
    tasks::{Task, TaskRegistry},
};

pub mod goals;
pub mod plan_data;
pub mod tree;

pub(crate) fn plugin(app: &mut App) {
    app.add_systems(Update, (plan_data::system_update_time_sliced_tree_gen));
}

#[derive(Component, Default)]
pub struct HtnAgent {
    goals: Vec<Goal>,
    current_plan: Option<plan_data::Plan>,
    available_tasks: Vec<Task>,
    goal_eval: GoalEvaluation,
}

impl HtnAgent {
    pub fn new() -> Self {
        Self::default()
    }
    pub fn add_task(&mut self, task: Task) -> &mut Self {
        self.available_tasks.push(task);
        self
    }

    pub fn add_goal(
        &mut self,
        name: impl Into<String>,
        goal: impl Into<Requirements>,
        static_utility: f32,
    ) -> &mut Self {
        self.goals.push(Goal::new(name, goal, static_utility));
        self
    }
    pub fn has_plan(&self) -> bool {
        self.current_plan.is_some()
    }

    pub fn get_next_goal(&self, world: &WorldState) -> Option<Goal> {
        self.goal_eval.next_goal(&self.goals, world)
    }
}

#[cfg(test)]
mod tests {

    use bevy::prelude::Component;

    use crate::{
        data::{Requirements, Variant},
        tasks::Task,
    };

    use super::*;

    #[test]
    fn goal_picking_planning() {
        let mut agent = HtnAgent::default();
        agent.goal_eval = GoalEvaluation::Top;
        let goal_a: WorldState = vec![("A", true.into())].into_iter().into();
        let goal_b: WorldState = vec![("B", true.into())].into_iter().into();
        let goal_c: WorldState = vec![("C", true.into())].into_iter().into();

        let world_ab = WorldState::new().add("A", true).add("B", true).build();
        let world_b = WorldState::new().add("B", true).build();
        let world_not_a = WorldState::new().add("A", false).add("B", true).build();

        agent.add_goal("A", goal_a.clone(), 1.0);
        agent.add_goal("B", goal_b.clone(), 1.0);
        agent.add_goal("C", goal_c.clone(), 1.0);

        let next_goal = agent.get_next_goal(&WorldState::new());
        assert!(next_goal.is_some());
        let next_goal = next_goal.unwrap();

        assert!(next_goal.requires.validate(&world_ab));
        assert!(!next_goal.requires.validate(&world_b));
        assert!(!next_goal.requires.validate(&world_not_a));
    }

    #[test]
    fn requirements_validation() {
        let req = Requirements::new()
            .req_equals("bool_eq", true)
            .req_equals("str_eq", "something")
            .req_equals("num_eq", 3.1415)
            .req_has("any_key")
            .req_greater("num_grt", 0.0)
            .req_less("num_lst", 0.0)
            .build();

        let valid_world = WorldState::new()
            .add("bool_eq", true)
            .add("str_eq", "something")
            .add("num_eq", 3.1415)
            .add("any_key", 25.)
            .add("num_grt", 10.)
            .add("num_lst", -12.36)
            .build();

        let invalid_world = WorldState::new()
            .add("bool_eq", false)
            .add("str_eq", "else")
            .add("num_eq", 3.)
            .add("num_grt", -10.)
            .add("num_lst", 12.36)
            .build();

        assert!(req.validate(&valid_world));
        assert!(!req.validate(&WorldState::new()));
        assert!(!req.validate(&invalid_world));
    }
}
