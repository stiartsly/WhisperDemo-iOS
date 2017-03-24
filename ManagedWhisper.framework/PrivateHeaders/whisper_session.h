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
 * \~English
 * Whisper stream types definition.
 * Reference:
 *      https://tools.ietf.org/html/rfc4566#section-5.14
 *      https://tools.ietf.org/html/rfc4566#section-8
 *
 * \~Chinese
 * Whisper stream 类型定义。
 * 参考如下：
 *      https://tools.ietf.org/html/rfc4566#section-5.14
 *      https://tools.ietf.org/html/rfc4566#section-8
 */
typedef enum WhisperStreamType {
    /**
     * \~English
     *  Audio stream.
     *
     *  \~Chinese
     *  音频流。
     */
    WhisperStreamType_audio = 0,
    /**
     * \~English
     *  Video stream.
     *
     *  \~Chinese
     *  视频流。
     */
    WhisperStreamType_video,
    /**
     * \~English
     *  Text stream.
     *
     *  \~Chinese
     *  文本流。
     */
    WhisperStreamType_text,
    /**
     * \~English
     *  Application stream.
     *
     *  \~Chinese
     *  应用流。
     */
    WhisperStreamType_application,
    /**
     * \~English
     *  Message stream.
     *
     *  \~Chinese
     *  消息流。
     */
    WhisperStreamType_message
} WhisperStreamType;

/**
 * \~English
 * Whisper session initialize options.
 *
 * \~Chinese
 * Whisper session 初始化选项。
 */
typedef struct WhisperSessionOptions {
    /**
     * \~English
     * STUN server.
     *
     * Format is: server_name_or_ip[:port]
     * The default port is 3478.
     *
     * \~Chinese
     * STUN服务器。
     *
     * 格式如下：server_name_or_ip[:port]
     * 默认端口port是3478。
     */
    const char *stun_server;
    /**
     * \~English
     * TURN server.
     *
     * Format is: server_name_or_ip[:port]
     * The default port is 3478.
     *
     * \~Chinese
     * TURN服务器。
     *
     * 格式如下：server_name_or_ip[:port]
     * 默认端口port是3478。
     */
    const char *turn_server;
    /**
     * \~English
     * TURN server user name.
     *
     * \~Chinese
     * TURN服务器用户名。
     */
    const char *turn_username;
    /**
     * \~English
     * TURN server password.
     *
     * \~Chinese
     * TURN服务器密码。
     */
    const char *turn_password;
} WhisperSessionOptions;

/* Global session APIs */

/**
 * \~English
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
 *
 * \~Chinese
 * 初始化Whisper session扩展。
 *
 * 应用必须在调用session API前初始化session扩展。
 *
 * @param
 *     w            [输入] 指向Whisper客户实例的句柄。
 * @param
 *     ops          [输入] 指向一个有效WhisperSessionOptions结构的指针。
 *
 * @return
 *     0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_session_init(Whisper *w, WhisperSessionOptions *opts);

/**
 * \~English
 * Clean up Whisper session extension.
 *
 * The application should call whisper_session_cleanup before quit,
 * to clean up the resources associated with the extension.
 *
 * If the extension is not initialized, this function has no effect.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 *
 * \~Chinese
 * 清除Whisper session扩展。
 *
 * 应用必须在退出前调用whisper_session_cleanup，用于清除和扩展相关的资源。
 *
 * @param
 *      w           [输入] 指向Whisper客户实例的句柄。
 */
WHISPER_API
void whisper_session_cleanup(Whisper *w);

/**
 * \~English
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
 *
 * \~Chinese
 * 应用定义用于处理session请求的函数。
 *
 * WhisperSessionRequestCallback是回调函数类型。
 *
 * @param
 *      w            [输入] 指向Whisper客户实例的句柄。
 * @param
 *      from         [输入] 消息发送者的id（userid@nodeid）
 * @param
 *      sdp          [输入] 对端用户的SDP，以null结尾。
 *                         具体参考：https://tools.ietf.org/html/rfc4566
 * @param
 *      len          [输入] SDP长度。
 * @param
 *      context      [输入] 应用定义的上下文数据。
 */
