# The Rescue+ Game Engine

Rescue+ is a in-progress 3D game engine written in C++ using the DirectX 11 graphics api.

The original Rescue engine was created for the [Snake on the Water](https://github.com/MAClavell/Snake-on-the-Water) project. Since then I have been solo working on the engine on my own time and for IGME 550 (Game Engine Design and Development). My goal is to create an easy and optimized pure C++ environment for experiementing with gameplay programming.

The current feature list (that I can remember, there's probably more) includes:
* GameObject/Component system for ease of implementing and reusing behaviours.
* GameObject parenting that updates child transforms.
* PhysX API including RigidBodies, Colliders, and collision callbacks.
* Bucket-sorted rendering to minimize the memory sent to the GPU.
* A [Lock-free work stealing Job System](https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/)
* A Resource Manager for loading multiple types of assets.
* A Input Manager for detecting input.
* Support for diffuse lighting or Physically Based Rendering.
* Transparency/blending.
* Shadows for directional lights.
* Config file (currently only supports framerate caps).

Stay tuned for engine demo code (in the meantime, look at [Game.cpp](https://github.com/MAClavell/Rescue-Plus-Game-Engine/blob/master/Rescue-Plus-Game-Engine/Game-App/Game.cpp)/[Scene.cpp](https://github.com/MAClavell/Rescue-Plus-Game-Engine/blob/master/Rescue-Plus-Game-Engine/Game-App/Scene.cpp))...
