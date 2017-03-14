#ifndef __WHISPER_PORTFORWARDING_H__
#define __WHISPER_PORTFORWARDING_H__

#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PortforwardingMode_client = 0,
    PortforwardingMode_server
} PortforwardingMode;

typedef struct WhisperService {
    const char *name;
    const char *host;
    const char *port;
} WhisperService;

typedef struct WhisperPortForwardingOptions {
    PortforwardingMode mode;

    /* Server options */
    const char *host;
    const char *port;

    /* real local host:port for listen */
    const char *listen_host;
    const char *listen_port;

    WhisperService services[];
} WhisperPortForwardingOptions;

WHISPER_API
int whisper_portforwarding_init(Whisper *w, WhisperPortForwardingOptions *config);

WHISPER_API
void whisper_portforwarding_cleanup(Whisper *w);

typedef void PortForwardingCompleteCallback(int pfd, int success, void *context);

WHISPER_API
int whisper_portforwarding_open(Whisper *w, const char *friendid,
        const char *service, const char *addr, const char *port,
        PortForwardingCompleteCallback *callback, void *context);

WHISPER_API
int whisper_portforwarding_close(Whisper *w, int pfd);

#ifdef __cplusplus
}
#endif

#endif /* __WHISPER_PORTFORWARDING_H__ */
