#include "cheats.hpp"

using namespace std::chrono_literals;
using namespace data;

void initBunnyHop() {
	while (true) {
        std::this_thread::sleep_for(20ms); // Cpu usage saver :pray:

        if (!GetAsyncKeyState(VK_SPACE)) {
            continue;
        }

        const uintptr_t jumpState = driver::read<uintptr_t>(clientDll + offsets::dwForceJump);
        const int32_t playerState = driver::read<int32_t>(localPlayer + offsets::fFlags);

        if (!playerState)
            continue;

        // If player is on the ground and status is pressing that wont allow jump again, then release and press again.
        if (playerState == status::ONGROUND) {
            if (jumpState == status::PRESS) {
                driver::write(clientDll + offsets::dwForceJump, status::RELEASE);
                std::this_thread::sleep_for(15ms);
                driver::write(clientDll + offsets::dwForceJump, status::PRESS);
            }
            else {
                driver::write(clientDll + offsets::dwForceJump, status::PRESS);
            }
        }
        else if (playerState == status::INAIR) {
            driver::write(clientDll + offsets::dwForceJump, status::RELEASE);
        }
	}
}