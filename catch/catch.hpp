#ifndef CERBERUS_CATCH_HPP
#define CERBERUS_CATCH_HPP

#include <cerberus/types.h>

using namespace std::string_literals;

namespace cerb::test {
    inline static std::random_device random_device{};
    inline static std::default_random_engine random_engine(random_device());
    inline static std::uniform_int_distribution<size_t> uniform_dist(10000, 1000000);

    class ConstructorChecker
    {
        u8 empty{};
        static inline std::atomic<size_t> m_constructions{ 0 };
        static inline std::atomic<size_t> m_copies{ 0 };
        static inline std::atomic<size_t> m_moves{ 0 };
        static inline std::atomic<size_t> m_copy_constructions{ 0 };
        static inline std::atomic<size_t> m_move_constructions{ 0 };
        static inline std::atomic<size_t> m_deconstructions{ 0 };
        static inline std::atomic<size_t> m_counter{ 0 };

    public:
        inline static auto counter() -> size_t
        {
            return m_counter;
        }

        inline static auto clear() -> void
        {
            m_counter            = 0;
            m_constructions      = 0;
            m_copy_constructions = 0;
            m_move_constructions = 0;
            m_copies             = 0;
            m_moves              = 0;
            m_deconstructions    = 0;
        }

        inline auto operator=(const ConstructorChecker &) -> ConstructorChecker &
        {
            ++m_copies;
            return *this;
        }
        inline auto operator=(ConstructorChecker &&) noexcept -> ConstructorChecker &
        {
            ++m_moves;
            return *this;
        }

        inline ConstructorChecker(u8 /*unused*/)
        {}

        inline ConstructorChecker()
        {
            ++m_counter;
            ++m_constructions;
        }

        inline ConstructorChecker(const ConstructorChecker & /*unused*/)
        {
            ++m_counter;
            ++m_copy_constructions;
        }

        inline ConstructorChecker(ConstructorChecker && /*unused*/) noexcept
        {
            ++m_counter;
            ++m_move_constructions;
        }

        inline ~ConstructorChecker()
        {
            --m_counter;
            ++m_deconstructions;
        }

        template<typename T>
        inline friend auto operator<<(T &os, const ConstructorChecker & /*checker*/)
            -> T &
        {
            os << "Constructions: " << cerb::ConstructorChecker::m_constructions
               << ", construction copies: "
               << cerb::ConstructorChecker::m_copy_constructions
               << ", construction moves: "
               << cerb::ConstructorChecker::m_move_constructions
               << ", copies: " << cerb::ConstructorChecker::m_copies
               << ", moves: " << cerb::ConstructorChecker::m_moves
               << ", deconstructions: "
               << cerb::ConstructorChecker::m_deconstructions
               << ", leaks: " << cerb::ConstructorChecker::m_counter;
            return os;
        }
    };
}// namespace cerb::test

#endif /* CERBERUS_CATCH_HPP */
