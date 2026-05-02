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

namespace movement {

	class Vector2D {
	public:
		float x, y;

		Vector2D(float x = 0, float y = 0) : x(x), y(y) {}

		// Helper to handle epsilon comparison for floating point types
		static bool approx_equal(float a, float b) {
			return std::abs(a - b) <= std::numeric_limits<float>::epsilon();
		}

		friend bool operator==(const Vector2D& lhs, const Vector2D& rhs) {
			return approx_equal(lhs.x, rhs.x) && approx_equal(lhs.y, rhs.y);
		}

		friend bool operator!=(const Vector2D& lhs, const Vector2D& rhs) noexcept {
			return !(lhs == rhs);
		}

		friend bool operator<(const Vector2D& lhs, const Vector2D& rhs) {
			if (!approx_equal(lhs.x, rhs.x)) {
				return lhs.x < rhs.x;
			}
			if (!approx_equal(lhs.y, rhs.y)) {
				return lhs.y < rhs.y;
			}
			return false;
		}

		friend bool operator>(const Vector2D& lhs, const Vector2D& rhs) {
			return rhs < lhs;
		}
	};

	using Position = Vector2D;
	using Direction = Vector2D;

	struct Speed {
		float value = -323;

		inline static const Vector2D direction = {1, 1};

		void apply(Position& pos) const {
			pos.x += direction.x * value;
			pos.y += direction.y * value;
		}

		DELEGATED_OPERATORS(Speed, value)
	};

	template <typename T>
	struct Container {
		T data;

		DELEGATED_OPERATORS(Container, data)
	};

	template <typename T, typename A, typename B>
	struct ContainerWrapper {
		Container<T> container;
		A memberA;
		B memberB;
		float value;

		DELEGATED_OPERATORS(ContainerWrapper, container)
	};

	using PositionVectorT = std::vector<Position>;	
	using PositionVectorTT = std::vector<PositionVectorT>;

	using PositionArrayT  = std::array<Position, 2>;

	using PositionVector = Container<PositionVectorT>;
	using PositionVectorNested = Container<PositionVectorTT>;

	using PositionArray  = Container<PositionArrayT>;

	struct Velocity {
		float linearSpeed;
		Direction direction;

		void apply(Position& pos) const {
			pos.x += direction.x * linearSpeed;
			pos.y += direction.y * linearSpeed;
		}
	};

	using WrappedPositionVector = 
		ContainerWrapper<PositionVectorT, Vector2D, Velocity>;

	// --- Advanced Movement Components ---
	struct Acceleration {
		Vector2D linear;
		float angular;
	};

	struct MovementState {
		Velocity currentVelocity;
		Acceleration currentAcceleration;
		float mass;
		float dragCoefficient;
	};

	struct TransformHistory {
		Vector2D previousPosition;
		float previousRotation;
		float deltaTimeAccumulator;
	};

	struct module {
		module(flecs::world& world);
	};
};
