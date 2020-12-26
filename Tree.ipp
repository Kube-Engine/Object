/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A tree of objects
 */

inline kF::ObjectUtils::Tree::Tree(void) noexcept
{
    _nodes.reserve(DefaultTableSize);
    _nodes.push(Node {});
}

inline kF::ObjectUtils::Tree::Index kF::ObjectUtils::Tree::add(const Index parentIndex, Object * const object, const HashedName id, const Flags flags) noexcept
{
    auto &parent = get(parentIndex);
    Index index;

    setAllDirtyFlags();
    if (!_freeList.empty()) {
        index = _freeList.back();
        _freeList.pop();
        get(index) = Node {
            object: object,
            id: id,
            parentIndex: parentIndex,
            flags: flags
        };
    } else {
        index = _nodes.size();
        _nodes.push(Node {
            object: object,
            id: id,
            parentIndex: parentIndex,
            flags: flags
        });
    }
    parent.children.push(index);
    return index;
}

inline void kF::ObjectUtils::Tree::remove(const Index index) noexcept
{
    auto &node = get(index);

    setAllDirtyFlags();
    _freeList.push(index);
    if (node.parentIndex != NullIndex) [[likely]] {
        auto &parent = get(node.parentIndex);
        parent.children.erase(std::find(parent.children.begin(), parent.children.end(), index));
    }
    for (const auto childIndex : node.children)
        get(childIndex).parentIndex = NullIndex;
}

inline void kF::ObjectUtils::Tree::setParent(const Index index, const Index parentIndex) noexcept
{
    auto &node = get(index);

    setAllDirtyFlags();
    if (node.parentIndex != NullIndex) [[likely]] {
        auto &parent = get(node.parentIndex);
        parent.children.erase(std::find(parent.children.begin(), parent.children.end(), index));
    }
    node.parentIndex = parentIndex;
    auto &parent = get(parentIndex);
    parent.children.push(index);
}

inline kF::ObjectUtils::Tree::Index kF::ObjectUtils::Tree::find(const HashedName id) const noexcept
{
    for (Index i = 0u; auto &node : _nodes) {
        if (node.id == id) [[unlikely]]
            return i;
        ++i;
    }
    return NullIndex;
}

inline kF::ObjectUtils::Tree::Index kF::ObjectUtils::Tree::findInScope(const HashedName id, const Index from) const noexcept
{
    auto &node = get(from);

    if (node.id == id) [[unlikely]]
        return from;
    // Start to search in close children
    for (const auto childIndex : node.children) {
        auto &child = get(childIndex);
        if (child.id == id) [[unlikely]]
            return childIndex;
    }
    // If not found, search for parents and their close children
    for (auto parentIndex = node.parentIndex; parentIndex != NullIndex;) {
        auto &parent = get(parentIndex);
        if (parent.id == id) [[unlikely]]
            return parentIndex;
        for (const auto childIndex : node.children) {
            auto &child = get(childIndex);
            if (child.id == id) [[unlikely]]
                return childIndex;
        }
        parentIndex = parent.parentIndex;
    }
    return NullIndex;
}

inline void kF::ObjectUtils::Tree::setAllDirtyFlags(void) noexcept
{
    // TODO: check if normal setter has same speed in benchmarks as uint32 setter
    _treeDirty = true;
    _visibleDirty = true;
    _enabledDirty = true;
}