typedef bool WhisperSessionRequestCallback(Whisper *w, const char *from,
        const char *sdp, size_t len, void *context);

/**
 * \~English
 * Set applications' session request callback.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      callback    [in] A pointer to the application-defined function of type
 *                       WhisperSessionRequestCallback.
 * @param
 *      context     [in] The application defined context data.
 *
 * \~Chinese
 * 设置应用的session请求回调函数。
 *
 * @param
 *      w            [输入] 指向Whisper客户实例的句柄。
 * @param
 *      callback     [输入] 指向应用定义的WhisperSessionRequestCallback类型函数。
 * @param
 *      context      [输入] 应用定义的上下文数据。
 */
WHISPER_API
void whisper_session_set_request_callback(Whisper *w,
        WhisperSessionRequestCallback *callback, void *context);

/* Session instance APIs */

/**
 * \~English
 * Create a new session to a friend.
 *
 * The session object represent a conversation handle to a friend.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      to          [in] The target id(userid or userid@nodeid).
 *
 * @return
 *      If no error occurs, return the pointer of WhisperSession object.
 *      Otherwise, return NULL, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 *
 * \~Chinese
 * 创建一个新session
 *
 * 一个session实例代表一次和friend的会话句柄。
 *
 * @param
 *     w            [输入] 指向Whisper客户实例的句柄。
 * @param
 *     to           [输入] 目标id（userid 或者 userid@nodeid）。
 *
 * @return
 *     如果成功，返回WishierSession对象的指针，否则返回NULL。
 *     具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
WhisperSession *whisper_session_new(Whisper *w, const char *to);

/**
 * \~English
 * Close a session to friend. All resources include streams, multiplexers
 * associated with current session will be destroyed.
 *
 * @param
 *      ws          [in] A handle to the Whisper session.
 *
 * \~Chinese
 * 关闭指定session。销毁所有和session有关资源，包括streams和multiplexers。
 *
 * @param
 *      ws          [输入] 指向Whisper客户实例的句柄。
 */
WHISPER_API
void whisper_session_close(WhisperSession *ws);

/**
 * \~English
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
 *
 * \~Chinese
 * 应用定义用于接收session请求完成的函数。
 *
 * WhisperSessionRequestCompleteCallback是回调函数类型。
 *
 * @param
 *      ws           [输入] 指向Whisper客户实例的句柄。
 * @param
 *      status       [输入] 响应的状态码。
 *                         0表示成功，否则表示失败。
 * @param
 *      reason       [输入] 若状态码表示失败，reason中存放失败的信息；否则为空。
 * @param
 *      sdp          [输入] 对端用户的SDP，以null结尾。
 *                         具体参考：https://tools.ietf.org/html/rfc4566
 * @param
 *      len          [输入] SDP长度。
 * @param
 *      context      [输入] 应用定义的上下文数据。
 */
typedef void WhisperSessionRequestCompleteCallback(WhisperSession *ws, int status,
        const char *reason, const char *sdp, size_t len, void *context);

