/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Object
 */

#pragma once

#include "Reflection.hpp"

namespace kF
{
    class Object;
}

/** @brief The object class is an abstraction of the meta reflection library on an arbitrary derived class
 *  It can be used to query meta properties or connect / emit meta signals */
class kF::Object
{
    K_ABSTRACT(Object)

public:
    /** @brief Helper to make template instantiation more clear */
    enum class IsEnsureConnection : bool {
        No,
        Yes
    };

    /** @brief Handle used to manipulate slots */
    using ConnectionHandle = Meta::SlotTable::OpaqueIndex;

    /** @brief Connection table of an object */
    struct alignas_cacheline ConnectionTable
    {
        Meta::SlotTable *slotTable { &Meta::Signal::GetSlotTable() };
        Core::Vector<std::pair<Meta::Signal, ConnectionHandle>> registeredSlots;
        Core::Vector<ConnectionHandle> ownedSlots;
    };

    static_assert_fit_cacheline(ConnectionTable);

    /** @brief Default constructor (very cheap) */
    Object(void) noexcept = default;

    /** @brief Move constructor is disabled since it could break signal / slot behavior */
    Object(Object &&other) noexcept = delete;

    /** @brief Virtual destructor (time vary upon connection count) */
    virtual ~Object(void) noexcept;

    /** @brief Checks if a meta-variable exists */
    [[nodiscard]] virtual bool varExists(const HashedName name) const noexcept { return getMetaType().findData(name).operator bool(); }

    /** @brief Get an opaque meta-variable */
    [[nodiscard]] virtual Var getVar(const HashedName name) const;

    /** @brief Get an opaque meta-variable */
    [[nodiscard]] Var getVar(const Meta::Data metaData) const { return metaData.get(this); }

    /** @brief Get a casted meta-variable's reference */
    template<typename As>
    [[nodiscard]] As &getVarAs(const HashedName name) { return getVar(name).cast<std::remove_reference_t<As>>(); }

    /** @brief Get a casted meta-variable's const reference */
    template<typename As>
    [[nodiscard]] const As &getVarAs(const HashedName name) const { return getVar(name).cast<std::remove_reference_t<As>>(); }

    /** @brief Set internal 'name' property */
    virtual void setVar(const HashedName name, const Var &var);

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
        { return connect<IsEnsureConnection::No, void, Slot>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), nullptr, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, void, Slot>(slotTable, getMetaType().findSignal<SignalPtr>(), nullptr, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::No, Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::No, const Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, Receiver, Slot>(slotTable, getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, const Receiver, Slot>(slotTable, getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }

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
        { return connect<IsEnsureConnection::No, void, Slot>(getDefaultSlotTable(), getMetaType().findSignal(name), nullptr, std::forward<Slot>(slot)); }
    template<typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, void, Slot>(slotTable, getMetaType().findSignal(name), nullptr, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const HashedName name, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::No, Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const HashedName name, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::No, const Receiver, Slot>(getDefaultSlotTable(), getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, Receiver, Slot>(slotTable, getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, const Receiver, Slot>(slotTable, getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }

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
        { return connect<IsEnsureConnection::No, void, Slot>(getDefaultSlotTable(), signal, std::forward<Slot>(slot)); }
    template<typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, void, Slot>(slotTable, signal, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::No, Receiver, Slot>(getDefaultSlotTable(), signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::No, const Receiver, Slot>(getDefaultSlotTable(), signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, Receiver, Slot>(slotTable, signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<IsEnsureConnection::Yes, const Receiver, Slot>(slotTable, signal, &receiver, std::forward<Slot>(slot)); }

    /** @brief Disconnect a slot using a signal function ptr and a connection handle (may take custom SlotTable) */
    template<auto SignalPtr>
    void disconnect(const ConnectionHandle handle)
        { disconnect(getDefaultSlotTable<IsEnsureConnection::No>(), getMetaType().findSignal<SignalPtr>(), handle); }
    template<auto SignalPtr>
    void disconnect(Meta::SlotTable &slotTable, const ConnectionHandle handle)
        { disconnect(slotTable, getMetaType().findSignal<SignalPtr>(), handle); }

    /** @brief Disconnect a slot using a signal name and a connection handle (may take custom SlotTable) */
    void disconnect(const HashedName name, const ConnectionHandle handle)
        { disconnect(getDefaultSlotTable<IsEnsureConnection::No>(), getMetaType().findSignal(name), handle); }
    void disconnect(Meta::SlotTable &slotTable, const HashedName name, const ConnectionHandle handle)
        { disconnect(slotTable, getMetaType().findSignal(name), handle); }

    /** @brief Disconnect a slot using a signal and a connection handle (may take custom SlotTable) */
    void disconnect(const Meta::Signal signal, const ConnectionHandle handle)
        { disconnect(getDefaultSlotTable<IsEnsureConnection::No>(), signal, handle); }
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
    void emit(Args &&...args)
        { emit<IsEnsureConnection::No, Args...>(getDefaultSlotTable(), getMetaType().findSignal<SignalPtr>(), std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'name' using default slot table */
    template<typename ...Args>
    void emit(const HashedName name, Args &&...args)
        { emit<IsEnsureConnection::No, Args...>(getDefaultSlotTable(), getMetaType().findSignal(name), std::forward<Args>(args)...); }

    /** @brief Emit a signal using default slot table */
    template<typename ...Args>
    void emit(const Meta::Signal signal, Args &&...args)
        { emit<IsEnsureConnection::No, Args...>(getDefaultSlotTable(), signal, std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'SignalPtr' using a specific slot table*/
    template<auto SignalPtr, typename ...Args>
    void emit(Meta::SlotTable &slotTable, Args &&...args)
        { emit<IsEnsureConnection::Yes, Args...>(slotTable, getMetaType().findSignal<SignalPtr>(), std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'name' using a specific slot table */
    template<typename ...Args>
    void emit(Meta::SlotTable &slotTable, const HashedName name, Args &&...args)
        { emit<IsEnsureConnection::Yes, Args...>(slotTable, getMetaType().findSignal(name), std::forward<Args>(args)...); }

    /** @brief Emit a signal using a specific slot table */
    template<typename ...Args>
    void emit(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args)
        { emit<IsEnsureConnection::Yes, Args...>(slotTable, signal, std::forward<Args>(args)...); }

    /** @brief Get the default slot table used for connection / disconnection */
    template<IsEnsureConnection EnsureConnectionTable = IsEnsureConnection::Yes>
    [[nodiscard]] Meta::SlotTable &getDefaultSlotTable(void) noexcept_ndebug;

    /** @brief Set a custom slot table used for connection / disconnection */
    template<IsEnsureConnection EnsureConnectionTable = IsEnsureConnection::Yes>
    void setDefaultSlotTable(Meta::SlotTable &slotTable) noexcept_ndebug;

private:
    std::unique_ptr<ConnectionTable> _connectionTable {};

    /** @brief Ensure that object has a connection table */
    void ensureConnectionTable(void) noexcept_ndebug;

    /** @brief Connection implementation */
    template<IsEnsureConnection EnsureConnectionTable, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot));

    /** @brief Disconnect a member slot implementation */
    template<typename Receiver>
    void disconnect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, const ConnectionHandle handle);

    /** @brief Emit a signal using a specific slot table */
    template<IsEnsureConnection EnsureConnectionTable, typename ...Args>
    void emit(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args);
};

#include "Object.ipp"