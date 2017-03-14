#ifndef __WHISPER_DIST_H__
#define __WHISPER_DIST_H__

#include "whisper.h"
#include "whisper_ext.h"
#include "whisper_session.h"
#include "whisper_portforwarding.h"

typedef struct WhisperNodeInfo CNodeInfo;
typedef struct WhisperUserInfo CUserInfo;
typedef struct WhisperFriendInfo CFriendInfo;
typedef struct WhisperOptions COptions;
typedef struct WhisperCallbacks CCallbacks;
typedef enum WhisperConnectionStatus CConnectionStatus;
typedef enum WhisperEntrustmentType CEntrustmentType;
typedef enum WhisperLogLevel CLogLevel;

typedef enum WhisperStreamType  CStreamType;
typedef enum WhisperStreamState CStreamState;
typedef enum CloseReason CCloseReason;
typedef enum PortForwardingProtocol CPortForwardingProtocol;

typedef struct WhisperSessionOptions CSessionOptions;
typedef struct WhisperStreamCallbacks CStreamCallbacks;

typedef struct WhisperPortForwardingOptions CPortForwardingOptions;

#endif /* __WHISPER_DIST_H__ */
