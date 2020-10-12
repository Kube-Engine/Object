/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Interpreter Object interface
 */

#pragma once

#include <Kube/Core/FlatVector.hpp>

#include "Reflection.hpp"

namespace kF
{
    class Object;
}

/** @brief The object class isan abstraction of the meta reflection library on an arbitrary derived class
 *  It can be used to query meta properties or connect / emit meta signals */
class kF::Object
{
    KUBE_REGISTER(Object)

public:
    /** @brief Handle used to manipulate slots */
    using ConnectionHandle = Meta::SlotTable::OpaqueIndex;

    /** @brief Connection table of an object */
    struct KF_ALIGN_CACHELINE ConnectionTable
    {
        std::vector<std::pair<Meta::Signal, ConnectionHandle>> registeredSlots;
        std::vector<ConnectionHandle> ownedSlots;
    };

    /** @brief Move constructor */
    Object(void) noexcept = default;

    /** @brief Virtual destructor */
    virtual ~Object(void) noexcept = default;// { disconnectAll(); }

    /** @brief Checks if a meta-variable exists */
    [[nodiscard]] virtual bool varExists(const HashedName name) const noexcept { return getMetaType().findData(name).operator bool(); }

    /** @brief Get an opaque meta-variable */
    [[nodiscard]] virtual Var getVar(const HashedName name) const;

    /** @brief Get an opaque meta-variable */
    [[nodiscard]] Var getVar(const Meta::Data metaData) const { return metaData.get(getTypeHandle()); }

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
     *  If: no receiver is passed, the slot is owned by the calling instance
     *  Else If: receiver is an Object-derived class, it will own the connection (and will release it when destroyed)
     *      If: the given slot is a member function, receiver must be the instance used to perform the call */
    template<auto SignalPtr, typename Slot>
    ConnectionHandle connect(Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<void>(Meta::Signal::GetSlotTable(), getMetaType().findSignal<SignalPtr>(), nullptr, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<void>(slotTable, getMetaType().findSignal<SignalPtr>(), nullptr, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<Receiver>(Meta::Signal::GetSlotTable(), getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<const Receiver>(Meta::Signal::GetSlotTable(), getMetaType().findSignal<SignalPtr>(), receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<Receiver>(slotTable, getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }
    template<auto SignalPtr, typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<const Receiver>(slotTable, getMetaType().findSignal<SignalPtr>(), &receiver, std::forward<Slot>(slot)); }

    /** @brief Register a slot into an owned signal using a signal hashed name (may take a custom SlotTable if needed)
     *  If: no receiver is passed, the slot is owned by the calling instance
     *  Else If: receiver is an Object-derived class, it will own the connection (and will release it when destroyed)
     *      If: the given slot is a member function, receiver must be the instance used to perform the call */
    template<typename Slot>
    ConnectionHandle connect(const HashedName name, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<void>(Meta::Signal::GetSlotTable(),getMetaType().findSignal(name), nullptr, std::forward<Slot>(slot)); }
    template<typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<void>(slotTable, getMetaType().findSignal(name), nullptr, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const HashedName name, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<Receiver>(Meta::Signal::GetSlotTable(), getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const HashedName name, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<const Receiver>(Meta::Signal::GetSlotTable(), getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<Receiver>(slotTable, getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const HashedName name, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<const Receiver>(slotTable, getMetaType().findSignal(name), &receiver, std::forward<Slot>(slot)); }

    /** @brief Register a slot into an owned signal using a raw signal (may take a custom SlotTable if needed)
     *  If: no receiver is passed, the slot is owned by the calling instance
     *  Else If: receiver is an Object-derived class, it will own the connection (and will release it when destroyed)
     *      If: the given slot is a member function, receiver must be the instance used to perform the call */
    template<typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect(Meta::Signal::GetSlotTable(), signal, std::forward<Slot>(slot)); }
    template<typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect(slotTable, signal, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<Receiver>(Meta::Signal::GetSlotTable(), signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(const Meta::Signal signal, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<const Receiver>(Meta::Signal::GetSlotTable(), signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<Receiver>(slotTable, signal, &receiver, std::forward<Slot>(slot)); }
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const Receiver &receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot))
        { return connect<const Receiver>(slotTable, signal, &receiver, std::forward<Slot>(slot)); }

    /** @brief Emit signal matching 'SignalPtr' using default slot table */
    template<auto SignalPtr, typename ...Args>
    void emit(Args &&...args)
        { emit(getMetaType().findSignal<SignalPtr>(), std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'name' using default slot table */
    template<typename ...Args>
    void emit(const HashedName name, Args &&...args)
        { emit(getMetaType().findSignal(name), std::forward<Args>(args)...); }

    /** @brief Emit a signal using default slot table */
    template<typename ...Args>
    void emit(const Meta::Signal signal, Args &&...args)
        { emit(Meta::Signal::GetSlotTable(), signal, std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'SignalPtr' using a specific slot table*/
    template<auto SignalPtr, typename ...Args>
    void emit(Meta::SlotTable &slotTable, Args &&...args)
        { emit(slotTable, getMetaType().findSignal<SignalPtr>(), std::forward<Args>(args)...); }

    /** @brief Emit signal matching 'name' using a specific slot table */
    template<typename ...Args>
    void emit(Meta::SlotTable &slotTable, const HashedName name, Args &&...args)
        { emit(slotTable, getMetaType().findSignal(name), std::forward<Args>(args)...); }

    /** @brief Emit a signal using a specific slot table */
    template<typename ...Args>
    void emit(Meta::SlotTable &slotTable, const Meta::Signal signal, Args &&...args);

private:
    std::unique_ptr<ConnectionTable> _connectionTable {};

    /** @brief Connection implementation */
    template<typename Receiver, typename Slot>
    ConnectionHandle connect(Meta::SlotTable &slotTable, const Meta::Signal signal, const void * const receiver, Slot &&slot)
        noexcept(nothrow_ndebug && nothrow_forward_constructible(Slot));

    /** @brief Ensure that object has a connection table */
    void ensureConnectionTable(void) noexcept_ndebug;
};

#include "Object.ipp"