#ifndef _SBF_SUB_PRIVATE_H_
#define _SBF_SUB_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTportStreamImpl;
struct sbfTportTopicImpl;
struct sbfTportHeaderImpl;

struct sbfSubImpl
{
    sbfTopic                   mTopic;
    u_int                      mWeight;

    int                        mDestroyed;
    int                        mRemoved;
    sbfRefCount                mRefCount;

    int                        mReady;
    int                        mQueueBypass;

    struct sbfTportImpl*       mTport;
    struct sbfTportStreamImpl* mTportStream;
    struct sbfTportTopicImpl*  mTportTopic;

    sbfQueue                   mQueue;
    sbfSubReadyCb              mReadyCb;
    sbfSubMessageCb            mMessageCb;
    void*                      mClosure;

    struct event               mEventAdd;
    struct event               mEventRemove;

    TAILQ_ENTRY (sbfSubImpl)   mEntry;
};

void sbfSub_ready (sbfSub sub);
void sbfSub_message (sbfSub sub, sbfBuffer buffer);

#endif /* _SBF_SUB_PRIVATE_H_ */
