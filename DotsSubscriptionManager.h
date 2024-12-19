#pragma once

#include <dots/dots.h>
#include <dots/io/Io.h>
#include <list>

struct DotsSubscriptionManager: boost::asio::execution_context::service
{
    using key_type = DotsSubscriptionManager;

    explicit DotsSubscriptionManager(boost::asio::execution_context& executionContext);

    DotsSubscriptionManager(const DotsSubscriptionManager& other) = delete;
    DotsSubscriptionManager(DotsSubscriptionManager&& other) noexcept(false) = delete;
    ~DotsSubscriptionManager() = default;

    DotsSubscriptionManager& operator = (const DotsSubscriptionManager& rhs) = delete;
    DotsSubscriptionManager& operator = (DotsSubscriptionManager&& rhs) noexcept(false) = delete;

    dots::Subscription subscribe(const std::string& typeName, dots::Transceiver::event_handler_t<> handler);

  private:

    dots::Transceiver& m_transceiver;

    std::list<std::pair<std::string, dots::Transceiver::event_handler_t<>>> m_pendingHandlers;

    void handleNewDescriptor(const dots::type::StructDescriptor& descriptor);
    void shutdown() noexcept override;

    std::optional<dots::Subscription> m_subscriptionStructData;
    std::optional<dots::Subscription> m_subscriptionEnumData;
    std::optional<dots::Subscription> m_typeSubscription;


};
