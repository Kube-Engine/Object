/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Interpreter Object interface
 */

inline kF::Object::~Object(void) noexcept
{
    if (!_connectionTable)
        return;
    auto &slotTable = *_connectionTable->slotTable;
    for (auto &owned : _connectionTable->ownedSlots)
        slotTable.remove(owned);
    for (auto &registered : _connectionTable->registeredSlots)
        slotTable.remove(registered.second);
}

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

template<kF::Object::IsEnsureConnection EnsureConnectionTable>
inline kF::Meta::SlotTable &kF::Object::getDefaultSlotTable(void) noexcept_ndebug
{
    if constexpr (EnsureConnectionTable == IsEnsureConnection::Yes)
        ensureConnectionTable();
    return *_connectionTable->slotTable;
}

template<kF::Object::IsEnsureConnection EnsureConnectionTable>
inline void kF::Object::setDefaultSlotTable(Meta::SlotTable &slotTable) noexcept_ndebug
{
    if constexpr (EnsureConnectionTable == IsEnsureConnection::Yes)
        ensureConnectionTable();
    _connectionTable->slotTable = &slotTable;
}

inline void kF::Object::ensureConnectionTable(void) noexcept_ndebug
{
    if (!_connectionTable) [[unlikely]]
        _connectionTable = std::make_unique<ConnectionTable>();
}

template<kF::Object::IsEnsureConnection EnsureConnectionTable, typename Receiver, typename Slot>
inline kF::Object::ConnectionHandle kF::Object::connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, Slot &&slot)
    noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
{
    using Decomposer = Meta::Internal::FunctionDecomposerHelper<Slot>;

    if constexpr (!std::is_same_v<Receiver, void> && Decomposer::IsMember && !Decomposer::IsFunctor) {
        static_assert(std::is_same_v<Receiver, typename Decomposer::ClassType>, "You tried to connect receiver to a non-receiver member function");
        static_assert(std::is_const_v<Receiver> == Decomposer::IsConst, "You tried to connect a volatile member slot with a constant receiver");
    }

    kFAssert(signal.operator bool(),
        throw std::logic_error("Object::connect: Can't establish connection to invalid signal"));

    auto handle { slotTable.insert<Receiver>(receiver, std::forward<Slot>(slot)) };

    handle = slotTable.insert<Receiver>(receiver, std::forward<Slot>(slot));
    if constexpr (EnsureConnectionTable == IsEnsureConnection::Yes)
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

inline void kF::Object::disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const ConnectionHandle handle)
{
    auto it = _connectionTable->registeredSlots.begin();
    const auto end = _connectionTable->registeredSlots.end();

    while (it != end) {
        if (it->first != signal || it->second != handle) [[likely]] {
            ++it;
            continue;
        }
        _connectionTable->registeredSlots.erase(it);
        slotTable.remove(handle);
        return;
    }
    throw std::logic_error("Object::disconnect: Slot not found");
}

template<typename Receiver>
inline void kF::Object::disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, const ConnectionHandle handle)
{
    disconnect(slotTable, signal, handle);
    if constexpr (std::is_base_of_v<Object, Receiver>) {
        auto it = reinterpret_cast<std::remove_cvref_t<Receiver *>>(receiver)->_connectionTable.ownedSlots.begin();
        const auto end = reinterpret_cast<std::remove_cvref_t<Receiver *>>(receiver)->_connectionTable.ownedSlots.end();

        while (it != end) {
            if (*it != handle) [[likely]] {
                ++it;
                continue;
            }
            _connectionTable->registeredSlots.erase(it);
            return;
        }
        throw std::logic_error("Object::disconnect: Slot not found in receiver");
    }
}

template<kF::Object::IsEnsureConnection EnsureConnectionTable, typename ...Args>
inline void kF::Object::emit(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args)
{
    kFAssert(signal,
        throw std::logic_error("Object::emit: Unknown signal"));
    if constexpr (EnsureConnectionTable == IsEnsureConnection::Yes)
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