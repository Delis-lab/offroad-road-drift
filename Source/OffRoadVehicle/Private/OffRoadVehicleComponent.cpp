#include "OffRoadVehicleComponent.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosVehicleWheel.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InputCoreTypes.h"
#include "VehicleInputComponent.h"

namespace
{
    void ConfigureOffRoadWheel(UChaosVehicleWheel* Wheel)
    {
        Wheel->WheelRadius = 48.0f;
        Wheel->WheelWidth = 28.0f;
        Wheel->SuspensionMaxRaise = 18.0f;
        Wheel->SuspensionMaxDrop = 32.0f;
        Wheel->SuspensionDampingRatio = 0.8f;
        Wheel->WheelLoadRatio = 0.25f;
        Wheel->SpringRate = 300.0f;
        Wheel->SpringPreload = 50.0f;
        Wheel->SuspensionSmoothing = 4;
        Wheel->bAffectedByEngine = true;
        Wheel->bAffectedByBrake = true;
        Wheel->MaxBrakeTorque = 1500.0f;
        Wheel->MaxHandBrakeTorque = 3000.0f;
        Wheel->LongitudinalFrictionForceMultiplier = 1.2f;
        Wheel->LateralFrictionForceMultiplier = 1.2f;
    }
}

UOffRoadFrontWheel::UOffRoadFrontWheel()
{
    ConfigureOffRoadWheel(this);
    AxleType = EAxleType::Front;
    MaxSteerAngle = 32.0f;
    bAffectedBySteering = true;
    bAffectedByHandbrake = false;
}

UOffRoadRearWheel::UOffRoadRearWheel()
{
    ConfigureOffRoadWheel(this);
    AxleType = EAxleType::Rear;
    MaxSteerAngle = 0.0f;
    bAffectedBySteering = false;
    bAffectedByHandbrake = true;
}

AOffRoadVehicleComponent::AOffRoadVehicleComponent()
{
    PrimaryActorTick.bCanEverTick = true;
    VehicleInput = CreateDefaultSubobject<UVehicleInputComponent>(TEXT("VehicleInput"));
}

void AOffRoadVehicleComponent::BeginPlay()
{
    Super::BeginPlay();

    bFourWheelDriveEnabled = bStartInFourWheelDrive;
    ApplyDriveConfiguration();
}

void AOffRoadVehicleComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    VehicleInput->BindInput(PlayerInputComponent);

    PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &ThisClass::ShiftUp);
    PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ThisClass::ShiftDown);
    PlayerInputComponent->BindKey(EKeys::Gamepad_RightShoulder, IE_Pressed, this, &ThisClass::ShiftUp);
    PlayerInputComponent->BindKey(EKeys::Gamepad_LeftShoulder, IE_Pressed, this, &ThisClass::ShiftDown);

    PlayerInputComponent->BindKey(EKeys::V, IE_Pressed, this, &ThisClass::ToggleFourWheelDriveInput);
    PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &ThisClass::ToggleDifferentialLockInput);
    PlayerInputComponent->BindKey(EKeys::Gamepad_DPad_Left, IE_Pressed, this, &ThisClass::ToggleFourWheelDriveInput);
    PlayerInputComponent->BindKey(EKeys::Gamepad_DPad_Right, IE_Pressed, this, &ThisClass::ToggleDifferentialLockInput);

    PlayerInputComponent->BindKey(EKeys::T, IE_Pressed, this, &ThisClass::BeginWinch);
    PlayerInputComponent->BindKey(EKeys::T, IE_Released, this, &ThisClass::EndWinch);
    PlayerInputComponent->BindKey(EKeys::Gamepad_LeftThumbstick, IE_Pressed, this, &ThisClass::BeginWinch);
    PlayerInputComponent->BindKey(EKeys::Gamepad_LeftThumbstick, IE_Released, this, &ThisClass::EndWinch);
}

void AOffRoadVehicleComponent::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UChaosWheeledVehicleMovementComponent* Movement = GetChaosMovement();
    if (!Movement || !VehicleInput)
    {
        return;
    }

    const FVehicleInputState Input = VehicleInput->GetInputState(GetVelocity().Size());
    Movement->SetThrottleInput(Input.Throttle);
    Movement->SetBrakeInput(Input.Brake);
    Movement->SetSteeringInput(Input.Steer);
    Movement->SetHandbrakeInput(Input.Handbrake);

    if (Input.Boost && GetMesh())
    {
        GetMesh()->AddForce(GetActorForwardVector() * BoostForce);
    }

    if (Input.ResetVehicle && !bResetWasPressed)
    {
        ResetVehicleUpright();
    }
    bResetWasPressed = Input.ResetVehicle;

    if (bWinchActive)
    {
        ApplyWinchForce();
    }
}

