#pragma once

#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <algorithm>
#include <cctype>
#include <functional>

#include <flecs.h>
#include "common.h"

namespace modifiers {

	template <typename T>
	using Appliable = modules_common::Appliable<T>;

	// --- String Component ---
	struct Label {
		std::string value;
	};

	enum class StringOperation {
		None,
		Lowercase,
		Uppercase,
	};

	struct StringModifier : Appliable<Label> {
		StringOperation operation;

		void apply(Label& label) const override {
			auto& data = label.value;
			bool isUpper = (operation == StringOperation::Uppercase);

			std::transform(data.begin(), data.end(), data.begin(),
				[isUpper](unsigned char c) {
					return isUpper ? std::toupper(c) : std::tolower(c);
				});
		}
	};

	struct module {
		module(flecs::world& world);
	};
}