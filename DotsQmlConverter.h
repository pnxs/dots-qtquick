#pragma once

#include <dots/type/TypeVisitor.h>

class DotsQmlConverter: dots::type::TypeVisitor<DotsQmlConverter>
{
public:
    enum class  StackItemType {
        Obj,
        Vector
    };

    struct StackItem {
        std::string propertyName;
        StackItemType type;
        QQmlPropertyMap* obj = nullptr;
        QList<QVariant> list;
    };

    QQmlPropertyMap* convert(const dots::type::Struct& instance, QObject* parent = nullptr)
    {
        m_parent = parent;
        visit(instance, instance._properties());
        //LOG_INFO_S("Stacksize after conversion: " << m_stack.size());
        return m_stack.top().obj;
    }

    std::stack<StackItem> m_stack;
    QObject* m_parent = nullptr;

protected:

    using visitor_base_t = dots::type::TypeVisitor<DotsQmlConverter>;
    using visitor_base_t::visit;
    friend visitor_base_t;

    void addProperty(const QVariant& variant) {
        auto& top = m_stack.top();

        if (top.type == StackItemType::Vector) {
            //LOG_INFO_S("  add to vector")
            top.list.push_back(QVariant(variant));
        } else {
            //LOG_INFO_S("  add to obj")
            top.obj->insert(top.propertyName.c_str(), variant);
        }
    }

    template <typename T>
    bool visitStructBeginDerived(const T& instance, dots::property_set_t&/* includedProperties*/)
    {
        //LOG_INFO_S("begin struct " << instance._descriptor().name())

        m_stack.push(StackItem {
            .type = StackItemType::Obj,
            .obj = new QQmlPropertyMap(m_parent)
        });

        return true;
    }

    template <typename T>
    void visitStructEndDerived(const T& instance, const dots::property_set_t&/* includedProperties*/)
    {
        //LOG_INFO_S("end struct " << instance._descriptor().name())
        auto& top = m_stack.top();
        if (top.type != StackItemType::Obj) throw std::runtime_error("Wrong type for obj");
        auto* obj = m_stack.top().obj;

        if (m_stack.size() > 1){
            m_stack.pop();
            addProperty(QVariant::fromValue(obj));
        }
    }

    template <typename T>
    bool visitPropertyBeginDerived(const T& property, bool/* first*/)
    {
        //LOG_INFO_S("begin property " << property.descriptor().name() << " valid " << property.isValid() )
        m_stack.top().propertyName = property.descriptor().name();

        if (property.isValid())
        {
            return true;
        }
        else
        {
            //LOG_INFO_S("  null");
            addProperty({});
            return false;
        }
    }

    template <typename T>
    bool visitVectorBeginDerived(const dots::vector_t<T>&/* vector*/, const dots::type::Descriptor<dots::vector_t<T>>&/* descriptor*/)
    {
        //LOG_INFO_S("begin vector ")

        m_stack.push(StackItem {
            .type = StackItemType::Vector,
            .list = {}
        });

        return true;
    }

    template <typename T>
    void visitVectorEndDerived(const dots::vector_t<T>&/* vector*/, const dots::type::Descriptor<dots::vector_t<T>>&/* descriptor*/)
    {
        //LOG_INFO_S("end vector ")
        auto& top = m_stack.top();
        if (top.type != StackItemType::Vector) throw std::runtime_error("Wrong type for vector");

        QVariant value = QVariant::fromValue(top.list);
        m_stack.pop();

        addProperty(value);
    }

    template <typename T>
    void visitEnumDerived(const T& value, const dots::type::EnumDescriptor& descriptor)
    {
        addProperty(QVariant::fromValue(descriptor.enumeratorFromValue(value).name()));
    }

    template <typename T>
    void visitFundamentalTypeDerived(const T& value, const dots::type::Descriptor<T>& descriptor)
    {
        //LOG_INFO_S("FundamentalType: " << m_stack.top().propertyName);

        if constexpr(std::is_arithmetic_v<T>)
        {
            addProperty( QVariant::fromValue(value));
        }
        else if constexpr(std::is_same_v<T, dots::property_set_t>)
        {
            //obj->insert(descriptor.name(), QVariant::fromValue(value));
        }
        else if constexpr (std::is_same_v<T, dots::timepoint_t>)
        {
            addProperty( QVariant::fromValue(QString::fromStdString(value.toString())));
        }
        else if constexpr (std::is_same_v<T, dots::steady_timepoint_t>)
        {
            addProperty( QVariant::fromValue(value.duration().toFractionalSeconds()));
        }
        else if constexpr (std::is_same_v<T, dots::duration_t>)
        {
            addProperty( QVariant::fromValue(value.toFractionalSeconds()));
        }
        else if constexpr (std::is_same_v<T, dots::uuid_t>)
        {
            addProperty( QVariant::fromValue(QString::fromStdString(value.toString())));
        }
        else if constexpr (std::is_same_v<T, dots::string_t>)
        {
            addProperty( QVariant::fromValue(QString::fromStdString(value)));
        }
        else
        {
            static_assert(!std::is_same_v<T, T>, "type not supported");
        }

    }

};