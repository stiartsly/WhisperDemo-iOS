
@import Foundation;

//! Project version number for ManagedWhisper.
FOUNDATION_EXPORT double ManagedWhisperVersionNumber;

//! Project version string for ManagedWhisper.
FOUNDATION_EXPORT const unsigned char ManagedWhisperVersionString[];

// In this header, you should import all the public headers of your framework
// using statements like #import <ManagedWhisper/PublicHeader.h>

/**
 The stream mode options.

 - WhisperStreamOptionPlain: Plain mode
 - WhisperStreamOptionReliable: Reliable mode
 - WhisperStreamOptionMultiplexing: Multiplexing mode
 - WhisperStreamOptionPortForwarding: Support portforwarding over multiplexing
 */
typedef NS_OPTIONS (int, WHWhisperStreamOptions) {
    WHWhisperStreamOptionCompress       = 0x01,
    WHWhisperStreamOptionPlain          = 0x02,
    WHWhisperStreamOptionReliable       = 0x04,
    WHWhisperStreamOptionMultiplexing   = 0x08,
    WHWhisperStreamOptionPortForwarding = 0x10,
} NS_SWIFT_NAME(WhisperStreamOptions);

/**
 Thread transport thread model

 - WHTransportSharedThreadModel: Shared thread model
 - WHTransportStandaloneThreadModel: Standalone thread model
 */
typedef NS_OPTIONS (int, WHTransportThreadModel) {
    WHTransportSharedThreadModel        = 0x01,
    WHTransportStandaloneThreadModel    = 0x02,
} NS_SWIFT_NAME(WhisperTransportThreadModel);
