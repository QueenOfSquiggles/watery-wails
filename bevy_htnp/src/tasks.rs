use crate::data::TruthSet;
use bevy::{ecs::system::EntityCommands, prelude::*, utils::HashMap};
use std::{
    fmt::Debug,
    marker::PhantomData,
    sync::{Arc, RwLock},
};

pub trait TaskData: Sync + Send {
    fn preconditions(&self) -> &TruthSet;
    fn postconditions(&self) -> &TruthSet;
    fn add(&self, entity: &mut EntityCommands);
    fn remove(&self, entity: &mut EntityCommands);
}

type TaskStorage = Arc<RwLock<Box<dyn TaskData>>>;
#[derive(Resource, Default)]
pub struct GlobalHtnTaskRegistry(pub HashMap<String, TaskStorage>);

impl GlobalHtnTaskRegistry {
    pub fn task<C, S>(&mut self, name: S, precon: Option<TruthSet>, postcon: Option<TruthSet>)
    where
        S: Into<String>,
        C: Component + Default,
    {
        let comp =
            SimpleTaskData::<C>::new(precon.unwrap_or_default(), postcon.unwrap_or_default());
        self.0
            .insert(name.into(), Arc::new(RwLock::new(Box::new(comp))));
    }

    pub fn custom_task<S>(&mut self, name: S, data: Box<dyn TaskData>)
    where
        S: Into<String>,
    {
        self.0.insert(name.into(), Arc::new(RwLock::new(data)));
    }
}

/// For instances where pre and post conditions are static and the task is accomplished through a default instance of a component, this can be used to make creation of new tasks much easier.
struct SimpleTaskData<C>
where
    C: Component,
{
    precon: TruthSet,
    postcon: TruthSet,
    phantom: PhantomData<C>,
}

impl<C> SimpleTaskData<C>
where
    C: Component + Default,
{
    fn new(precon: TruthSet, postcon: TruthSet) -> Self {
        Self {
            precon,
            postcon,
            phantom: PhantomData,
        }
    }
}

impl<C> TaskData for SimpleTaskData<C>
where
    C: Component + Default,
{
    fn preconditions(&self) -> &TruthSet {
        &self.precon
    }

    fn postconditions(&self) -> &TruthSet {
        &self.postcon
    }

    fn add(&self, entity: &mut EntityCommands) {
        entity.insert(C::default());
    }

    fn remove(&self, entity: &mut EntityCommands) {
        entity.remove::<C>();
    }
}

#[derive(Debug, Clone)]
pub enum Task {
    Primitive {
        precon: TruthSet,
        postcon: TruthSet,
        name: String,
    },
    Macro(Vec<Task>),
}

impl Task {
    pub fn decompose(&self) -> Vec<Task> {
        match self {
            Task::Primitive {
                precon: _,
                postcon: _,
                name: _,
            } => {
                vec![self.clone()]
            }
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

    pub fn postconditions(&self) -> TruthSet {
        match self {
            Task::Primitive {
                precon: _,
                postcon,
                name: _,
            } => postcon.clone(),
            Task::Macro(steps) => steps
                .into_iter()
                .map(|t| t.postconditions())
                .reduce(|agg, item| agg.concat(&item))
                .unwrap_or_default(),
        }
    }

    pub fn preconditions(&self) -> TruthSet {
        match self {
            Task::Primitive {
                precon,
                postcon: _,
                name: _,
            } => precon.clone().into(),
            Task::Macro(steps) => steps
                .into_iter()
                .rev()
                .map(|t| t.preconditions())
                .reduce(|agg, item| agg.concat(&item))
                .unwrap_or_default(),
        }
    }
}
