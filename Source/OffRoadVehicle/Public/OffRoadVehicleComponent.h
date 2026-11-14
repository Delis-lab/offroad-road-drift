#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "WheeledVehiclePawn.h"
#include "OffRoadVehicleComponent.generated.h"

class UVehicleInputComponent;
class UChaosWheeledVehicleMovementComponent;
class UInputComponent;
class AActor;

UENUM(BlueprintType)
enum class EOffRoadSurfaceType : uint8
{
    Asphalt,
    Gravel,
    Mud
};

UCLASS()
class OFFROADVEHICLE_API UOffRoadFrontWheel : public UChaosVehicleWheel
{
    GENERATED_BODY()

public:
    UOffRoadFrontWheel();
};

UCLASS()
class OFFROADVEHICLE_API UOffRoadRearWheel : public UChaosVehicleWheel
{
    GENERATED_BODY()

public:
    UOffRoadRearWheel();
};

UCLASS(Blueprintable)
class OFFROADVEHICLE_API AOffRoadVehicleComponent : public AWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    AOffRoadVehicleComponent();

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "Off-road Vehicle|Drivetrain")
    void ToggleFourWheelDrive();

    UFUNCTION(BlueprintCallable, Category = "Off-road Vehicle|Drivetrain")
    void ToggleDifferentialLock();

    UFUNCTION(BlueprintImplementableEvent, Category = "Off-road Vehicle|Drivetrain")
    void OnDifferentialLockChanged(bool bIsLocked);

    UFUNCTION(BlueprintCallable, Category = "Off-road Vehicle|Drivetrain")
    void ShiftUp();

    UFUNCTION(BlueprintCallable, Category = "Off-road Vehicle|Drivetrain")
    void ShiftDown();

    UFUNCTION(BlueprintCallable, Category = "Off-road Vehicle|Surface")
    void SetSurfaceType(EOffRoadSurfaceType SurfaceType);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Winch")
    TObjectPtr<AActor> WinchAnchor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Winch", meta = (ClampMin = "0.0"))
    float WinchPullForce = 300000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Winch", meta = (ClampMin = "0.0"))
    float MaxWinchDistance = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Boost", meta = (ClampMin = "0.0"))
    float BoostForce = 300000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Surface", meta = (ClampMin = "0.0"))
    float AsphaltFrictionMultiplier = 1.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Surface", meta = (ClampMin = "0.0"))
    float GravelFrictionMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Surface", meta = (ClampMin = "0.0"))
    float MudFrictionMultiplier = 0.65f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Off-road Vehicle|Drivetrain")
    bool bStartInFourWheelDrive = true;

    UPROPERTY(BlueprintReadOnly, Category = "Off-road Vehicle|Drivetrain")
    bool bDifferentialLocked = false;

    UPROPERTY(EditAnywhere, Category = "Off-road Vehicle|Drivetrain", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float UnlockedFrontTorqueSplit = 0.65f;

    UPROPERTY(EditAnywhere, Category = "Off-road Vehicle|Drivetrain", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float LockedFrontTorqueSplit = 0.5f;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void BeginWinch();

    UFUNCTION()
    void EndWinch();

    UFUNCTION()
    void ToggleFourWheelDriveInput();

    UFUNCTION()
    void ToggleDifferentialLockInput();

    void ResetVehicleUpright();
    void ApplyWinchForce();
    void ApplyDriveConfiguration();
    UChaosWheeledVehicleMovementComponent* GetChaosMovement() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Off-road Vehicle|Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UVehicleInputComponent> VehicleInput;

    bool bFourWheelDriveEnabled = true;
    bool bWinchActive = false;
    bool bResetWasPressed = false;
};
