#ifndef __WHISPER_H__
#define __WHISPER_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WHISPER_STATIC)
  #define WHISPER_API
#elif defined(WHISPER_DYNAMIC)
  #ifdef WHISPER_BUILD
    #if defined(_WIN32) || defined(_WIN64)
      #define WHISPER_API        __declspec(dllexport)
    #else
      #define WHISPER_API        __attribute__((visibility("default")))
    #endif
  #else
    #if defined(_WIN32) || defined(_WIN64)
      #define WHISPER_API        __declspec(dllimport)
    #else
      #define WHISPER_API
    #endif
  #endif
#else
  #define WHISPER_API
#endif

/**
 * Whisper managed App/Node/User ID max length.
 */
#define WHISPER_MAX_ID_LEN                  45

/**
 * Whisper managed App/Node/User key max length.
 */
#define WHISPER_MAX_KEY_LEN                 45

/**
 * Whisper managed user login name max length.
 */
#define WHISPER_MAX_LOGIN_LEN               127

/**
 * Whisper managed user password max length.
 */
#define WHISPER_MAX_PASSWORD_LEN            63

/**
 * Whisper managed server URI max length.
 */
#define WHISPER_MAX_SERVER_URI_LEN          127

/**
 * Whisper managed user name max length.
 */
#define WHISPER_MAX_USER_NAME_LEN           63

/**
 * Whisper managed user description max length.
 */
#define WHISPER_MAX_USER_DESCRIPTION_LEN    127

/**
 * Whisper managed user phone number max length.
 */
#define WHISPER_MAX_PHONE_LEN               31

/**
 * Whisper managed user email address max length.
 */
#define WHISPER_MAX_EMAIL_LEN               127

/**
 * Whisper managed user region max length.
 */
#define WHISPER_MAX_REGION_LEN              127

/**
 * Whisper managed user gender max length.
 */
#define WHISPER_MAX_GENDER_LEN              31

/**
 * Whisper managed user name max length.
 */
#define WHISPER_MAX_NODE_NAME_LEN           63

/**
 * Whisper managed user description max length.
 */
#define WHISPER_MAX_NODE_DESCRIPTION_LEN    127

/**
 * Whisper managed user presence max length.
 */
#define WHISPER_MAX_USER_PRESENCE_LEN       31

/**
 * Whisper managed App message max length.
 */
#define WHISPER_MAX_APP_MESSAGE_LEN         2048

typedef struct Whisper Whisper;

/******************************************************************************
 * Creation & destruction
 *****************************************************************************/

/**
 * WhisperOptions defines several settings that control the way the
 * client connects to an Whisper managed server.
 *
 * @remark
 *      Default values are not defined for server_uri of WhisperOptions,
 *      so application should be set server_url clearly.
 *      If the WhisperOptions structure is defined as an automatic
 *      variable, all members are set to random values and thus must
 *      be set by the client application. If the WhisperOptions structure
 *      is defined as a static variable, initialization (in compliant
 *      compilers) sets all values to 0 (NULL for pointers).
 */
