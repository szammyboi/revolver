#pragma once

#include <memory>

template <typename T>
using Owned = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Owned<T> CreateOwned(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Shared = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Shared<T> CreateShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
