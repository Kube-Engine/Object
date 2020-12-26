/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Object
 */

#pragma once

#include "Reflection.hpp"
#include "Runtime.hpp"
#include "Tree.hpp"

namespace kF
{
    class Object;
}

/** @brief The object class is an abstraction of the meta reflection library on an arbitrary derived class
 *  It can be used to query meta properties or connect / emit meta signals */
class kF::Object
{
    K_ABSTRACT(Object,
        K_PROPERTY_CUSTOM_COPY(Object *, parent,
            static_cast<Object*(Object::*)(void) noexcept>(&Object::parent),
            static_cast<void(Object::*)(Object*) noexcept_ndebug>(&Object::parent)),
        K_PROPERTY_CUSTOM_GETONLY(ObjectIndex, childrenCount, &Object::childrenCount)
    )

public:
    /** @brief Objects flags */
    using ObjectFlags = ObjectUtils::Tree::Flags;

    /** @brief Object index */
    using ObjectIndex = ObjectUtils::Tree::Index;

    /** @brief Helper to make template instantiation more clear */
    enum class IsEnsureCache : bool {
        No,
        Yes
    };

    /** @brief Handle used to manipulate slots */
    using ConnectionHandle = Meta::SlotTable::OpaqueIndex;

    /** @brief Connection table of an object */
    struct alignas_cacheline Cache
    {
        std::unique_ptr<ObjectUtils::Runtime> data {};
        ObjectUtils::Tree *tree { nullptr };
        ObjectIndex index { ObjectUtils::Tree::NullIndex };
        ObjectIndex parentIndex { ObjectUtils::Tree::NullIndex };
        Meta::SlotTable *slotTable { &Meta::Signal::GetSlotTable() };
        Core::TinyVector<std::pair<Meta::Signal, ConnectionHandle>> registeredSlots {};
        Core::TinyVector<ConnectionHandle> ownedSlots {};
    };

    static_assert_fit_cacheline(Cache);

    /** @brief Default constructor (very cheap) */
    Object(void) noexcept = default;

    /** @brief Parent constructor */
    Object(Object &parent, const HashedName id = 0u) noexcept { Object::parent(parent, id); }

    /** @brief Move constructor is disabled since it could break signal / slot behavior */
    Object(Object &&other) noexcept = delete;

    /** @brief Virtual destructor (time vary upon connection count) */
    virtual ~Object(void) noexcept;


    /** @brief Virtual object flags getter
     *  This function determines what the object can and cannot do when parsing the object tree */
    virtual ObjectFlags getObjectFlags(void) const noexcept { return ObjectFlags::None; }

    /** @brief Virtual callback on parent changed */
    virtual void onParentChanged(Object *object) { parentChanged(); }

    /** @brief Virtual callback on child added */
    virtual void onChildAdded(Object &object) {}

    /** @brief Virtual callback on child removed */
    virtual void onChildRemoved(Object &object) {}


    /** @brief Get the object ID if any */
    [[nodiscard]] HashedName id(void) const noexcept;

    /** @brief Set the object ID if attached to any tree */
    void id(const HashedName id) noexcept_ndebug;


    /** @brief Check if the instance has a parent and thus is in a tree */
    [[nodiscard]] bool hasParent(void) const noexcept
        { return _cache && _cache->parentIndex != ObjectUtils::Tree::NullIndex; }

    /** @brief Get the parent object if any */
    [[nodiscard]] Object *parent(void) noexcept
        { return const_cast<Object *>(const_cast<const Object *>(this)->parent()); }
    [[nodiscard]] const Object *parent(void) const noexcept;

    /** @brief Get the parent asserting that object is in a tree */
    [[nodiscard]] Object *parentUnsafe(void) noexcept
        { return _cache->tree->get(_cache->parentIndex).object; }
    [[nodiscard]] const Object *parentUnsafe(void) const noexcept
        { return _cache->tree->get(_cache->parentIndex).object; }

