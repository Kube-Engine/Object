/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Object
 */

inline kF::Object::~Object(void) noexcept
{
    if (!_cache)
        return;
    disconnect();
    if (_cache->tree)
        removeFromTree();
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
        removeFromTree();
}

inline void kF::Object::parent(Object &parentRef) noexcept_ndebug
{
    HashedName id = 0u;

    if (isInTree()) [[unlikely]]
        id = _cache->tree->get(_cache->index).id;
    parent(parentRef, id);
}

inline void kF::Object::parent(Object &parentRef, const HashedName id) noexcept_ndebug
{
    ensureObjectCache();
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
    ensureObjectCache();
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

inline void kF::Object::removeFromTree(void) noexcept_ndebug
{
    kFAssert(isInTree(),
        throw std::logic_error("Object::removeFromTree: Object is not in a tree"));
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
        return idUnsafe();
    else [[unlikely]]
        return 0u;
}

inline kF::HashedName kF::Object::idUnsafe(void) const noexcept
{
    return _cache->tree->get(_cache->index).id;
}

inline void kF::Object::id(const HashedName id) noexcept_ndebug
{
    kFAssert(_cache->index != ObjectUtils::Tree::NullIndex,
        throw std::logic_error("Object::id: To set an object's id, the object must live inside an ObjectUtils::Tree"));
    _cache->tree->get(_cache->index).id = id;
}

inline bool kF::Object::enabled(void) const noexcept
{
    if (_cache && _cache->index != ObjectUtils::Tree::NullIndex) [[likely]]
        return enabledUnsafe();
    else [[unlikely]]
        return false;
}

inline bool kF::Object::enabledUnsafe(void) const noexcept
{
    return _cache->tree->get(_cache->index).enabled;
}

inline void kF::Object::enabled(const bool state) noexcept_ndebug
{
    kFAssert(_cache->index != ObjectUtils::Tree::NullIndex,
        throw std::logic_error("Object::enabled: To set an object's enabled state, the object must live inside an ObjectUtils::Tree"));
    _cache->tree->get(_cache->index).enabled = state;
}

inline bool kF::Object::visible(void) const noexcept
{
    if (_cache && _cache->index != ObjectUtils::Tree::NullIndex) [[likely]]
        return visibleUnsafe();
    else [[unlikely]]
        return false;
}

inline bool kF::Object::visibleUnsafe(void) const noexcept
{
    return _cache->tree->get(_cache->index).visible;
}

inline void kF::Object::visible(const bool state) noexcept_ndebug
{
    kFAssert(_cache->index != ObjectUtils::Tree::NullIndex,
        throw std::logic_error("Object::visible: To set an object's visible state, the object must live inside an ObjectUtils::Tree"));
    _cache->tree->get(_cache->index).visible = state;
}

inline kF::Object::ObjectIndex kF::Object::childrenCount(void) const noexcept
{
    if (isInTree()) [[likely]]
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

[[nodiscard]] inline kF::Meta::Data kF::Object::findMetaData(const HashedName name) const noexcept
{
    auto meta = getMetaType().findData(name);

    if (!meta && hasObjectCache()) [[unlikely]]
        meta = _cache->runtime.findData(name);
    return meta;
}

[[nodiscard]] inline kF::Meta::Signal kF::Object::findMetaSignal(const HashedName name) const noexcept
{
    auto meta = getMetaType().findSignal(name);

    if (!meta && hasObjectCache()) [[unlikely]]
        meta = _cache->runtime.findSignal(name);
    return meta;
}

[[nodiscard]] inline kF::Meta::Function kF::Object::findMetaFunction(const HashedName name) const noexcept
{
    auto meta = getMetaType().findFunction(name);

    if (!meta && hasObjectCache()) [[unlikely]]
        meta = _cache->runtime.findFunction(name);
    return meta;
}

inline kF::Var kF::Object::getVar(const HashedName name) const
{
    if (auto data = findMetaData(name); !data)
        throw std::logic_error("Object::get: Invalid hashed name '" + std::to_string(name) + '\'');
    else if (auto res = data.get(this); res)
        return res;
    throw std::logic_error("Object::get: Empty property '" + std::to_string(name) + '\'');
}

inline void kF::Object::setVar(const HashedName name, const Var &var)
{
    if (auto data = findMetaData(name); !data)
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
    auto fct = findMetaFunction(name);

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
        ensureObjectCache();
    return *_cache->slotTable;
}

template<kF::Object::IsEnsureCache EnsureCache>
inline void kF::Object::setDefaultSlotTable(Meta::SlotTable &slotTable) noexcept_ndebug
{
    if constexpr (EnsureCache == IsEnsureCache::Yes)
        ensureObjectCache();
    _cache->slotTable = &slotTable;
}

inline void kF::Object::ensureObjectCache(void) noexcept_ndebug
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

    if constexpr (EnsureCache == IsEnsureCache::Yes)
        ensureObjectCache();
    _cache->registeredSlots.push(signal, handle);
    if constexpr (std::is_base_of_v<Object, Receiver>) {
        if (this != receiver) {
            reinterpret_cast<Object*>(const_cast<void *>(receiver))->ensureObjectCache();
            reinterpret_cast<Object*>(const_cast<void *>(receiver))->_cache->ownedSlots.push(handle);
        }
    }
    return handle;
}

template<typename Receiver, typename Slot>
inline kF::Object::ConnectionHandle kF::Object::ConnectMultiple(Meta::SlotTable &slotTable,
        Object *objectBegin, Object *objectEnd,
        const Meta::Signal *signalBegin, const Meta::Signal *signalEnd,
        const void * const receiver, Slot &&slot)
    noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
{
    using Decomposer = Meta::Internal::FunctionDecomposerHelper<Slot>;

    if constexpr (!std::is_same_v<Receiver, void> && Decomposer::IsMember && !Decomposer::IsFunctor) {
        static_assert(std::is_same_v<Receiver, typename Decomposer::ClassType>, "You tried to connect receiver to a non-receiver member function");
        static_assert(std::is_const_v<Receiver> == Decomposer::IsConst, "You tried to connect a volatile member slot with a constant receiver");
    }

    kFAssert(std::distance(signalBegin, signalEnd) == std::distance(objectBegin, objectEnd),
        throw std::logic_error("Object::ConnectMultiple: Number of objects must be equal to number of signals"));

    auto handle { slotTable.insert<Receiver>(receiver, std::forward<Slot>(slot)) };

    while (objectBegin != objectEnd) {
        kFAssert(signalBegin->operator bool(),
            throw std::logic_error("Object::ConnectMultiple: Invalid signal in the list"));
        objectBegin->ensureObjectCache();
        objectBegin->_cache->registeredSlots.push(*signalBegin, handle);
        ++objectBegin;
        ++signalBegin;
    }
    if constexpr (std::is_base_of_v<Object, Receiver>) {
        reinterpret_cast<Object*>(const_cast<void *>(receiver))->ensureObjectCache();
        reinterpret_cast<Object*>(const_cast<void *>(receiver))->_cache->ownedSlots.push(handle);
    }
    return handle;
}

inline void kF::Object::disconnect(void)
{
    auto &slotTable = getDefaultSlotTable<IsEnsureCache::No>();

    for (auto owned : _cache->ownedSlots)
        slotTable.remove(owned);
    _cache->ownedSlots.clear();
    for (const auto &registered : _cache->registeredSlots)
        slotTable.remove(registered.second);
    _cache->registeredSlots.clear();
}

inline bool kF::Object::disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal)
{
    auto it = std::remove_if(_cache->registeredSlots.begin(), _cache->registeredSlots.end(),
        [&slotTable, signal](const auto &pair) {
            if (pair.first != signal) [[likely]]
                return false;
            slotTable.remove(pair.second);
            return true;
        }
    );

    if (it != _cache->registeredSlots.end()) [[likely]] {
        _cache->registeredSlots.erase(it, _cache->registeredSlots.end());
        return true;
    }
    return false;
}

