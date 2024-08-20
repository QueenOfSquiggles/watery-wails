use bevy::{core_pipeline::tonemapping::Tonemapping, prelude::*};
use leafwing_input_manager::{
    prelude::{ActionState, GamepadStick, InputMap, KeyboardVirtualDPad},
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
    assets: Res<AssetServer>,
) {
    command
        .spawn(SceneBundle {
            scene: assets.load(GltfAssetLabel::Scene(0).from_asset("Sandwich Character.glb")),
            ..default()
        })
        .insert(Name::new("Sandwich Goober"));

    let input_map = InputMap::default()
        // Move Action
        .with_dual_axis(InputActions::Move, GamepadStick::LEFT)
        .with_dual_axis(InputActions::Move, KeyboardVirtualDPad::WASD)
        .with_dual_axis(InputActions::Move, KeyboardVirtualDPad::ARROW_KEYS)
        // Accept Action
        .with(InputActions::Accept, KeyCode::Enter)
        .with(InputActions::Accept, KeyCode::KeyZ)
        .with(InputActions::Accept, MouseButton::Left)
        .with(InputActions::Accept, GamepadButtonType::South)
        // Cancel Action
        .with(InputActions::Cancel, KeyCode::Escape)
        .with(InputActions::Cancel, KeyCode::KeyX)
        .with(InputActions::Cancel, MouseButton::Right)
        .with(InputActions::Cancel, GamepadButtonType::East);
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
        transform: Transform {
            translation: Vec3::new(3., 0., 0.),
            ..default()
        },
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
    let data = action.axis_pair(&InputActions::Move);
    trans.translation += data.extend(0.0) * SPEED * time.delta_seconds();
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
