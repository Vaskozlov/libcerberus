#ifndef CERBERUS_SLR1_HPP
#define CERBERUS_SLR1_HPP

#include <cerberus/types.h>
#include <cerberus/map.hpp>
#include <cerberus/set.hpp>
#include <cerberus/math.hpp>
#include <cerberus/analyzation/exceptions.hpp>

#define SLR1_S(state) SLR1Item(false, state, 0, {})
#define SLR1_E SLR1Item(false, SYE, SYE, UNDEFINED)
#define SLR1_R(state, size, token) SLR1Item(true, state, size, token)

namespace cerb::analysis::parser {
    constexpr u32 SyntaxError = std::numeric_limits<u32>::max();

    template<
        typename CharT, typename TokenType, typename token_t, size_t HandlesCount,
        size_t StatesCount>
    struct SLR1
    {
        template<typename T>
        static constexpr auto char_cast(T chr) noexcept -> CharT
        {
            return static_cast<CharT>(chr);
        }

        static constexpr u32 SYE = std::numeric_limits<u32>::max();

        struct SLR1Item
        {
            bool reduce{};
            u32 reduce_size{};
            u32 state{};
            TokenType token{};

            constexpr SLR1Item() = default;

            constexpr SLR1Item(
                bool reduce_,
                u32 state_,
                u32 reduce_size_,
                TokenType token_)
              : reduce(reduce_), reduce_size(reduce_size_), state(state_),
                token(token_)
            {}
        };

        Vector<Pair<u32, token_t>> m_stack{};
        std::array<cerb::gl::Map<TokenType, SLR1Item, HandlesCount>, StatesCount>
            m_action_goto_table{};
        token_t next_token{};
        u32 m_processed_token_count{};
        u32 m_current_state{};

        constexpr auto get(u32 state, TokenType token) const noexcept
            -> const SLR1Item &
        {
            return m_action_goto_table[state][token];
        }

        constexpr auto get(const Pair<u32, TokenType> &pos) const noexcept
            -> const SLR1Item &
        {
            return m_action_goto_table[pos.first][pos.second];
        }

        constexpr auto is_shifting(u32 state, TokenType token) const noexcept
        {
            return !get(state, token).reduce;
        }

        constexpr auto is_shifting(const Pair<u32, TokenType> &pos) const noexcept
        {
            return !get(pos).reduce;
        }

        constexpr auto is_reducing(u32 state, TokenType token) const noexcept
        {
            return get(state, token).reduce;
        }

        constexpr auto is_reducing(const Pair<u32, TokenType> &pos) const noexcept
        {
            return !get(pos).reduce;
        }

        constexpr auto get_reduce_info(u32 state, TokenType token) const noexcept
            -> Pair<u32, TokenType>
        {
            const auto elem = get(state, token);
            return { elem.reduce_size, elem.token };
        }

        constexpr auto
            get_reduce_info(const Pair<u32, TokenType> &pos) const noexcept
            -> Pair<u32, TokenType>
        {
            const auto elem = get(pos);
            return { elem.reduce_size, elem.token };
        }

        constexpr auto get_next_state(u32 state, TokenType token) const noexcept
        {
            return get(state, token).state;
        }

        constexpr auto get_next_state(const Pair<u32, TokenType> &pos) const noexcept
        {
            return get(pos).state;
        }

        constexpr auto is_next_token_reducing(
            u32 state,
            TokenType token,
            TokenType next) const noexcept
        {
            return is_reducing(get_next_state(state, token), next);
        }

        constexpr auto print_stack() const
        {
            fmt::print("Stack: \n");
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : m_stack) {
                fmt::print(
                    "{{ {}: {} }}, ", elem.first,
                    convert(elem.second.type).to_string());
            }
            std::cout << std::endl;
        }

        constexpr auto yield(token_t token) -> void
        {
            std::swap(next_token, token);

            if (m_processed_token_count != 0) {
                if (is_shifting(m_current_state, token.type)) {
                    m_stack.emplace_back(m_current_state, token);
                    m_current_state = get_next_state(m_current_state, token.type);
                    print_stack();
                }

                while (is_reducing(m_current_state, next_token.type)) {
                    const SLR1Item &future = get(m_current_state, next_token.type);

                    CERBLIB_UNROLL_N(2)
                    for (size_t i = 1; i < future.reduce_size; ++i) {
                        m_stack.pop_back();
                    }

                    m_stack.back().second.type = future.token;
                    m_current_state =
                        get_next_state(m_stack.back().first, future.token);

                    print_stack();

                    if (m_current_state == SYE || m_current_state == 2) {
                        break;
                    }
                }
            }

            ++m_processed_token_count;
        }

        constexpr SLR1(
            const std::initializer_list<TokenType> &token_order,
            const std::initializer_list<const std::initializer_list<SLR1Item>>
                &action_goto_table)
        {
            // static_assert(
            //    token_order.size() == HandlesCount &&
            //    action_goto_table.size() == StatesCount);

            size_t index  = 0;
            auto iterator = token_order.begin();

            for (const auto &raw : action_goto_table) {
                CERBLIB_UNROLL_N(2)
                for (const auto &elem : raw) {
                    m_action_goto_table[index].emplace(*iterator, elem);
                    ++iterator;
                }
                ++index;
                iterator = token_order.begin();
            }
            return;
        }
    };
}// namespace cerb::analysis::parser

#endif /* CERBERUS_SLR1_HPP */
