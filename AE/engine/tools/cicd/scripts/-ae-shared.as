// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <cicd.as>

const string		ci_test				= " -DAE_CI_BUILD_TEST=ON -DAE_ENABLE_LOGS=ON -DAE_GRAPHICS_STRONG_VALIDATION=OFF -DAE_ENABLE_MEMLEAK_CHECKS=ON  -DAE_EXCLUDE_PACK_RES=ON";
const string		ci_perf				= " -DAE_CI_BUILD_PERF=ON -DAE_ENABLE_LOGS=ON -DAE_GRAPHICS_STRONG_VALIDATION=OFF -DAE_ENABLE_MEMLEAK_CHECKS=OFF -DAE_EXCLUDE_PACK_RES=ON";

const string		vk					= " -DAE_ENABLE_VULKAN=ON";
const string		mtl					= " -DAE_ENABLE_METAL=ON -DAE_ENABLE_OPENVR=OFF";
const string		rmg					= " -DAE_ENABLE_REMOTE_GRAPHICS=ON -DAE_ENABLE_OPENVR=OFF";

const string		pch_base			= " -DAE_ENGINE_BASE_PCH=ON";
const string		pch_all				= " -DAE_WHOLE_ENGINE_PCH=ON";
const string		no_pch				= "";	// default
const string		unity_build			= " -DAE_USE_UNITY_BUILD=ON";

const string		sse4				= " -DAE_SIMD_SSE=42";
const string		avx2				= " -DAE_SIMD_AVX=2";
const string		avx512_zen4			= " -DAE_SIMD_AVX=33";
const string		avx2_vnni			= " -DAE_SIMD_AVX=21";

const string		osx_targ			= " -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15";
const string		cxx20				= " -DAE_FORCE_CXX20=ON";
const string		cxx23				= " -DAE_FORCE_CXX23=ON";

const string		c_Branch			= "ci";
const string		c_GitServer			= "TODO";
const uint			c_ThreadCount		= 6;

const array<string>	c_ConfigList		= { "Debug" /*, "Develop", "Profile", "Release"*/ };
const array<string>	c_AndroidConfigs	= { "Debug", "Release" };

const array<string>	c_TargetList		= { "Tests.Base", "Tests.Serializing", "Tests.Scripting", "Tests.Threading",
											"Tests.Networking", "Tests.ECS", "Tests.GraphicsRHI", "Tests.Graphics", "Tests.VFS",
											"Tests.GeometryTools", "Tests.AtlasTools", "Tests.HuLang" };
const array<string>	c_GraphicsTargets	= { "Tests.GraphicsRHI", "Tests.Graphics" };
const array<string>	c_DesktopTargets	= { /*"Tests.ShaderTrace",*/ "Tests.PipelineCompiler" };
const array<string>	c_WindowsTargets	= { "Tests.Video" };	// "Tests.Platform", "Tests.AssetPacker",

const string		c_CompileResConfig	= c_ConfigList[0];

const string		c_PerfConfig		= "Release";
const array<string>	c_PerfTargets		= { "LockFreeAlgSandbox" };
										//{ "Perf.Base", "Perf.ECS", "Perf.Threading", "Perf.Graphics", "LockFreeAlgSandbox" };

const uint			c_LinuxGCC			= 14;
const uint			c_LinuxClang		= 20;
const uint			c_MacClang			= 17;
