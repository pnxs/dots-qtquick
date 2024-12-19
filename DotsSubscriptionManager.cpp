#include "DotsSubscriptionManager.h"
#include <dots/dots.h>

#include "DotsDescriptorRequest.dots.h"
#include <EnumDescriptorData.dots.h>
#include <StructDescriptorData.dots.h>


DotsSubscriptionManager::DotsSubscriptionManager(boost::asio::execution_context& executionContext)
: boost::asio::execution_context::service(executionContext)
, m_transceiver(dots::global_transceiver())
{
    m_typeSubscription = m_transceiver.subscribe<dots::type::StructDescriptor>([&](auto& d) { handleNewDescriptor(d);});

    m_subscriptionStructData = m_transceiver.subscribe<StructDescriptorData>([](auto &){});
    m_subscriptionEnumData = m_transceiver.subscribe<EnumDescriptorData>([](auto &){});
    m_transceiver.publish(DotsDescriptorRequest{ .whitelist = {} });
}

dots::Subscription DotsSubscriptionManager::subscribe(const std::string& typeName, dots::Transceiver::event_handler_t<> handler)
{
    auto* td = m_transceiver.registry().findStructType(typeName);
    if (td == nullptr) {
        m_pendingHandlers.push_back({typeName, std::move(handler)});
        auto p = m_pendingHandlers.back();
        return dots::Subscription([this, p] {
            erase_if(m_pendingHandlers, [&](auto &e) { return e.first == p.first; });
        });
    } else {
        return m_transceiver.subscribe(*td, std::move(handler));
    }

}

void DotsSubscriptionManager::handleNewDescriptor(const dots::type::StructDescriptor& descriptor)
{
    // Check if a descriptor is pending for subscription
    if (!m_pendingHandlers.empty()) {
        for (auto iter = m_pendingHandlers.begin(); iter != m_pendingHandlers.end(); ++iter) {
            if (iter->first == descriptor.name()) {
                //TODO: implement unsubscribe
                m_transceiver.subscribe(descriptor, std::move(iter->second)).discard();
                iter = m_pendingHandlers.erase(iter);
            }
        }
    }

}

void DotsSubscriptionManager::shutdown() noexcept
{
    m_subscriptionEnumData.reset();
    m_subscriptionStructData.reset();
}