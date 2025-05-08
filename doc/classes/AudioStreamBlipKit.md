# Class: AudioStreamBlipKit

Inherits: *AudioStream*

**An audio stream used to attach and play audio generated from [`BlipKitTrack`](BlipKitTrack.md)s.**

## Description

When the stream is playing, its internal master clock is ticking at a rate of 240 *ticks* per second per default. Every *tick* updates effects of attached [`BlipKitTrack`](BlipKitTrack.md)s, and envelopes of [`BlipKitInstrument`](BlipKitInstrument.md)s. See also `BlipKitTrack.add_divider()`.

The stream audio is always resampled to 44100 Hz.

This resource reuses the same [`AudioStreamBlipKitPlayback`](AudioStreamBlipKitPlayback.md) instance between playbacks.

**Note:** If an [`AudioStreamBlipKit`](AudioStreamBlipKit.md) resource is freed, all attached [`BlipKitTrack`](BlipKitTrack.md)s are detached.

## Online Tutorials

- [Power On!](https://github.com/detomon/godot-blipkit/blob/master/examples/power_on/power_on.md)

## Properties

- *int* [**`clock_rate`**](#int-clock_rate) `[default: 240]`
- `bool resource_local_to_scene` `[overrides Resource: true]`

## Methods

- *void* [**`call_synced`**](#void-call_syncedcallback-callable)(callback: Callable)

## Property Descriptions

### `int clock_rate`

*Default*: `240`

Sets the number of *ticks* per second of the internal master clock.


## Method Descriptions

### `void call_synced(callback: Callable)`

Calls the callback synced to the audio thread. This can be used to ensure that multiple modifications are executed on the same time. (For example, ensuring that multiple [`BlipKitTrack`](BlipKitTrack.md)s are attached at the same time with `BlipKitTrack.attach()`.)

For updating properties of individual [`BlipKitTrack`](BlipKitTrack.md)s over time, consider using `BlipKitTrack.add_divider()`.