inline bool kF::Object::disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const ConnectionHandle handle)
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
        return true;
    }
    return false;
}

template<typename Receiver>
inline bool kF::Object::disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, const ConnectionHandle handle)
{
    auto it = _cache->registeredSlots.begin();
    const auto end = _cache->registeredSlots.end();
    bool found = disconnect<std::is_base_of_v<Object, Receiver>>(slotTable, signal, handle);

    if constexpr (std::is_base_of_v<Object, Receiver>) {
        auto it = reinterpret_cast<std::remove_cvref_t<Receiver *>>(receiver)->_cache.ownedSlots.begin();
        const auto end = reinterpret_cast<std::remove_cvref_t<Receiver *>>(receiver)->_cache.ownedSlots.end();

        while (it != end) {
            if (*it != handle) [[likely]] {
                ++it;
                continue;
            }
            _cache->ownedSlots.erase(it);
            if (!found) {
                slotTable.remove(handle);
                found = true;
            }
            break;
        }
    }
    return found;
}

template<kF::Object::IsEnsureCache EnsureCache, typename ...Args>
inline void kF::Object::emitSignal(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args)
{
    kFAssert(signal,
        throw std::logic_error("Object::emitSignal: Unknown signal"));
    kFAssert(signal.argsCount() == sizeof...(Args),
        throw std::logic_error("Object::emitSignal: Invalid number of argument"));
    if constexpr (EnsureCache == IsEnsureCache::Yes)
        ensureObjectCache();
    Var arguments[sizeof...(Args)] { Var::Assign(std::forward<Args>(args))... };
    const auto it = std::remove_if(_cache->registeredSlots.begin(), _cache->registeredSlots.end(),
        [&slotTable, signal, &arguments](auto &pair) -> bool {
            if (signal != pair.first) [[likely]] {
                return false;
            } else [[unlikely]] {
                return !slotTable.invoke(pair.second, arguments);
            }
        }
    );
    if (it != _cache->registeredSlots.end()) [[unlikely]]
        _cache->registeredSlots.erase(it, _cache->registeredSlots.end());
}