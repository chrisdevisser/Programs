//Copyright Chris DeVisser 2013
//Code may be used freely in personal and commercial environments.
//Compiled with GCC 4.8.1 and MinGW - nuwen.net/mingw.html Distro 10.0

//A smart pointer for COM objects.
//based off of http://www.flipcode.com/archives/COM_Smart_Pointer.shtml

#ifndef DXUTIL_COMPTR_H
#define DXUTIL_COMPTR_H

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace DxUtil {

template<typename I>
struct ComPtr {
    ComPtr(std::nullptr_t = nullptr)
        : p_() {}

	explicit ComPtr(I *i)
        : p_(i) {}

	ComPtr(const ComPtr &other)
        : p_(other.p_) {
		if(p_)
			p_->AddRef();
	}

	ComPtr(ComPtr &&other)
        : p_(other.p_) {
        other.p_ = nullptr;
    }

    ~ComPtr() {
		if(p_)
			p_->Release();
		p_ = nullptr;
	}

    ComPtr &operator=(I *i) {
        p_ = i;
        return *this;
    }

	ComPtr &operator=(ComPtr other) {
		std::swap(p_, other.p_);

		if (p_) {
            p_->AddRef();
		}

		return *this;
	}

	ComPtr &operator=(ComPtr &&other) {
        p_ = other.p_;
        other.p_ = nullptr;
        return *this;
	}

	I **address() {
		if(p_) {
			throw std::logic_error("address() called on non-null interface.");
		}

		return &p_;
	}

	I *get() const {
	    return p_;
    }

	I *operator->() const {
	    return p_;
    }

	operator bool() const {
	    return p_ != nullptr;
    }

private:
	I *p_;
};

}

#endif
