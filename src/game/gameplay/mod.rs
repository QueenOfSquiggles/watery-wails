use avian3d::{
    math::Quaternion,
    prelude::{ColliderConstructor, ColliderConstructorHierarchy, PhysicsDebugPlugin},
    PhysicsPlugins,
};
use bevy::prelude::*;
use debug::{DebugPlugin, DebugVisual};
use player::PlayerPlugin;

mod debug;
mod player;

pub fn plugin(app: &mut App) {
    app.add_plugins((
        DebugPlugin,
        PlayerPlugin,
        PhysicsPlugins::default(),
        PhysicsDebugPlugin::default(),
    ));
    app.add_systems(Startup, startup);
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
            transform: Transform {
                translation: Vec3::new(0.7, -1.5, -1.6),
                rotation: Quaternion::from_euler(EulerRot::XYZ, 0., -1.2, 0.),
                ..default()
            },
            ..default()
        })
        .insert((
            Name::new("Sandwich Goober"),
            // DebugVisual::capsule(1., 1.5).with_offset(Vec3::new(0., 0.75, 0.)),
            ColliderConstructorHierarchy::new(ColliderConstructor::ConvexDecompositionFromMesh),
        ));
    command
        .spawn(SceneBundle {
            scene: assets.load(GltfAssetLabel::Scene(0).from_asset("cameras_testing.glb")),
            transform: Transform {
                translation: Vec3 {
                    x: -7.7,
                    y: -3.2,
                    z: -14.1,
                },
                rotation: Quaternion::from_rotation_y(0.8),
                ..default()
            },
            ..default()
        })
        .insert((
            Name::new("SceneLevel"),
            ColliderConstructorHierarchy::new(ColliderConstructor::ConvexHullFromMesh),
            // DebugVisual::capsule(1., 1.5).with_offset(Vec3::new(0., 0.75, 0.)),
        ));

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
    command
        .spawn(PbrBundle {
            mesh: meshes.add(Sphere::default().mesh().uv(32, 18)),
            material: materials.add(StandardMaterial::from_color(Color::hsl(0.0, 0.7, 0.5))),
            transform: Transform {
                translation: Vec3::new(3., 0., 0.),
                ..default()
            },
            ..default()
        })
        .insert((
            DebugVisual::cube(1.5).with_colour(Color::linear_rgb(1., 0., 0.)),
            Name::new("Test Sphere"),
        ));
}

// fn move_cam(
//     mut query: Query<(&mut Transform, &ActionState<InputActions>), With<Camera3d>>,
//     time: Res<Time>,
// ) {
//     const SPEED: f32 = 2.0;
//     let (mut trans, action) = query.single_mut();
//     let data = action.axis_pair(&InputActions::Move);
//     trans.translation += data.extend(0.0) * SPEED * time.delta_seconds();
// }

// fn manage_active_camera(
//     mut query: Query<&mut Camera>,
//     query_input: Query<&ActionState<InputActions>>,
//     mut index: Local<usize>,
// ) {
//     let Ok(input) = query_input.get_single() else {
//         warn_once!("No available ActionState components at this moment");
//         return;
//     };
//     let entries = query.iter().count();
//     let last_index = *index;
//     if input.just_pressed(&InputActions::Accept) {
//         *index += 1;
//         if *index >= entries {
//             *index = 0;
//         }
//     } else if input.just_pressed(&InputActions::Cancel) {
//         if *index == 0 {
//             *index = entries;
//         }
//         *index -= 1;
//     }
//     if last_index != *index {
//         info!("Currently active camera: {} of {}", *index, (entries - 1));
//     }
//     for (e, mut cam) in query.iter_mut().enumerate() {
//         cam.is_active = e == *index;
//     }
// }
