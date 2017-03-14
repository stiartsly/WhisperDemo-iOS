#ifndef __WHISPER_EXT_H__
#define __WHISPER_EXT_H__

#include <stdint.h>
#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Whisper ID & KEY is a key pair from NaCl. */

/**
 * Whisper binary ID length.
 */
#define WHISPER_ID_LEN                      32U

/**
 * Whisper binary key length.
 */
#define WHISPER_KEY_LEN                     32U

/**
 * The max length of string format nonce that used in encryption.
 */
#define WHISPER_MAX_NONCE_LEN               48

/**
 * Maximum length of Whisper extension name.
 */
#define WHISPER_MAX_EXTENSION_NAME          31

/**
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
 */
WHISPER_API
int whisper_register_extension(Whisper *w, const char *name, void *ext,
                               WhisperCallbacks *callbacks);

/**
 * Retrieve the extension object pointer according to given name.
 *
 * @param
 *      w           [in] A handle to whisper client instance.
 * @param
 *      name        [in] The name of new extension.
 *
 * @return
 *       The extension object pointer, or NULL if not exist.
 */
WHISPER_API
void *whisper_get_extension(Whisper *w, const char *name);

/**
 * Unregister the extension.
 *
 * @param
 *      w           [in] A handle to whisper client instance.
 * @param
 *      name        [in] The name of new extension.
 */
WHISPER_API
void whisper_unregister_extension(Whisper *w, const char *name);

/**
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
 */
WHISPER_API
int whisper_generate_key(Whisper *w, const char *friendid,
                uint8_t *key, size_t keylen);

/**
 * Set Whisper last error code.
 *
 * @param
 *      err         [in] The error code to be set.
 */
WHISPER_API
void whisper_set_error(int err);

#ifdef __cplusplus
}
#endif

#endif /* __WHISPER_EXT_H__ */
