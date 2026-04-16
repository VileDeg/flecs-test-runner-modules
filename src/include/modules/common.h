#pragma once

#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <algorithm>
#include <cctype>
#include <functional>

#include <flecs.h>

#define DERIVED_OPERATORS(_Type)\
friend bool operator!=(const _Type& lhs, const _Type& rhs) { \
	return !(lhs == rhs);																			 \
}																														 \
friend bool operator>(const _Type& lhs, const _Type& rhs) {	 \
	return rhs < lhs;																					 \
}																														 \
friend bool operator>=(const _Type& lhs, const _Type& rhs) { \
	return !(lhs < rhs);																			 \
}																														 \
friend bool operator<=(const _Type& lhs, const _Type& rhs) { \
	return !(lhs > rhs);																			 \
}

#define DELEGATED_OPERATORS(_Type, _Member)\
friend bool operator==(const _Type& lhs, const _Type& rhs) { \
	return lhs._Member == rhs._Member;												 \
	}																													 \
friend bool operator<(const _Type& lhs, const _Type& rhs) {	 \
	return lhs._Member < rhs._Member;													 \
}																														 \
DERIVED_OPERATORS(_Type)

namespace modules_common {

template <typename T>
struct Appliable {
	using Target = T;

	virtual void apply(T& target) const = 0;
};

}