typedef struct WhisperOptions {
    /**
     * \~English
     * The user login identifier passed to the server when the client connects
     * to it.
     *
     * \~Chinese
     * 用户登录ID。
     */
    const char *login;
    /**
     * The user password to the server when the client connects to it.
     *
     * IMPORTANT:
     *     The application should clear the password in memory ASAP for
     *     security issues.
     */
    const char *password;
    /**
     * The app identifier passed to the server when the client connects
     * to it.
     */
    const char *appid;
    /**
     * The app key passed to the server when the client connects to it.
     */
    const char *appkey;
    /**
     * The Web API server URL;
     */
    const char *api_server_url;
    /**
     * A null - terminated string specifying the server to which the client
     * will connect.It takes the form <i>protocol://host:port</i>.
     * Currently, <i>protocol< / i> must be <i>tcp< / i> or <i>ssl< / i>.
     * For <i>host< / i>, you can specify either an IP address or a host name.
     * For instance, to connect to a server running on the local machine,
     * specify <i>tcp ://localhost:1883</i>.
     */
    const char *mqtt_server_uri;
    /**
     * The file in PEM format containing the server public digital
     * certificates trusted by the client.
     */
    const char* trust_store;
    /**
     * The "keep alive" interval, measured in seconds, defines the maximum
     * time that should pass without communication between the client and
     * the server The client will ensure that at least one message travels
     * across the network within each keep alive period. In the absence of
     * a data-related message during the time period, the client sends
     * a very small "ping" message, which the server will acknowledge.
     * The keep alive interval enables the client to detect when the server
     * is no longer available without having to wait for the long TCP/IP
     * timeout. The default value is zero, will use system default interval.
     */
    int keepalive_interval;
    /**
     * The time interval in seconds to allow a connect to complete.
     * The default value is zero, will use system default timeout.
     */
    int connect_timeout;
    /**
     * The time interval in seconds for retry connect.
     * The default value is zero, means retry immediately.
     */
    int retry_interval;
    /**
     * The max number of times to retry connect.
     * The default value is zero, means no retry.
     */
    int retry_times;
    /**
     * The application defined persistent data location.
     * The location must be set.
     */
    const char *persistent_location;
    /**
     * The virtual device ID specified by application.
     * This attribute only for advanced user & test purpose
     */
     const char *deviceid;
} WhisperOptions;

/**
 * Get the current version of Whisper client.
 */
WHISPER_API
const char *whisper_get_version(void);

/**
 * Whisper log level to control or filter log output.
 */
typedef enum WhisperLogLevel {
    /**
     * Log level None
     * Indicate disable log output.
     */
    WhisperLogLevel_None = 0,
    /**
     * Log level error.
     * Indicate output log with level 'Error' only.
     */
    WhisperLogLevel_Error = 1,
    /**
     * Log level warning.
     * Indicate output log with level 'Error' and 'Warning'.
     */
    WhisperLogLevel_Warning = 2,
    /**
     * Log level info.
     * Indicate output log with level 'Error', 'Warning' and 'Info'.
     */
    WhisperLogLevel_Info = 3,
    /*
     * Log level debug.
     * Indicate output log with no filters.
     */
    WhisperLogLevel_Debug = 4

} WhisperLogLevel;

/**
 * initialize log options for Whisper. The default level to control log output
 * is 'Info'.
 *
 * @param
 *      level       [in] The log level to control internal log output.
 * @param
 *      log_file    [in] the log file name.
 *                       If the log_file is NULL, Whisper will not write
 *                       log to file.
 * @param
 *      log_printer [in] the user defined log printer. can be NULL.
 */
WHISPER_API
void whisper_log_init(WhisperLogLevel level, const char *log_file,
        void (*log_printer)(const char *format, va_list args));

/**
 * Check if the whisper ID is valid.
 *
 * @param
 *      id          [in] the whisper id to be check.
 *
 * @return
 *      true if id is valid, or false if id is not valid.
 */
WHISPER_API
bool whisper_id_is_valid(const char *id);

/**
 * Create a new Whisper client instance. after create the instance, it's
 * ready for connection to the server
 *
 * @param
 *      options     [in] A pointer to a valid WhisperOptions structure.
 *
 * @return
 *      If no error occurs, return the pointer of Whisper client instance.
 *      Otherwise, return NULL, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
Whisper *whisper_new(const WhisperOptions *options);

/**
 * Disconnect from the server, and destroy all associated resources with the
 * Whisper client instance.
 *
 * After calling the function, the Whisper pointer becomes invalid.
 * No other function can be called.
 *
 * @param
 *      w           [in] A handle identifying the Whisper client instance
 *                       to kill.
 */
WHISPER_API
void whisper_kill(Whisper *w);

/******************************************************************************
 * Connection & event loop
 *****************************************************************************/

