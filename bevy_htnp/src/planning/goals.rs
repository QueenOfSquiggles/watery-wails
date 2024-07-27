use rand::{seq::IteratorRandom, thread_rng};

use crate::data::{Requirements, WorldState};

#[derive(Default)]
pub enum GoalEvaluation {
    // TODO: what kinds of context may be needed and/or interesting for agents to determine top goal?
    Random,
    // TODO: how to handle weighted random? Or just rely on custom function?
    // RandomWeighted,
    #[default]
    Top,
    Custom(fn(&Vec<Goal>, &WorldState) -> Option<Goal>),
}

impl GoalEvaluation {
    pub fn next_goal(&self, goals: &Vec<Goal>, world: &WorldState) -> Option<Goal> {
        match *self {
            GoalEvaluation::Top => goals.first().cloned(),
            GoalEvaluation::Custom(f) => f(goals, world),
            GoalEvaluation::Random => goals.iter().choose(&mut thread_rng()).cloned(),
        }
    }
}

#[derive(Default, Clone, Debug)]
pub struct Goal(pub Requirements);
