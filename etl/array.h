#ifndef ETL_ARRAY_H
#define ETL_ARRAY_H

namespace Project::etl {

    template <class T, size_t N>
    struct Array {
        static_assert(N > 0, "Array size can't be 0");
        typedef T Type;
        T buffer[N];

        static constexpr size_t size() { return N; }
        [[nodiscard]] constexpr size_t len() const { return N; }
        void clear() { memset(buffer, 0, N); }

        T* data()   { return buffer; }
        T* begin()  { return buffer; }
        T* end()    { return buffer + N; }
        T& front()  { return buffer[0]; }
        T& back()   { return buffer[N - 1]; }

        [[nodiscard]] const T* data()     const { return buffer; }
        [[nodiscard]] const T* begin()    const { return buffer; }
        [[nodiscard]] const T* end()      const { return buffer + N; }
        [[nodiscard]] const T& front()    const { return buffer[0]; }
        [[nodiscard]] const T& back()     const { return buffer[N - 1]; }

        T& operator [](size_t i) { return buffer[i]; }
        const T& operator [](size_t i) const { return buffer[i]; }
    };

}

#endif //ETL_ARRAY_H