/**
 * Attempts to connect the client to server, if the client successfully
 * connects to the server, then start the client's main event loop.
 * The connect options was specified by previously create options.
 * @see whisper_create().
 *
 * @param
 *      w           [in] A handle identifying the Whisper client instance.
 * @param
 *      interval    [in] Internal loop interval, in milliseconds.
 *
 * @return
 *      0 if the client successfully connected to the server and start the
 *      event loop. Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_run(Whisper *w, int interval);

/**
 * Whisper client connection status to the server.
 */
typedef enum WhisperConnectionStatus {
    /**
     * Attempting to connect to server.
     * Not connected yet, so it's offline.
     */
    WhisperConnectionStatus_Connecting,
    /**
     * Whisper client connected to the server.
     * Indicate the client is online.
     */
    WhisperConnectionStatus_Connected,
    /**
     * There is no connection to the server.
     * Indicate the client is offline.
     */
    WhisperConnectionStatus_Disconnected,
} WhisperConnectionStatus;


/******************************************************************************
 * Internal client information
 *****************************************************************************/

/**
 * Get app identifier associated with the Whisper client.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      appid       [out] The buffer that will receive the identifier.
 *                        The buffer size should at least
 *                        (WHISPER_MAX_ID_LEN + 1) bytes.
 * @param
 *      len         [in] The buffer size of appid.
 *
 * @return
 *      The appId string pointer, or NULL if buffer is too small.
 */
WHISPER_API
char *whisper_get_appid(Whisper *w, char *appid, size_t len);

/**
 * Get node identifier associated with the Whisper client.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      nodeid      [out] The buffer that will receive the identifier.
 *                        The buffer size should at least
 *                        (WHISPER_MAX_ID_LEN + 1) bytes.
 * @param
 *      len         [in] The buffer size of nodeid.
 *
 * @return
 *      The nodeId string pointer, or NULL if buffer is too small.
 */
WHISPER_API
char *whisper_get_nodeid(Whisper *w, char *nodeid, size_t len);

/**
 * Get user identifier associated with the Whisper client.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      userid      [out] The buffer that will receive the identifier.
 *                        The buffer size should at least
 *                        (WHISPER_MAX_ID_LEN + 1) bytes.
 * @param
 *      len         [in] The buffer size of userid.
 *
 * @return
 *      The userId string pointer, or NULL if buffer is too small.
 */
WHISPER_API
char *whisper_get_userid(Whisper *w, char *userid, size_t len);

/**
 * Get user login name associated with the Whisper client.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      login       [out] The buffer that will receive the login name.
 *                        The buffer size should at least
 *                        (WHISPER_MAX_LOGIN_LEN + 1) bytes.
 * @param
 *      len         [in] The buffer size of login name.
 *
 * @return
 *      The login string pointer, or NULL if buffer is too small.
 */
WHISPER_API
char *whisper_get_login(Whisper *w, char *login, size_t len);

/******************************************************************************
 * Client information
 *****************************************************************************/

/**
 * A structure representing the Whisper user information.
 *
 * In whisper managed SDK, self and all friends are Whisper user, and have
 * same user attributes.
 */
typedef struct WhisperUserInfo {
    /**
     * User ID. Read only to the client application.
     */
    char userid[WHISPER_MAX_ID_LEN+1];
    /**
     * Nickname, also as display name.
     */
    char name[WHISPER_MAX_USER_NAME_LEN+1];
    /**
     * User's description, also as what's up.
     */
    char description[WHISPER_MAX_USER_DESCRIPTION_LEN+1];
    /**
     * If user has avatar.
     */
    int has_avatar;
    /**
     * User's gender.
     */
    char gender[WHISPER_MAX_GENDER_LEN+1];
    /**
     * User's phone number.
     */
    char phone[WHISPER_MAX_PHONE_LEN+1];
    /**
     * User's email address.
     */
    char email[WHISPER_MAX_EMAIL_LEN+1];
    /**
     * User's region information.
     */
    char region[WHISPER_MAX_REGION_LEN+1];
} WhisperUserInfo;

