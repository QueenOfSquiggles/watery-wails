use crate::data::World;
use bevy::{ecs::system::EntityCommands, prelude::*, utils::HashMap};
use std::fmt::Debug;

// TODO: figure out a way to use regular function pointers? (fn instead of Fn)
type HtnTaskComponentAdderFunc = Box<dyn Fn(&mut EntityCommands)>;
#[derive(Resource, Default)]
pub struct GlobalHtnTaskRegistry(pub HashMap<String, HtnTaskComponentAdderFunc>);

impl GlobalHtnTaskRegistry {
    pub fn task<S, C>(&mut self, name: S, comp: C)
    where
        S: Into<String>,
        C: Component + Clone,
    {
        self.0.insert(
            name.into(),
            Box::new(move |cmd| {
                cmd.insert(comp.clone());
            }),
        );
    }

    pub fn tasks<S, C>(&mut self, collection: impl Iterator<Item = (S, C)>)
    where
        S: Into<String>,
        C: Component + Clone,
    {
        for (name, comp) in collection {
            self.task(name, comp);
        }
    }
}

#[derive(Clone, Debug)]
pub struct Preconditions(pub World);

#[derive(Clone, Debug)]
pub struct Postconditions(pub World);

#[derive(Clone, Debug)]
pub struct TaskComponent(pub String);

#[derive(Debug, Clone)]
pub enum Task {
    Primitive(Preconditions, TaskComponent, Postconditions),
    Macro(Vec<Task>),
}

impl Task {
    pub fn decompose(&self) -> Vec<TaskComponent> {
        match self {
            Task::Primitive(_, task, _) => vec![task.clone()],
            Task::Macro(m) => m
                .iter()
                .map(|p| p.decompose())
                .reduce(|agg, item| {
                    let mut n_agg = agg.clone();
                    for i in item {
                        n_agg.push(i);
                    }
                    n_agg
                })
                .unwrap_or_default(),
        }
    }

    pub fn postconditions(&self) -> World {
        match self {
            Task::Primitive(_, _, post) => post.clone().into(),
            Task::Macro(steps) => steps
                .into_iter()
                .map(|t| t.postconditions())
                .reduce(|agg, item| agg.concat(&item))
                .unwrap_or_default(),
        }
    }

    pub fn preconditions(&self) -> World {
        match self {
            Task::Primitive(_, _, post) => post.clone().into(),
            Task::Macro(steps) => steps
                .into_iter()
                .rev()
                .map(|t| t.preconditions())
                .reduce(|agg, item| agg.concat(&item))
                .unwrap_or_default(),
        }
    }
}

impl From<Preconditions> for World {
    fn from(value: Preconditions) -> Self {
        value.0
    }
}
impl From<Postconditions> for World {
    fn from(value: Postconditions) -> Self {
        value.0
    }
}
