#include <modules/movement.h>
#include <string>

template <typename Elem, typename Vector = std::vector<Elem>>
flecs::opaque<Vector, Elem> stdVectorSupport(flecs::world& world) {
	return flecs::opaque<Vector, Elem>()
		.as_type(world.vector<Elem>())

		// Forward elements of std::vector value to serializer
		.serialize([](const flecs::serializer* s, const Vector* data) {
		for (const auto& el : *data) {
			s->value(el);
		}
		return 0;
			})

		// Return vector count
		.count([](const Vector* data) {
		return data->size();
			})

		// Resize contents of vector
		.resize([](Vector* data, size_t size) {
		data->resize(size);
			})

		// Ensure element exists, return pointer
		.ensure_element([](Vector* data, size_t elem) {
		if (data->size() <= elem) {
			data->resize(elem + 1);
		}

		return &data->data()[elem];
			});
}

template <typename Elem, size_t Size, typename Array = std::array<Elem, Size>>
flecs::opaque<Array, Elem> std_array_support(flecs::world& world) {
	return flecs::opaque<Array, Elem>()
		.as_type(world.array<Elem>(Size))

		// Forward elements of std::vector value to serializer
		.serialize([](const flecs::serializer* s, const Array* data) {
		for (const auto& el : *data) {
			s->value(el);
		}
		return 0;
			})

		.count([](const Array* data) {
		return data->size();
			});
}



template <typename Appl, typename Target>
void registerSystemAppliable(flecs::world& world, const std::string& name) {
	world.system<const Appl, Target>(name.c_str())
		.each([](const Appl& appliable, Target& target) {
			appliable.apply(target);
		});
}

template <typename Appl, typename TargetVector>
void registerSystemAppliableContainer(flecs::world& world, const char* name) {
	world.system<const Appl, TargetVector>(name)
		.each([](const Appl& appliable, TargetVector& targetVector) {
				for (auto& target : targetVector.data) {
					appliable.apply(target);
				}
			});
}

namespace movement {
	module::module(flecs::world& world) {
		// String
		world.component<std::string>()
			.on_equals()
			.on_compare()
			.opaque(flecs::String) // Opaque type that maps to string
			.serialize([](const flecs::serializer* s, const std::string* data) {
				const char* str = data->c_str();
				return s->value(flecs::String, &str); // Forward to serializer
			})
			.assign_string([](std::string* data, const char* value) {
				*data = value; // Assign new value to std::string
			});

		// Vector2D Reflection
		world.component<Vector2D>()
			.on_equals()
			.on_compare()
			.member<float>("x")
			.member<float>("y");

		world.component<Speed>()
			.on_equals()
			.on_compare()
			// Have to pass offset manually because struct is polymorphic, 
			// it is not a POD type anymore
			.member<float>("value", 0, offsetof(Speed, value));

		// Vectors
		world.component<PositionVectorT>()
			.opaque(stdVectorSupport<Position>)
			.on_equals()
			.on_compare();
		world.component<PositionVectorTT>()
			.opaque(stdVectorSupport<PositionVectorT>)
			.on_equals()
			.on_compare();

		// Containers
		world.component<PositionVector>("PositionVector")
			.on_equals()
			.on_compare()
			.member<PositionVectorT>("data");
		world.component<PositionVectorNested>("PositionVectorNested")
			.on_equals()
			.on_compare()
			.member<PositionVectorTT>("data");
		
		// Label
		world.component<Label>()
			.member<std::string>("value");
		world.component<StringOperation>()
			.constant("None", StringOperation::None)
			.constant("Lowercase", StringOperation::Lowercase)
			.constant("Uppercase", StringOperation::Uppercase);
		world.component<StringModifier>()
			.member<StringOperation>("operation", 0, offsetof(StringModifier, operation));

		// Nested types
		world.component<Velocity>()
			// Have to pass offset manually because struct is polymorphic, 
			// it is not a POD type anymore
			.member<float>("linearSpeed", 0, offsetof(Velocity, linearSpeed))
			.member<Vector2D>("direction", 0, offsetof(Velocity, direction));
		world.component<Acceleration>()
			.member<Vector2D>("linear")
			.member<float>("angular");
		world.component<MovementState>()
			.member<Velocity>("currentVelocity")
			.member<Acceleration>("currentAcceleration")
			.member<float>("mass")
			.member<float>("dragCoefficient");
		world.component<TransformHistory>()
			.member<Vector2D>("previousPosition")
			.member<float>("previousRotation")
			.member<float>("deltaTimeAccumulator");

		// Nested container
		world.component<WrappedPositionVector>("WrappedPositionVector")
			.on_equals()
			.on_compare()
			.member<PositionVector>("container")
			.member<Vector2D>("memberA")
			.member<Velocity>("memberB")
			.member<float>("value");

		registerSystemAppliable<Velocity, Position>(world, "move_Velocity");
		registerSystemAppliable<Speed, Position>(world, "move_Speed");

		registerSystemAppliable<StringModifier, Label>(world, "modify_Label");

		registerSystemAppliableContainer<Speed, PositionVector>(world, "moveVector_Speed");
		registerSystemAppliableContainer<Speed, PositionArray>(world, "moveArray_Speed");

		world.system<Position, MovementState>("IntegrateMovement")
			.each([](flecs::iter& it, size_t i, Position& p, MovementState& s) {
					float dt = it.delta_time();

					// 1. Apply Drag/Friction to linear speed
					s.currentVelocity.linearSpeed *= (1.0f - s.dragCoefficient * dt);

					// 2. Integrate Acceleration into Velocity Direction
					s.currentVelocity.direction.x += s.currentAcceleration.linear.x * dt;
					s.currentVelocity.direction.y += s.currentAcceleration.linear.y * dt;

					// 3. Integrate Velocity into Position
					p.x += s.currentVelocity.direction.x * s.currentVelocity.linearSpeed * dt;
					p.y += s.currentVelocity.direction.y * s.currentVelocity.linearSpeed * dt;
				});

  }
};