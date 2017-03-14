#ifndef __WHISPER_SESSION_H__
#define __WHISPER_SESSION_H__

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WhisperSession WhisperSession;

/**
 * Whisper stream types definition.
 * Reference: 
 *      https://tools.ietf.org/html/rfc4566#section-5.14
 *      https://tools.ietf.org/html/rfc4566#section-8
 */
typedef enum WhisperStreamType {
    /** Audio stream. */
    WhisperStreamType_audio = 0,
    /** Video stream. */
    WhisperStreamType_video,
    /** Text stream. */
    WhisperStreamType_text,
    /* Application stream */
    WhisperStreamType_application,
    /* Message stream */
    WhisperStreamType_message
} WhisperStreamType;

/**
 * WHisper session initialize options.
 */
typedef struct WhisperSessionOptions {
    /**
     * STUN server.
     *
     * Format is: server_name_or_ip[:port]
     * The default port is 3478.
     */
    const char *stun_server;
    /**
     * TURN server.
     *
     * Format is: server_name_or_ip[:port]
     * The default port is 3478.
     */
    const char *turn_server;
    /**
     * TURN server user name.
     */
    const char *turn_username;
    /**
     * TURN server password.
     */
    const char *turn_password;
} WhisperSessionOptions;

/* Global session APIs */

/**
 * Initialize Whisper session extension.
 * 
 * The application must initialize the session extension before calling
 * any session API.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      opts        [in] A pointer to a valid WhisperSessionOptions structure.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_session_init(Whisper *w, WhisperSessionOptions *opts);

/**
 * Clean up Whisper session extension.
 * 
 * The application should call whisper_session_cleanup before quit,
 * to clean up the resources associated with the extension.
 *
 * If the extension is not initialized, this function has no effect.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 */
WHISPER_API
void whisper_session_cleanup(Whisper *w);

/**
 * An application-defined function that handle session requests.
 *
 * WhisperSessionRequestCallback is the callback function type.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      from        [in] The id(userid@nodeid) from who send the message.
 * @param
 *      sdp         [in] The remote users SDP. End the null terminal.
 *                       Reference: https://tools.ietf.org/html/rfc4566
 * @param
 *      len         [in] The length of the SDP.
 * @param
 *      context     [in] The application defined context data.
 *
 * @return
 *      true if current callback consume this event, otherwise return false.
 */
typedef bool WhisperSessionRequestCallback(Whisper *w, const char *from,
        const char *sdp, size_t len, void *context);

/**
 * Set applications' session request callback.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      callback    [in] A pointer to the application-defined function of type
 *                       WhisperSessionRequestCallback.
 * @param
 *      context     [in] The application defined context data.
 */
WHISPER_API
void whisper_session_set_request_callback(Whisper *w,
        WhisperSessionRequestCallback *callback, void *context);

/* Session instance APIs */

/**
 * Create a new session to a friend.
 *
 * The session object represent a conversation handle to a friend.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      to          [in] The target id(userid or userid@nodeid).
 * @param
 *      offerer     [in] The role in current conversation.
 *                       If offer is true, current client will act as 
 *                       Controlling Agent, otherwise current client will
 *                       act as Controlled Agent.
 *                       Reference: https://tools.ietf.org/html/rfc5245#section-7.1.2.2 
 *
 * @return
 *      If no error occurs, return the pointer of WhisperSession object.
 *      Otherwise, return NULL, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
WhisperSession *whisper_session_new(Whisper *w, const char *to, bool offerer);

/**
 * Close a session to friend. All resources include streams, multiplexers 
 * associated with current sesion will be destroied.
 *
 * @param
 *      ws          [in] A handle to the Whisper session.
 */
WHISPER_API
void whisper_session_close(WhisperSession *ws);

/**
 * An application-defined function that receive session request complete
 * event.
 *
 * WhisperSessionRequestCompleteCallback is the callback function type.
 *
 * @param
 *      ws          [in] A handle to the WhisperSession.
 * @param
 *      status      [in] The status code of the response.
 *                       0 is success, otherwise is error.
 * @param
 *      reason      [in] The error message if status is error, or NULL
 * @param
 *      sdp         [in] The remote users SDP. End the null terminal.
 *                       Reference: https://tools.ietf.org/html/rfc4566
 * @param
 *      len         [in] The length of the SDP.
 * @param
 *      context     [in] The application defined context data.
 */
typedef void WhisperSessionRequestCompleteCallback(WhisperSession *ws, int status,
        const char *reason, const char *sdp, size_t len, void *context);

