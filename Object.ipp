/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Object
 */

inline kF::Object::~Object(void) noexcept
{
    if (_cache) {
        auto &slotTable = *_cache->slotTable;
        for (auto &owned : _cache->ownedSlots)
            slotTable.remove(owned);
        for (auto &registered : _cache->registeredSlots)
            slotTable.remove(registered.second);
    }
    if (_cache->tree)
        unsetParent();
}

inline const kF::Object *kF::Object::parent(void) const noexcept
{
    if (_cache && _cache->parentIndex != ObjectUtils::Tree::NullIndex) [[likely]]
        return parentUnsafe();
    else [[unlikely]]
        return nullptr;
}

inline void kF::Object::parent(Object * const parentPtr) noexcept_ndebug
{
    if (parentPtr) [[likely]]
        parent(*parentPtr);
    else if (hasParent())
        unsetParent();
}

inline void kF::Object::parent(Object &parentRef) noexcept_ndebug
{
    HashedName id = 0u;

    if (_cache && _cache->tree) [[unlikely]]
        id = _cache->tree->get(_cache->index).id;
    parent(parentRef, id);
}

inline void kF::Object::parent(Object &parentRef, const HashedName id) noexcept_ndebug
{
    ensureCache();
    kFAssert(parentRef._cache && parentRef._cache->tree,
        throw std::logic_error("Object::setParent: Parent object is not in a tree"));
    // Check if the object is already in a tree
    if (_cache->parentIndex != ObjectUtils::Tree::NullIndex) [[unlikely]] {
        const auto oldParent = parentUnsafe();
        // Check if the new parent object is in the same tree
        if (_cache->tree == parentRef._cache->tree) [[likely]] {
            _cache->parentIndex = parentRef._cache->index;
            _cache->tree->setParent(_cache->index, _cache->parentIndex);
            if (oldParent) [[likely]] {
                oldParent->onChildRemoved(*this);
                oldParent->childrenCountChanged();
            }
            parentRef.onChildAdded(*this);
            emit parentRef.childrenCountChanged();
            onParentChanged(&parentRef);
            return;
        // Else remove the object from its tree
        } else [[unlikely]] {
            _cache->tree->remove(_cache->index);
            if (oldParent) [[likely]] {
                oldParent->onChildRemoved(*this);
                emit oldParent->childrenCountChanged();
            }
        }
    }
    _cache->tree = parentRef._cache->tree;
    _cache->parentIndex = parentRef._cache->index;
    _cache->index = _cache->tree->add(_cache->parentIndex, this, id, getObjectFlags());
    parentRef.onChildAdded(*this);
    emit parentRef.childrenCountChanged();
    onParentChanged(&parentRef);
    emit parentChanged();
}

inline void kF::Object::parent(ObjectUtils::Tree &tree, ObjectUtils::Tree::Index parentIndex, const HashedName id) noexcept
{
    ensureCache();
    // Check if the object is already in a tree
    if (_cache->parentIndex != ObjectUtils::Tree::NullIndex) [[unlikely]] {
        const auto oldParent = parentUnsafe();
        // Check if the new parent object is in the same tree
        if (_cache->tree == &tree) [[likely]] {
            _cache->parentIndex = parentIndex;
            _cache->tree->setParent(_cache->index, _cache->parentIndex);
            const auto parentPtr = parentUnsafe();
            if (oldParent) [[likely]] {
                oldParent->onChildRemoved(*this);
                emit oldParent->childrenCountChanged();
            }
            if (parentPtr) [[likely]] {
                parentPtr->onChildAdded(*this);
                emit oldParent->childrenCountChanged();
            }
            onParentChanged(parentPtr);
            emit parentChanged();
            return;
        // Else remove the object from its tree
        } else [[unlikely]] {
            _cache->tree->remove(_cache->index);
            if (oldParent) [[likely]] {
                oldParent->onChildRemoved(*this);
                emit oldParent->childrenCountChanged();
            }
        }
    }
    _cache->tree = &tree;
    _cache->parentIndex = parentIndex;
    _cache->index = tree.add(_cache->parentIndex, this, id, getObjectFlags());
    const auto parentPtr = _cache->tree->get(_cache->parentIndex).object;
    if (parentPtr) [[likely]] {
        parentPtr->onChildAdded(*this);
        emit parentPtr->childrenCountChanged();
    }
    onParentChanged(parentPtr);
    emit parentChanged();
}

inline void kF::Object::unsetParent(void) noexcept_ndebug
{
    kFAssert(hasParent(),
        throw std::logic_error("Object::unsetParent: Object has no parent"));
    const auto oldParent = parentUnsafe();
    _cache->tree->remove(_cache->index);
    _cache->tree = nullptr;
    _cache->index = ObjectUtils::Tree::NullIndex;
    _cache->parentIndex = ObjectUtils::Tree::NullIndex;
    if (oldParent) [[likely]] {
        oldParent->onChildRemoved(*this);
        emit oldParent->childrenCountChanged();
    }
    onParentChanged(nullptr);
    emit parentChanged();
}

inline kF::HashedName kF::Object::id(void) const noexcept
{
    if (_cache && _cache->index != ObjectUtils::Tree::NullIndex) [[likely]]
        return _cache->tree->get(_cache->index).id;
    else
        return 0u;
}

inline void kF::Object::id(const HashedName id) noexcept_ndebug
{
    kFAssert(_cache->index != ObjectUtils::Tree::NullIndex,
        throw std::logic_error("Object::id: To set an object's id, the object must live inside an ObjectUtils::Tree"));
    _cache->tree->get(_cache->index).id = id;
}

