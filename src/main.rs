use bevy::prelude::*;
use bevy_tweening::TweeningPlugin;
use haalka::HaalkaPlugin;

fn main() {
    let mut app = App::new();
    #[cfg(target_family = "wasm")]
    {
        // this helps WASM builds to work better especially with ItchIO, according to an official bevy example.
        use bevy::asset::AssetMetaCheck;
        app = app.insert_resource(AssetMetaCheck::Never);
    }
    app.add_plugins(DefaultPlugins)
        .add_plugins(bevy_panic_handler::PanicHandler::new().build())
        .add_plugins(TweeningPlugin)
        .add_plugins(HaalkaPlugin)
        .run();
}
