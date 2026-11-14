using UnrealBuildTool;

public class OffRoadVehicle : ModuleRules
{
    public OffRoadVehicle(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "PhysicsCore",
            "ChaosVehicles",
            "VehicleInput"
        });
    }
}
