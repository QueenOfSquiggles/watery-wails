use bevy::prelude::*;
use leafwing_input_manager::{
    prelude::{ActionState, DualAxis, InputMap, VirtualDPad},
    InputManagerBundle,
};

use super::{GameStateSystems, InputActions};

pub fn plugin(app: &mut App) {
    app.add_systems(Startup, startup);
    app.add_systems(Update, (move_cam).in_set(GameStateSystems::Gameplay));
}

fn startup(mut command: Commands, mut meshes: ResMut<Assets<Mesh>>) {
    let input_map = InputMap::default()
        .insert(InputActions::Move, DualAxis::right_stick())
        .insert(InputActions::Move, VirtualDPad::arrow_keys())
        .insert(InputActions::Move, VirtualDPad::wasd())
        .insert(InputActions::Accept, KeyCode::Enter)
        .insert(InputActions::Accept, KeyCode::KeyZ)
        .insert(InputActions::Accept, MouseButton::Left)
        .insert(InputActions::Accept, GamepadButtonType::South)
        .insert(InputActions::Cancel, KeyCode::Escape)
        .insert(InputActions::Cancel, KeyCode::KeyX)
        .insert(InputActions::Cancel, MouseButton::Right)
        .insert(InputActions::Cancel, GamepadButtonType::East)
        .build();

    command
        .spawn(Camera3dBundle {
            transform: Transform::from_translation(Vec3::new(0., 0., 3.)),
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
        ..default()
    });
    command.spawn(PbrBundle {
        mesh: meshes.add(Sphere::default().mesh().uv(32, 18)),
        ..default()
    });
}

fn move_cam(
    mut query: Query<(&mut Transform, &ActionState<InputActions>), With<Camera3d>>,
    time: Res<Time>,
) {
    const SPEED: f32 = 2.0;
    let (mut trans, action) = query.single_mut();
    if action.just_pressed(&InputActions::Accept) {
        info!("Accept pressed");
    }
    if action.just_pressed(&InputActions::Cancel) {
        info!("Cancel pressed");
    }
    if action.pressed(&InputActions::Move) {
        let Some(data) = action.axis_pair(&InputActions::Move) else {
            return;
        };
        trans.translation += Vec3::new(data.x(), data.y(), 0.) * SPEED * time.delta_seconds();
    }
}
