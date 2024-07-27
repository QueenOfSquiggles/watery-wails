use std::fmt::Debug;

use crate::tasks::Task;

use std::collections::VecDeque;

#[derive(Default)]
pub struct Plan(VecDeque<Task>);

#[derive(Clone, Debug)]
pub struct PlanningPath(pub Vec<Task>);

impl PlanningPath {
    pub fn decompose_tasks(&self) -> Vec<String> {
        self.0
            .iter()
            .map(|t| t.decompose())
            .reduce(|agg, item| agg.into_iter().chain(item.into_iter()).collect())
            .unwrap_or_default()
    }

    pub fn simple_print_tasks(&self) -> Vec<String> {
        self.0.iter().map(|t| t.name()).collect()
    }
}

impl Debug for Plan {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_tuple("Plan")
            .field(&format_args!("Steps: {}", self.0.len()))
            .finish()
    }
}