/**
 * \~English
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
 *      0 if the session request successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 *
 * \~Chinese
 * 发送session请求。
 *
 * @param
 *     ws            [输入] 指向Whisper客户实例的句柄。
 * @param
 *     callback      [输入] 指向接收session响应的WhisperSessionRequestCompleteCallback函数。
 * @param
 *     context       [输入] 应用定义的上下文数据。
 *
 * @return
 *     若session请求发送成功，则返回0。否则，返回-1。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_session_request(WhisperSession *ws,
        WhisperSessionRequestCompleteCallback *callback, void *context);

/**
 * \~English
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
 *      0 if the session response successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 *
 * \~Chinese
 * 回复session请求。
 *
 * 函数发送session响应。
 *
 * @param
 *      ws           [输入] 指向Whisper客户实例的句柄。
 * @param
 *      status       [输入] 响应的状态码。
 *                         0表示成功，否则表示失败。
 * @param
 *      reason       [输入] 若状态码表示失败，reason中存放失败的信息；否则为空。
 *
 * @return
 *      若session响应发送成功，则返回0。否则，返回-1。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_session_reply_request(WhisperSession *ws, int status,
        const char* reason);

/**
 * \~English
 * Begin to start a session.
 *
 * All streams in current session will try to connect with remote friend,
 * the stream status will update to application by stream's callbacks.
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
 *
 * \~Chinese
 * 开始启动session。
 *
 * 当前session的所有stream都尝试连接对端，相应引起的stream状态更新通过回调传递给应用。
 *
 * @param
 *      ws           [输入] 指向WhisperSession的句柄。
 * @param
 *      sdp          [输入] 对端用户的SDP，以null结尾。
 *                         具体参考：https://tools.ietf.org/html/rfc4566
 * @param
 *      len          [输入] SDP长度。
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_session_start(WhisperSession *ws, const char *sdp, size_t len);

/* Session stream APIs */

/**
 * \~English
 * Whisper stream state.
 * The stream status will be changed according to the phase of the stream.
 *
 * \~Chinese
 * Whisper stream状态。
 * stream状态根据stream所处阶段不同而不同。
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
    /** The stream closed normally */
    WhisperStreamState_closed,
    /** The stream is deactivated */
    WhisperStreamState_deactivated,
    /** The stream is failed, cannot to continue. */
    WhisperStreamState_failed
} WhisperStreamState;

/**
 * \~English
 * Portforwarding supported protocols.
 *
 * \~Chinese
 * Portforwarding支持的协议。
 */
typedef enum PortForwardingProtocol {
    /** UDP protocol. */
    PortForwardingProtocol_UDP = 0,
    /** TCP protocol. */
    PortForwardingProtocol_TCP = 1
} PortForwardingProtocol;

/**
 * \~English
 * Multiplexing channel close reason code.
 *
 * \Chinese
 * 多路复用通道关闭原因。
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
 * \~English
 * Whisper stream callbacks.
 *
 * Include stream status callback, stream data callback, and multiplexing
 * callbacks.
 *
 * \~Chinese
 * Whisper stream回调函数组合。
 *
 * 该结构包含回调函数可以更新stream状态，接收stream数据和实现多路复用。
 */
