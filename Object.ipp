/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Interpreter Object interface
 */

inline kF::Var kF::Object::getVar(const HashedName name) const
{
    if (auto data = getMetaType().findData(name); !data)
        throw std::logic_error("Object::get: Invalid hashed name '" + std::to_string(name) + '\'');
    else if (auto res = data.get(getTypeHandle()); res)
        return res;
    throw std::logic_error("Object::get: Empty property '" + std::to_string(name) + '\'');
}

inline void kF::Object::setVar(const HashedName name, const Var &var)
{
    if (auto data = getMetaType().findData(name); !data)
        throw std::logic_error("Object::set: Invalid hashed name '" + std::to_string(name) + '\'');
    else if (!data.set(getTypeHandle(), const_cast<Var &>(var)))
        throw std::logic_error("Object::set: Argument type doesn't match type of hashed name '" + std::to_string(name) + '\'');
}

inline void kF::Object::setVar(const Meta::Data metaData, const Var &var)
{
    if (!metaData.set(getTypeHandle(), const_cast<Var &>(var)))
        throw std::logic_error("Object::set: Argument type doesn't match type");
}

template<typename ...Args>
inline kF::Var kF::Object::invoke(const HashedName name, Args &&...args)
{
    auto handle = getTypeHandle();
    auto fct = handle.type().findFunction(name);

    kFAssert(fct,
        throw std::logic_error("Object::invoke: Invalid hashed name '" + std::to_string(name) + '\''));
    return fct.invoke(handle, std::forward<Args>(args)...);
}

template<typename ...Args>
inline kF::Var kF::Object::invoke(const Meta::Function metaFunc, Args &&...args)
{
    return metaFunc.invoke(getTypeHandle(), std::forward<Args>(args)...);
}

template<typename Receiver, typename Slot>
inline kF::Object::ConnectionHandle kF::Object::connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, Slot &&slot)
    noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
{
    using Decomposer = Meta::Internal::FunctionDecomposerHelper<Slot>;

    if constexpr (!std::is_same_v<Receiver, void> && Decomposer::IsMember) {
        static_assert(std::is_same_v<Receiver, typename Decomposer::ClassType>, "You tried to connect receiver to a non-receiver member function");
        static_assert(std::is_const_v<Receiver> == Decomposer::IsConst, "You tried to connect a volatile member slot with a constant receiver");
    }

    auto handle { slotTable.insert<Receiver>(receiver, std::forward<Slot>(slot)) };

    handle = slotTable.insert<Receiver>(receiver, std::forward<Slot>(slot));
    ensureConnectionTable();
    _connectionTable->registeredSlots.emplace_back(signal, handle);
    if constexpr (std::is_base_of_v<Object, Receiver>) {
        if (this != receiver) {
            reinterpret_cast<Object*>(const_cast<void *>(receiver))->ensureConnectionTable();
            reinterpret_cast<Object*>(const_cast<void *>(receiver))->_connectionTable->ownedSlots.emplace_back(handle);
        }
    }
    return handle;
}

template<typename ...Args>
inline void kF::Object::emit(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args)
{
    kFAssert(signal,
        throw std::logic_error("Object::emit: Unknown signal"));
    ensureConnectionTable();
    Var arguments[sizeof...(Args)] { Var::Assign(std::forward<Args>(args))... };
    const auto it = std::remove_if(_connectionTable->registeredSlots.begin(), _connectionTable->registeredSlots.end(),
        [&slotTable, signal, &arguments](auto &pair) -> bool {
            if (signal != pair.first) [[likely]]
                return false;
            else [[unlikely]]
                return !slotTable.invoke(pair.second, arguments).operator bool();
        }
    );
    if (it != _connectionTable->registeredSlots.end()) [[unlikely]]
        _connectionTable->registeredSlots.erase(it, _connectionTable->registeredSlots.end());
}

inline void kF::Object::ensureConnectionTable(void) noexcept_ndebug
{
    if (!_connectionTable) [[unlikely]]
        _connectionTable = std::make_unique<ConnectionTable>();
}