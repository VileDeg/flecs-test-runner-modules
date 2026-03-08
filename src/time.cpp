#include <modules/time.h>
#include <thread>
#include <chrono>

namespace waiting {
    module::module(flecs::world& world) {
        //world.set_threads(4);

        world.component<Timer>()
            .member<int>("time");

        world.component<TimeStep>()
            .member<int>("step");

        world.system<Timer, const TimeStep>("wait")
            .kind(0) // Prevent being registered to a pipeline
            //.multi_threaded() // TODO:
            .each([](Timer& t, const TimeStep& s) {
                // This intentionally blocks the system thread for the duration.
                std::this_thread::sleep_for(std::chrono::milliseconds(s.step));
                t.time += s.step;
            });
    }
};