/**
 * Update self information.
 *
 * After self information changed, client will update self information
 * to server. then server will broadcast the change to all friends and
 * the other node that current user signed in.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      info        [in] The WhisperUserInfo pointer to the updated user info.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_set_self_info(Whisper *w, const WhisperUserInfo *info);

/**
 * Get self information.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      info        [in] The WhisperUserInfo pointer to receive user info.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_get_self_info(Whisper *w, WhisperUserInfo *info);

/**
 * A structure representing the Whisper node information.
 */
typedef struct WhisperNodeInfo {
    /**
     * The node id. Read only to the client application.
     */
    char nodeid[WHISPER_MAX_ID_LEN+1];
    /**
     * The node name.
     */
    char name[WHISPER_MAX_NODE_NAME_LEN+1];
    /**
     * The node description.
     */
    char description[WHISPER_MAX_NODE_DESCRIPTION_LEN+1];
} WhisperNodeInfo;

/**
 * Set node information.
 *
 * After node information changed, client will update node information
 * to server.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      info        [in] The WhisperNodeInfo pointer to the updated node info.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_set_node_info(Whisper *w, const WhisperNodeInfo *info);

/**
 * Get node information.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      info        [in] The WhisperNodeInfo pointer to receive the node info.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_get_node_info(Whisper *w, WhisperNodeInfo *info);


/******************************************************************************
 * Friend information
 *****************************************************************************/

 /**
 * Whisper client entrustment type to friend.
 */
typedef enum WhisperEntrustmentType {
    /**
     * Entrust the friend.
     */
    WhisperEntrustmentType_Entrusted = 1,
    /**
     * No entrusted relationship with friend.
     */
    WhisperEntrustmentType_None = 0,
    /**
     * Entrusted by friend.
     */
    WhisperEntrustmentType_EntrustedBy = -1
} WhisperEntrustmentType;

/**
 * A structure representing the Whisper friend information.
 *
 * Include the basic user information and the extra friend information.
 */
typedef struct WhisperFriendInfo {
    /**
     * Friend's user information.
     */
    WhisperUserInfo user_info;
    /**
     * Entrustment relationship with this friend, @see WhisperEntrustmentType
     */
    int entrusted;
    /**
     * Your label for the friend.
     */
    char label[WHISPER_MAX_USER_NAME_LEN+1];
    /**
     * Friend's presence status.
     */
    char presence[WHISPER_MAX_USER_PRESENCE_LEN+1];
} WhisperFriendInfo;

/**
 * An application-defined function that iterate the each friends list item.
 *
 * WhisperFriendsIterateCallback is the callback function type.
 *
 * @param
 *      info        [in] A pointer to WhisperFriendInfo structure that
 *                       representing a friend
 * @param
 *      context     [in] The application defined context data.
 *
 * @return
 *      Return true to continue iterate next friend user info,
 *      false to stop iterate.
 */
typedef bool WhisperFriendsIterateCallback(const WhisperFriendInfo *info,
                                           void *context);
/**
 * Get friends list. For each friend will call the application defined
 * iterate callback.
 *
 * @param
 *      w           [in] a handle to the Whisper client instance.
 * @param
 *      callback    [in] a pointer to WhisperFriendsIterateCallback function.
 * @param
 *      context     [in] the application defined context data.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_get_friends(Whisper *w,
                        WhisperFriendsIterateCallback *callback, void *context);

/**
 * Get friend information.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      friendid    [in] The friend's user id.
 * @param
 *      info        [in] The WhisperFriendInfo pointer to receive the friend
 *                       information.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_get_friend_info(Whisper *w, const char *friendid,
                            WhisperFriendInfo *info);

/**
 * Set the label of the specified friend.
 *
 * If the value length is 0 or value is NULL, the attribute will be cleared.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      friendid    [in] The friend's user id.
 * @param
 *      label       [in] the new label of the specified friend.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling whisper_get_error().
 *
 * @remarks
 *      The label of a friend is a private alias named by yourself. It can be
 *      seen by yourself only, and has no impact to the target friend.
 */
