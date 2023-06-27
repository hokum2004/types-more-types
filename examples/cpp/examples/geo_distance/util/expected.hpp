#pragma once
#ifdef __clang__

// It is needed that clangd doesn't produce compilation errors, because clang
// doesn't yet have an implementation for it.

#include <exception>
#include <utility>

namespace std {
// class template unexpected
template <class E>
class unexpected;

// class template bad_expected_access
template <class E>
class bad_expected_access;

// specialization of bad_expected_access for void
template <>
class bad_expected_access<void>;

// in-place construction of unexpected values
struct unexpect_t {
  explicit unexpect_t() = default;
};
inline constexpr unexpect_t unexpect{};

// class template expected
template <class T, class E>
class expected;

// partial specialization of expected for void types
template <class T, class E>
  requires is_void_v<T>
class expected<T, E>;
} // namespace std

namespace std {
template <class E>
class unexpected {
public:
  // constructors
  constexpr unexpected(const unexpected &) = default;
  constexpr unexpected(unexpected &&) = default;
  template <class... Args>
  constexpr explicit unexpected(in_place_t, Args &&...);
  template <class U, class... Args>
  constexpr explicit unexpected(in_place_t, initializer_list<U>, Args &&...);
  template <class Err = E>
  constexpr explicit unexpected(Err &&);

  // assignment
  constexpr unexpected &operator=(const unexpected &) = default;
  constexpr unexpected &operator=(unexpected &&) = default;

  // observer
  constexpr const E &error() const &noexcept;
  constexpr E &error() &noexcept;
  constexpr const E &&error() const &&noexcept;
  constexpr E &&error() &&noexcept;

  // swap
  constexpr void swap(unexpected &other) noexcept;

  friend constexpr void swap(unexpected &x,
                             unexpected &y) noexcept(noexcept(x.swap(y)));

  // equality operator
  template <class E2>
  friend constexpr bool operator==(const unexpected &, const unexpected<E2> &);

private:
  E unex; // exposition only
};

template <class E>
unexpected(E) -> unexpected<E>;
} // namespace std

namespace std {
template <class T, class E>
class expected {
public:
  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template <class U>
  using rebind = expected<U, error_type>;

  // constructors
  constexpr expected();
  constexpr explicit expected(const expected &);
  constexpr explicit expected(expected &&) noexcept;
  template <class U, class G>
  constexpr explicit expected(const expected<U, G> &);
  template <class U, class G>
  constexpr explicit expected(expected<U, G> &&);

  template <class U = T>
  constexpr explicit expected(U &&v);

  template <class G>
  constexpr expected(const unexpected<G> &);
  template <class G>
  constexpr expected(unexpected<G> &&);

  template <class... Args>
  constexpr explicit expected(in_place_t, Args &&...);
  template <class U, class... Args>
  constexpr explicit expected(in_place_t, initializer_list<U>, Args &&...);
  template <class... Args>
  constexpr explicit expected(unexpect_t, Args &&...);
  template <class U, class... Args>
  constexpr explicit expected(unexpect_t, initializer_list<U>, Args &&...);

  // destructor
  constexpr ~expected();

  // assignment
  constexpr expected &operator=(const expected &);
  constexpr expected &operator=(expected &&) noexcept;
  template <class U = T>
  constexpr expected &operator=(U &&);
  template <class G>
  constexpr expected &operator=(const unexpected<G> &);
  template <class G>
  constexpr expected &operator=(unexpected<G> &&);

  template <class... Args>
  constexpr T &emplace(Args &&...) noexcept;
  template <class U, class... Args>
  constexpr T &emplace(initializer_list<U>, Args &&...) noexcept;

  // swap
  constexpr void swap(expected &) noexcept;
  friend constexpr void swap(expected &, expected &) noexcept;

  // observers
  constexpr const T *operator->() const noexcept;
  constexpr T *operator->() noexcept;
  constexpr const T &operator*() const &noexcept;
  constexpr T &operator*() &noexcept;
  constexpr const T &&operator*() const &&noexcept;
  constexpr T &&operator*() &&noexcept;
  constexpr explicit operator bool() const noexcept;
  constexpr bool has_value() const noexcept;
  constexpr const T &value() const &;
  constexpr T &value() &;
  constexpr const T &&value() const &&;
  constexpr T &&value() &&;
  constexpr const E &error() const &;
  constexpr E &error() &;
  constexpr const E &&error() const &&;
  constexpr E &&error() &&;
  template <class U>
  constexpr T value_or(U &&) const &;
  template <class U>
  constexpr T value_or(U &&) &&;

  // equality operators
  template <class T2, class E2>
  friend constexpr bool operator==(const expected &x,
                                   const expected<T2, E2> &y);
  template <class T2>
  friend constexpr bool operator==(const expected &, const T2 &);
  template <class E2>
  friend constexpr bool operator==(const expected &, const unexpected<E2> &);

private:
  bool has_val; // exposition only
  union {
    T val;  // exposition only
    E unex; // exposition only
  };
};
} // namespace std

namespace std {
template <class T, class E>
  requires is_void_v<T>
class expected<T, E> {
public:
  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template <class U>
  using rebind = expected<U, error_type>;

  // constructors
  constexpr expected() noexcept;
  constexpr explicit expected(const expected &);
  constexpr explicit expected(expected &&) noexcept;
  template <class U, class G>
  constexpr explicit expected(const expected<U, G> &);
  template <class U, class G>
  constexpr explicit expected(expected<U, G> &&);

  template <class G>
  constexpr expected(const unexpected<G> &);
  template <class G>
  constexpr expected(unexpected<G> &&);

  constexpr explicit expected(in_place_t) noexcept;
  template <class... Args>
  constexpr explicit expected(unexpect_t, Args &&...);
  template <class U, class... Args>
  constexpr explicit expected(unexpect_t, initializer_list<U>, Args &&...);

  // destructor
  constexpr ~expected();

  // assignment
  constexpr expected &operator=(const expected &);
  constexpr expected &operator=(expected &&) noexcept;
  template <class G>
  constexpr expected &operator=(const unexpected<G> &);
  template <class G>
  constexpr expected &operator=(unexpected<G> &&);
  constexpr void emplace() noexcept;

  // swap
  constexpr void swap(expected &) noexcept;
  friend constexpr void swap(expected &, expected &) noexcept;

  // observers
  constexpr explicit operator bool() const noexcept;
  constexpr bool has_value() const noexcept;
  constexpr void operator*() const noexcept;
  constexpr void value() const &;
  constexpr void value() &&;
  constexpr const E &error() const &;
  constexpr E &error() &;
  constexpr const E &&error() const &&;
  constexpr E &&error() &&;

  // equality operators
  template <class T2, class E2>
    requires is_void_v<T2>
  friend constexpr bool operator==(const expected &x,
                                   const expected<T2, E2> &y);
  template <class E2>
  friend constexpr bool operator==(const expected &, const unexpected<E2> &);

private:
  bool has_val; // exposition only
  union {
    E unex; // exposition only
  };
};
} // namespace std

#endif
