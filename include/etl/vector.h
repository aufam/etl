#ifndef ETL_VECTOR_H
#define ETL_VECTOR_H

#include "etl/allocator.h"
#include "etl/algorithm.h"

namespace Project::etl {

    /// dynamic contiguous arrays
    template <typename T, typename A = etl::Allocator<T>>
    class Vector {
        T* buf;
        size_t nItems, capacity;

    public:
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;
        typedef A Alloc;

        /// empty constructor
        constexpr Vector() : Vector(nullptr, 0, 0) {}

        /// construct and set the capacity
        explicit Vector(size_t capacity) : Vector() {
            buf = allocate(capacity);
            if (buf) this->capacity = capacity;
        }

        /// construct from initializer list
        Vector(std::initializer_list<T>&& items) : Vector() {
            buf = allocate(items.size());
            if (!buf) return;

            capacity = items.size();            
            for (auto& it : items) {
                new(buf + nItems) T(etl::move(it));
                ++nItems;
            }
        }

        /// copy constructor
        Vector(const Vector& other) : Vector() { *this = other; }

        /// move constructor
        Vector(Vector&& other) noexcept : Vector() { *this = etl::move(other); }

        /// copy assignment
        Vector& operator=(const Vector& other) {
            if (this == &other) return *this;

            T* temp = nullptr;
            if (other.nItems > capacity) {
                temp = allocate(other.nItems);
                if (!temp) return *this;
            }

            clear();
            
            if (temp) {
                deallocate(buf, capacity);
                reset_(temp, 0, other.nItems);

                for (auto& it : other) {
                    new(buf + nItems) T(it);
                    ++nItems;
                }
            }
            else {
                etl::copy(other.begin(), other.end(), buf);
                nItems = other.nItems;
            }
            return *this;
        }

        /// move assignment
        Vector& operator=(Vector&& other) noexcept {
            if (this == &other) return *this;
            reset_delete_(other.buf, other.nItems, other.capacity);
            other.reset_();
            return *this;
        }

        /// destructor
        ~Vector() noexcept { reset_delete_(); }

        [[nodiscard]] size_t len() const { return nItems; }     ///< returns the number of items
        [[nodiscard]] size_t size() const { return capacity; }  ///< returns the capacity

        iterator data()   { return buf; }
        iterator begin()  { return buf; }
        iterator end()    { return buf + nItems; }
        reference front() { return buf[0]; }
        reference back()  { return buf[nItems - 1]; }

        const_iterator data()   const { return buf; }
        const_iterator begin()  const { return buf; }
        const_iterator end()    const { return buf + nItems; }
        const_reference front() const { return buf[0]; }
        const_reference back()  const { return buf[nItems - 1]; }

        /// get i-th item by dereference
        /// @warning it will be error null dereference if index is not valid
        reference operator[](int i) { return is_valid_index_(i) ? buf[i] : *static_cast<iterator>(nullptr); }
        const_reference operator[](int i) const { return is_valid_index_(i) ? buf[i] : *static_cast<const_iterator>(nullptr); }

        /// return forward iter object
        Iter<iterator> iter() { return Iter(begin(), end(), 1); }
        Iter<const_iterator> iter() const { return Iter(begin(), end(), 1); }

        /// return reversed iter object
        Iter<iterator> reversed() { return Iter(end() - 1, begin() - 1, -1); }
        Iter<const_iterator> reversed() const { return Iter(end() - 1, begin() - 1, -1); }

        /// return true if buf is not null
        explicit operator bool() const { return buf != nullptr; }

        /// add an item to the vector
        template <typename U, typename = enable_if_t<is_convertible_v<decay_t<U>, T>>>
        void append(U&& other) {
            auto newCapacity = nItems + 1;
            if (capacity < newCapacity && !reserve(newCapacity))
                return;

            new(buf + nItems) T(etl::forward<U>(other));
            ++nItems;
        }

        /// add all items from another vector to this vector
        void append(const Vector& other) {
            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity && !reserve(newCapacity)) 
                return;
            
            auto ptr = end();
            for (auto src = etl::begin(other); src != etl::end(other); ++src, ++ptr)
                new(ptr) T(*src);

            nItems = newCapacity;
        }

        /// add all items from another vector to this vector
        void append(Vector&& other) {
            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity && !reserve(newCapacity)) 
                return;

            auto ptr = end();
            for (auto src = etl::begin(other); src != etl::end(other); ++src, ++ptr)
                new(ptr) T(etl::move(*src));
            
