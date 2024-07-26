use std::{collections::VecDeque, fmt::Debug, rc::Rc};

use bevy::{
    app::App,
    log::{debug, error},
    prelude::Component,
};
use goals::{Goal, GoalEvaluation};
use plan_data::PlanningPath;
use tree::Node;

use crate::{
    data::Context,
    tasks::{Task, TaskRegistry},
};

pub mod goals;
pub mod plan_data;
pub mod tree;

pub(crate) fn plugin(_app: &mut App) {}

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

    pub fn add_goal(&mut self, goal: Context) -> &mut Self {
        self.goals.push(Goal(goal));
        self
    }
    pub fn has_plan(&self) -> bool {
        self.current_plan.is_some()
    }

    pub fn get_next_goal(&self, world: &Context) -> Option<Goal> {
        self.goal_eval.next_goal(&self.goals, world)
    }

    pub fn create_plan(
        &self,
        world: Context,
        task_registry: &TaskRegistry,
    ) -> Result<plan_data::PlanningPath, ()> {
        // this task tree is somewhat expensive to compute, but it is also verstile
        let plans = self.generate_task_tree(world, task_registry);
        debug!("Generated task tree with {} leaf nodes", plans.len());
        if plans.is_empty() {
            return Err(());
        }
        let Some(target_plan) = plans
            .into_iter()
            .min_by(|a, b| a.value.cost.total_cmp(&b.value.cost))
        else {
            return Err(());
        };
        // unravel graph
        let mut curr = target_plan;
        let mut sequence = Vec::<Task>::new();
        while curr.parent.is_some() {
            let val = curr.value.clone();
            let Some(task) = val.task else {
                error!("Found a None task while unravelling task graph");
                continue;
            };
            sequence.push(task);
            let Some(next_curr) = curr.parent.clone() else {
                break;
            };
            curr = next_curr;
        }

        debug!("Found ideal plan: {:#?}", sequence);
        Ok(PlanningPath(sequence))
    }

    fn generate_task_tree(
        &self,
        world: Context,
        task_registry: &TaskRegistry,
    ) -> Vec<Rc<Node<PlanNode>>> {
        let Some(goal) = self.goal_eval.next_goal(&self.goals, &world) else {
            error!("Failed to find a top priority goal!");
            return Vec::new();
        };
        // TODO: we should time slice this operation. Maybe just precache all valid paths?
        let mut active_nodes = VecDeque::<Rc<Node<PlanNode>>>::new();
        let mut valid_nodes = Vec::<Rc<Node<PlanNode>>>::new();
        let mut iter_depth: u32 = 0;

        // TODO: extract these into bevy resource
        const MAX_ITER_DEPTH: u32 = 100_00;
        const MAX_NODE_DEPTH: u32 = 16;
        // seed node
        active_nodes.push_front(Rc::new(Node::<PlanNode> {
            value: PlanNode {
                task: None,
                world: world.clone(),
                cost: 0.,
                depth: 1,
            },
            parent: None,
        }));
        loop {
            // loop management
            iter_depth += 1;
            if iter_depth >= MAX_ITER_DEPTH {
                error!("Reached max depth searching for plan!!!");
                break;
            }

            // process a single node (so we can modify the dequeue without extra vecs to track)
            let Some(node) = active_nodes.pop_front() else {
                break;
            };
            if node.value.world.validate(&goal.0) {
                valid_nodes.push(node);
                continue;
            }
            if node.value.depth >= MAX_NODE_DEPTH {
                // purges runaway branches
                continue;
            }
            if self.has_recursion(&node) {
                continue; // drops node that is a recursive branch
            }
            let tasks = self.possible_tasks(&node.value.world, task_registry);
            for t in tasks {
                if let Some(new_node) = Self::make_node(node.clone(), &t, task_registry) {
                    active_nodes.push_front(Rc::new(new_node));
                }
            }

            // loop management
            if active_nodes.is_empty() {
                break;
            }
        }
        valid_nodes
    }

    // this is a total band-aid solution. Probably need a better way to coerce the plan to avoid repetitive tasks?
    fn has_recursion(&self, node: &Rc<Node<PlanNode>>) -> bool {
        let Some(ref parent) = node.parent else {
            return false;
        };
        let Some(ref parent2) = parent.parent else {
            return false;
        };
        let Some(ref parent3) = parent2.parent else {
            return false;
        };
        let t0 = node
            .value
            .task
            .as_ref()
            .and_then(|task| Some(task.name()))
            .unwrap_or("0".into());
        let t1 = parent
            .value
            .task
            .as_ref()
            .and_then(|task| Some(task.name()))
            .unwrap_or("0".into());
        let t2 = parent2
            .value
            .task
            .as_ref()
            .and_then(|task| Some(task.name()))
            .unwrap_or("0".into());
        let t4 = parent3
            .value
            .task
            .as_ref()
            .and_then(|task| Some(task.name()))
            .unwrap_or("0".into());

        // this only catches A-B-A-B patterns, not A-B-C-A-B-C patterns
        // goddamn I need a better solution
        t0 == t2 && t1 == t4
    }

    fn possible_tasks(&self, world: &Context, task_registry: &TaskRegistry) -> Vec<Task> {
        self.available_tasks
            .clone()
            .into_iter()
            .filter(|p| world.validate(&task_registry.precon(p).unwrap_or_default()))
            .collect()
    }
    fn make_node(
        parent: Rc<Node<PlanNode>>,
        task: &Task,
        registry: &TaskRegistry,
    ) -> Option<Node<PlanNode>> {
        let Some(data) = registry.get_task(task) else {
            return None;
        };
        let virtual_world = parent.value.world.concat(data.postconditions());
        Some(Node::<PlanNode> {
            value: PlanNode {
                task: Some(task.clone()),
                cost: parent.value.cost + data.cost(&virtual_world),
                world: virtual_world,
                depth: parent.value.depth + 1,
            },
            parent: Some(parent),
        })
    }
}
#[derive(Debug, Clone)]
struct PlanNode {
    task: Option<Task>,
    world: Context,
    cost: f32,
    depth: u32,
}

