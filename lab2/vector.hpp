#pragma once

#include <iostream>

const int min_capacity = 5;
const int MAX_BUFFER_SIZE = 1e7;

namespace vector {

    template <class T>
    class Vector {
    private:

        int len;
        int capacity;
        T* buffer = nullptr;

        bool is_lazy = false;

        bool try_increase_buffer();

        bool try_decrease_buffer();


    public:

        class iterator;

        template<class Type>
        friend void swap(Vector<Type>& A, Vector<Type>& B);

        class iterator {
        private:
            Vector<T>* vector;
            T* current;

        public:

            iterator() :
                vector(nullptr), current(nullptr) {}

            iterator(Vector<T>* vectorPtr, T* current):
                vector(vectorPtr), current(current) {}

            iterator(Vector<T>::iterator& sIterator): 
                vector(sIterator.vector), current(sIterator.current) {
            }


            iterator& operator=(const iterator& sIterator);
            iterator& operator++(); 
            iterator& operator--(); 
            const T operator*() const;

            friend class Vector;
        };


        Vector() : capacity(min_capacity), len(0) {
            buffer = new T[capacity];
        }

        Vector(bool _is_lazy) : capacity(min_capacity), len(0), is_lazy(_is_lazy) {
            buffer = new T[capacity];
        }

        ~Vector() {
            delete[] buffer;
            buffer = nullptr;
            len = 0;
            capacity = 0;
            is_lazy = false;
        }

        Vector(int _size) : len(_size) {
            if(len <= 0) 
                throw std::runtime_error("Error: Wrong given len");
            capacity = len * 2;
            buffer = new T[capacity];
        } 

        Vector(int _size, T element) : len(_size) {
            if(len <= 0) 
                throw std::runtime_error("Error: Wrong given len");

            capacity = len * 2;
            buffer = new T[capacity];

            for(int i = 0; i < capacity; ++i) 
                buffer[i] = element;
        } 

        Vector(int _size, bool _is_lazy) : len(_size), is_lazy(_is_lazy) {
            if(len <= 0) 
                throw std::runtime_error("Error: Wrong given len");
            capacity = len * 2;
            buffer = new T[capacity];
        } 

        Vector(int _size, T element, bool _is_lazy) : len(_size), is_lazy(_is_lazy) {
            if(len <= 0) 
                throw std::runtime_error("Error: Wrong given len");

            capacity = len * 2;
            buffer = new T[capacity];

            for(int i = 0; i < capacity; ++i) 
                buffer[i] = element;
        } 

        Vector::iterator begin();
        Vector::iterator end();
        void push_back(T element);
        void resize(int sz);
        void pop_back();

        T& operator[](const int& idx) {
            return buffer[idx];
        } 

        const T& operator[](const int& idx) const {
            return buffer[idx];
        }

        bool empty() const {
            return len == 0;
        }

        int size() const {
            return len;
        }

        int get_capacity() const {
            return capacity;
        }

    };

    template <class T>
    typename Vector<T>::iterator Vector<T>::begin() {
        Vector::iterator iterator;
        iterator.vector = this;
        iterator.current = this->buffer;
        return iterator;
    }

    template<class T>
    typename Vector<T>::iterator Vector<T>::end() {
        Vector::iterator iterator;
        iterator.vector = this;
        iterator.current = &(this->buffer[this->len]);
        return iterator;
    }

    template <class T>
    typename Vector<T>::iterator& Vector<T>::iterator::operator=(const Vector<T>::iterator& sIterator) {
        current = sIterator.current;
        vector = sIterator.vector;
    }

    template <class T>
    typename Vector<T>::iterator& Vector<T>::iterator::operator++() {
        ++current;
        return *this;
    }

    template <class T>
    typename Vector<T>::iterator& Vector<T>::iterator::operator--() {
        --current;
        return *this;
    }

    template<class T>
    inline bool Vector<T>::try_increase_buffer() {
        
        if(is_lazy) {
            if(capacity > len ) 
                return true;
        } else {
            if(capacity > len * 2)
                return true;
        }

        int new_capacity = std::min(capacity * 2, MAX_BUFFER_SIZE);
        T* tmp_buffer = new T[new_capacity];

        if(tmp_buffer == nullptr)
            return false;

        for(int i = 0; i < len; ++i) {
            tmp_buffer[i] = buffer[i];
        }

        delete[] buffer;
        buffer = tmp_buffer;
        capacity = new_capacity;
        return true;
    }

    template<class T>
    inline bool Vector<T>::try_decrease_buffer() {
        if(capacity < len * 4) 
            return true;
        
        int new_capacity = std::min(capacity / 2, min_capacity);
        T* tmp_buffer = new T[new_capacity];

        if(tmp_buffer == nullptr) 
            return false;
        
        for(int i = 0; i < len; ++i) 
            tmp_buffer[i] = buffer[i];
        
        delete[] buffer;
        buffer = tmp_buffer;
        capacity = new_capacity;
        return false;
    }

    template<class T>
    inline void Vector<T>::push_back(T element) {
        if(!try_increase_buffer()) 
            throw std::runtime_error("Error: Can't increase buffer");

        buffer[len++] = element;       
    }

    template<class T>
    inline void Vector<T>::pop_back() {
        if(!try_decrease_buffer()) 
            throw std::runtime_error("Error: Can't decrease buffer");
        
        ~buffer[len--];
    }

    template <class T>
    void swap(Vector<T>& A, Vector<T>& B) {
        T* tmp_pointer = A.buffer;
        A.buffer = B.buffer;
        B.buffer = tmp_pointer;
    }

    template <class T>
    void Vector<T>::resize(int sz) {
        if(sz == this->len) {
            return;
        }
        if((sz < this->len) || (sz <= this->capacity)) {
            this->len = sz;
            return;
        }

        T *newBuf = new T[sz];
        for (int i = 0; i < this->len; ++i) {
            newBuf[i] = this->buffer[i];
        }
        this->len = sz;
        this->capacity = sz;
        delete[] buffer;
        buffer = newBuf;
    }

}