typedef struct WhisperStreamCallbacks {
    /* Common callbacks */
    /**
     * \~English
     * Callback to report status of various stream operations.
     *
     * @param
     *      ws          [in] The handle to the WhisperSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      state       [in] Stream state defined in WhisperStreamState.
     * @param
     *      context     [in] The application defined context data.
     *
     * \~Chinese
     * 该回调提示因不同stream操作引起的状态变化。
     *
     * @param
     *      ws          [输入] 指向WhisperSession的句柄。
     * @param
     *      stream      [输入] stream id。
     * @param
     *      state       [输入] Whisper Stream状态。
     * @param
     *      context     [输入] 应用定义的上下文数据。
     */
    void (*state_changed)(WhisperSession *ws, int stream,
                          WhisperStreamState state, void *context);

    /* Stream callbacks */
    /**
     * \~English
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
     *
     * \~Chinese
     * 该回调接收对端传输到应用的数据包。
     *
     * 如果stream处于多路复用工作模式，应用将不会通过该回调来接收stream数据。所有数据都将通过
     * 多路复用通道传递给应用。
     *
     * @param
     *      ws           [输入] 指向WhisperSession的句柄。
     * @param
     *      stream       [输入] stream id。
     * @param
     *      component    [输入] component id。
     * @param
     *      data         [输入] 接收到的数据包。
     * @param
     *      len          [输入] 接收到的数据长度。
     * @param
     *      context      [输入] 应用定义的上下文数据。
     */
    void (*stream_data)(WhisperSession *ws, int stream, int component,
                        const void *data, size_t len, void *context);

    /* Multiplexer callbacks */
    /**
     * \~English
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
     *      cookie      [in] Application defined string data receive from peer.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      True on success, or false if an error occurred.
     *      The channel will continue to open only this callback return true,
     *      otherwise the channel will be closed.
     *
     * \~Chinese
     * 应用收到创建一条多路复用请求，调用该函数。
     *
     * @param
     *      ws           [输入] 指向WhisperSession的句柄。
     * @param
     *      stream       [输入] stream id。
     * @param
     *      component    [输入] component id。
     * @param
     *      channel      [输入] 当前channel id。
     * @param
     *      cookie       [输入] 对端应用的自定义数据。
     * @param
     *      context      [输入] 应用定义的上下文数据。
     * @return
     *      true表示成功，false表示失败。若返回true， 应用继续创建通道，否则通道关闭。
     */
    bool (*channel_open)(WhisperSession *ws, int stream, int component,
                         int channel, const char *cookie, void *context);

    /**
     * \~English
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
     *
     * \~Chinese
     * 应用成功创建一条多路复用通道，调用该函数。
     *
     * @param
     *      ws           [输入] 指向WhisperSession的句柄。
     * @param
     *      stream       [输入] stream id。
     * @param
     *      component    [输入] component id。
     * @param
     *      channel      [输入] 当前channel id。
     * @param
     *      context      [输入] 应用定义的上下文数据。
     */
    void (*channel_opened)(WhisperSession *ws, int stream, int component,
                           int channel, void *context);

    /**
     * \~English
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
     *
     * \~Chinese
     * 应用关闭通道，调用该函数。
     *
     * @param
     *      ws           [输入] 指向WhisperSession的句柄。
     * @param
     *      stream       [输入] stream id。
     * @param
     *      component    [输入] component id。
     * @param
     *      channel      [输入] 当前channel id。
     * @param
     *      reason       [输入] 关闭通道的原因，原因类型定义在CloseReason。
     * @param
     *      context      [输入] 应用定义的上下文数据。
     */
    void (*channel_close)(WhisperSession *ws, int stream, int component,
                          int channel, CloseReason reason, void *context);

    /**
     * \~English
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
     *
     * \~Chinese
     * 应用接收到对端数据，调用该函数。
     *
     * @param
     *      ws           [输入] 指向WhisperSession的句柄。
     * @param
     *      stream       [输入] stream id。
     * @param
     *      component    [输入] component id。
     * @param
     *      channel      [输入] 当前channel id。
     * @param
     *      data         [输入] 接收到的数据包。
     * @param
     *      len          [输入] 接收到的数据长度。
     * @param
     *      context      [输入] 应用定义的上下文数据。
     *
     * @return
     *      true表示成功，false表示失败。若返回false， 通道关闭失败原因保存在CloseReason_Error。
     */
    bool (*channel_data)(WhisperSession *ws, int stream, int component,
                         int channel, const void *data, size_t len, void *context);

    /**
     * \~English
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
     *
     * \~Chinese
     * 接收到对端要求暂停数据发送，应用调用该函数。
     *
     * @param
     *      ws           [输入] 指向WhisperSession的句柄。
     * @param
     *      stream       [输入] stream id。
     * @param
     *      component    [输入] component id。
     * @param
     *      channel      [输入] 当前channel id。
     * @param
     *      context      [输入] 应用定义的上下文数据。
     */
    void (*channel_pending)(WhisperSession *ws, int stream, int component,
                            int channel, void *context);

    /**
     * \~English
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
     *
     * \~Chinese
     * 接收到对端要求回复数据发送，应用调用该函数。
     *
     * @param
     *      ws           [输入] 指向WhisperSession的句柄。
     * @param
     *      stream       [输入] stream id。
     * @param
     *      component    [输入] component id。
     * @param
     *      channel      [输入] 当前channel id。
     * @param
     *      context      [输入] 应用定义的上下文数据。
     */
    void (*channel_resume)(WhisperSession *ws, int stream, int component,
                           int channel, void *context);
} WhisperStreamCallbacks;

