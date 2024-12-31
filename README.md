# BlipKit for Godot Engine 4.3

An addon for creating the beautiful sound of old sound chips. This uses the [BlipKit](https://github.com/detomon/BlipKit) library to render audio.

- [Installation](#installation)
- [Usage](#usage)

## Installation

1. Search for `blipkit` in the `AssetLib` editor tab
2. Download and install the plugin
3. Enable the plugin in the project settings via the `Plugins` tab

or...

1. Clone this repository or download the archive
2. Copy the folder `addons/detomon.blipkit` to your project's `addons` folder (create it if needed)
3. Enable the plugin in the project settings via the `Plugins` tab

## Usage

- Create an `AudioStreamPlayer` node and add an `AudioStreamBlipKit` resource
- Set the `AudioStreamPlayer` node to `playing` which produces a silent audio output for the moment
- Get the stream playback with `AudioStreamPlayer.get_stream_playback()`
- Configure and attach `BlipKitTrack` objects to the playback object

See the [examples](examples) directory on how to use the extension.

All class descirptions are available in the Editor in the reference documentation. Also see the [doc/classes](doc/classes) directory.
