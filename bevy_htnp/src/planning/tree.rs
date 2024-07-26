use std::rc::Rc;

/// Super basic non-cyclic, directional graph (aka a tree)
pub(crate) struct Node<T> {
    pub value: T,
    pub parent: Option<Rc<Node<T>>>,
}
