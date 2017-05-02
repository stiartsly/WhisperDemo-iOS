
#import <UIKit/UIKit.h>

//! Project version number for ManagedWhisper.
FOUNDATION_EXPORT double ManagedWhisperVersionNumber;

//! Project version string for ManagedWhisper.
FOUNDATION_EXPORT const unsigned char ManagedWhisperVersionString[];

// In this header, you should import all the public headers of your framework
// using statements like #import <ManagedWhisper/PublicHeader.h>

typedef NS_OPTIONS (int, WhisperTransportOptions) {
    WhisperTransportOptionICE   = 1 << 0,
    WhisperTransportOptionUDP   = 1 << 1,
    WhisperTransportOptionTCP   = 1 << 2,
};

/**
 The stream mode options.

 - WhisperStreamOptionEncrypt: Enctypted mode
 - WhisperStreamOptionReliable: Reliable mode
 - WhisperStreamOptionMultiplexing: Multiplexing mode
 - WhisperStreamOptionPortForwarding: Support portforwarding over multiplexing
 */
typedef NS_OPTIONS (int, WhisperStreamOptions) {
    WhisperStreamOptionEncrypt          = 1 << 0,
    WhisperStreamOptionReliable         = 1 << 1,
    WhisperStreamOptionMultiplexing     = 1 << 2,
    WhisperStreamOptionPortForwarding   = 1 << 3,
};

