// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class RuyiSDKDemo : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string LibPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "lib/")); }
    } 

    public RuyiSDKDemo(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseRTTI = true;
        bEnableExceptions = true;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        Definitions.Add("BOOST_ALL_NO_LIB");

		PublicDependencyModuleNames.AddRange
            (
                new string[] 
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "InputCore",
                    "HeadMountedDisplay",
                    "Json",
                    //"DesktopPlatform",
                    "ImageWrapper",
                    "RHI",
                    "RenderCore",
                    "UMG",
                }
            );

        PublicIncludePaths.AddRange(
            new string[] {

                "RuyiSDKDemo/include",
                "RuyiSDKDemo/include/Generated/CommonType",
                //"RuyiSDKDemo/Public",
				// ... add public include paths required here ...
			}
            );

        PublicDelayLoadDLLs.Add(Path.Combine(LibPath, "zmq", "libzmq.dll"));

        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "RuyiSDK.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "zmq", "libzmq.lib"));

        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_chrono-vc141-mt-1_64.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_chrono-vc141-mt-gd-1_64.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_date_time-vc141-mt-1_64.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_date_time-vc141-mt-gd-1_64.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_system-vc141-mt-1_64.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_system-vc141-mt-gd-1_64.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_thread-vc141-mt-1_64.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_thread-vc141-mt-gd-1_64.lib"));

        PrivateIncludePathModuleNames.AddRange(
            new string[] 
            {
                "DesktopPlatform",
            }
            );
    }
}
