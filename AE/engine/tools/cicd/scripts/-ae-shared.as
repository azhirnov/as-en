// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <cicd.as>

const string		ci_test				= " -DAE_CI_BUILD_TEST=ON -DAE_ENABLE_LOGS=ON -DAE_GRAPHICS_STRONG_VALIDATION=OFF -DAE_ENABLE_MEMLEAK_CHECKS=ON";
const string		ci_perf				= " -DAE_CI_BUILD_PERF=ON -DAE_ENABLE_LOGS=ON -DAE_GRAPHICS_STRONG_VALIDATION=OFF -DAE_ENABLE_MEMLEAK_CHECKS=OFF";
const string		vk					= " -DAE_ENABLE_VULKAN=ON";
const string		mtl					= " -DAE_ENABLE_METAL=ON -DAE_ENABLE_OPENVR=OFF";
const string		rmg					= " -DAE_ENABLE_REMOTE_GRAPHICS=ON -DAE_ENABLE_OPENVR=OFF";
const string		pch					= " -DAE_USE_PCH=ON";
const string		no_pch				= " -DAE_USE_PCH=OFF";
const string		avx2				= " -DAE_SIMD_AVX=2";
const string		osx_targ			= " -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15";

const string		c_Branch			= "ci";
const string		c_GitServer			= "TODO";
const uint			c_ThreadCount		= 6;

const array<string>	c_ConfigList		= { "Debug", /*"Develop", "Profile",*/ "Release" };
const array<string>	c_AndroidConfigs	= {"Debug", "Release"};

const array<string>	c_TargetList		= { "Tests.Base", "Tests.Serializing", "Tests.Scripting", "Tests.Threading",
											"Tests.Networking", "Tests.ECS-st", "Tests.Graphics", "Tests.GraphicsHL", "Tests.VFS",
											"Tests.GeometryTools", "Tests.AtlasTools", "Tests.HuLang" };
const array<string>	c_GraphicsTargets	= { "Tests.Graphics", "Tests.GraphicsHL" };
const array<string>	c_DesktopTargets	= { "Tests.ShaderTrace", "Tests.PipelineCompiler" };
const array<string>	c_WindowsTargets	= { "Tests.Video" };	// "Tests.Platform", "Tests.AssetPacker",

const string		c_PerConfig			= "Release";
const array<string>	c_PerfTargets		= { "Perf.Base", "Perf.Threading", "LockFreeAlgSandbox" };
