
#include "DotsSubscription.h"

#include <DotsSubscriptionManager.h>
#include <QQmlPropertyMap>
#include <StructDescriptorData.dots.h>
#include <vector>
#include <dots/io/Io.h>

#include "DotsQmlConverter.h"

DotsSubscription::DotsSubscription(QQuickItem *parent)
: QQuickItem(parent)
{
    LOG_INFO_S("Subscribe " << name().toStdString());
}

void DotsSubscription::handleDotsEvent(const dots::Event<>& e)
{
    //LOG_INFO_S("Handle event" << dots::to_string(e.updated()));
    auto* obj = DotsQmlConverter().convert(e.updated(), this);

    eventReceived(obj);
}

void DotsSubscription::subscribe(const std::string& typeName)
{
    auto& subscriptionManager = dots::io::global_service<DotsSubscriptionManager>();

    m_subscription = subscriptionManager.subscribe(typeName, [this](auto& e) { handleDotsEvent(e); } );
}
