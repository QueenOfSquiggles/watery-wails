use bevy::{
    log::{Level, LogPlugin},
    prelude::*,
};
use bevy_htnp::prelude::*;

fn main() {
    let mut app = App::new();
    app.add_plugins((
        MinimalPlugins,
        HtnPlanningPlugin::new(),
        LogPlugin {
            level: Level::DEBUG,
            filter: "info,bevy_htnp=debug".into(),
            ..Default::default()
        },
    ))
    .add_systems(Update, system_taskstub); // Handles `TaskStub`

    // example problem space:
    // - Agent must pick up item
    // - Agent is in room A
    // - Item is in room B
    // - Door A/B is closed

    // world/context declaration
    let world = Context::new()
        .add("in_room_a", true)
        .add("in_room_b", false)
        .add("door_open", false)
        .add("item_picked_up", false)
        .build();

    // task declaration
    let mut task_registry = TaskRegistry::new();
    task_registry.task::<TaskStub, _>(
        "pickup_item",
        Context::new()
            .add("in_room_b", true)
            .add("item_picked_up", false)
            .build(),
        Context::new().add("item_picked_up", true).build(),
        1.,
    );

    task_registry.task::<TaskStub, _>(
        "goto_b",
        Context::new()
            .add("door_open", true)
            .add("in_room_b", false)
            .build(),
        Context::new()
            .add("in_room_b", true)
            .add("in_room_a", false)
            .build(),
        1.,
    );

    task_registry.task::<TaskStub, _>(
        "open_door",
        Context::new().add("door_open", false).build(),
        Context::new().add("door_open", true).build(),
        1.,
    );
    // red herring tasks (tasks available to the agent that are unhelpful towards the goal)
    task_registry.task::<TaskStub, _>(
        "goto_a",
        Context::new()
            .add("door_open", true)
            .add("in_room_a", false)
            .build(),
        Context::new()
            .add("in_room_a", true)
            .add("in_room_b", false)
            .build(),
        1.,
    );

    task_registry.task::<TaskStub, _>(
        "close_door",
        Context::new().add("door_open", true).build(),
        Context::new().add("door_open", false).build(),
        1.,
    );
    app.insert_resource(task_registry); // note: a task registry is already inserted by default.

    // init agent
    let mut agent = HtnAgent::new();
    agent.add_task(Task::primitive("pickup_item"));
    agent.add_task(Task::primitive("goto_b")); // note how we have pairs of tasks that undo each other. This could create an infinite loop
    agent.add_task(Task::primitive("goto_a"));
    agent.add_task(Task::primitive("open_door"));
    agent.add_task(Task::primitive("close_door"));

    agent.add_goal(Context::new().add("item_picked_up", true).build());

    // spawn entity
    let entity_agent = app
        .world_mut()
        .spawn((Transform::default(), agent, HtnAgentWorld(world)))
        .id();

    // make plan
    app.update();
    // assert!(!has_component::<HtnAgentPlan>(&app, entity_agent));
}

/// An example marker component that is used to denote a task. Typically you have a specific marker component for each task
#[derive(Component, Default)]
struct TaskStub;

/// the system which operates on agents that have a TaskStub component. Since no data is stored on TaskStub, we just check `With<T>`
fn system_taskstub(query: Query<(Entity, &HtnAgentState), With<TaskStub>>, mut command: Commands) {
    for (e, state) in query.iter() {
        // this prevents processing an agent who is not actively using this component, but has not had it cleared yet.
        if *state != HtnAgentState::Running {
            continue;
        }

        // literally do nothing and just push through
        info!("Processing TaskStub for {}", e);

        // this tells the planner that this task finished successfully and to continue the plan as normal
        command.entity(e).insert(HtnAgentState::Success);
    }
}

// Assertion Utility
fn has_component<C: Component>(app: &App, entity: Entity) -> bool {
    app.world().get::<C>(entity).is_some()
}
