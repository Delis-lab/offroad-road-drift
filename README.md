# offroad-road-drift

## Unreal Engine 5 vehicle modules

`Source/VehicleInput` contains the reusable `UVehicleInputComponent` input
module. `Source/OffRoadVehicle` contains `AOffRoadVehicleComponent`, an
`AWheeledVehiclePawn` subclass that consumes that input and controls a Chaos
wheeled vehicle. Add both `VehicleInput` and `OffRoadVehicle` to the game's
module dependencies, and enable the `ChaosVehiclesPlugin` project plugin.

Merge `Config/DefaultInput.ini` into the Unreal project's
`Config/DefaultInput.ini`. In the vehicle Blueprint or defaults, set the
movement component's four wheel setups to use `UOffRoadFrontWheel` for the
front bones and `UOffRoadRearWheel` for the rear bones. The wheel radius,
spring rate/preload, suspension travel, damping, steering, and brake settings
are tuned in those wheel classes; mesh-specific wheel bone names must come
from the vehicle skeletal mesh.

The pawn binds vehicle input automatically. It applies the speed-reduced
steering, throttle, brake, and handbrake to `UChaosWheeledVehicleMovementComponent`.
The R / gamepad North reset input rights the vehicle on the press edge; boost
applies forward force while held. AWD/rear-drive switching, manual shifting,
and winch controls are also bound by the pawn. The built-in UE5 differential
supports drive type and front/rear torque split, not a physical hard locker:
the lock toggle selects a fixed 50:50 center split and calls
`OnDifferentialLockChanged` for a project-specific locker implementation.
Assign `WinchAnchor` to an actor for the winch to pull toward. Call
`SetSurfaceType` from the ground/material handling code to apply the asphalt,
gravel, or mud wheel-friction multiplier.

Speed is in Unreal centimeters per second: steering tapers linearly to 45% of
its input value by 3000 cm/s. Keyboard throttle, brake, and steering ramp at
5.0 per second. Gamepad stick and trigger axes use a 0.12 deadzone.

### Controls

| Action | Keyboard | Gamepad |
| --- | --- | --- |
| Throttle | W, Up Arrow | Right Trigger |
| Brake / reverse | S, Down Arrow | Left Trigger |
| Steer | A / D, Left / Right Arrow | Left Stick X |
| Handbrake | Spacebar | South button (A / Cross) |
| Boost | Left Shift | East button (B / Circle) |
| Reset vehicle | R | North button (Y / Triangle) |
| Shift up / down | E / Q | Right / Left Shoulder |
| Toggle AWD / rear drive | V | D-pad Left |
| Toggle differential lock | C | D-pad Right |
| Winch (hold) | T | Left Stick Click |