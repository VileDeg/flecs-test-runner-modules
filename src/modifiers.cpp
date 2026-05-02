#include <modules/modifiers.h>

#include <random>
#include <limits>

#define MODIFIER_TARGET(_Type) \
	world.component<_Type>()  \
		.member<std::decay_t<decltype(std::declval<_Type>().value)>>("value");  \
	registerSystemAppliable<StringModifier, _Type>(world, "modify_" #_Type);  


namespace modifiers {

	template <typename Appl, typename Target>
	void registerSystemAppliable(flecs::world& world, const std::string& name) {
		world.system<const Appl, Target>(name.c_str())
			.each([](const Appl& appliable, Target& target) {
					appliable.apply(target);
				});
	}

	module::module(flecs::world& world) {
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

		world.component<StringOperation>()
			.constant("None", StringOperation::None)
			.constant("Lowercase", StringOperation::Lowercase)
			.constant("Uppercase", StringOperation::Uppercase);
		world.component<StringModifier>()
			.member<StringOperation>("operation");

		// Label
		// TODO: could make single system accept any type
		MODIFIER_TARGET(Label);
		MODIFIER_TARGET(LabelChar);
		MODIFIER_TARGET(LabelUnsignedChar);
	}
}