# Xist's Lyra Mouse Tutorial

Main Branch: https://github.com/XistGG/LyraMouseTutorial

This is a Plugin intended to be used in a `LyraStarterGame` project.
The purpose of this is to demonstrate how to work with CommonUI as it
is implemented in Lyra 5.2, such that you have full control over
your game's input mode and mouse capture settings.


## Tutorial

See the related
[Dev Log](https://x157.github.io/UE5/LyraStarterGame/Tutorials/How-to-Take-Control-of-the-Mouse)
and/or the related
[YouTube Video](https://youtu.be/A9dp3cmCFtQ)
for more info on How to Take Control of the Mouse in Lyra/CommonUI.


## Setup Notes

### Requirements

- You must be using UE 5.2+ and Lyra 5.2+ source
  - Lyra 5.1 and previous will not work without modifying the Engine

### Initial Setup (Project-specific)

- Copy this plugin to your Lyra project into the `/Plugins/LyraMouseTutorial` directory
  - Add `LyraMouseTutorial` to the list of enabled `Plugins` in your `.uproject`

### Initial Setup (Lyra API export)

- Publicly export Lyra's `ULyraActivatableWidget` class using `LYRAGAME_API`

### Build Project

- Generate Project Files
- Build your project in your IDE
- Open your project in UEditor
