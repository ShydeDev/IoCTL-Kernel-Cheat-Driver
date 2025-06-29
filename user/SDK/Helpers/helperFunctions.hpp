#pragma once

#include "../Data/offsets.hpp"
#include "../Memory/memory.hpp"
#include "../Data/data.hpp"
#include <functional>

// https://github.com/roflmuffin/CounterStrikeSharp/blob/main/managed/CounterStrikeSharp.API/Modules/Entities/EntitySystem.cs

constexpr uint16_t maxEntities = 1 << 15;							// 32768
constexpr uint16_t maxEntitiesPerChunk = 1 << 9;					// 512

constexpr uint8_t sizeOfEntityIdentity = 0x78;						// 120
constexpr uint8_t handleOffset = 0x10;								// 16

void forEachEntity(std::function<void(uintptr_t entity, int index)> callback) {
	const uintptr_t entityList = driver::read<uintptr_t>(data::clientDll + offsets::entityList);

	for (int i = 1; i < maxEntities; ++i) {
		const uintptr_t listEntry = driver::read<uintptr_t>(entityList + 8 * (i / maxEntitiesPerChunk) + handleOffset);
		const uintptr_t enemy = driver::read<uintptr_t>(listEntry + sizeOfEntityIdentity * (i % maxEntitiesPerChunk));

		if (enemy) {
			callback(enemy, i);
		}
	}
}