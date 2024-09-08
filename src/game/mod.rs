use bevy::{prelude::*, window::WindowFocused};

pub struct GamePlugin;

mod gameplay;

impl Plugin for GamePlugin {
    fn build(&self, app: &mut App) {
        app.insert_state(GameState::Gameplay);
        app.init_state::<PausedState>();
        app.configure_sets(
            Update,
            (
                GameStateSystems::MainMenu.run_if(in_state(GameState::MainMenu)),
                GameStateSystems::LaunchAnim.run_if(in_state(GameState::LaunchAnim)),
                GameStateSystems::OptionsMenu.run_if(in_state(GameState::OptionsMenu)),
                GameStateSystems::CreditsMenu.run_if(in_state(GameState::CreditsMenu)),
                GameStateSystems::Gameplay
                    .run_if(in_state(GameState::Gameplay).and_then(in_state(PausedState::Running))),
                GameStateSystems::InGamePaused.run_if(
                    not(in_state(PausedState::Running)).and_then(in_state(GameState::Gameplay)),
                ),
            ),
        );
        let (window_entity, _) = app
            .world_mut()
            .query::<(Entity, &Window)>()
            .single(app.world());

        app.world_mut()
            .entity_mut(window_entity)
            .observe(watch_for_focus_lost);

        // app.observe(watch_for_focus_lost);
        gameplay::plugin(app);
    }
}
#[derive(States, Default, Debug, Clone, PartialEq, Eq, Hash)]
pub enum GameState {
    #[default]
    MainMenu,
    LaunchAnim,
    OptionsMenu,
    CreditsMenu,
    Gameplay,
}
#[derive(SystemSet, Debug, Clone, PartialEq, Eq, Hash)]
enum GameStateSystems {
    MainMenu,
    LaunchAnim,
    OptionsMenu,
    CreditsMenu,
    Gameplay,
    InGamePaused,
}

#[derive(States, Default, Debug, Clone, PartialEq, Eq, Hash)]
pub enum PausedState {
    #[default]
    Running,
    PausedByPlayer,
    PausedBySystem,
}

fn watch_for_focus_lost(
    trigger: Trigger<WindowFocused>,
    mut pause_state: ResMut<NextState<PausedState>>,
    current_state: Res<State<PausedState>>,
) {
    // TODO: learn how to detect window focus lost
    if *current_state.get() != PausedState::Running || trigger.event().focused {
        info!(
            "System focus lost but was in the wrong state {:?}, event={:?}",
            current_state.get(),
            trigger.event()
        );
        return;
    }
    info!("System focus lost");
    pause_state.set(PausedState::PausedBySystem);
}
