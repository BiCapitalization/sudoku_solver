#ifndef UTILITY_HPP
#define UTILITY_HPP
namespace solve {
   template <typename... Fns>
   struct overload_set : public Fns... {
       using Fns::operator()...;
   }; 

   template <typename... Fns>
   overload_set(Fns...) -> overload_set<Fns...>;
} /* namespace solve */
#endif // UTILITY_HPP