inline kF::Object::ObjectIndex kF::Object::childrenCount(void) const noexcept
{
    if (_cache && _cache->tree) [[likely]]
        return childrenCountUnsafe();
    else [[unlikely]]
        return 0u;
}

inline kF::Object *kF::Object::find(const HashedName id) const noexcept
{
    if (!_cache || !_cache->tree) [[unlikely]]
        return nullptr;
    const auto index = _cache->tree->findInScope(id, _cache->index);
    if (index != ObjectUtils::Tree::NullIndex) [[likely]]
        return _cache->tree->get(index).object;
    else [[unlikely]]
        return nullptr;
}

inline kF::Object *kF::Object::findGlobal(const HashedName id) const noexcept
{
    if (!_cache || !_cache->tree) [[unlikely]]
        return nullptr;
    const auto index = _cache->tree->find(id);
    if (index != ObjectUtils::Tree::NullIndex) [[likely]]
        return _cache->tree->get(index).object;
    else [[unlikely]]
        return nullptr;
}

inline kF::Var kF::Object::getVar(const HashedName name) const
{
    if (auto data = getMetaType().findData(name); !data)
        throw std::logic_error("Object::get: Invalid hashed name '" + std::to_string(name) + '\'');
    else if (auto res = data.get(this); res)
        return res;
    throw std::logic_error("Object::get: Empty property '" + std::to_string(name) + '\'');
}

inline void kF::Object::setVar(const HashedName name, const Var &var)
{
    if (auto data = getMetaType().findData(name); !data)
        throw std::logic_error("Object::set: Invalid hashed name '" + std::to_string(name) + '\'');
    else if (!data.set(this, const_cast<Var &>(var)))
        throw std::logic_error("Object::set: Argument type doesn't match type of hashed name '" + std::to_string(name) + '\'');
}

inline void kF::Object::setVar(const Meta::Data metaData, const Var &var)
{
    if (!metaData.set(this, const_cast<Var &>(var)))
        throw std::logic_error("Object::set: Argument type doesn't match type");
}

template<typename ...Args>
inline kF::Var kF::Object::invoke(const HashedName name, Args &&...args)
{
    auto fct = getMetaType().findFunction(name);

    kFAssert(fct,
        throw std::logic_error("Object::invoke: Invalid hashed name '" + std::to_string(name) + '\''));
    return fct.invoke(this, std::forward<Args>(args)...);
}

template<typename ...Args>
inline kF::Var kF::Object::invoke(const Meta::Function metaFunc, Args &&...args)
{
    return metaFunc.invoke(this, std::forward<Args>(args)...);
}

template<kF::Object::IsEnsureCache EnsureCache>
inline kF::Meta::SlotTable &kF::Object::getDefaultSlotTable(void) noexcept_ndebug
{
    if constexpr (EnsureCache == IsEnsureCache::Yes)
        ensureCache();
    return *_cache->slotTable;
}

template<kF::Object::IsEnsureCache EnsureCache>
inline void kF::Object::setDefaultSlotTable(Meta::SlotTable &slotTable) noexcept_ndebug
{
    if constexpr (EnsureCache == IsEnsureCache::Yes)
        ensureCache();
    _cache->slotTable = &slotTable;
}

inline void kF::Object::ensureCache(void) noexcept_ndebug
{
    if (!_cache) [[unlikely]]
        _cache = std::make_unique<Cache>();
}

template<kF::Object::IsEnsureCache EnsureCache, typename Receiver, typename Slot>
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
    if constexpr (EnsureCache == IsEnsureCache::Yes)
        ensureCache();
    _cache->registeredSlots.push(signal, handle);
    if constexpr (std::is_base_of_v<Object, Receiver>) {
        if (this != receiver) {
            reinterpret_cast<Object*>(const_cast<void *>(receiver))->ensureCache();
            reinterpret_cast<Object*>(const_cast<void *>(receiver))->_cache->ownedSlots.push(handle);
        }
    }
    return handle;
}

inline void kF::Object::disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const ConnectionHandle handle)
{
    auto it = _cache->registeredSlots.begin();
    const auto end = _cache->registeredSlots.end();

    while (it != end) {
        if (it->first != signal || it->second != handle) [[likely]] {
            ++it;
            continue;
        }
        _cache->registeredSlots.erase(it);
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
        auto it = reinterpret_cast<std::remove_cvref_t<Receiver *>>(receiver)->_cache.ownedSlots.begin();
        const auto end = reinterpret_cast<std::remove_cvref_t<Receiver *>>(receiver)->_cache.ownedSlots.end();

        while (it != end) {
            if (*it != handle) [[likely]] {
                ++it;
                continue;
            }
            _cache->registeredSlots.erase(it);
            return;
        }
        throw std::logic_error("Object::disconnect: Slot not found in receiver");
    }
}

template<kF::Object::IsEnsureCache EnsureCache, typename ...Args>
inline void kF::Object::emitSignal(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args)
{
    kFAssert(signal,
        throw std::logic_error("Object::emitSignal: Unknown signal"));
    if constexpr (EnsureCache == IsEnsureCache::Yes)
        ensureCache();
    Var arguments[sizeof...(Args)] { Var::Assign(std::forward<Args>(args))... };
    const auto it = std::remove_if(_cache->registeredSlots.begin(), _cache->registeredSlots.end(),
        [&slotTable, signal, &arguments](auto &pair) -> bool {
            if (signal != pair.first) [[likely]]
                return false;
            else [[unlikely]]
                return !slotTable.invoke(pair.second, arguments).operator bool();
        }
    );
    if (it != _cache->registeredSlots.end()) [[unlikely]]
        _cache->registeredSlots.erase(it, _cache->registeredSlots.end());
}