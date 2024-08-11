use bevy::prelude::*;
use leafwing_input_manager::prelude::*;

pub struct GamePlugin;

mod gameplay;

impl Plugin for GamePlugin {
    fn build(&self, app: &mut App) {
        app.add_plugins(InputManagerPlugin::<InputActions>::default());
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
            ),
        );
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
}

#[derive(States, Default, Debug, Clone, PartialEq, Eq, Hash)]
pub enum PausedState {
    #[default]
    Running,
    PausedByPlayer,
    PausedBySystem,
}

#[derive(SystemSet, Debug, Clone, PartialEq, Eq, Hash)]
enum PausedStateSystems {
    Running,
    PausedByPlayer,
    PausedBySystem,
}

#[derive(PartialEq, Eq, Clone, Copy, Hash, Debug, Reflect)]
enum InputActions {
    Move,
    Accept,
    Cancel,
}

impl Actionlike for InputActions {}
