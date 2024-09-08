use bevy::{prelude::*, reflect::TypeRegistry};

pub struct DebugPlugin;

impl Plugin for DebugPlugin {
    fn build(&self, app: &mut App) {
        app.init_state::<DebugState>();
        app.configure_sets(
            Update,
            DebuggingSystemsSet.run_if(in_state(DebugState::Debugging)),
        );
        app.add_systems(Update, draw_debug_gizmos.in_set(DebuggingSystemsSet));
    }
}

#[derive(Debug, Hash, Clone, PartialEq, Eq, SystemSet)]
struct DebuggingSystemsSet;

#[derive(Debug, Hash, Clone, PartialEq, Eq, States, Default)]
pub enum DebugState {
    #[default]
    Debugging,
    None,
}

#[derive(Debug, Reflect, Default, Clone, Component)]
#[reflect(Default)]
pub struct DebugVisual {
    pub shape: VisualShape,
    pub color: Color,
    pub offset: Vec3,
}

const DEFAULT_GIZMO_COLOUR: Color = Color::linear_rgb(1., 1., 0.);
const CURVES_RESOLUTION: usize = 16;

impl DebugVisual {
    pub fn cube(size: f32) -> Self {
        Self {
            shape: VisualShape::Cube { size },
            color: DEFAULT_GIZMO_COLOUR,
            offset: Vec3::ZERO,
        }
    }
    pub fn cuboid(size: Vec3) -> Self {
        Self {
            shape: VisualShape::Cuboid { size },
            color: DEFAULT_GIZMO_COLOUR,
            offset: Vec3::ZERO,
        }
    }
    pub fn sphere(radius: f32) -> Self {
        Self {
            shape: VisualShape::Sphere { radius },
            color: DEFAULT_GIZMO_COLOUR,
            offset: Vec3::ZERO,
        }
    }
    pub fn capsule(radius: f32, height: f32) -> Self {
        Self {
            shape: VisualShape::Capsule { radius, height },
            color: DEFAULT_GIZMO_COLOUR,
            offset: Vec3::ZERO,
        }
    }
    pub fn with_colour(self, color: Color) -> Self {
        Self {
            shape: self.shape,
            color,
            offset: self.offset,
        }
    }

    pub fn with_offset(self, offset: Vec3) -> Self {
        Self {
            shape: self.shape,
            color: self.color,
            offset,
        }
    }
}

#[derive(Debug, Reflect, Clone)]
#[reflect(Default)]
pub enum VisualShape {
    Cube { size: f32 },
    Cuboid { size: Vec3 },
    Sphere { radius: f32 },
    Capsule { radius: f32, height: f32 },
}
impl Default for VisualShape {
    fn default() -> Self {
        Self::Sphere { radius: 1. }
    }
}

fn draw_debug_gizmos(query: Query<(&DebugVisual, &Transform)>, mut gizmos: Gizmos) {
    for (visuals, trans) in query.iter() {
        match visuals.shape {
            VisualShape::Cube { size } => gizmos.cuboid(
                Transform::from_translation(trans.translation + visuals.offset)
                    .with_rotation(trans.rotation)
                    .with_scale(Vec3::ONE * size),
                visuals.color,
            ),
            VisualShape::Cuboid { size } => gizmos.cuboid(
                Transform::from_translation(trans.translation + visuals.offset)
                    .with_rotation(trans.rotation)
                    .with_scale(size),
                visuals.color,
            ),
            VisualShape::Sphere { radius } => {
                gizmos
                    .sphere(
                        trans.translation + visuals.offset,
                        trans.rotation,
                        radius,
                        visuals.color,
                    )
                    .resolution(CURVES_RESOLUTION);
            }
            VisualShape::Capsule { radius, height } => {
                gizmos
                    .primitive_3d(
                        &Capsule3d::new(radius, height),
                        trans.translation + visuals.offset,
                        trans.rotation,
                        visuals.color,
                    )
                    .resolution(CURVES_RESOLUTION);
            }
        }
    }
}
