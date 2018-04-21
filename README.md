Whisper Demo on iOS
===========================

Whisper Demo is an easy-understand application to show what Whisper framework can do over whisper network. It shows you can use Apps on Android/iOS/Raspberry to control each other with p2p technology.

## Whisper network types

Two types of whisper network would be supprted:

- Managed whisper network
- Decentralzied whisper network

## Feaures:

The items for remote control currently includes:

- Turn on/off torch (or light)
- Turn on/off ringtone
- Increase/Decrease ringtone volume
- Turn on/off camera video

## Build from source

You should get source code from the following repository on github.com:

```
https://github.com/stiartsly/WhisperDemo-iOS.git
```
Then open this xcode project with Apple xcode to build it.

## Build dependencies

Before buiding whisper demo, you have to download and build the following dependencies:

- ManagedWhisper.framework (vanilla)
- ffmpeg
- QRCode

As to whisper android sdk, you need to get source from

```
https://github.com/stiartsly/whisper-ios.git
```
and after building, copy (or replace) it's ditributions 'ManagedWhisper' to top directory of project.

As for dependency 'ffmpeg', the project source code already includes it's pre-build distrbutions under directory 'FFmpeg".

Last for QRcode, you can use following command to install and build distrbution

```shell
$ sudo gem install cocoapods  
$ pod install
```

## Deploy && Run

Run on Phone or Simulator with iOS version 9.0 or higher.

## License

Whisper Demo project source code files are made available under the MIT License.