// Copy-and-swap
// C++11

#include <utility>

class resource {
  int x = 0;
};

class foo
{
  public:
    foo()
      : p{new resource{}}
    { }

    foo(const foo& other)
      : p{new resource{*(other.p)}}
    { }

    foo(foo&& other)
      : p{other.p}
    {
      other.p = nullptr;
    }

    foo& operator=(foo other)
    {
      swap(*this, other);

      return *this;
    }

    ~foo()
    {
      delete p;
    }

    friend void swap(foo& first, foo& second)
    {
      using std::swap;

      swap(first.p, second.p);
    }

  private:
    resource* p;
};



int main()
{
  foo f1, f2, f3;
  f2 = f1;
  f3 = std::move(f1);

  swap(f2, f3);
}

// Delegate behavior to derived classes

template<typename derived>
class base
{
  public:
    void do_something()
    {
      // ...
      static_cast<derived*>(this)->do_something_impl();
      // ...
    }

  private:
    void do_something_impl()
    {
      // Default implementation
    }
};

class foo : public base<foo>
{
  public:
    void do_something_impl()
    {
      // Derived implementation
    }
};

class bar : public base<bar>
{ };

template<typename derived>
void use(base<derived>& b)
{
  b.do_something();
}


int main()
{
  foo f;
  use(f);

  bar b;
  use(b);
}



#include <tuple>

class foo
{
  public:
    foo(int n_, char c_, double d_)
      : n{n_}, c{c_}, d{d_}
    {}

    friend bool operator<(const foo& lh, const foo& rh)
    {
      return std::tie(lh.n, lh.c, lh.d) <
             std::tie(rh.n, rh.c, rh.d);
    }
  private:
    int n;
    char c;
    double d;
};


int main()
{
  foo f1(1, 'a', 3.14);
  foo f2(1, 'b', 2.78);

  if (f1 < f2)
  {
    return 1;
  }
}

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

template<typename F, typename Tuple, size_t ...S >
decltype(auto) apply_tuple_impl(F&& fn, Tuple&& t, std::index_sequence<S...>)
{
  return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
decltype(auto) apply_from_tuple(F&& fn, Tuple&& t)
{
  std::size_t constexpr tSize
    = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;

  return apply_tuple_impl(std::forward<F>(fn),
                          std::forward<Tuple>(t),
                          std::make_index_sequence<tSize>());
}

int do_sum(int a, int b)
{
  return a + b;
}

int main()
{
  int sum = apply_from_tuple(do_sum, std::make_tuple(10, 20));
}

#include <optional>

void foo(int i,
         std::optional<double> f,
         std::optional<bool> b)
{ }

int main()
{
  foo(5, 1.0, true);
  foo(5, std::nullopt, true);
  foo(5, 1.0, std::nullopt);
  foo(5, std::nullopt, std::nullopt);
}