    /** @brief Set the parent object and inserts this instance into parent's object-tree
     *  Note that this function will preserve the ID of the object if already in a tree
     *  Note that if parent is null, unsetParent is called */
    void parent(Object * const parent) noexcept_ndebug;

    /** @brief Set the parent object and inserts this instance into parent's object-tree
     *  Note that this function will preserve the ID of the object if already in a tree */
    void parent(Object &parent) noexcept_ndebug;

    /** @brief Set the parent object and its ID and inserts this instance into parent's object-tree
     *  Note that this function will NOT preserve the ID of the object if already in a tree */
    void parent(Object &parent, const HashedName id) noexcept_ndebug;

    /** @brief Set the parent object and its ID and inserts this instance into an object-tree
     *  Note that this function will NOT preserve the ID of the object if already in a tree */
    void parent(ObjectUtils::Tree &tree, ObjectIndex parentIndex, const HashedName id) noexcept;

    /** @brief Remove links to the actual parent
     *  This function assert that a parent is set
     *  Note that this function loses the actual ID of the object */
    void unsetParent(void) noexcept_ndebug;

    /** @brief Returns the number of children if any */
    [[nodiscard]] ObjectIndex childrenCount(void) const noexcept;

    /** @brief Returns the number of children without check */
    [[nodiscard]] ObjectIndex childrenCountUnsafe(void) const noexcept
        { return _cache->tree->get(_cache->index).children.size(); }

    /** @brief Returns child object at index */
    [[nodiscard]] Object *getChild(const ObjectIndex index) noexcept
        { return _cache->tree->get(_cache->tree->get(_cache->index).children[index]).object; }
    [[nodiscard]] const Object *getChild(const ObjectIndex index) const noexcept
        { return _cache->tree->get(_cache->tree->get(_cache->index).children[index]).object; }


    /** @brief Finds an object in the attached tree from 'this' scope using its hashed name
     *  This function will search in 'from' close children then in every parent and their close children
     *  This mean you can't access a sub-child or the sub-child of a parent
     *  Be aware that id can collide and thus, the function will return the first match */
    [[nodiscard]] Object *find(const HashedName id) const noexcept;

    /** @brief Finds an object in the attached tree using its hashed name
     *  Be aware that id can collide and thus, the function will return the first match */
    [[nodiscard]] Object *findGlobal(const HashedName id) const noexcept;


    /** @brief Checks if a meta-variable exists */
    [[nodiscard]] bool varExists(const HashedName name) const noexcept { return getMetaType().findData(name).operator bool(); }

    /** @brief Get an opaque meta-variable */
    [[nodiscard]] Var getVar(const HashedName name) const;

    /** @brief Get an opaque meta-variable */
    [[nodiscard]] Var getVar(const Meta::Data metaData) const { return metaData.get(this); }

    /** @brief Get a casted meta-variable's reference */
    template<typename As>
    [[nodiscard]] As &getVarAs(const HashedName name) { return getVar(name).cast<std::remove_reference_t<As>>(); }

    /** @brief Get a casted meta-variable's const reference */
    template<typename As>
    [[nodiscard]] const As &getVarAs(const HashedName name) const { return getVar(name).cast<std::remove_reference_t<As>>(); }

    /** @brief Set internal 'name' property */
    void setVar(const HashedName name, const Var &var);

    /** @brief Set internal 'name' property directly using Meta::Data (faster) */
    void setVar(const Meta::Data metaData, const Var &var);


    /** @brief Invoke a meta-function */
    template<typename ...Args>
    Var invoke(const HashedName name, Args &&...args);

    /** @brief Invoke a meta-function */
    template<typename ...Args>
    Var invoke(const Meta::Function metaFunc, Args &&...args);