WHISPER_API
int whisper_set_friend_label(Whisper *w,
                             const char *friendid, const char *label);

/**
 * Check if the user ID is friend.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      userid      [in] The userid to check.
 *
 * @return
 *      true if the user id is friend, or false if not;
 */
WHISPER_API
bool whisper_is_friend(Whisper* w, const char* userid);

/******************************************************************************
 * Friend requests & add & remove
 *****************************************************************************/

/**
 * Attempt to send a new friend request.
 *
 * This function will send a friend request to server, and the server
 * return the result within a friend response. The whisper client can
 * check whether the friend request be confirmed or refused in friend
 * response callback.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      userid      [in] The target user id.
 * @param
 *      hello       [in] PIN for target user, or any application defined
 *                       content.
 *
 * @return
 *      0 if the friend request successfully send to the server.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_friend_request(Whisper *w, const char *userid, const char *hello);

/**
 * Reply the friend request.
 *
 * This function will send a friend response to server.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      userid      [in] The user id who wants friend with self.
 * @param
 *      status      [in] The status code of the response.
 *                       0 is success, otherwise is error.
 * @param
 *      reason      [in] The error message if status is error, or NULL
 *                       if success.
 * @param
 *      entrusted   [in] Entrust this friend or not.
 * @param
 *      expire      [in] Expire time or NULL never expire.
 *
 * @return
 *      0 if the friend request successfully send to the server.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_reply_friend_request(Whisper *w, const char *userid,
                                 int status, const char *reason,
                                 bool entrusted,
                                 const char *expire);

/**
 * Remove a friend.
 *
 * This function will send a remove friend indicator to server.
 *
 * If all correct, the server will clean the friend relationship, and send
 * friend removed message to both.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      userid      [in] The target user id.
 *
 * @return
 *      0 if the indicator successfully send to the server.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_friend_remove(Whisper *w, const char *userid);

/******************************************************************************
 * Application transactions between friends
 *****************************************************************************/

/**
 * Send a message to a friend.
 *
 * The message length may not exceed WHISPER_MAX_APP_MESSAGE_LEN, and message
 * itself should be text-formatted. Larger messages must be split by application
 * and sent as separate messages. Other clients can reassemble the fragments.
 *
 * Message may not be empty or NULL.
 *
 * If the message is binary, it will encoded to text use base64.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      to          [in] The target id(userid or userid@nodeid).
 * @param
 *      msg         [in] The message content defined by application.
 * @param
 *      len         [in] The message length in bytes.
 *
 * @return
 *      0 if the text message successfully send to the server.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_send_friend_message(Whisper *w, const char *to,
                                const char *msg, size_t len);

/**
 * An application-defined function that process the friend invite response.
 *
 * WhisperFriendInviteResponseCallback is the callback function type.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      from        [in] The target user id.
 * @param
 *      status      [in] The status code of the response.
 *                       0 is success, otherwise is error.
 * @param
 *      reason      [in] The error message if status is error, or NULL
 * @param
 *      data        [in] The application defined data return by target user.
 * @param
 *      len         [in] The data length in bytes.
 * @param
 *      context      [in] The application defined context data.
 */
typedef void WhisperFriendInviteResponseCallback(Whisper *w,
                                                 const char *from,
                                                 int status, const char *reason,
                                                 const char *data, size_t len,
                                                 void *context);

