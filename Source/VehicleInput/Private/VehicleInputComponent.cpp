#include "VehicleInputComponent.h"

#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

namespace
{
    constexpr float GamepadDeadzone = 0.12f;
}

UVehicleInputComponent::UVehicleInputComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleInputComponent::BindInput(UInputComponent* InputComponent)
{
    check(InputComponent);

    InputComponent->BindAxis(TEXT("VehicleKeyboardThrottle"), this, &ThisClass::SetKeyboardThrottle);
    InputComponent->BindAxis(TEXT("VehicleKeyboardBrake"), this, &ThisClass::SetKeyboardBrake);
    InputComponent->BindAxis(TEXT("VehicleKeyboardSteer"), this, &ThisClass::SetKeyboardSteer);
    InputComponent->BindAxis(TEXT("VehicleGamepadThrottle"), this, &ThisClass::SetGamepadThrottle);
    InputComponent->BindAxis(TEXT("VehicleGamepadBrake"), this, &ThisClass::SetGamepadBrake);
    InputComponent->BindAxis(TEXT("VehicleGamepadSteer"), this, &ThisClass::SetGamepadSteer);

    InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ThisClass::HandbrakePressed);
    InputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ThisClass::HandbrakeReleased);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Bottom, IE_Pressed, this, &ThisClass::HandbrakePressed);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Bottom, IE_Released, this, &ThisClass::HandbrakeReleased);

    InputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &ThisClass::BoostPressed);
    InputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &ThisClass::BoostReleased);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Right, IE_Pressed, this, &ThisClass::BoostPressed);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Right, IE_Released, this, &ThisClass::BoostReleased);

    InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ThisClass::ResetPressed);
    InputComponent->BindKey(EKeys::R, IE_Released, this, &ThisClass::ResetReleased);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Top, IE_Pressed, this, &ThisClass::ResetPressed);
    InputComponent->BindKey(EKeys::Gamepad_FaceButton_Top, IE_Released, this, &ThisClass::ResetReleased);
}

FVehicleInputState UVehicleInputComponent::GetInputState(float SpeedCmPerSecond) const
{
    FVehicleInputState Result = InputState;
    const float SpeedAlpha = FMath::Clamp(
        FMath::Abs(SpeedCmPerSecond) / FullSteeringReductionSpeed,
        0.0f,
        1.0f);
    const float SteeringScale = FMath::Lerp(1.0f, MinimumHighSpeedSteering, SpeedAlpha);
    Result.Steer *= SteeringScale;
    return Result;
}

void UVehicleInputComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    SmoothedKeyboardThrottle = FMath::FInterpTo(
        SmoothedKeyboardThrottle, KeyboardThrottleTarget, DeltaTime, KeyboardRampSpeed);
    SmoothedKeyboardBrake = FMath::FInterpTo(
        SmoothedKeyboardBrake, KeyboardBrakeTarget, DeltaTime, KeyboardRampSpeed);
    SmoothedKeyboardSteer = FMath::FInterpTo(
        SmoothedKeyboardSteer, KeyboardSteerTarget, DeltaTime, KeyboardRampSpeed);

    InputState.Throttle = FMath::Clamp(
        FMath::Max(SmoothedKeyboardThrottle, GamepadThrottle), 0.0f, 1.0f);
    InputState.Brake = FMath::Clamp(
        FMath::Max(SmoothedKeyboardBrake, GamepadBrake), 0.0f, 1.0f);
    InputState.Steer = FMath::Clamp(SmoothedKeyboardSteer + GamepadSteer, -1.0f, 1.0f);
}

float UVehicleInputComponent::ApplyDeadzone(float Value)
{
    const float Magnitude = FMath::Abs(Value);
    if (Magnitude <= GamepadDeadzone)
    {
        return 0.0f;
    }

    return FMath::Sign(Value) * FMath::Clamp(
        (Magnitude - GamepadDeadzone) / (1.0f - GamepadDeadzone),
        0.0f,
        1.0f);
}

void UVehicleInputComponent::SetKeyboardThrottle(float Value)
{
    KeyboardThrottleTarget = FMath::Clamp(Value, 0.0f, 1.0f);
}

void UVehicleInputComponent::SetKeyboardBrake(float Value)
{
    KeyboardBrakeTarget = FMath::Clamp(Value, 0.0f, 1.0f);
}

void UVehicleInputComponent::SetKeyboardSteer(float Value)
{
    KeyboardSteerTarget = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UVehicleInputComponent::SetGamepadThrottle(float Value)
{
    GamepadThrottle = ApplyDeadzone(Value);
}

void UVehicleInputComponent::SetGamepadBrake(float Value)
{
    GamepadBrake = ApplyDeadzone(Value);
}

void UVehicleInputComponent::SetGamepadSteer(float Value)
{
    GamepadSteer = ApplyDeadzone(Value);
}

void UVehicleInputComponent::HandbrakePressed()
{
    InputState.Handbrake = true;
}

void UVehicleInputComponent::HandbrakeReleased()
{
    InputState.Handbrake = false;
}

void UVehicleInputComponent::BoostPressed()
{
    InputState.Boost = true;
}

void UVehicleInputComponent::BoostReleased()
{
    InputState.Boost = false;
}

void UVehicleInputComponent::ResetPressed()
{
    InputState.ResetVehicle = true;
}

void UVehicleInputComponent::ResetReleased()
{
    InputState.ResetVehicle = false;
}
