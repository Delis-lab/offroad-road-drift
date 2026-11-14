#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleInputComponent.generated.h"

class UInputComponent;

USTRUCT(BlueprintType)
struct VEHICLEINPUT_API FVehicleInputState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Input")
    float Throttle = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Input")
    float Brake = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Input")
    float Steer = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Input")
    bool Handbrake = false;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Input")
    bool ResetVehicle = false;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Input")
    bool Boost = false;
};

UCLASS(ClassGroup = (Vehicle), meta = (BlueprintSpawnableComponent))
class VEHICLEINPUT_API UVehicleInputComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVehicleInputComponent();

    void BindInput(UInputComponent* InputComponent);

    UFUNCTION(BlueprintPure, Category = "Vehicle Input")
    FVehicleInputState GetInputState(float SpeedCmPerSecond) const;

protected:
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

private:
    static float ApplyDeadzone(float Value);

    void SetKeyboardThrottle(float Value);
    void SetKeyboardBrake(float Value);
    void SetKeyboardSteer(float Value);
    void SetGamepadThrottle(float Value);
    void SetGamepadBrake(float Value);
    void SetGamepadSteer(float Value);

    void HandbrakePressed();
    void HandbrakeReleased();
    void BoostPressed();
    void BoostReleased();
    void ResetPressed();
    void ResetReleased();

    UPROPERTY(EditAnywhere, Category = "Vehicle Input", meta = (ClampMin = "0.0"))
    float KeyboardRampSpeed = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Vehicle Input", meta = (ClampMin = "1.0"))
    float FullSteeringReductionSpeed = 3000.0f;

    UPROPERTY(EditAnywhere, Category = "Vehicle Input", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MinimumHighSpeedSteering = 0.45f;

    float KeyboardThrottleTarget = 0.0f;
    float KeyboardBrakeTarget = 0.0f;
    float KeyboardSteerTarget = 0.0f;
    float SmoothedKeyboardThrottle = 0.0f;
    float SmoothedKeyboardBrake = 0.0f;
    float SmoothedKeyboardSteer = 0.0f;
    float GamepadThrottle = 0.0f;
    float GamepadBrake = 0.0f;
    float GamepadSteer = 0.0f;

    FVehicleInputState InputState;
};