#define WHISPER_STREAM_ENCRYPT              0x01
#define WHISPER_STREAM_RELIABLE             0x02
#define WHISPER_STREAM_MULTIPLEXING         0x04
#define WHISPER_STREAM_PORT_FORWARDING      0x08

/**
 * \~English
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
 *
 * \~Chinese
 * 指定session中添加stream。
 *
 * Whisper stream支持以下几种传输机制：
 *
 *   - 普通/加密UDP数据报传输
 *   - 普通/加密TCP可靠数据流传输
 *   - 基于UDP的多路复用
 *   - 基于TCP可靠传输的多路复用
 *
 *  应用通过参数options来表明添加stream的传输机制。基于UDP的多路复用不能提供可靠传输。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      type        [输入] stream类型，定义在WhisperStreamType。
 * @param
 *      options     [输入] stream工作模式。该参数通过位操作或者以下的flag来设置：
 *
 *                       - WHISPER_STREAM_ENCRYPT
 *                         加密模式。
 *                       - WHISPER_STREAM_RELIABLE
 *                         可靠传输模式。
 *                       - WHISPER_STREAM_MULTIPLEXING
 *                         多路复用模式。
 *                       - WHISPER_STREAM_PORT_FORWARDING
 *                         基于多路复用的端口转发模式。
 *
 * @param
 *      components  [输入] component数目, 最大值为4。
 * @param
 *      callbacks   [输入] 应用定义的WhisperStreamCallbacks类型回调函数。
 * @param
 *      context     [输入] 应用定义的上下文数据。
 *
 * @return
 *      若添加stream成功，返回stream id，反之，返回-1。
 *      具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_session_add_stream(WhisperSession *ws, WhisperStreamType type,
        int options, int components,
        WhisperStreamCallbacks *callbacks, void *context);

/**
 * \~English
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
 *
 * \~Chinese
 * 删除session中的指定stream。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] 需删除的stream id。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_session_remove_stream(WhisperSession *ws, int stream);

/**
 * \~English
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
 *
 * \~Chinese
 * 添加一个新端口转发服务。
 *
 * 新注册的服务可以被对端用于端口转发请求。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      service     [输入] 新服务名，在session中具有唯一性。
 * @param
 *      protocol    [输入] 服务支持的协议。
 * @param
 *      host        [输入] 服务的主机名或者ip地址。
 * @param
 *      port        [输入] 服务的端口。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_session_add_service(WhisperSession *ws, const char *service,
        PortForwardingProtocol protocol, const char *host, const char *port);

/**
 * \~English
 * Remove a portforwarding service to session.
 *
 * This function has not effect on existing portforwarings.
 *
 * @param
 *      ws          [in] The handle to the WhisperSession.
 * @param
 *      service     [in] The service name.
 *
 * \~Chinese
 * 删除session中的指定的端口转发服务。
 *
 * 该函数不能用于已存在的portforwaring中。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      service     [输入] 服务名。
 */
WHISPER_API
void whisper_session_remove_service(WhisperSession *ws, const char *service);