/**
 * Send invite request to a friend.
 *
 * Application can attach the application defined data with in the invite
 * request, and the data will send to target friend.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      to          [in] The target id(userid or userid@nodeid).
 * @param
 *      data        [in] The application defined data send to target user.
 * @param
 *      len         [in] The data length in bytes.
 * @param
 *      callback    [in] A pointer to WhisperFriendInviteResponseCallback
 *                       function to receive the invite response.
 * @param
 *      context      [in] The application defined context data.
 *
 * @return
 *      0 if the invite request successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_invite_friend(Whisper *w, const char *to,
                          const char *data, size_t len,
                          WhisperFriendInviteResponseCallback *callback,
                          void *context);

/**
 * Reply the friend invite request.
 *
 * This function will send a invite response to friend.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      to          [in] The id(userid@nodeid) who send invite request.
 * @param
 *      status      [in] The status code of the response.
 *                       0 is success, otherwise is error.
 * @param
 *      reason      [in] The error message if status is error, or NULL
 *                       if success.
 * @param
 *      data        [in] The application defined data send to target user.
 *                       If the status is error, this will be ignored.
 * @param
 *      len         [in] The data length in bytes.
 *                       If the status is error, this will be ignored.
 *
 * @return
 *      0 if the invite response successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling whisper_get_error().
 */
WHISPER_API
int whisper_reply_friend_invite(Whisper *w, const char *to,
                                int status, const char *reason,
                                const char *data, size_t len);

/******************************************************************************
 * Whisper callbacks
 *****************************************************************************/

/**
 * Whisper callbacks, include all global callbacks for whisper.
 */
typedef struct WhisperCallbacks {
    /**
     * An application-defined function that perform idle work.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*idle)(Whisper *w, void *context);

    /**
     * An application-defined function that process the self connection status.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      status      [in] Current connection status. @see WhisperConnection.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*connection_status)(Whisper *w,
                              WhisperConnectionStatus status, void *context);

    /**
     * An application-defined function that process the ready notification.
     * Notice: application should wait this callback invoked before calling any
     * whisper function to interact with friends.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*ready)(Whisper *w, void *context);

    /**
     * An application-defined function that process the self info change event.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      info        [in] The WhisperUserInfo pointer to the new user info.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*self_info)(Whisper *w, const WhisperUserInfo *info, void *context);

    /**
     * An application-defined function that iterate the each friends list item.
     *
     * WhisperFriendsIterateCallback is the callback function type.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      info        [in] A pointer to WhisperFriendInfo structure that
     *                       representing a friend
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      Return true to continue iterate next friend user info,
     *      false to stop iterate.
     */
    bool (*friend_list)(Whisper *w, const WhisperFriendInfo* info, void* context);

    /**
     * An application-defined function that process the friend information
     * change event.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      friendid    [in] The friend's user id.
     * @param
     *      info        [in] The WhisperFriendInfo pointer to the new friend info.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*friend_info)(Whisper *w, const char *friendid,
                        const WhisperFriendInfo *info, void *context);

    /**
     * An application-defined function that process the friend presence
     * change event.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      friendid    [in] The friend's user id.
     * @param
     *      presence    [in] The presence status of the friend.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*friend_presence)(Whisper *w, const char *friendid,
                            const char *presence, void *context);

    /**
     * An application-defined function that process the friend request.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      userid      [in] The user id who wants friend with self.
     * @param
     *      info        [in] The user info who wants to friend with self.
     * @param
     *      hello       [in] PIN for target user, or any application defined
     *                       content.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      true if current callback consume this event, otherwise return false.
     */
    bool (*friend_request)(Whisper *w, const char *userid,
                           const WhisperUserInfo *info,
                           const char *hello, void *context);

    /**
     * An application-defined function that process the friend response.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      userid      [in] The target user id.
     * @param
     *      status      [in] The status code of the response.
     *                       0 is success, otherwise is error.
     * @param
     *      reason      [in] The error message if status is error, or NULL
     *                       if success.
     * @param
     *      entrusted   [in] Entrusted by this friend or not.
     * @param
     *      expire      [in] The expire time, or NULL if never expire.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      true if current callback consume this event, otherwise return false.
     */
    bool (*friend_response)(Whisper *w, const char *userid,
                            int status, const char *reason,
                            bool entrusted, const char *expire,
                            void *context);

    /**
     * An application-defined function that process the new friend added
     * event.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      info        [in] The firend's information.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*friend_added)(Whisper *w, const WhisperFriendInfo *info,
                         void *context);

    /**
     * An application-defined function that process the friend removed
     * event.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      friendid    [in] The friend's user id.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*friend_removed)(Whisper *w, const char *friendid,
                           void *context);

    /**
     * An application-defined function that process the friend messages.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      from        [in] The id(userid@nodeid) from who send the message.
     * @param
     *      msg         [in] The message content.
     * @param
     *      len         [in] The message length in bytes.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      true if current callback consume this event, otherwise return false.
     */
    bool (*friend_message)(Whisper *w, const char *from,
                           const char *msg, size_t len, void *context);

    /**
     * An application-defined function that process the friend invite request.
     *
     * @param
     *      w           [in] A handle to the Whisper client instance.
     * @param
     *      from        [in] The user id from who send the invite request.
     * @param
     *      data        [in] The application defined data send from friend.
     * @param
     *      len         [in] The data length in bytes.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      true if current callback consume this event, otherwise return false.
     */
    bool (*friend_invite)(Whisper *w, const char *from,
                          const char *data, size_t len,
                          void *context);

} WhisperCallbacks;

