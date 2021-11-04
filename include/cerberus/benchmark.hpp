#ifndef CERBERUS_BENCHMARK_HPP
#define CERBERUS_BENCHMARK_HPP

#include <utility>
#include <functional>
#include <iostream>
#include <cerberus/types.h>
#include <cerberus/range.hpp>

namespace cerb::experimental {
    template<
        auto &&GetTime      = std::chrono::high_resolution_clock::now,
        typename Difference = std::chrono::duration<double>>
    class Benchmark
    {
        Difference m_result;

    public:
        auto result() const -> Difference
        {
            return m_result;
        }

        template<typename F>
        Benchmark(auto complete, F &&function, auto&&... args)
        {
            auto begin = GetTime();
            CERBLIB_UNROLL_N(1)
            CERBLIB_RANGE(i, 200) {
                function(args...);
            }
            auto end = GetTime();
            m_result = end - begin;

            complete(m_result, typeid(F).name());
        }
    };
}// namespace cerb::experimental

#endif /* CERBERUS_BENCHMARK_HPP */
