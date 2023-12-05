#pragma once

#include <unordered_map>
#include <cassert>
#include <cstdint>
#include <memory>
#include <bitset>
#include <array>
#include <queue>

using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;

using ComponentType = uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

class EntityManager {
public:
	EntityManager() {
		// Initialize the queue with all possible entity IDs
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
			availableEntities.push(entity);
		}
	}

	Entity createEntity() {
		assert(livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

		// Take an ID from the front of the queue
		Entity id = availableEntities.front();
		availableEntities.pop();
		++livingEntityCount;

		return id;
	}

	void destroyEntity(Entity entity) {
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Invalidate the destroyed entity's signature
		//signatures[entity].reset();

		// Put the destroyed ID at the back of the queue
		availableEntities.push(entity);
		--livingEntityCount;
	}

	void setSignature(Entity entity, Signature signature) {
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Put this entity's signature into the array
		signatures[entity] = signature;
	}

	Signature getSignature(Entity entity) {
		assert(entity < MAX_ENTITIES && "Entity out of range.");

		// Get this entity's signature from the array
		return signatures[entity];
	}
private:
	// Queue of unused entity IDs
	std::queue<Entity> availableEntities{};

	// Array of signatures where the index corresponds to the entity ID
	std::array<Signature, MAX_ENTITIES> signatures{};

	// Total living entities - used to keep limits on how many exist
	uint32_t livingEntityCount{};
};