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
    data::{Requirements, WorldState},
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

    pub fn add_goal(&mut self, goal: Requirements) -> &mut Self {
        self.goals.push(Goal(goal));
        self
    }
    pub fn has_plan(&self) -> bool {
        self.current_plan.is_some()
    }

    pub fn get_next_goal(&self, world: &WorldState) -> Option<Goal> {
        self.goal_eval.next_goal(&self.goals, world)
    }

    pub fn create_plan(
        &self,
        world: WorldState,
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
            .filter(|n| n.value.cost > 0.)
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
        world: WorldState,
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
        // this allows every task available to be used once. Which should generally prevent recursion but won't always
        let max_node_depth: u32 = self.available_tasks.len() as u32;
        // seed node
        active_nodes.push_front(Rc::new(Node::<PlanNode> {
            value: PlanNode {
                task: None,
                world: world.clone(),
                cost: 0.,
                depth: 0,
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
            if goal.0.validate(&node.value.world) {
                // found a leaf! stop processing it
                eprintln!("Found Leaf Node: {:#?}", node.value);
                valid_nodes.push(node);
                continue;
            }
            if node.value.depth >= max_node_depth {
                // purges runaway branches
                continue;
            }
            if self.has_recursion(&node) {
                // drops node that is a recursive branch
                continue;
            }
            let tasks = self.possible_tasks(&node.value.world, task_registry);
            if tasks.is_empty() {
                eprintln!("No possible tasks for node: {:#?}", node.value);
            }
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
        eprintln!("Found {} leaf nodes", valid_nodes.len());
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

    fn possible_tasks(&self, world: &WorldState, task_registry: &TaskRegistry) -> Vec<Task> {
        // self.available_tasks
        //     .clone()
        //     .into_iter()
        //     .filter(|p| task_registry.precon(p).unwrap_or_default().validate(world))
        //     .collect()
        let mut n_vec = Vec::new();
        for task in self.available_tasks.iter() {
            let Some(precon) = task_registry.precon(task) else {
                continue;
            };
            if precon.validate(world) {
                n_vec.push(task.clone());
            }
        }
        n_vec
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
    world: WorldState,
    cost: f32,
    depth: u32,
}

#[cfg(test)]
mod tests {

    use bevy::prelude::Component;

    use crate::{
        data::{Requirements, Variant},
        tasks::Task,
    };

    use super::*;

    #[derive(Component, Default)]
    struct TaskStub;

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

        agent.add_goal(goal_a.clone().into());
        agent.add_goal(goal_b.clone().into());
        agent.add_goal(goal_c.clone().into());

        let next_goal = agent.get_next_goal(&WorldState::new());
        assert!(next_goal.is_some());
        let next_goal = next_goal.unwrap();

        assert!(next_goal.0.validate(&world_ab));
        assert!(!next_goal.0.validate(&world_b));
        assert!(!next_goal.0.validate(&world_not_a));
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

    #[test]
    fn single_task_planning() {
        let mut registry = TaskRegistry::new();
        registry.task::<TaskStub, _>(
            "test",
            Requirements::new().req_equals("hungry", true).build(),
            WorldState::new().add("hungry", false).build(),
            1.,
        );

        let mut agent = HtnAgent::default();
        agent.add_task(Task::Primitive("test".into()));
        agent.add_goal(Requirements::new().req_equals("hungry", false).build());

        let result = agent.create_plan(WorldState::new().add("hungry", true).build(), &registry);

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
            Requirements::new()
                .req_equals("room", "A")
                .req_equals("near_door", false)
                .build(),
            WorldState::new().add("near_door", true).build(),
            1.,
        );
        registry.task::<TaskStub, _>(
            "open_door",
            Requirements::new()
                .req_equals("near_door", true)
                .req_equals("door_open", false)
                .build(),
            // WorldState::new()
            //     .add("near_door", true)
            //     .add("door_open", false)
            //     .build(),
            WorldState::new().add("door_open", true).build(),
            1.,
        );
        registry.task::<TaskStub, _>(
            "walk_thru_door",
            Requirements::new()
                .req_equals("room", "A")
                .req_equals("door_open", true)
                .req_equals("near_door", true)
                .build(),
            WorldState::new().add("room", "B").build(),
            1.,
        );
        agent.add_task(Task::primitive("goto_door"));
        agent.add_task(Task::primitive("open_door"));
        agent.add_task(Task::primitive("walk_thru_door"));

        agent.add_goal(Requirements::new().req_equals("room", "B").build());

        let result = agent.create_plan(
            WorldState::new()
                .add("room", "A")
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
