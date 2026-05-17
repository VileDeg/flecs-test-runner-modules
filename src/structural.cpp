#include <modules/structural.h>

#include <random>
#include <iostream>

#include <limits>


namespace structural {

	module::module(flecs::world& world) {
		world.component<std::string>()
			.on_equals()
			.on_compare()
			.opaque(flecs::String) // Opaque type that maps to string
			.serialize(
				[](const flecs::serializer* s, const std::string* data) {
					const char* str = data->c_str();
					return s->value(flecs::String, &str); // Forward to serializer
				}
			).assign_string(
				[](std::string* data, const char* value) {
					*data = value; // Assign new value to std::string
				}
			);

		world.component<SpawnedDataReceiver>();
		world.component<SpawnedData>()
			.member<std::string>("by")
			.member<int>("data");

		world.component<Spawned>()
			.member<std::string>("by");

		world.component<Spawner>()
			.member<std::string>("name")
			.member<int>("count");

		flecs::query<Spawner> spawners =
			world.query<Spawner>("Spawners");

		world.system("SpawnData")
			.with<SpawnedDataReceiver>()
			.immediate()
			.each(
				[&spawners](flecs::entity e) {
					spawners.each(
						[&](const Spawner& spawner) {
							std::random_device rd;
							std::mt19937 gen(rd());
							std::uniform_int_distribution<> distr(
								std::numeric_limits<int>::min(),
								std::numeric_limits<int>::max()
							);

							int random_num = distr(gen);
							e.set<SpawnedData>({ spawner.name, random_num });
						}
					);
				}
			);

		world.system<Spawner>("SpawnEntity")
			.immediate()
			.each(
				[](flecs::iter& it, size_t, Spawner& spawner) {
					std::string entityName = "Spawned by " + spawner.name +
						" [" + std::to_string(spawner.count) + "]";

					auto e = it.world().entity(entityName.c_str())
						.set<Spawned>(Spawned{ spawner.name });
				}
			);

		world.system<const Spawned>("DeleteSpawnedEntities")
			.immediate()
			.each(
				[&spawners](flecs::entity e, const Spawned& spawned) {
					std::cout << "Run system DeleteSpawnedEntities\n";

					spawners.each(
						[&](Spawner& spawner) {
							if (spawner.name == spawned.by) {
								e.destruct();
								--spawner.count;
							}
						}
					);
				}
			);

		world.system<SpawnedData>("DeleteSpawnedData")
			.immediate()
			.with<SpawnedDataReceiver>()
			.each(
				[&spawners](flecs::entity e, SpawnedData& spawnedData) {
					
					spawners.each(
						[&](Spawner& spawner) {
							if (spawner.name == spawnedData.by) {
								e.remove<SpawnedData>();
							}
						}
					);
				}
			);
	}
};