#pragma once

#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <algorithm>
#include <cctype>
#include <functional>

#include <flecs.h>

namespace structural {

	/**
	 * Tag for entity that can accept a component from spawner.
	 */
	struct SpawnedDataReceiver {};

	/**
	 * Component spawned by spawner.
	 */
	struct Spawned {
		std::string by; // spawned by name
	};

	struct Spawner {
		std::string name;
		int count; // num spawned so far
	};

	struct SpawnedData {
		std::string by; // spawned by name
		int data; // random generated data
	};

	struct module {
		module(flecs::world& world);
	};
}