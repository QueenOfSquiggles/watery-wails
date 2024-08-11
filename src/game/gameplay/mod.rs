use bevy::{core_pipeline::tonemapping::Tonemapping, prelude::*};
use leafwing_input_manager::{
    prelude::{ActionState, DualAxis, InputMap, VirtualDPad},
    InputManagerBundle,
};

use super::{GameStateSystems, InputActions, PausedState};

pub fn plugin(app: &mut App) {
    app.add_systems(Startup, startup);
    app.add_systems(Update, (move_cam).in_set(GameStateSystems::Gameplay));
    app.add_systems(
        Update,
        (unpause).run_if(not(in_state(PausedState::Running))),
    );
}

fn startup(
    mut command: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    let input_map = InputMap::default()
        // Move Action
        .insert(InputActions::Move, DualAxis::right_stick())
        .insert(InputActions::Move, VirtualDPad::arrow_keys())
        .insert(InputActions::Move, VirtualDPad::wasd())
        // Accept Action
        .insert(InputActions::Accept, KeyCode::Enter)
        .insert(InputActions::Accept, KeyCode::KeyZ)
        .insert(InputActions::Accept, MouseButton::Left)
        .insert(InputActions::Accept, GamepadButtonType::South)
        // Cancel Action
        .insert(InputActions::Cancel, KeyCode::Escape)
        .insert(InputActions::Cancel, KeyCode::KeyX)
        .insert(InputActions::Cancel, MouseButton::Right)
        .insert(InputActions::Cancel, GamepadButtonType::East)
        .build();
    // TODO support remapping

    command
        .spawn(Camera3dBundle {
            transform: Transform::from_translation(Vec3::new(0., 0., 3.)),
            projection: PerspectiveProjection {
                fov: 60.0_f32.to_radians(),
                ..default()
            }
            .into(),
            tonemapping: Tonemapping::AcesFitted,
            ..default()
        })
        .insert(InputManagerBundle::with_map(input_map));
    command.spawn(DirectionalLightBundle {
        directional_light: DirectionalLight {
            color: Color::linear_rgb(1., 1., 1.),
            illuminance: 30_000.0,
            shadows_enabled: true,
            ..default()
        },
        transform: Transform {
            rotation: Quat::from_euler(
                EulerRot::XYZ,
                f32::to_radians(-90.0),
                f32::to_radians(45.0),
                0.,
            ),
            ..default()
        },
        ..default()
    });
    command.spawn(PbrBundle {
        mesh: meshes.add(Sphere::default().mesh().uv(32, 18)),
        material: materials.add(StandardMaterial::from_color(Color::hsl(0.0, 0.7, 0.5))),
        ..default()
    });
}

fn move_cam(
    mut query: Query<(&mut Transform, &ActionState<InputActions>), With<Camera3d>>,
    time: Res<Time>,
    mut next_pause_state: ResMut<NextState<PausedState>>,
) {
    const SPEED: f32 = 2.0;
    let (mut trans, action) = query.single_mut();

    if action.just_pressed(&InputActions::Accept) {
        next_pause_state.set(PausedState::PausedByPlayer);
        info!("Accept pressed");
    }
    if action.pressed(&InputActions::Move) {
        let Some(data) = action.axis_pair(&InputActions::Move) else {
            return;
        };
        trans.translation += Vec3::new(data.x(), data.y(), 0.) * SPEED * time.delta_seconds();
    }
}

fn unpause(
    mut query: Query<&ActionState<InputActions>, With<Camera3d>>,
    mut next_pause_state: ResMut<NextState<PausedState>>,
) {
    let action = query.single_mut();

    if action.just_pressed(&InputActions::Cancel) {
        next_pause_state.set(PausedState::Running);
        info!("Cancel pressed");
    }
}
