use bevy::prelude::*;

pub struct GamePlugin;

impl Plugin for GamePlugin {
    fn build(&self, app: &mut App) {
        // TODO
        app.add_systems(Startup, startup);
    }
}

fn startup(mut command: Commands, mut meshes: ResMut<Assets<Mesh>>) {
    command.spawn(Camera3dBundle {
        transform: Transform::from_translation(Vec3::new(0., 0., -3.)),
        ..default()
    });
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
    // command.spawn()
}
