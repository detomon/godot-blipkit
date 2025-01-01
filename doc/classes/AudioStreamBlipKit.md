# Class: AudioStreamBlipKit

Inherits: *AudioStream*

**An audio stream used to attach and play audio generated from [`BlipKitTrack`](BlipKitTrack.md)s.**

## Description

**Note:** This resource uses the same [`AudioStreamBlipKitPlayback`](AudioStreamBlipKitPlayback.md) instance between playbacks.

## Online Tutorials

- [Power On!](https://github.com/detomon/godot-blipkit/blob/master/examples/power_on/power_on.md)

## Properties

- *int* [**`clock_rate`**](#int-clock_rate)

## Methods

- *void* [**`call_synced`**](#void-call_syncedcallback-callable)(callback: Callable)

## Property Descriptions

### `int clock_rate`

*Default*: `240`

Sets the frequency of the master clock as number of *ticks* per second. This influences the speed of effects, instrument envelopes, and dividers.


## Method Descriptions

### `void call_synced(callback: Callable)`

Calls the callback synced to the audio thread. This can be used to ensure that multiple modifications are executed on the same time.