/**
 * \~English
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
 *
 * \~Chinese
 * 设置whisper stream类型
 *
 * 该函数不能用于已连接stream。
 * 应用可以在开启session之前改变stream类型。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      type        [输入] 新stream类型，具体定义WhisperStreamType。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_set_type(WhisperSession *ws, int stream,
        WhisperStreamType type);

/**
 * \~English
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
 *
 * \~Chinses
 * 获取whisper stream类型。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      type        [输入] 新stream类型，具体定义WhisperStreamType。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_get_type(WhisperSession *ws, int stream,
        WhisperStreamType *type);

/**
 * \~English
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
 *
 * \~Chinese
 * 发送数据到对端。
 *
 * 若stream处于多路复用模式，应用不能调用该函数去发送数据。如果多路复用stream调用该函数发送数据，
 * 函数返回错误。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      data        [输入] 发送的数据。
 * @param
 *      len         [输入] 发送数据的长度。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
ssize_t whisper_stream_write(WhisperSession *ws, int stream,
        int component, const void *data, size_t len);

/**
 * \~English
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
 *
 * \~Chinese
 * 创建一个新多路复用通道。
 *
 * 若stream不支持多路复用，调用该函数失败。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      cookie      [输入] 传递给对端的自定义数据。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_open_channel(WhisperSession *ws, int stream,
        int component, const char *cookie);

/**
 * \~English
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
 *
 * \~Chinese
 * 关闭指定多路复用通道。
 *
 * 若stream不支持多路复用，调用该函数失败。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      channel     [输入] channel id。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_close_channel(WhisperSession *ws, int stream,
        int component, int channel);

/**
 * \~English
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
 *
 * \~Chinese
 * 发送数据到对端。
 *
 * 如果stream不支持多路复用，调用该函数失败。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      channel     [输入] channel id。
 * @param
 *      data        [输入] 发送的数据。
 * @param
 *      len         [输入] 发送数据的长度。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
ssize_t whisper_stream_write_channel(WhisperSession *ws, int stream,
        int component, int channel, const void *data, size_t len);

/**
 * \~English
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
 *
 * \~Chinese
 * 请求对端暂停数据发送。
 *
 * 如果stream不支持多路复用，调用该函数失败。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      channel     [输入] channel id。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_pend_channel(WhisperSession *ws, int stream,
        int component, int channel);

/**
 * \~English
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
 *
 * \~Chinese
 * 请求对端恢复数据传输
 *
 * 如果stream不支持多路复用，调用该函数失败。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      channel     [输入] channel id。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_resume_channel(WhisperSession *ws, int stream,
        int component, int channel);

/**
 * \~English
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
 *                       If host is NULL, portforwarding will bind to 127.0.0.1.
 * @param
 *      port        [in] Local port to binding, can not be NULL.
 *
 * @return
 *      Portforwarding ID on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      whisper_get_error().
 *
 * \~Chinese
 * 创建一个基于多路复用的端口转发。
 *
 * 如果stream不支持多路复用，调用该函数失败。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      service     [输入] 对端服务名。
 * @param
 *      protocol    [输入] 端口转发协议。
 * @param
 *      host        [输入] 用于绑定的本地主机或者ip地址。
 *                        如果本地主机为空，则绑定127.0.0.1。
 * @param
 *      port        [输入] 用于绑定的本地端口，不可为空。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_open_port_forwarding(WhisperSession *ws, int stream,
        int component, const char *service, PortForwardingProtocol protocol,
        const char *host, const char *port);

/**
 * \~English
 * Close a portforwarding to remote service over multiplexing.
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
 *
 * \~Chinese
 * 关闭一个基于多路复用的端口转发。
 *
 * 如果stream不支持多路复用，调用该函数失败。
 *
 * @param
 *      ws          [输入] 指向WhisperSession的句柄。
 * @param
 *      stream      [输入] stream id。
 * @param
 *      component   [输入] component id。
 * @param
 *      portforwarding   [输入] portforwarding id。
 *
 * @return
 *      0表示成功，-1表示失败。具体错误可以调用whisper_get_error()来获得。
 */
WHISPER_API
int whisper_stream_close_port_forwarding(WhisperSession *ws, int stream,
        int component, int portforwarding);

#ifdef __cplusplus
}
#endif

#endif /* __WHISPER_SESSION_H__ */
