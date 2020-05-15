#include <iostream>
#include <map>
#include <string>
#include <functional>

using namespace std;

auto lambda = [](auto x, auto y) {return x + y;};

int main() {
  std::cout << lambda(1,2) << std::endl;
  map<int, std::string> m;
  m[1] = "1";
  m[2] = "hello";

  cout << m[2] << endl;

  [out = std::ref(std::cout << "Result from C code: " << lambda(1, 2))](){
    out.get() << ".\n";
  }();

  return 0;
}

class a {
 public:
  a() = default;
  ~a() = default;
};

template <class F, class Tuple, std::size_t... I>
constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
	return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
	// Note: std::invoke is a C++17 feature
}
template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t)
{
	return apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
		std::make_index_sequence < std::tuple_size<std::decay_t<Tuple>>::value > {});
}
