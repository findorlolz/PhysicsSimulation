#pragma once

#include <malloc.h>

template<typename T>
class nsVector
{
public:
	nsVector() : size_(0), capacity_(0), data_(0) {}
	~nsVector() { release(); }

	static const size_t s  = sizeof(T*);

	void push_back(const T& v)
	{
		if(size_ == capacity_)
		{
			capacity_ = capacity_ ? capacity_ * 2 : 3;
			std::cout << "New capasity " << capacity_ << std::endl;
			allocateAndCopy();
		}
		new((void*) (data_+size_)) T(v);
        ++size_;
	}

	void pop_back() { --size_; }
	
	void reserve(const size_t i) 
	{
		capacity_ = i;
		allocateAndCopy();
	}

	T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    T* begin() const { return data_; }
    T* end() const { return data_ + size_; }
	size_t size() const { return size_; }
	size_t getCapacity() const { return capacity_;}
	bool empty() const { return size_ == 0u; }
	T& back(){ return data_[size_-1]; }
	void clear() { size_ = 0; }

private:
	inline void allocateAndCopy()
	{
		T* newBegin = reinterpret_cast<T*>(_mm_malloc(capacity_*sizeof(T), 64));
		copyAndDeleteRange(data_ , data_+size_, newBegin);
		_mm_free(data_);
		data_ = newBegin;
	}

	static inline void copyAndDeleteRange(T* begin, T* end, T* newBegin)
	{
		while(begin != end)
		{
			new((void*) newBegin) T(*begin);
			begin->~T();
			++begin;
			++newBegin;
		}
	}

	static inline void deleteRange(T* begin, T* end)
	{
		while(begin != end)
		{
			begin->~T();
			++begin;
		}
	}

	inline void release()
	{
		if(data_ != nullptr)
		{
			deleteRange(data_ , data_ + size_);
			_mm_free(data_);
			data_ = nullptr;
		}
	}

	T* data_;
	size_t size_;
	size_t capacity_;
};