/**
 * Send session request to the friend.
 *
 * @param
 *      ws          [in] A handle to the WhisperSession.
 * @param
 *      callback    [in] A pointer to WhisperSessionRequestCompleteCallback
 *                       function to receive the session response.
 * @param
 *      context      [in] The application defined context data.
 *
 * @return
 *      0 if the invite request successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_session_request(WhisperSession *ws,
        WhisperSessionRequestCompleteCallback *callback, void *context);

/**
 * Reply the session request from friend.
 *
 * This function will send a session response to friend.
 *
 * @param
 *      ws          [in] A handle to the WhisperSession.
 * @param
 *      status      [in] The status code of the response.
 *                       0 is success, otherwise is error.
 * @param
 *      reason      [in] The error message if status is error, or NULL
 *                       if success.
 *
 * @return
 *      0 if the invite response successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_session_reply_request(WhisperSession *ws, int status,
        const char* reason);

/**
 * Begin to start a session.
 *
 * All streams in current session will try to connect with remote friend,
 * The stream status will update to application by stream's callbacks.
 *
 * @param
 *      ws          [in] A handle to the WhisperSession.
 * @param
 *      sdp         [in] The remote users SDP. End the null terminal.
 *                       Reference: https://tools.ietf.org/html/rfc4566
 * @param
 *      len         [in] The length of the SDP.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_session_start(WhisperSession *ws, const char *sdp, size_t len);

/* Session stream APIs */

/**
 * Whisper stream status.
 * The stream status will be changed according to the phase of the stream.
 */
typedef enum WhisperStreamState {
    /** New stream */
    WhisperStreamState_initialized,
    /** The stream gatherd all available candidates. */
    WhisperStreamState_candidate_gathered,
    /** The ICE context is ready for the stream. */
    WhisperStreamState_ice_ready,
    /** The stream is trying to connecting the remote. */
    WhisperStreamState_negotiating,
    /** The stream connected with remote */
    WhisperStreamState_connected,
    /** The stream is failed, cannot to continue. */
    WhisperStreamState_failed
} WhisperStreamState;

/**
 * Portforwarding supported protocols.
 */
typedef enum PortForwardingProtocol {
    /** UDP protocol. */
    PortForwardingProtocol_UDP = 0,
    /** TCP protocol. */    
    PortForwardingProtocol_TCP = 1
} PortForwardingProtocol;

/**
 * Multiplexing channel close reason code.
 */
typedef enum CloseReason {
    /* Channel closed normally. */
    CloseReason_Normal = 0,
    /* Channel closed because timeout. */
    CloseReason_Timeout = 1,
    /* Channel closed because error ocurred. */
    CloseReason_Error = 2
} CloseReason;

/**
 * Whisper stream callbacks.
 *
 * Include stream status callback, stream data callback, and multiplexing
 * callbacks.
 */
