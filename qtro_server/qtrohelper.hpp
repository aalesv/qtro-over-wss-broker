// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#ifndef QTROHELPER_HPP
#define QTROHELPER_HPP

#include <QRemoteObjectPendingCallWatcher>

namespace qtrohelper
{
//Convert QVariant to scalar templates
template <typename T>
T qvariant_to_scalar(QVariant v);

template <>
inline long qvariant_to_scalar<long>(QVariant v)
{
    return v.toLongLong();
}

template <>
inline int qvariant_to_scalar<int>(QVariant v)
{
    return v.toInt();
}

template <>
inline bool qvariant_to_scalar<bool>(QVariant v)
{
    return v.toBool();
}

template <>
inline QString qvariant_to_scalar<QString>(QVariant v)
{
    return v.toString();
}

/*
 * Do syncronous call of remote object method
 *
 * Note that QRemoteObjectPendingReply is template class.
 * This template first type argument must be
 * class QRemoteObjectPendingReply<return_type>
 * Second type argument is returning type of remote object method
 * For example source class implementation is:
 *
 * class QtroRemote : public QtroRemoteSimpleSource
 *   {
 *       Q_OBJECT
 *   public:
 *       long someFunc(QString s) override;
 *   };
 *
 *   On replica:
 *
 *  QScopedPointer<QtroRemoteReplica> qtro_remote(
 *       node.acquire<QtroRemoteReplica>("source_name")
 *   );
 *
 *   Then type of 'qtro_remote->someFunc()' will be deduced as
 *   QRemoteObjectPendingReply<long>.
 *   Note that 'qtro_remote->someFunc()' return type is 'long'.
 *   And 'slot_sync(qtro_remote->someFunc("text"))' call deduces to
 *
 *   slot_sync<QRemoteObjectPendingReply<long>, long>(qtro_remote->someFunc("text"))
*/
template <template <typename> typename QRemoteObjectPendingReply, typename RET_TYPE>
RET_TYPE slot_sync(QRemoteObjectPendingReply<RET_TYPE> SLOT)
{
    QVariant r;
    QScopedPointer<QRemoteObjectPendingCallWatcher>
        watcher{new QRemoteObjectPendingCallWatcher(SLOT)};
    QObject::connect(watcher.data(), &QRemoteObjectPendingCallWatcher::finished,
        watcher.data(), [&](QRemoteObjectPendingCallWatcher* watch)
        {
            r = watch->returnValue();
        }, Qt::DirectConnection);
    watcher->waitForFinished();
    return qvariant_to_scalar<RET_TYPE>(r);
}

} //namespace qtrohelper

#endif // QTROHELPER_HPP
