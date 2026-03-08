#pragma once
#include <flecs.h>
#include <vector>

namespace movement {
	// TODO: move types into movement namespace?

	//template <typename T>
	//class Vector2D {
	//public:
	//	T x, y;

	//	Vector2D(T x = 0, T y = 0) : x(x), y(y) {}

	//	// Helper to handle epsilon comparison for floating point types
	//	static bool approx_equal(T a, T b) {
	//		if constexpr (std::is_floating_point_v<T>) {
	//			return std::abs(a - b) <= std::numeric_limits<T>::epsilon();
	//		} else {
	//			return a == b;
	//		}
	//	}

	//	friend bool operator<(const Vector2D& lhs, const Vector2D& rhs) {
	//		if (!approx_equal(lhs.x, rhs.x)) {
	//			return lhs.x < rhs.x;
	//		}
	//		if (!approx_equal(lhs.y, rhs.y)) {
	//			return lhs.y < rhs.y;
	//		}
	//		return false;
	//	}

	//	friend bool operator>(const Vector2D& lhs, const Vector2D& rhs) {
	//		return rhs < lhs;
	//	}
	//};

	/*using Position = Vector2D<float>;
	using Direction = Vector2D<float>;
	using Vector2Df = Vector2D<float>;*/

	class Vector2D {
	public:
		float x, y;

		Vector2D(float x = 0, float y = 0) : x(x), y(y) {}

		// Helper to handle epsilon comparison for floating point types
		static bool approx_equal(float a, float b) {
			return std::abs(a - b) <= std::numeric_limits<float>::epsilon();
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

	/*struct PositionArray {
		std::vector<Position> positions;
	};*/

	struct Speed {
		float value;

		friend bool operator==(const Speed& lhs, const Speed& rhs) {
			return lhs.value == rhs.value;
		}

		friend bool operator<(const Speed& lhs, const Speed& rhs) {
			return lhs.value < rhs.value;
		}

		friend bool operator>(const Speed& lhs, const Speed& rhs) {
			return rhs < lhs;
		}
	};

	struct Velocity {
		float linearSpeed;
		Direction direction;
	};

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

	/*struct PathFollower {
		std::vector<Vector2D> waypoints;
		int currentWaypointIndex;
		float acceptanceRadius;
		bool isLooping;
	};*/

	struct TransformHistory {
		Vector2D previousPosition;
		float previousRotation;
		float deltaTimeAccumulator;
	};

	struct module {
		module(flecs::world& world);
	};
};