#[cfg(test)]
mod tests {

    use bevy::prelude::Component;

    use crate::tasks::Task;

    use super::*;

    #[derive(Component, Default)]
    struct TaskStub;

    #[test]
    fn goal_picking_planning() {
        let mut agent = HtnAgent::default();
        agent.goal_eval = GoalEvaluation::Top;
        let goal_a: Context = vec![("A", true.into())].into_iter().into();
        let goal_b: Context = vec![("B", true.into())].into_iter().into();
        let goal_c: Context = vec![("C", true.into())].into_iter().into();

        agent.add_goal(goal_a.clone());
        agent.add_goal(goal_b.clone());
        agent.add_goal(goal_c.clone());

        let next_goal = agent.get_next_goal(&Context::new());
        assert!(next_goal.is_some());
        let next_goal = next_goal.unwrap();
        assert!(goal_a.validate(&next_goal.0));
        assert!(!goal_b.validate(&next_goal.0));
        assert!(!goal_c.validate(&next_goal.0));
    }

    #[test]
    fn single_task_planning() {
        let precon = Context::new().add("hungry", true).build();
        let postcon = Context::new().add("hungry", true).build();
        let mut registry = TaskRegistry::new();
        registry.task::<TaskStub, _>("test", precon.clone(), postcon.clone(), 0.);

        let mut agent = HtnAgent::default();
        agent.add_task(Task::Primitive("test".into()));
        agent.add_goal(postcon);
        let result = agent.create_plan(precon, &registry);

        eprintln!("{:#?}", result);

        assert!(result.is_ok());
        let plan = result.unwrap();
        assert_eq!(plan.0.len(), 1);
    }

    #[test]
    fn multi_task_planning() {
        let mut agent = HtnAgent::default();
        let mut registry = TaskRegistry::new();
        registry.task::<TaskStub, _>(
            "goto_door",
            Context::new()
                .add("in_room_a", true)
                .add("near_door", false)
                .build(),
            Context::new().add("near_door", true).build(),
            0.,
        );
        registry.task::<TaskStub, _>(
            "open_door",
            Context::new()
                .add("near_door", true)
                .add("door_open", false)
                .build(),
            Context::new().add("door_open", true).build(),
            0.,
        );
        registry.task::<TaskStub, _>(
            "walk_thru_door",
            Context::new()
                .add("in_room_a", true)
                .add("door_open", true)
                .add("near_door", true)
                .build(),
            Context::new().add("in_room_a", false).build(),
            0.,
        );
        agent.add_task(Task::primitive("goto_door"));
        agent.add_task(Task::primitive("open_door"));
        agent.add_task(Task::primitive("walk_thru_door"));

        agent.add_goal(Context::new().add("in_room_a", false).build());

        let result = agent.create_plan(
            Context::new()
                .add("in_room_a", true)
                .add("near_door", false)
                .add("door_open", false)
                .build(),
            &registry,
        );

        assert!(result.is_ok());
        let plan = result.unwrap();
        assert_eq!(plan.0.len(), 3);
    }
}
