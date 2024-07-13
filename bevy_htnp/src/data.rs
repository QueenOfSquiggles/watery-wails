use bevy::prelude::*;
use std::collections::HashMap;

#[derive(Clone, Debug, PartialEq, Default)]
pub enum Truth {
    Bool(bool),
    String(String),
    #[default]
    None,
}

#[derive(Default, Clone, Debug, PartialEq, Resource)]
/// For an HTN, a world is simply a collection of known 'truth's.
pub struct World {
    entries: HashMap<String, Truth>,
}

impl World {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn insert(&mut self, key: impl ToString, value: Truth) -> Option<Truth> {
        self.entries.insert(key.to_string(), value)
    }

    /// ensure that the other world's set of truths is a subset of this World's truths.
    /// Early exit if a value in other is not present in this world or if the values between worlds do not match
    pub fn validate(&self, other: &World) -> bool {
        for (name, truth) in &other.entries {
            let Some(val) = self.entries.get(name) else {
                return false;
            };
            if *val != *truth {
                return false;
            }
        }
        return true;
    }

    pub fn get(&self, s: impl ToString) -> Truth {
        let Some(value) = self.entries.get(&s.to_string()) else {
            return Truth::None;
        };
        return value.clone();
    }

    pub fn append(&mut self, other: &World) {
        for (name, truth) in &other.entries {
            self.entries.insert(name.clone(), truth.clone());
        }
    }

    pub fn concat(&self, other: &World) -> Self {
        let mut n_world = self.clone();
        n_world.append(other);
        n_world
    }
}

impl<I, S> From<I> for World
where
    I: Iterator<Item = (S, Truth)>,
    S: ToString,
{
    fn from(value: I) -> Self {
        let mut map = HashMap::new();

        for (name, truth) in value {
            if let Some(_) = map.insert(name.to_string(), truth) {
                warn!("Duplicate entries for key: {}", name.to_string());
            }
        }
        Self { entries: map }
    }
}

impl From<Truth> for World {
    fn from(value: Truth) -> Self {
        let mut map = HashMap::new();
        map.insert("value".into(), value);
        Self { entries: map }
    }
}
