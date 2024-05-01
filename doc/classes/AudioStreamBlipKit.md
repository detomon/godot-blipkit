# Class: AudioStreamBlipKit

Inherits: *AudioStream*

**An audio stream used to attach and play audio generated from [`BlipKitTrack`](BlipKitTrack.md)s.**

## Description

When the stream is playing, its internal master clock is ticking at a rate of 240 *ticks* per second. Every *tick* updates effects and dividers of attached [`BlipKitTrack`](BlipKitTrack.md)s, and envelope values of [`BlipKitInstrument`](BlipKitInstrument.md)s.

This resource reuses the same [`AudioStreamBlipKitPlayback`](AudioStreamBlipKitPlayback.md) instance between playbacks.

The stream audio is always resampled to 44100 Hz.

**Note:** If an [`AudioStreamBlipKit`](AudioStreamBlipKit.md) resource is freed, all attached [`BlipKitTrack`](BlipKitTrack.md)s are detached.

## Online Tutorials

- [Power On!](https://github.com/detomon/godot-blipkit/blob/master/examples/power_on/power_on.md)

## Properties

- `bool resource_local_to_scene` `[overrides Resource: true]`

## Methods

- *void* [**`call_synced`**](#void-call_syncedcallback-callable)(callback: Callable)

## Method Descriptions

### `void call_synced(callback: Callable)`

Calls the callback synced to the audio thread. This can be used to ensure that multiple modifications are executed on the same time. (For example, ensuring that multiple [`BlipKitTrack`](BlipKitTrack.md)s are attached at the same time with `BlipKitTrack.attach()`.)

For updating properties of individual [`BlipKitTrack`](BlipKitTrack.md)s over time, consider using `BlipKitTrack.add_divider()`.


