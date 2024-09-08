use avian3d::math::Quaternion;
use bevy::{
    prelude::*,
    utils::tracing::span::Entered,
    window::{CursorGrabMode, PrimaryWindow},
};
use leafwing_input_manager::prelude::*;

use crate::game::{GameState, PausedState};

pub struct PlayerPlugin;

impl Plugin for PlayerPlugin {
    fn build(&self, app: &mut App) {
        app.add_plugins(InputManagerPlugin::<PlayerActions>::default());
        app.init_state::<PlayerState>();
        app.configure_sets(
            Update,
            (
                PlayerSystems::Fps.run_if(
                    in_state(PlayerState::Fps)
                        .and_then(in_state(PausedState::Running))
                        .and_then(in_state(GameState::Gameplay)),
                ),
                PlayerSystems::Desk.run_if(
                    in_state(PlayerState::Desk)
                        .and_then(in_state(PausedState::Running))
                        .and_then(in_state(GameState::Gameplay)),
                ),
                PlayerSystems::Cutscene.run_if(
                    in_state(PlayerState::Cutscene)
                        .and_then(in_state(PausedState::Running))
                        .and_then(in_state(GameState::Gameplay)),
                ),
            ),
        );
        app.add_systems(Startup, add_player);
        app.add_systems(
            Update,
            (camera_look, player_move).in_set(PlayerSystems::Fps),
        );

        // fn grab_cursor_on_enter_fps_mode() {}
        // fn release_cursor_on_enter_desk_mode() {}

        app.add_systems(OnEnter(PlayerState::Fps), grab_cursor_on_enter_fps_mode);
        app.add_systems(
            OnEnter(PlayerState::Desk),
            release_cursor_on_enter_desk_mode,
        );
        // app.add_systems(Update, player_input_test.in_set(PlayerSystems::Desk));
    }
}

#[derive(Hash, Clone, Debug, PartialEq, Eq, SystemSet)]
enum PlayerSystems {
    Fps,
    Desk,
    Cutscene,
}

#[derive(Debug, Clone, Hash, Reflect, Default, PartialEq, Eq, States)]
enum PlayerState {
    #[default]
    Fps,
    Desk,
    Cutscene,
}

#[derive(Component)]
struct Player;

#[derive(Component)]
struct PlayerCamera;

#[derive(Hash, Debug, Clone, Copy, PartialEq, Eq, Reflect)]
enum PlayerActions {
    Move,
    Look,
    Accept,
    Back,
}

impl Actionlike for PlayerActions {
    fn input_control_kind(&self) -> InputControlKind {
        match self {
            PlayerActions::Move => InputControlKind::DualAxis,
            PlayerActions::Look => InputControlKind::DualAxis,
            PlayerActions::Accept => InputControlKind::Button,
            PlayerActions::Back => InputControlKind::Button,
        }
    }
}

impl PlayerActions {
    fn default_mappings() -> InputMap<Self> {
        const SIG: f32 = 0.1;
        const CONTROLLER_SCALAR: f32 = 10.;
        let input_map = InputMap::default()
            .with_multiple([
                (PlayerActions::Accept, KeyCode::Enter),
                (PlayerActions::Accept, KeyCode::KeyZ),
                (PlayerActions::Back, KeyCode::Escape),
                (PlayerActions::Back, KeyCode::KeyX),
            ])
            .with_multiple([
                (PlayerActions::Accept, MouseButton::Left),
                (PlayerActions::Back, MouseButton::Right),
            ])
            .with_multiple([
                (PlayerActions::Accept, GamepadButtonType::South),
                (PlayerActions::Back, GamepadButtonType::East),
            ])
            .with_dual_axis(PlayerActions::Look, MouseMove::default())
            .with_dual_axis(
                PlayerActions::Look,
                GamepadStick::RIGHT
                    .with_circle_deadzone(SIG)
                    .sensitivity_x(CONTROLLER_SCALAR)
                    .sensitivity_y(-CONTROLLER_SCALAR),
            )
            .with_dual_axis(PlayerActions::Move, KeyboardVirtualDPad::WASD)
            .with_dual_axis(
                PlayerActions::Move,
                GamepadStick::LEFT.with_circle_deadzone(SIG),
            );
        input_map
    }
}

fn add_player(mut commands: Commands) {
    commands
        .spawn((
            Player,
            InputManagerBundle::with_map(PlayerActions::default_mappings()),
            GlobalTransform::default(),
            Transform::default(),
        ))
        .with_children(|c| {
            c.spawn((
                Camera3dBundle {
                    projection: Projection::Perspective(PerspectiveProjection {
                        fov: 70.,
                        ..default()
                    }),
                    ..default()
                },
                PlayerCamera,
            ));
        });
}

const DEG_TO_RAD: f32 = 0.01745329;
const LOOK_SPEED: f32 = 45.0 * DEG_TO_RAD;
const LOOK_CLAMP: f32 = 75.0 * DEG_TO_RAD;
const MOVE_SPEED: f32 = 2.0;

fn grab_cursor_on_enter_fps_mode(mut query: Query<&mut Window, With<PrimaryWindow>>) {
    let Ok(mut window) = query.get_single_mut() else {
        return;
    };
    window.cursor.grab_mode = CursorGrabMode::Locked;
    window.cursor.visible = false;
}
fn release_cursor_on_enter_desk_mode(mut query: Query<&mut Window, With<PrimaryWindow>>) {
    let Ok(mut window) = query.get_single_mut() else {
        return;
    };
    window.cursor.grab_mode = CursorGrabMode::None;
    window.cursor.visible = true;
}

fn camera_look(
    mut cam_query: Query<&mut Transform, With<PlayerCamera>>,
    action_query: Query<&ActionState<PlayerActions>>,
    time: Res<Time>,
) {
    let Ok(input) = action_query.get_single() else {
        warn_once!("Failed to find ActionState");
        return;
    };
    let Ok(mut trans) = cam_query.get_single_mut() else {
        warn_once!("Failed to find player camera");
        return;
    };
    let look_dir = -input.axis_pair(&PlayerActions::Look);
    let mut euler = trans.rotation.to_euler(EulerRot::XYZ);
    euler.0 += look_dir.y * LOOK_SPEED * time.delta_seconds();
    euler.0 = euler.0.clamp(-LOOK_CLAMP, LOOK_CLAMP);
    trans.rotation = Quaternion::from_euler(EulerRot::XYZ, euler.0, euler.1, euler.2);
}

fn player_move(
    mut player_query: Query<&mut Transform, With<Player>>,
    action_query: Query<&ActionState<PlayerActions>>,
    time: Res<Time>,
) {
    let Ok(input) = action_query.get_single() else {
        warn_once!("failed to find ActionState");
        return;
    };
    let Ok(mut trans) = player_query.get_single_mut() else {
        warn_once!("Failed to find player transform");
        return;
    };
    let look_dir = -input.axis_pair(&PlayerActions::Look);
    let move_dir = input.axis_pair(&PlayerActions::Move);
    let right = trans.right();
    let forward = trans.forward();

    trans.rotate_local_y(look_dir.x * LOOK_SPEED * time.delta_seconds());
    if let Some(dir) = move_dir.try_normalize() {
        trans.translation += (dir.x * right + dir.y * forward).normalize_or_zero()
            * time.delta_seconds()
            * MOVE_SPEED;
    }
}
