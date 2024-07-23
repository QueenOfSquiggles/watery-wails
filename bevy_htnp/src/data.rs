use bevy::prelude::*;
use std::collections::HashMap;

#[derive(Clone, Debug, PartialEq, Default)]
pub enum Predicate {
    Bool(bool),
    String(String),
    #[default]
    None,
}

#[derive(Default, Clone, Debug, PartialEq, Resource)]
/// For an HTN, a context is simply a collection of known 'predicate's.
pub struct Context {
    entries: HashMap<String, Predicate>,
}

impl Context {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn add(&mut self, key: impl Into<String>, value: impl Into<Predicate>) -> &mut Self {
        self.insert(Into::<String>::into(key), value.into());
        self
    }

    pub fn build(&mut self) -> Self {
        self.clone()
    }

    pub fn insert(&mut self, key: impl ToString, value: Predicate) -> Option<Predicate> {
        self.entries.insert(key.to_string(), value)
    }

    /// ensure that the other world's set of truths is a subset of this World's truths.
    /// Early exit if a value in other is not present in this world or if the values between worlds do not match
    pub fn validate(&self, other: &Context) -> bool {
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

    pub fn get(&self, s: impl ToString) -> Predicate {
        let Some(value) = self.entries.get(&s.to_string()) else {
            return Predicate::None;
        };
        return value.clone();
    }

    pub fn append(&mut self, other: &Context) {
        for (name, truth) in &other.entries {
            self.entries.insert(name.clone(), truth.clone());
        }
    }

    pub fn concat(&self, other: &Context) -> Self {
        let mut n_world = self.clone();
        n_world.append(other);
        n_world
    }
}

impl<I, S> From<I> for Context
where
    I: Iterator<Item = (S, Predicate)>,
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

impl From<Predicate> for Context {
    fn from(value: Predicate) -> Self {
        let mut map = HashMap::new();
        map.insert("value".into(), value);
        Self { entries: map }
    }
}

impl From<bool> for Predicate {
    fn from(value: bool) -> Self {
        Self::Bool(value)
    }
}

impl From<String> for Predicate {
    fn from(value: String) -> Self {
        Self::String(value)
    }
}
impl From<&str> for Predicate {
    fn from(value: &str) -> Self {
        Self::String(value.to_string())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_truth_equality() {
        let bool_true = Predicate::Bool(true);
        let bool_false = Predicate::Bool(false);
        let string_empty = Predicate::String("".into());
        let string_test = Predicate::String("test".into());
        let none = Predicate::None;

        // bools matrix
        assert_eq!(bool_true, Predicate::Bool(true));
        assert_eq!(bool_false, Predicate::Bool(false));

        assert_ne!(bool_true, bool_false);
        assert_ne!(bool_true, Predicate::Bool(false));
        assert_ne!(bool_false, Predicate::Bool(true));

        // strings matrix
        assert_eq!(string_empty, Predicate::String("".into()));
        assert_eq!(string_test, Predicate::String("test".into()));

        assert_ne!(string_empty, string_test);

        // nones
        assert_eq!(none, Predicate::None);

        // cross comparisons
        assert_ne!(bool_true, string_empty);
        assert_ne!(bool_true, string_test);
        assert_ne!(bool_true, none);

        assert_ne!(bool_false, string_empty);
        assert_ne!(bool_false, string_test);
        assert_ne!(bool_false, none);

        assert_ne!(string_empty, bool_true);
        assert_ne!(string_empty, bool_false);
        assert_ne!(string_empty, none);

        assert_ne!(string_test, bool_true);
        assert_ne!(string_test, bool_false);
        assert_ne!(string_test, none);
    }

    #[test]
    fn test_world_validation() {
        let truths_base: Context = vec![
            ("safe", true.into()),
            ("running", false.into()),
            ("happy", true.into()),
        ]
        .into_iter()
        .into();
        let truths_valid: Context = vec![("safe", true.into()), ("happy", true.into())]
            .into_iter()
            .into();
        let truths_invalid: Context = vec![("running", true.into())].into_iter().into();

        assert!(truths_base.validate(&truths_valid));
        assert!(!truths_base.validate(&truths_invalid));

        assert!(!truths_valid.validate(&truths_base));
        assert!(!truths_valid.validate(&truths_invalid));

        assert!(!truths_invalid.validate(&truths_base));
        assert!(!truths_invalid.validate(&truths_valid));

        let concatenated_set: Context = vec![("running", true.into())].into_iter().into();
        let super_set = truths_base.concat(&concatenated_set);
        assert!(!truths_base.validate(&truths_invalid)); // ensure that concating doesn't change base value
        assert!(super_set.validate(&truths_invalid)); // ensure new concatenation is valid for both
        assert!(super_set.validate(&truths_valid)); // ensure new concatenation is valid for both
    }
}
