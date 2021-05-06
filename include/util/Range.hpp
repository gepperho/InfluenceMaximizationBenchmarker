#pragma once
#include <iterator>
#include <type_traits>

namespace utils::impl {

template<typename Counter>
struct RangeIterator
{
    // iterator traits
    using difference_type = Counter;
    using value_type = Counter;
    using pointer = Counter;
    using reference = Counter;
    using iterator_category = std::random_access_iterator_tag;

    explicit RangeIterator(Counter current)
        : current_(current),
          start_(current)
    {
    }

    auto operator++() -> RangeIterator&
    {
        ++current_;
        return *this;
    }

    auto operator--() -> RangeIterator&
    {
        --current_;
        return *this;
    }

    auto operator++(int) -> RangeIterator
    {

        auto ret = *this;
        (*this)++;
        return ret;
    }

    auto operator--(int) -> RangeIterator
    {
        auto ret = *this;
        (*this)--;
        return ret;
    }

    auto operator+(RangeIterator const& other) const
        -> difference_type
    {
        return current_ + other.current_;
    }

    auto operator+(difference_type other) const
        -> RangeIterator
    {
        return RangeIterator{current_ + other};
    }

    auto operator-(RangeIterator const& other) const
        -> difference_type
    {
        return current_ - other.current_;
    }

    auto operator-(difference_type other) const
        -> RangeIterator
    {
        return RangeIterator{current_ - other};
    }

    auto operator+=(RangeIterator const& other)
        -> RangeIterator&
    {
        current_ += other.current_;
        return *this;
    }

    auto operator-=(RangeIterator const& other)
        -> RangeIterator&
    {
        current_ -= other.current_;
        return *this;
    }

    auto operator+=(difference_type other)
        -> RangeIterator&
    {
        current_ += other;
        return *this;
    }

    auto operator-=(difference_type other)
        -> RangeIterator&
    {
        current_ -= other;
        return *this;
    }


    auto operator==(RangeIterator const& other) const
        -> bool
    {
        return current_ == other.current_;
    }

    auto operator!=(RangeIterator const& other) const
        -> bool
    {
        return !(*this == other);
    }

    auto operator<=(RangeIterator const& other) const
        -> bool
    {
        return current_ <= other.current_;
    }

    auto operator>=(RangeIterator const& other) const
        -> bool
    {
        return current_ >= other.current_;
    }

    auto operator<(RangeIterator const& other) const
        -> bool
    {
        return current_ < other.current_;
    }

    auto operator>(RangeIterator const& other) const
        -> bool
    {
        return current_ > other.current_;
    }

    auto operator*() const
        -> Counter
    {
        return current_;
    }

    auto operator->() const
        -> Counter
    {
        return current_;
    }

    auto operator[](int idx) const
        -> Counter
    {
        return start_ + idx;
    }

private:
    Counter current_;
    Counter start_;
};

template<typename Counter, bool forward>
struct RangeWrapper
{
    using ValueType = typename std::remove_cv<Counter>::type;
    using IteratorBase = RangeIterator<ValueType>;
    using Iterator = typename std::conditional<forward,
                                               IteratorBase,
                                               std::reverse_iterator<IteratorBase>>::type;

    RangeWrapper(Counter from, Counter to)
        : begin_(from),
          end_(to)
    {
    }

    auto begin() const
        -> Iterator
    {
        return Iterator(IteratorBase(begin_));
    }
    auto end() const
        -> Iterator
    {
        return Iterator(IteratorBase(end_));
    }

    auto operator[](int idx) const
        -> Counter
    {
        return begin_ + idx;
    }

    auto size() const
        -> std::size_t
    {
        return end_ - begin_;
    }

private:
    ValueType const begin_;
    ValueType const end_;
};

} // namespace utils::impl


namespace utils {

template<typename Counter>
auto range(Counter to)
    -> impl::RangeWrapper<Counter, true>
{
    return {0, to};
}

template<typename Counter>
auto range(Counter from, Counter to)
    -> impl::RangeWrapper<Counter, true>
{
    return {from, to};
}

template<typename Counter>
auto reverseRange(Counter to)
    -> impl::RangeWrapper<Counter, false>
{
    return {to, 0};
}

template<typename Counter>
auto reverseRange(Counter from, Counter to)
    -> impl::RangeWrapper<Counter, false>
{
    return {to, from};
}

} // namespace utils
