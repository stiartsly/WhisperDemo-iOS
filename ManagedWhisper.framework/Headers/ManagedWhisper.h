
@import Foundation;

//! Project version number for ManagedWhisper.
FOUNDATION_EXPORT double ManagedWhisperVersionNumber;

//! Project version string for ManagedWhisper.
FOUNDATION_EXPORT const unsigned char ManagedWhisperVersionString[];

// In this header, you should import all the public headers of your framework
// using statements like #import <ManagedWhisper/PublicHeader.h>

typedef NS_OPTIONS (int, WMWhisperTransportOptions) {
    WMWhisperTransportOptionICE = 0x01,
    WMWhisperTransportOptionUDP = 0x02,
    WMWhisperTransportOptionTCP = 0x04,
} NS_SWIFT_NAME(WhisperTransportOptions);

/**
 The stream mode options.

 - WhisperStreamOptionEncrypt: Enctypted mode
 - WhisperStreamOptionReliable: Reliable mode
 - WhisperStreamOptionMultiplexing: Multiplexing mode
 - WhisperStreamOptionPortForwarding: Support portforwarding over multiplexing
 */
typedef NS_OPTIONS (int, WMWhisperStreamOptions) {
    WMWhisperStreamOptionCompress       = 0x01,
    WMWhisperStreamOptionEncrypt        = 0x02,
    WMWhisperStreamOptionReliable       = 0x04,
    WMWhisperStreamOptionMultiplexing   = 0x08,
    WMWhisperStreamOptionPortForwarding = 0x10,
} NS_SWIFT_NAME(WhisperStreamOptions);