typedef struct WhisperStreamCallbacks {
    /* Common callbacks */
    /**
     * Callback to report status of various stream operations.
     * 
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      status      [in] Operation status defined in WhisperStreamState.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*state_changed)(WhisperSession *ws, int stream,
                          WhisperStreamState state, void *context);

    /* Stream callbacks */
    /**
     * Callback will be called when the stream receives
     * incoming packet.
     * 
     * If the stream enabled multiplexing mode, application will not
     * receive stream_data callback any more. All data will reported
     * as multiplexing channel data.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      component   [in] The component ID.
     * @param
     *      data        [in] The received packet data.
     * @param
     *      len         [in] The received data length.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*stream_data)(WhisperSession *ws, int stream, int component,
                        const void *data, size_t len, void *context);

    /* Multiplexer callbacks */
    /**
     * Callback will be called when new multiplexing channel request to open.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      component   [in] The component ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      cookie      [in] Application defined string data send from peer.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      True on success, or false if an error occurred. 
     *      The channel will continue to open only this callback return true,
     *      otherwise the channel will be closed.
     */
    bool (*channel_open)(WhisperSession *ws, int stream, int component,
                         int channel, const char *cookie, void *context);

    /**
     * Callback will be called when new multiplexing channel opened.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      component   [in] The component ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_opened)(WhisperSession *ws, int stream, int component,
                           int channel, void *context);

    /**
     * Callback will be called when channel close.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      component   [in] The component ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      reason      [in] Channel close reason code, defined in CloseReason.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_close)(WhisperSession *ws, int stream, int component,
                          int channel, CloseReason reason, void *context);

    /**
     * Callback will be called when channel received incoming data.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      component   [in] The component ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      data        [in] The received data.
     * @param
     *      len         [in] The received data length.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      True on success, or false if an error occurred. 
     *      If this callback return false, the channel will be closed
     *      with CloseReason_Error.
     */
    bool (*channel_data)(WhisperSession *ws, int stream, int component,
                         int channel, const void *data, size_t len, void *context);

    /**
     * Callback will be called when remote peer ask to pend data sending.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      component   [in] The component ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_pending)(WhisperSession *ws, int stream, int component,
                            int channel, void *context);

    /**
     * Callback will be called when remote peer ask to resume data sending.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      component   [in] The component ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_resume)(WhisperSession *ws, int stream, int component,
                           int channel, void *context);
} WhisperStreamCallbacks;

#define WHISPER_STREAM_ENCRYPT              0x01
#define WHISPER_STREAM_RELIABLE             0x02
#define WHISPER_STREAM_MULTIPLEXING         0x04
#define WHISPER_STREAM_PORT_FORWARDING      0x08

/**
 * Add a new stream to session.
 *
 * Whisper stream supports several underlying transport mechanisms:
 *
 *   - Plain/encrypted UDP data gram protocol
 *   - Plain/encrypted TCP like reliable stream protocol
 *   - Multiplexing over UDP
 *   - Multiplexing over TCP like reliable protocol
 *
 *  Application can use options to specify the new stream mode.
 *  Multiplexing over UDP can not provide reliable transport.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      type        [in] The stream type defined in WhisperStreamType.
 * @param
 *      options     [in] The stream mode options. options are constructed
 *                       by a bitwise-inclusive OR of flags from the 
 *                       following list:
 * 
 *                       - WHISPER_STREAM_ENCRYPT
 *                         Enctypted mode.
 *                       - WHISPER_STREAM_RELIABLE
 *                         Reliable mode.
 *                       - WHISPER_STREAM_MULTIPLEXING
 *                         Multiplexing mode.
 *                       - WHISPER_STREAM_PORT_FORWARDING
 *                         Support portforwarding over multiplexing.
 *
 * @param
 *      components  [in] The numer of components, max to 4.
 * @param
 *      callbacks   [in] The Application defined callback functions in 
 *                       WhisperStreamCallbacks.
 * @param
 *      context     [in] The application defined context data.
 *
 * @return
 *      Return stream id on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_session_add_stream(WhisperSession *ws, WhisperStreamType type,
        int options, int components,
        WhisperStreamCallbacks *callbacks, void *context);

/**
 * Remove a stream from session.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream id to be removed.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_session_remove_stream(WhisperSession *ws, int stream);

/**
 * Add a new portforwarding service to session.
 *
 * The registered services can be used by remote peer in portforwarding
 * request.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      service     [in] The new service name, should be unique
 *                       in session scope.
 * @param
 *      protocol    [in] The protocol of the service.
 * @param
 *      host        [in] The host name or ip of the service.
 * @param
 *      port        [in] The port of the service.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_session_add_service(WhisperSession *ws, const char *service,
        PortForwardingProtocol protocol, const char *host, const char *port);

/**
 * Remove a portforwarding server to session.
 *
 * This function has not effect on existing portforwarings.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      service     [in] The service name.
 */
WHISPER_API
void whisper_session_remove_service(WhisperSession *ws, const char *service);

/**
 * Set the whisper stream type.
 *
 * This function has not effect on connected streams.
 * Application can change the stream type before start session.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      type        [in] New stream type defined in WhisperStreamType.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_set_type(WhisperSession *ws, int stream,
        WhisperStreamType type);

/**
 * Get the whisper stream type.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      type        [out] The stream type defined in WhisperStreamType.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_get_type(WhisperSession *ws, int stream,
        WhisperStreamType *type);

/**
 * Send outgoing data to remote peer.
 *
 * If the stream is in multiplexing mode, application can not
 * call this function to send data. If this function is called
 * on multiplexing mode stream, it will return error.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      data        [in] The outgoing data.
 * @param
 *      len         [in] The outgoing data length.
 *
 * @return
 *      Sent bytes on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
ssize_t whisper_stream_write(WhisperSession *ws, int stream,
        int component, const void *data, size_t len);

/**
 * Open a new channel on multiplexing stream.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      cookie      [in] Application defined data pass to remote peer.
 *
 * @return
 *      New channel ID on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_open_channel(WhisperSession *ws, int stream,
        int component, const char *cookie);

/**
 * Close a new channel on multiplexing stream.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      channel     [in] The channel ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_close_channel(WhisperSession *ws, int stream,
        int component, int channel);

/**
 * Send outgoing data to remote peer.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      channel     [in] The channel ID.
 * @param
 *      data        [in] The outgoing data.
 * @param
 *      len         [in] The outgoing data length.
 *
 * @return
 *      Sent bytes on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
ssize_t whisper_stream_write_channel(WhisperSession *ws, int stream,
        int component, int channel, const void *data, size_t len);

/**
 * Request remote peer to pend channel data sending.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      channel     [in] The channel ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_pend_channel(WhisperSession *ws, int stream, 
        int component, int channel);

/**
 * Request remote peer to resume channel data sending.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      channel     [in] The channel ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_resume_channel(WhisperSession *ws, int stream,
        int component, int channel);

/**
 * Open a portforwarding to remote service over multiplexing.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      service     [in] The remote service name.
 * @param
 *      protocol    [in] Portforwarding protocol.
 * @param
 *      host        [in] Local host or ip to binding.
 *                       If host is NULL, portforwarding will bind to localhost.
 * @param
 *      port        [in] Local port to binding, can not be NULL.
 *
 * @return
 *      Portforwarding ID on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_open_port_forwarding(WhisperSession *ws, int stream,
        int component, const char *service, PortForwardingProtocol protocol,
        const char *host, const char *port);

/**
 * Open a portforwarding to remote service over multiplexing.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      component   [in] The component ID.
 * @param
 *      portforwarding  [in] The portforwarding ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred. 
 *      The specific error code can be retrieved by calling 
 *      whisper_get_error().
 */
WHISPER_API
int whisper_stream_close_port_forwarding(WhisperSession *ws, int stream,
        int component, int portforwarding);

#ifdef __cplusplus
}
#endif

#endif /* __WHISPER_SESSION_H__ */
