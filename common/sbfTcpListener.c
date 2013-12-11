#include "sbfTcpConnection.h"
#include "sbfTcpConnectionPrivate.h"
#include "sbfTcpListener.h"
#include "sbfTcpListenerPrivate.h"

static void
sbfTcpListenerReadyQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpListener tl = closure;

    if (!tl->mDestroyed)
        tl->mReadyCb (tl, tl->mClosure);

    if (sbfRefCount_decrement (&tl->mRefCount))
        free (tl);
}

static void
sbfTcpListenerAcceptQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;
    sbfTcpListener   tl = tc->mListener;

    if (!tl->mDestroyed)
        tl->mAcceptCb (tl, tc, tl->mClosure);

    if (sbfRefCount_decrement (&tl->mRefCount))
        free (tl);
}

static void
sbfTcpListenerEventCb (struct evconnlistener* listener,
                       evutil_socket_t socket,
                       struct sockaddr* address,
                       int length,
                       void* closure)
{
    sbfTcpListener   tl = closure;
    sbfTcpConnection tc;

    tc = sbfTcpConnection_wrap (socket);
    tc->mListener = tl;

    /*
     * There is no need to add a reference to the connection since nobody else
     * has a pointer to it yet.
     */
    sbfRefCount_increment (&tl->mRefCount);
    sbfQueue_enqueue (tl->mQueue, sbfTcpListenerAcceptQueueCb, tc);
}

sbfTcpListener
sbfTcpListener_create (struct sbfMwThreadImpl* thread,
                       struct sbfQueueImpl* queue,
                       uint16_t port,
                       sbfTcpListenerReadyCb readyCb,
                       sbfTcpListenerAcceptCb acceptCb,
                       void* closure)
{
    sbfTcpListener     tl;
    struct sockaddr_in sin;

    tl = xcalloc (1, sizeof *tl);
    tl->mThread = thread;
    tl->mQueue = queue;

    tl->mReadyCb = readyCb;
    tl->mAcceptCb = acceptCb;
    tl->mClosure = closure;

    tl->mDestroyed = 0;
    sbfRefCount_init (&tl->mRefCount, 1);

    sbfLog_debug ("creating %p: port %hu", tl, port);

    memset (&sin, 0, sizeof sin);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons (port);
    tl->mListener = evconnlistener_new_bind (sbfMw_getThreadEventBase (thread),
                                             sbfTcpListenerEventCb,
                                             tl,
                                             LEV_OPT_THREADSAFE|
                                             LEV_OPT_CLOSE_ON_FREE|
                                             LEV_OPT_CLOSE_ON_EXEC,
                                             -1,
                                             (struct sockaddr*)&sin,
                                             sizeof sin);
    if (tl->mListener == NULL)
        goto fail;

    sbfRefCount_increment (&tl->mRefCount);
    sbfQueue_enqueue (queue, sbfTcpListenerReadyQueueCb, tl);

    return tl;

fail:
    sbfTcpListener_destroy (tl);
    return NULL;
}

void
sbfTcpListener_destroy (sbfTcpListener tl)
{
    sbfLog_debug ("destroying %p", tl);

    tl->mDestroyed = 1;
    if (tl->mListener != NULL)
        evconnlistener_free (tl->mListener);

    if (sbfRefCount_decrement (&tl->mRefCount))
        free (tl);
}