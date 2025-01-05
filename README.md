# Xist's Lyra Mouse Tutorial

Main Branch: https://github.com/XistGG/LyraMouseTutorial

**Updated for UE 5.5**

This is a Plugin intended to be used in a `LyraStarterGame` project.
The purpose of this is to demonstrate how to work with CommonUI as it
is implemented in Lyra 5.5.

Note this code has changed somewhat from UE 5.2 as CommonUI has evolved.
The old code (which is reviewed in the YT video) is in the `Lyra-5.2` branch
if you want to see that to follow the video.

I have simplified this code somewhat in 5.5 so you don't quite have as much control
over the exact input mode, but it's also easier to use.

In 5.5+ you change the mode like:

```cpp
const ECommonInputMode Mode = ECommonInputMode::All;  // or ECommonInputMode::Game, etc
XistedSetInputMode(PlayerController, Mode);
```


## Tutorial

See the related
[Dev Log](https://x157.github.io/UE5/LyraStarterGame/Tutorials/How-to-Take-Control-of-the-Mouse)
and/or the related
[YouTube Video](https://youtu.be/A9dp3cmCFtQ)
for more info on How to Take Control of the Mouse in Lyra/CommonUI.

The concepts are still the same, but the code reviewed in that video is
slightly different from the current UE 5.5 code.


## Setup Notes

### Requirements

- You must be using UE 5.5+ and Lyra 5.5+ source

### Initial Setup (Project-specific)

- Copy this plugin to your Lyra project into the `/Plugins/LyraMouseTutorial` directory
  - Add `LyraMouseTutorial` to the list of enabled `Plugins` in your `.uproject`
- Modify `LogXim` references to match your project's log descriptors
- `#define WITH_XIM_INPUT_DEBUG` to be either `true` or `false` depending on whether you want verbose input mode debug logging

### Initial Setup (Lyra API export)

- Publicly export Lyra's `ULyraActivatableWidget` class using `LYRAGAME_API`

### Build Project

- Generate Project Files
- Build your project in your IDE
- Open your project in UEditor