            nItems = newCapacity;
            other.reset_delete_();
        }

        /// insert new item given the index
        template <typename U, typename = enable_if_t<is_convertible_v<decay_t<U>, T>>>
        void insert(int index, U&& item) {
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            if (index == int(nItems))
                return append(etl::forward<U>(item));

            auto newCapacity = nItems + 1;
            if (capacity < newCapacity && !reserve(newCapacity)) 
                return;

            new(buf + nItems) T(etl::move(buf[nItems - 1]));
            for (int i = nItems - 1; i > index; --i) {
                buf[i] = etl::move(buf[i - 1]);
            }

            buf[index] = T(etl::forward<U>(item));
            ++nItems;
        }

        /// insert another vector given the index
        void insert(int index, const Vector& other) {
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            if (index == int(nItems))
                return append(other);
            
            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity && !reserve(newCapacity)) 
                return;
            
            // shift
            for (int i = index; i < int(nItems); ++i) {
                int idx = i + etl::len(other);
                if (idx >= int(nItems))
                    new(buf + idx) T(etl::move(buf[i]));
                else
                    buf[idx] = etl::move(buf[i]);
            }

            // copy
            for (int i = 0; i < int(etl::len(other)); i++) {
                int idx = i + index;
                if (idx >= int(nItems))
                    new(buf + idx) T(other[i]);
                else
                    buf[idx] = other[i];
            }

            nItems += etl::len(other);
        }

        /// insert another vector given the index
        void insert(int index, Vector&& other) {
            Vector other_ = etl::move(other);
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            if (index == int(nItems))
                return append(etl::move(other));
            
            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity && !reserve(newCapacity)) 
                return;
            
            // shift
            for (int i = index; i < int(nItems); ++i) {
                int idx = i + etl::len(other);
                if (idx >= int(nItems))
                    new(buf + idx) T(etl::move(buf[i]));
                else
                    buf[idx] = etl::move(buf[i]);
            }

            // move
            for (int i = 0; i < int(etl::len(other)); i++) {
                int idx = i + index;
                if (idx >= int(nItems))
                    new(buf + idx) T(etl::move(other[i]));
                else
                    buf[idx] = etl::move(other[i]);
            }

            nItems += etl::len(other);
        }

        /// remove an item given the index
        bool remove_at(int index) {
            if (!is_valid_index_(index)) 
                return false;

            buf[index].~T();
            for (; index < int(nItems) - 1; ++index)
                buf[index] = etl::move(buf[index + 1]);

            --nItems;
            return true;
        }

        /// remove the first item found in the vector
        bool remove(const_reference x) {
            int index = etl::find(begin(), end(), x) - begin();
            return remove_at(index);
        }

        /// set new capacity, return true if success
        bool reserve(size_t newCapacity) {
            if (newCapacity == 0) {
                reset_delete_();
                return true;
            }
            else if (newCapacity == capacity) {
                return true;
            }

            auto newBuf = move_alloc_(newCapacity);
            if (newBuf == nullptr) 
                return false;
            
            for (size_t i = newCapacity; i < nItems; ++i) 
                buf[i].~T();

            deallocate(buf, capacity);
            reset_(newBuf, etl::min(nItems, newCapacity), newCapacity);
            return true;
        }

        /// set n items to 0, capacity remains the same
        void clear() { 
            for (auto ptr = begin(); ptr != end(); ++ptr)
                ptr->~T();
            nItems = 0;
        }

        /// shrink the capacity to fit the number of items
        bool shrink() { return reserve(nItems); }

        /// slice operator
        Iter<iterator> operator()(int start, int stop, int step = 1) { 
            return (start < stop && step > 0) || (start > stop && step < 0) ? 
                etl::iter(&operator[](start), &operator[](stop), step) : // valid index
                etl::iter(begin(), begin()); // invalid index
        }

        Iter<const_iterator> operator()(int start, int stop, int step = 1) const { 
            return (start < stop && step > 0) || (start > stop && step < 0) ? 
                etl::iter(&operator[](start), &operator[](stop), step) : // valid index
                etl::iter(begin(), begin()); // invalid index
        }

        /// create new vector by adding an item
        template <typename U, typename = enable_if_t<is_convertible_v<decay_t<U>, T>>>
        Vector operator+(U&& other) const {
            auto newCapacity = nItems + 1;
            auto temp = copy_alloc_(newCapacity);
            new(temp + nItems) T(etl::forward<U>(other));
            return { temp, newCapacity, newCapacity }; 
        }

        /// create new vector by adding another vector
        Vector operator+(const Vector& other) const { 
            auto newCapacity = nItems + etl::len(other);
            auto temp = copy_alloc_(newCapacity);

            
            auto dest = temp + nItems;
            for (auto &it : other) {
                new(dest) T(it);
                ++dest;
            }

            return { temp, newCapacity, newCapacity }; 
        }

        /// create new vector by adding another vector
        Vector operator+(Vector&& other) const {
            auto newCapacity = nItems + etl::len(other);
            auto temp = copy_alloc_(newCapacity);
            
            auto dest = temp + nItems;
            for (auto &it : other) {
                new(dest) T(etl::move(it));
                ++dest;
            }
            other.reset_delete_();

            return { temp, newCapacity, newCapacity };
        }

        /// append operator
        template <typename U>
        Vector& operator+=(U&& other) { append(etl::forward<U>(other)); return *this; }
    
    private:
        Vector(T* buffer, size_t nItems, size_t capacity) 
            : buf(buffer)
            , nItems(buffer ? nItems : 0)
            , capacity(buffer ? capacity : 0) {}

        iterator copy_alloc_(size_t newCapacity) const {
            auto temp = allocate(newCapacity);
            if (!temp) return temp;

            auto src = begin();
            for (auto dest = temp; dest != temp + newCapacity && src != end(); ++dest, ++src)
                new(dest) T(*src);
            return temp;
        }

        iterator move_alloc_(size_t newCapacity) {
            auto temp = allocate(newCapacity);
            if (!temp) return temp;

            auto src = begin();
            for (auto dest = temp; dest != temp + newCapacity && src != end(); ++dest, ++src)
                new(dest) T(etl::move(*src));
            return temp;
        }

        void reset_(iterator ptr = nullptr, size_t n = 0, size_t cap = 0) {
            buf = ptr;
            nItems = n;
            capacity = cap;
        }

        void reset_delete_(iterator ptr = nullptr, size_t n = 0, size_t cap = 0) {
            for (auto item = begin(); item != end(); ++item)
                item->~T();
            
            deallocate(buf, capacity);
            reset_(ptr, n, cap);
        }

        bool is_valid_index_(int& index) const {
            if (nItems == 0) return false;
            if (index < 0) index = int(nItems) + index; // allowing negative index
            if (index < 0 || size_t(index) >= nItems) return false; // out of range
            return true;
        }

        static iterator allocate(size_t n) {
            Alloc alloc;
            return alloc.allocate(n);
        }

        static void deallocate(iterator ptr, size_t n) {
            Alloc alloc;
            alloc.deallocate(ptr, n);
        }
    };

    /// create vector with variadic template function, the type can be explicitly specified
    template <typename T, typename A = etl::Allocator<T>, typename... Ts> auto
    vector(Ts&&...vals) { return Vector<T, A> { T(etl::forward<Ts>(vals))... }; }

    /// create vector with variadic template function with default allocator, the type can be implicitly specified
    template <typename T, typename... Ts> auto
    vector(T&& val, Ts&&...vals) { return Vector<T> { etl::forward<T>(val), T{etl::forward<Ts>(vals)}... }; }

    /// create vector from initializer list
    template <typename T, typename A = etl::Allocator<T>> auto
    vector(std::initializer_list<T>&& items) { return Vector<T, A>(etl::move(items)); }

    /// create empty vector and set the capacity
    template <typename T, typename A = etl::Allocator<T>> auto
    vector_reserve(size_t capacity) { return Vector<T, A>(capacity); }

    /// convert any sequence to a vector
    template <typename T, typename A = etl::Allocator<T>, typename Sequence> auto
    vectorize(Sequence&& seq) {
        auto res = vector<T, A>();

        if constexpr (has_len_v<remove_reference_t<Sequence>>)
            res.reserve(etl::len(seq));

        if constexpr (is_lvalue_reference_v<Sequence>) {
            for (decltype(auto) item : seq)
                res += item;
        } else {
            auto seq_ = etl::move(seq);
            for (decltype(auto) item : seq_)
                res += etl::move(item);
        }

        return res;
    }
        
    /// convert any sequence to a vector with default allocator
    template <typename Sequence> auto
    vectorize(Sequence&& seq) {
        using T = decay_t<decltype(*etl::begin(etl::declval<Sequence>()))>;
        return vectorize<T>(etl::forward<Sequence>(seq));
    }

    /// type traits
    template <typename T> struct is_vector : false_type {};
    template <typename T, typename A> struct is_vector<Vector<T, A>> : true_type {};
    template <typename T, typename A> struct is_vector<const Vector<T, A>> : true_type {};
    template <typename T, typename A> struct is_vector<volatile Vector<T, A>> : true_type {};
    template <typename T, typename A> struct is_vector<const volatile Vector<T, A>> : true_type {};
    template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

    template <typename T, typename A> struct remove_extent<Vector<T, A>> { typedef T type; };
    template <typename T, typename A> struct remove_extent<const Vector<T, A>> { typedef T type; };
    template <typename T, typename A> struct remove_extent<volatile Vector<T, A>> { typedef T type; };
    template <typename T, typename A> struct remove_extent<const volatile Vector<T, A>> { typedef T type; };
}

#endif //ETL_VECTOR_H
