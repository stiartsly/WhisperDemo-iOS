#ifndef __WHISPER_EXT_H__
#define __WHISPER_EXT_H__

#include <stdint.h>
#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Whisper ID & KEY is a key pair from NaCl. */

/**
 * \~English
 * Whisper binary ID length.
 *
 * \~Chinese
 * Whisper二进制ID长度。
 */
#define WHISPER_ID_LEN                      32U

/**
 * \~English
 * Whisper binary key length.
 *
 * \~Chinese
 * Whisper二进制密钥长度。
 */
#define WHISPER_KEY_LEN                     32U

/**
 * \~English
 * The max length of string format nonce that used in encryption.
 *
 * \~Chinese
 * 在加密过程中使用的字符串格式的nonce的最大长度。
 */
#define WHISPER_MAX_NONCE_LEN               48

/**
 * \~English
 * Maximum length of Whisper extension name.
 *
 * \~Chinese
 * Whisper扩展名的最大长度。
 */
#define WHISPER_MAX_EXTENSION_NAME          31

/**
 * \~English
 * Regisiter a new extension to Whisper instance.
 *
 * @param
 *      w           [in] A handle to whisper client instance.
 * @param
 *      name        [in] The name of new extension.
 * @param
 *      ext         [in] The user defined extension object pointer.
 * @param
 *      callbacks   [in] The extension interested callbacks.
 *                       The callback context is the extension object pointer.
 *
 * @return
 *      0 if the extension register successfully. Otherwise, return -1, 
 *      and a specific error code can be retrieved by calling 
 *      whisper_get_error().
 *
 * \~Chinese
 * 向Whisper对象注册一个新的扩展。
 *
 * @param
 *      w           [输入] Whisper客户端对象的句柄。
 * @param
 *      name        [输入] 新扩展的名称。
 * @param
 *      ext         [输入] 用户定义的扩展对象地址。
 * @param
 *      callbacks   [输入] Whisper扩展感兴趣的回调函数。
 *                         回调环境中的信息是扩展对象的地址。
 *
 * @return
 *      如果注册成功，返回0。否则，返回-1，并可以通过调用whisper_get_error()
 *      获取错误码。
 */
WHISPER_API
int whisper_register_extension(Whisper *w, const char *name, void *ext,
                               WhisperCallbacks *callbacks);

/**
 * \~English
 * Retrieve the extension object pointer according to given name.
 *
 * @param
 *      w           [in] A handle to whisper client instance.
 * @param
 *      name        [in] The name of new extension.
 *
 * @return
 *      The extension object pointer, or NULL if not exist.
 *
 * \~Chinese
 * 根据指定的名称获取扩展对象的地址。
 *
 * @param
 *      w           [输入] Whisper客户端对象的句柄。
 * @param
 *      name        [输入] 新扩展对象的名称。
 *
 * @return
 *      扩展对象的地址，如果不存在相关的对象，则返回空。
 */
WHISPER_API
void *whisper_get_extension(Whisper *w, const char *name);

/**
 * \~English
 * Unregister the extension.
 *
 * @param
 *      w           [in] A handle to whisper client instance.
 * @param
 *      name        [in] The name of new extension.
 *
 * \~Chinese
 * 注销指定名称的扩展。
 *
 * @param
 *      w           [输入] Whisper客户端对象的句柄。
 * @param
 *      name        [输入] 新扩展对象的名称。
 */
WHISPER_API
void whisper_unregister_extension(Whisper *w, const char *name);

/**
 * \~English
 * Generate a symmetric session encrypt key. This key can be used to 
 * encrypt any data send to target friend.
 *
 * @param
 *      w           [in] A handle to whisper client instance.
 * @param
 *      friendid    [in] The target feiend's ID.
 * @param
 *      key         [in, out] The buffer to receive new generated key.
 *                            The buffer size must >= WHISPER_KEY_LEN.
 * @param
 *      keylen      [in] The key buffer size.
 *
 * @return
 *      0 if success or -1 on error,  and a specific error code can be
 *      retrieved by calling whisper_get_error().
 *
 * \~Chinese
 * 生成对称的会话加密密钥。此密钥可以用来给任何要发给朋友的数据加密。
 *
 * @param
 *      w           [输入] Whisper客户端对象的句柄。
 * @param
 *      friendid    [输入] 朋友ID。
 * @param
 *      key         [输入, 输出] 保存新生成密钥的存储空间。
 *                               该存储空间的长度必须不小于WHISPER_KEY_LEN字节。
 * @param
 *      keylen      [输入] 保存新生成密钥的存储空间的长度。
 *
 * @return
 *      如果成功，返回0。如果出错，返回-1，并可以通过whisper_get_error()获取错误码。
 */
WHISPER_API
int whisper_generate_key(Whisper *w, const char *friendid,
                uint8_t *key, size_t keylen);

/**
 * \~English
 * Set Whisper last error code.
 *
 * @param
 *      err         [in] The error code to be set.
 *
 * \~Chinese
 * 设置Whisper最后一次的错误码。
 *
 * @param
 *      err         [输入] 要设置的错误码。
 */
WHISPER_API
void whisper_set_error(int err);

#ifdef __cplusplus
}
#endif

#endif /* __WHISPER_EXT_H__ */
