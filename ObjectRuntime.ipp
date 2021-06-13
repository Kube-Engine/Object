/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Object runtime meta data
 */

template<kF::HashedName Name, kF::HashedName SignalName>
inline kF::Meta::Data kF::ObjectUtils::ObjectRuntime::addData(const Meta::Type type, Var &&value)
{
    auto ptr = std::make_unique<RuntimeData>();
    *ptr = RuntimeData {
        descriptor: Meta::Data::Descriptor {
            name: name,
            isStatic: false,
            type: type,
            getFunc: [value = &ptr->value](const void *) -> Var {
                return Var::Assign(*value);
            },
            setCopyFunc: [value = &ptr->value](const void *instance, const Var &other) {
                // runtime->value.value = other;
                return Var();
            },
            setMoveFunc: [value = &ptr->value](const void *instance, Var &&other) {
                static Meta::Signal signal;
                Object *obj = reinterpret_cast<Object *>(const_cast<void *>(instance));
                if (*value != other) {
                    *value = std::move(other);
                    if (!signal) [[unlikely]]
                        signal = obj->objectRuntime().findSignal(SignalName);
                    obj->emitSignal(signal);
                }
                return Var();
            }
        },
        value: std::move(value)
    };
    const auto &runtime = _datas.push(std::move(ptr));

    return Meta::Data(&runtime->descriptor);
}

inline kF::Meta::Signal kF::ObjectUtils::ObjectRuntime::addSignal(const HashedName name, const std::size_t argsCount)
{
    const auto &runtime = _signals.push(std::make_unique<RuntimeSignal>(
        RuntimeSignal {
            descriptor: Meta::Signal::Descriptor {
                signalPtr: nullptr,
                name: name,
                argsCount: static_cast<std::uint32_t>(argsCount)
            }
        }
    ));

    return Meta::Signal(&runtime->descriptor);
}

template<typename Functor>
inline kF::Meta::Function kF::ObjectUtils::ObjectRuntime::addFunction(const HashedName name, Functor &&functor)
{
    using Decomposer = Meta::Internal::ToFunctionDecomposer<Functor>;

    const auto &runtime = _functions.push(std::make_unique<RuntimeFunction>(
        RuntimeFunction {
            descriptor: Meta::Function::Descriptor {
                name: name,
                isStatic: true,
                isConst: false,
                argsCount: Decomposer::ArgsTuple.size(),
                returnType: Meta::Factory<typename Decomposer::ReturnType>::Resolve(),
                argTypeFunc: &Decomposer::ArgsType,
                invokeFunc: [functor = std::forward<Functor>(functor)](const void *, Var *args) -> Var {
                    return Internal::Invoke<Type, true, Decomposer>(functor, args, Decomposer::IndexSequence);
                }
            }
        }
    ));

    return Meta::Function(&runtime->descriptor);
}

inline kF::Meta::Data kF::ObjectUtils::ObjectRuntime::findData(const HashedName name) const noexcept
{
    if (!_datas.size())
        return Meta::Data();
    const auto &names = _datas.headerCustomType();
    const auto it = names.find(name);
    const auto end = names.endUnsafe();
    if (it != end) [[likely]]
        return Meta::Data(&_datas.at(static_cast<std::uint32_t>(std::distance(it, end))).descriptor);
    else [[unlikely]]
        return Meta::Data();
}

inline kF::Meta::Signal kF::ObjectUtils::ObjectRuntime::findSignal(const HashedName name) const noexcept
{
    if (!_signals.size())
        return Meta::Signal();
    const auto &names = _signals.headerCustomType();
    const auto it = names.find(name);
    const auto end = names.endUnsafe();
    if (it != end) [[likely]]
        return Meta::Signal(&_signals.at(static_cast<std::uint32_t>(std::distance(it, end))).descriptor);
    else [[unlikely]]
        return Meta::Signal();
}

inline kF::Meta::Function kF::ObjectUtils::ObjectRuntime::findFunction(const HashedName name) const noexcept
{
    if (!_functions.size())
        return Meta::Function();
    const auto &names = _functions.headerCustomType();
    const auto it = names.find(name);
    const auto end = names.endUnsafe();
    if (it != end) [[likely]]
        return Meta::Function(&_functions.at(static_cast<std::uint32_t>(std::distance(it, end))).descriptor);
    else [[unlikely]]
        return Meta::Function();
}