/**
 * Set Whisper callbacks.
 *
 * @param
 *      w           [in] A handle to the Whisper client instance.
 * @param
 *      callbacks   [in] The Application defined callback functions.
 * @param
 *      context     [in] The application defined context data.
 */
WHISPER_API
void whisper_set_callbacks(Whisper *w, WhisperCallbacks *callbacks, void *context);

/******************************************************************************
 * Error handling
 *****************************************************************************/

#define WSUCCESS                                0

// Facility code
#define WF_GENERAL                              1
#define WF_SYS                                  2
#define WF_HTTP                                 3
#define WF_MQTT                                 4
#define WF_ICE                                  5


#define WERR_INVALID_ARGS                       1
#define WERR_OUT_OF_MEMORY                      2
#define WERR_BUFFER_TOO_SMALL                   3
#define WERR_BAD_PERSISTENT_DATA                4
#define WERR_INVALID_PERSISTENCE_FILE           5
#define WERR_INVALID_CONTROL_PACKET             6
#define WERR_INVALID_CREDENTIAL                 7
#define WERR_SERVER_FAILED                      8
#define WERR_ALREADY_RUN                        9
#define WERR_NOT_READY                          10
#define WERR_NOT_EXIST                          11
#define WERR_ALREADY_EXIST                      12
#define WERR_NO_MATCHED_REQUEST                 13
#define WERR_INVALID_USERID                     14
#define WERR_INVALID_NODEID                     15
#define WERR_INVALID_APPID                      16
#define WERR_INVALID_DESCRIPTOR                 17
#define WERR_WRONG_STATE                        18
#define WERR_BUSY                               19
#define WERR_LANGUAGE_BINDING                   20
#define WERR_ENCRYPT                            21



#define W_MK_ERROR(facility, code)  (0x80000000 | ((facility) << 24) | \
                    ((((code) & 0x80000000) >> 8) | ((code) & 0x7FFFFFFF)))

#define W_GENERAL_ERROR(code)       W_MK_ERROR(WF_GENERAL, code)
#define W_SYS_ERROR(code)           W_MK_ERROR(WF_SYS, code)
#define W_HTTP_ERROR(code)          W_MK_ERROR(WF_HTTP, code)
#define W_MQTT_ERROR(code)          W_MK_ERROR(WF_MQTT, code)
#define W_ICE_ERROR(code)           W_MK_ERROR(WF_ICE, code)

/**
 * Retrieves the last-error code value. The last-error code is maintained on a
 * per-instance basis. Multiple instance do not overwrite each other's
 * last-error code.
 *
 * @return
 *      The return value is the last-error code.
 */
WHISPER_API
int whisper_get_error(void);

/**
 * Clear the last-error code of a Whisper instance.
 */
WHISPER_API
void whisper_clear_error(void);

#ifdef __cplusplus
}
#endif

#endif /* __WHISPER_H_ */
