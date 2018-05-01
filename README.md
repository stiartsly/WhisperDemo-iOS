Whisper Demo on iOS
===================

## Introduction

Whisper Demo is an exemplary and easy-understand app to show what whisper framework can do over whisper network. It shows you can use apps on Android/iOS/Raspberry to control remote device (vice versa) based on P2P technology.

## Whisper network types

Two types of whisper network would be supported:

- Managed whisper network (or centralized whisper network)
- Decentralized whisper network

## Feaures:

The items for remote control currently includes:

- Turn on/off torch (or flashlight)
- Increase/Decrease screen backgroud light
- Turn on/off ringtone
- Increase/Decrease ringtone volume
- Turn on/off camera video

## Build from source

Run following command to get source code:

```shell
$ git clone https://github.com/stiartsly/WhisperDemo-iOS.git
```

Then open this project with Apple Xcode to build app.

## Build dependencies

Before building whisper demo, you have to download and build the following dependencies:

- ManagedWhisper.framework (currently Vanilla)
- ffmpeg
- QRCode

As to dependency **ManagedWhisper.framework**, you need to get source from

```
https://github.com/stiartsly/whisper-ios.git
```

to build it, then copy (or replace) it's ditributions **ManagedWhisper.framework** to top directory of project.

As to dependency **ffmpeg**, the project source code already includes pre-build distrbutions under directory **FFmpeg**.

Last for **QRcode**, run following command to install and build it:

```shell
$ sudo gem install cocoapods  
$ pod install
```

## Deploy && Run

Run on iOS Phone or Simulator with iOS version **9.0 or higher**.

## License

MIT