void AOffRoadVehicleComponent::ToggleFourWheelDrive()
{
    bFourWheelDriveEnabled = !bFourWheelDriveEnabled;
    ApplyDriveConfiguration();
}

void AOffRoadVehicleComponent::ToggleDifferentialLock()
{
    bDifferentialLocked = !bDifferentialLocked;
    ApplyDriveConfiguration();
    OnDifferentialLockChanged(bDifferentialLocked);
}

void AOffRoadVehicleComponent::ShiftUp()
{
    if (UChaosWheeledVehicleMovementComponent* Movement = GetChaosMovement())
    {
        Movement->SetTargetGear(Movement->GetCurrentGear() + 1, true);
    }
}

void AOffRoadVehicleComponent::ShiftDown()
{
    if (UChaosWheeledVehicleMovementComponent* Movement = GetChaosMovement())
    {
        Movement->SetTargetGear(Movement->GetCurrentGear() - 1, true);
    }
}

void AOffRoadVehicleComponent::SetSurfaceType(EOffRoadSurfaceType SurfaceType)
{
    UChaosWheeledVehicleMovementComponent* Movement = GetChaosMovement();
    if (!Movement)
    {
        return;
    }

    float FrictionMultiplier = AsphaltFrictionMultiplier;
    switch (SurfaceType)
    {
    case EOffRoadSurfaceType::Gravel:
        FrictionMultiplier = GravelFrictionMultiplier;
        break;
    case EOffRoadSurfaceType::Mud:
        FrictionMultiplier = MudFrictionMultiplier;
        break;
    case EOffRoadSurfaceType::Asphalt:
    default:
        break;
    }

    for (int32 WheelIndex = 0; WheelIndex < Movement->WheelSetups.Num(); ++WheelIndex)
    {
        Movement->SetWheelFrictionMultiplier(WheelIndex, FrictionMultiplier);
    }
}

void AOffRoadVehicleComponent::BeginWinch()
{
    if (!WinchAnchor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Winch input ignored: assign a WinchAnchor to the vehicle."));
        return;
    }

    bWinchActive = true;
}

void AOffRoadVehicleComponent::EndWinch()
{
    bWinchActive = false;
}

void AOffRoadVehicleComponent::ToggleFourWheelDriveInput()
{
    ToggleFourWheelDrive();
}

void AOffRoadVehicleComponent::ToggleDifferentialLockInput()
{
    ToggleDifferentialLock();
}

void AOffRoadVehicleComponent::ResetVehicleUpright()
{
    FRotator UprightRotation = GetActorRotation();
    UprightRotation.Pitch = 0.0f;
    UprightRotation.Roll = 0.0f;
    SetActorLocationAndRotation(
        GetActorLocation() + FVector(0.0f, 0.0f, 100.0f),
        UprightRotation,
        false,
        nullptr,
        ETeleportType::TeleportPhysics);

    if (USkeletalMeshComponent* VehicleMesh = GetMesh())
    {
        VehicleMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
        VehicleMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    }
}

void AOffRoadVehicleComponent::ApplyWinchForce()
{
    USkeletalMeshComponent* VehicleMesh = GetMesh();
    if (!VehicleMesh || !WinchAnchor)
    {
        return;
    }

    const FVector ToAnchor = WinchAnchor->GetActorLocation() - GetActorLocation();
    const float Distance = ToAnchor.Size();
    if (Distance > UE_KINDA_SMALL_NUMBER && Distance <= MaxWinchDistance)
    {
        VehicleMesh->AddForce(ToAnchor.GetSafeNormal() * WinchPullForce);
    }
}

void AOffRoadVehicleComponent::ApplyDriveConfiguration()
{
    UChaosWheeledVehicleMovementComponent* Movement = GetChaosMovement();
    if (!Movement)
    {
        return;
    }

    Movement->SetUseAutomaticGears(false);
    Movement->DifferentialSetup.DifferentialType = bFourWheelDriveEnabled
        ? EVehicleDifferential::AllWheelDrive
        : EVehicleDifferential::RearWheelDrive;
    Movement->DifferentialSetup.FrontRearSplit = bFourWheelDriveEnabled
        ? (bDifferentialLocked ? LockedFrontTorqueSplit : UnlockedFrontTorqueSplit)
        : 0.0f;
}

UChaosWheeledVehicleMovementComponent* AOffRoadVehicleComponent::GetChaosMovement() const
{
    return FindComponentByClass<UChaosWheeledVehicleMovementComponent>();
}
