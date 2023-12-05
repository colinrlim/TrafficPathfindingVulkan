#pragma once

#include "ComponentArray.h"

class ComponentManager {
public:
	template <typename T>
	void registerComponent() {
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) == componentTypes.end() && "Registering component type more than once.");

		// Add this component type to the component type map
		componentTypes.insert({ typeName, nextComponentType });

		// Create a ComponentArray pointer and add it to the component arrays map
		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		// Increment the value so that the next component registered will be different
		++nextComponentType;
	}

	template <typename T>
	ComponentType getComponentType() {
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");

		// Return this component's type - used for creating signatures
		return componentTypes[typeName];
	}

	template <typename T>
	void addComponent(Entity entity, T component) {
		// Add a component to the array for an entity
		getComponentArray<T>()->insertData(entity, component);
	}

	template <typename T>
	void removeComponent(Entity entity) {
		// Remove a component from the array for an entity
		getComponentArray<T>()->removeData(entity);
	}

	template <typename T>
	T& getComponent(Entity entity) {
		// Get a reference to a component from the array for an entity
		return getComponentArray<T>()->getData(entity);
	}

	void entityDestroyed(Entity entity) {
		// Notify each component array that an entity has been destroyed
		// If it has a component for that entity, it will remove it
		for (auto const& pair : componentArrays) {
			auto const& component = pair.second;

			component->entityDestroyed(entity);
		}
	}
private:
	// Map from type string pointer to a component type
	std::unordered_map<const char*, ComponentType> componentTypes{};

	// Map from type string pointer to a component array
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays{};

	// The component type to be assigned to the next registered component - starting at 0
	ComponentType nextComponentType{};

	// Convenience function to get the statically casted pointer to the ComponentArray of type T.
	template <typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray() {
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use.");

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
	}
};