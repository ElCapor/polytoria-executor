#ifndef META_H
#define META_H
#include <concepts>

namespace nasec {
    namespace meta {

        /**
         * @brief Compile-Time String
         * 
         * @tparam N 
         */
        template<std::size_t N>
        struct String {
            char data[N];
            std::size_t size() const { return N - 1; }
            const char* c_str() const { return data; }

            constexpr String(const char (&str)[N]) {
                std::copy_n(str, N, data);
            }
        };
        

        /**
         * @brief Compile-Time String Helper to produce from const char*
         * 
         */
        template<std::size_t N>
        String(const char (&str)[N]) -> String<N>;
    }
}

#endif /* META */
