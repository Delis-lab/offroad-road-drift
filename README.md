# offroad-road-drift

## Vehicle input (Unreal Engine C++)

`Source/VehicleInput` contains a reusable `UVehicleInputComponent` input module.
Add `VehicleInput` to your game's module dependencies and add the component to
the vehicle pawn. Merge `Config/DefaultInput.ini` into the Unreal project's
`Config/DefaultInput.ini` so the named axes are available. In the pawn's
`SetupPlayerInputComponent`, call `VehicleInputComponent->BindInput(PlayerInputComponent)`.

The vehicle controller can read the current input and account for speed when
applying its steering angle:

```cpp
const FVehicleInputState Input = VehicleInputComponent->GetInputState(GetVelocity().Size());
```

Apply `Input.Throttle`, `Input.Brake`, and `Input.Steer` to the vehicle's
drivetrain and steering; handle `Input.Handbrake`, `Input.Boost`, and
`Input.ResetVehicle` in the corresponding vehicle systems. The reset flag is
held while its key/button is held, so the vehicle should trigger a reset on its
press edge. Speed is in Unreal centimeters per second: steering tapers linearly
to 45% of its input value by 3000 cm/s. Keyboard throttle, brake, and steering
ramp at 5.0 per second. Gamepad stick and trigger axes use a 0.12 deadzone.

### Controls

| Action | Keyboard | Gamepad |
| --- | --- | --- |
| Throttle | W, Up Arrow | Right Trigger |
| Brake / reverse | S, Down Arrow | Left Trigger |
| Steer | A / D, Left / Right Arrow | Left Stick X |
| Handbrake | Spacebar | South button (A / Cross) |
| Boost | Left Shift | East button (B / Circle) |
| Reset vehicle | R | North button (Y / Triangle) |