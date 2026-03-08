#include <modules/test.h>
#include <string>

template <typename Elem, typename Vector = std::vector<Elem>>
flecs::opaque<Vector, Elem> std_vector_support(flecs::world& world) {
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

		// TODO?

		// Resize contents of vector
		//.resize([](Array* data, size_t size) {
		//		data->resize(size);
		//	})

		// Ensure element exists, return pointer
		//.ensure_element([](Array* data, size_t elem) {
		//		if (data->size() <= elem) {
		//			data->resize(elem + 1);
		//		}

		//		return &data->data()[elem];
		//	});
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
			.opaque(std_vector_support<Position>)
			.on_equals()
			.on_compare();
		world.component<PositionVectorTT>()
			.opaque(std_vector_support<PositionVectorT>)
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
		
			
		/*
		world.component<std::array<Position, 2>>()
			.opaque(std_array_support<Position, 2>);

		world.component<PositionArray>("PositionArray")
			.on_equals()
			.on_compare()
			.member<std::array<Position, 2>>("data");
		*/

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

		registerSystemAppliableContainer<Speed, PositionVector>(world, "moveVector_Speed");
		registerSystemAppliableContainer<Speed, PositionArray>(world, "moveArray_Speed");

#if 0
		//*/
    world.system<const Velocity>("move")
			.with<Velocity>()
			.with<Position>()
      .each([&world ](flecs::iter& it, size_t i, const Velocity& velocity) {
					auto pos = it.field<Vector2D>(1);
					const auto applyVelocity = [](Position& pos, const Velocity& velocity) {
							pos.x += velocity.direction.x * velocity.linearSpeed;
							pos.y += velocity.direction.y * velocity.linearSpeed;
						};

					if (it.id(0) == world.id<Vector2D>()) {
						auto pos = it.range().get<Vector2D>();
						applyVelocity(*pos, velocity);
					}
				});

		/*/
		world.system<const Velocity>("move")
			.with<Velocity>()
			.with<Position>().or_()
			.with<PositionArray>()
			.each([&world ](flecs::iter& it, size_t i, const Velocity& velocity) {
					auto pos = it.field<Vector2D>(1);
					auto posArray = it.field<PositionArray>(1);

					const auto applyVelocity = [](Position& pos, const Velocity& velocity) {
							pos.x += velocity.direction.x * velocity.linearSpeed;
							pos.y += velocity.direction.y * velocity.linearSpeed;
						};

					if (it.id(0) == world.id<Vector2D>()) {
						auto pos = it.range().get<Vector2D>();
						applyVelocity(*pos, velocity);
					} else if (it.id(0) == world.id<PositionArray>()) {
						auto posArray = it.range().get<PositionArray>();

						for (auto& pos : posArray->positions) {
							applyVelocity(pos, velocity);
						}
					}
				});
		//*/
#endif

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