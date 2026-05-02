#pragma once

#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <algorithm>
#include <cctype>
#include <functional>
#include <variant>

#include <flecs.h>
#include "common.h"

namespace modifiers {

	struct Label {
		std::string value;
	};

	enum class StringOperation {
		None,
		Lowercase,
		Uppercase,
	};


	struct LabelChar {
		char value;
	};

	struct LabelUnsignedChar {
		unsigned char value;
	};

	struct ModifierVisitor {
		using Target = std::variant<Label, LabelChar, LabelUnsignedChar>;


		StringOperation operation;

		// Helper for char-level transformation to avoid duplication
		inline unsigned char transform(unsigned char c) const {
			return (operation == StringOperation::Uppercase)
				? static_cast<unsigned char>(std::toupper(c))
				: static_cast<unsigned char>(std::tolower(c));
		}

		// Overload for Label (String-like)
		void operator()(Label& label) const {
			std::transform(label.value.begin(), label.value.end(), label.value.begin(),
				[this](unsigned char c) { return transform(c); });
		}

		// Overload for LabelChar
		void operator()(LabelChar& label) const {
			label.value = static_cast<char>(transform(static_cast<unsigned char>(label.value)));
		}

		// Overload for LabelUnsignedChar
		void operator()(LabelUnsignedChar& label) const {
			label.value = transform(label.value);
		}
	};

	struct StringModifier {
		using TargetVariant = ModifierVisitor::Target;
		StringOperation operation;

		template <typename T>
		void apply(T& specificTarget) const {
			ModifierVisitor{ operation }(specificTarget);
		}

		void apply(TargetVariant& variantTarget) const {
			std::visit(ModifierVisitor{ operation }, variantTarget);
		}
	};

	struct module {
		module(flecs::world& world);
	};
}