    /** @brief Register a slot into an owned signal using signal pointer (may take a custom SlotTable if needed)
     *  Be careful, if you pass a custom SlotTable, you will have to disconnect it manually
     *  If you wish to change the default SlotTable, you can use the function 'setSlotTable'
     *
     *  If: no receiver is passed, the slot is owned by the calling instance
     *  Else If: receiver is an Object-derived class, it will own the connection (and will release it when destroyed)
     *      If: the given slot is a member function, receiver must be the instance used to perform the call */
    template<auto SignalPtr, typename Slot>
    ConnectionHandle connect(Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, void, Slot>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), nullptr, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, void, Slot>(slotTable, getMetaType().findSignal<SignalPtr>(), nullptr, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, const Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, Receiver, Slot>(slotTable, getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, const Receiver, Slot>(slotTable, getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }

    /** @brief Register a slot into an owned signal using a signal hashed name (may take a custom SlotTable if needed)
     *  Be careful, if you pass a custom SlotTable, you will have to disconnect it manually
     *  If you wish to change the default SlotTable, you can use the function 'setSlotTable'
     *
     *  If: no receiver is passed, the slot is owned by the calling instance
     *  Else If: receiver is an Object-derived class, it will own the connection (and will release it when destroyed)
     *      If: the given slot is a member function, receiver must be the instance used to perform the call */
    template<typename Slot>
    ConnectionHandle connect(const HashedName name, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, void, Slot>(getDefaultSlotTable(), getMetaType().findSignal(name), nullptr, std::forward<Slot>(slot)); }
    template<typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, void, Slot>(slotTable, getMetaType().findSignal(name), nullptr, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const HashedName name, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const HashedName name, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, const Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, Receiver, Slot>(slotTable, getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, const Receiver, Slot>(slotTable, getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }

    /** @brief Register a slot into an owned signal using a raw signal (may take a custom SlotTable if needed)
     *  Be careful, if you pass a custom SlotTable, you will have to disconnect it manually
     *  If you wish to change the default SlotTable, you can use the function 'setDefaultSlotTable'
     *
     *  If: no receiver is passed, the slot is owned by the calling instance
     *  Else If: receiver is an Object-derived class, it will own the connection (and will release it when destroyed)
     *      If: the given slot is a member function, receiver must be the instance used to perform the call */
    template<typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, void, Slot>(getDefaultSlotTable(), signal, std::forward<Slot>(slot)); }
    template<typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, void, Slot>(slotTable, signal, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, Receiver, Slot>(getDefaultSlotTable(), signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::No, const Receiver, Slot>(getDefaultSlotTable(), signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, Receiver, Slot>(slotTable, signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureCache::Yes, const Receiver, Slot>(slotTable, signal, &receiver, std::forward<Slot>(slot)); }


    /** @brief Disconnect a slot using a signal function ptr and a connection handle (may take custom SlotTable) */
    template<auto SignalPtr>
    void disconnect(const ConnectionHandle handle)
        { disconnect(getDefaultSlotTable<IsEnsureCache::No>(), getMetaType().findSignal<SignalPtr>(), handle); }
    template<auto SignalPtr>
    void disconnect(Meta::SlotTable &slotTable, const ConnectionHandle handle)
        { disconnect(slotTable, getMetaType().findSignal<SignalPtr>(), handle); }

    /** @brief Disconnect a slot using a signal name and a connection handle (may take custom SlotTable) */
    void disconnect(const HashedName name, const ConnectionHandle handle)
        { disconnect(getDefaultSlotTable<IsEnsureCache::No>(), getMetaType().findSignal(name), handle); }
    void disconnect(Meta::SlotTable &slotTable, const HashedName name, const ConnectionHandle handle)
        { disconnect(slotTable, getMetaType().findSignal(name), handle); }

    /** @brief Disconnect a slot using a signal and a connection handle (may take custom SlotTable) */
    void disconnect(const Meta::Signal signal, const ConnectionHandle handle)
        { disconnect(getDefaultSlotTable<IsEnsureCache::No>(), signal, handle); }
    void disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const ConnectionHandle handle);

    /** @brief Disconnect a slot using a signal name, a receiver and a connection handle (may take custom SlotTable) */
    template<auto SignalPtr, typename Receiver>
    void disconnect(const Receiver &receiver, const ConnectionHandle handle)
        { disconnect<Receiver>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), &receiver, handle); }
    template<auto SignalPtr, typename Receiver>
    void disconnect(Meta::SlotTable &slotTable, const Receiver &receiver, const ConnectionHandle handle)
        { disconnect<Receiver>(slotTable, getMetaType().findSignal<SignalPtr>(), &receiver, handle); }

    /** @brief Disconnect a slot using a signal name, a receiver and a connection handle (may take custom SlotTable) */
    template<typename Receiver>
    void disconnect(const HashedName name, const Receiver &receiver, const ConnectionHandle handle)
        { disconnect<Receiver>(getDefaultSlotTable(), getMetaType().findSignal(name), &receiver, handle); }
    template<typename Receiver>
    void disconnect(Meta::SlotTable &slotTable, const HashedName name, const Receiver &receiver, const ConnectionHandle handle)
        { disconnect<Receiver>(slotTable, getMetaType().findSignal(name), &receiver, handle); }

    /** @brief Disconnect a slot using a signal, a receiver and a connection handle (may take custom SlotTable) */
    template<typename Receiver>
    void disconnect(const Meta::Signal signal, const Receiver &receiver, const ConnectionHandle handle)
        { disconnect<Receiver>(getDefaultSlotTable(), signal, &receiver, handle); }
    template<typename Receiver>
    void disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const Receiver &receiver, const ConnectionHandle handle)
        { disconnect<Receiver>(slotTable, signal, &receiver, handle); }


    /** @brief Emit signal matching 'SignalPtr' using default slot table */
    template<auto SignalPtr, typename ...Args>
    void emitSignal(Args &&...args)
        { emitSignal<IsEnsureCache::No, Args...>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'name' using default slot table */
    template<typename ...Args>
    void emitSignal(const HashedName name, Args &&...args)
        { emitSignal<IsEnsureCache::No, Args...>(getDefaultSlotTable(), getMetaType().findSignal(name), std::forward<Args>(args)...); }

    /** @brief Emit a signal using default slot table */
    template<typename ...Args>
    void emitSignal(const Meta::Signal signal, Args &&...args)
        { emitSignal<IsEnsureCache::No, Args...>(getDefaultSlotTable(), signal, std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'SignalPtr' using a specific slot table*/
    template<auto SignalPtr, typename ...Args>
    void emitSignal(Meta::SlotTable &slotTable, Args &&...args)
        { emitSignal<IsEnsureCache::Yes, Args...>(slotTable, getMetaType().findSignal<SignalPtr>(), std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'name' using a specific slot table */
    template<typename ...Args>
    void emitSignal(Meta::SlotTable &slotTable, const HashedName name, Args &&...args)
        { emitSignal<IsEnsureCache::Yes, Args...>(slotTable, getMetaType().findSignal(name), std::forward<Args>(args)...); }

    /** @brief Emit a signal using a specific slot table */
    template<typename ...Args>
    void emitSignal(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args)
        { emitSignal<IsEnsureCache::Yes, Args...>(slotTable, signal, std::forward<Args>(args)...); }


    /** @brief Get the default slot table used for connection / disconnection */
    template<IsEnsureCache EnsureCache = IsEnsureCache::Yes>
    [[nodiscard]] Meta::SlotTable &getDefaultSlotTable(void) noexcept_ndebug;

    /** @brief Set a custom slot table used for connection / disconnection */
    template<IsEnsureCache EnsureCache = IsEnsureCache::Yes>
    void setDefaultSlotTable(Meta::SlotTable &slotTable) noexcept_ndebug;


private:
    std::unique_ptr<Cache> _cache {};

    /** @brief Ensure that object has a connection table */
    void ensureCache(void) noexcept_ndebug;

    /** @brief Connection implementation */
    template<IsEnsureCache EnsureCache, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot));

    /** @brief Disconnect a member slot implementation */
    template<typename Receiver>
    void disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, const ConnectionHandle handle);

    /** @brief Emit a signal using a specific slot table */
    template<IsEnsureCache EnsureCache, typename ...Args>
    void emitSignal(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args);
};

#include "Object.ipp"