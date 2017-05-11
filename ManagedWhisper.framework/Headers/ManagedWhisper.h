
#import <UIKit/UIKit.h>

//! Project version number for ManagedWhisper.
FOUNDATION_EXPORT double ManagedWhisperVersionNumber;

//! Project version string for ManagedWhisper.
FOUNDATION_EXPORT const unsigned char ManagedWhisperVersionString[];

// In this header, you should import all the public headers of your framework
// using statements like #import <ManagedWhisper/PublicHeader.h>

typedef NS_OPTIONS (int, WMWhisperTransportOptions) {
    WMWhisperTransportOptionICE = 1 << 0,
    WMWhisperTransportOptionUDP = 1 << 1,
    WMWhisperTransportOptionTCP = 1 << 2,
} NS_SWIFT_NAME(WhisperTransportOptions);

/**
 The stream mode options.

 - WhisperStreamOptionEncrypt: Enctypted mode
 - WhisperStreamOptionReliable: Reliable mode
 - WhisperStreamOptionMultiplexing: Multiplexing mode
 - WhisperStreamOptionPortForwarding: Support portforwarding over multiplexing
 */
typedef NS_OPTIONS (int, WMWhisperStreamOptions) {
    WMWhisperStreamOptionEncrypt        = 1 << 0,
    WMWhisperStreamOptionReliable       = 1 << 1,
    WMWhisperStreamOptionMultiplexing   = 1 << 2,
    WMWhisperStreamOptionPortForwarding = 1 << 3,
} NS_SWIFT_NAME(WhisperStreamOptions);

