//Copyright Chris DeVisser 2013
//Code may be used freely in personal and commercial environments.

//An RAII Windows API handle.
//Might be changed to a less hackish solution.
//Shouldn't interfere with things like HMODULE that only unload after frees == loads.

#pragma once
#ifndef W32_HANDLE_H
#define W32_HANDLE_H

#include <memory>
#include <type_traits>
#include <utility>

namespace w32 {

template<typename HandleType>
struct Handle {
    static_assert(std::is_pointer<HandleType>::value, "Handle template argument must be pointer.");

    //could take things like FreeLibrary, but they're all __stdcall
    using deleter_type = void(*)(HandleType);

    //assumes ownership
    explicit Handle(HandleType existing = nullptr, deleter_type del = [](HandleType){})
        : handle_{existing, del} {}

    HandleType handle() const {
        return handle_.get();
    }

private:
    //internal handle type without the pointer; like HWND__ or HMODULE__, but subject to change
    using underlying_type = typename std::remove_cv<typename std::remove_reference<decltype(*std::declval<HandleType>())>::type>::type;

    std::shared_ptr<underlying_type> handle_;
};

}

#endif
