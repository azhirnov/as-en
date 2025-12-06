//9feb6265
#pragma once
#include <vector>
#include <string>

#define funcdef // typedef for function

using int8		= std::int8_t;
using uint8		= std::uint8_t;
using int16		= std::int16_t;
using uint16	= std::uint16_t;
using uint		= std::uint32_t;
using int32		= std::int32_t;
using uint32	= std::uint32_t;
using int64		= std::int64_t;
using uint64	= std::uint64_t;
using string	= std::string;

template <typename T>
struct RC;

template <typename T>
using array = std::vector<T>;

using namespace std::string_literals;


enum class CPUArch : uint8
{
	x86,
	x64,
	Armv7,
	Armv8,
	Arm64,
	RISCV,
	RISCV_64,
	Loong64,
	E2K,
};
uint8  operator | (CPUArch lhs, CPUArch rhs);
uint8  operator | (uint8 lhs, CPUArch rhs);
uint8  operator | (CPUArch lhs, uint8 rhs);

enum class OS : uint8
{
	Windows,
	Android,
	Linux,
	MacOS,
	BSD,
};
uint8  operator | (OS lhs, OS rhs);
uint8  operator | (uint8 lhs, OS rhs);
uint8  operator | (OS lhs, uint8 rhs);

enum class ECompiler : uint8
{
	MSVC,
	MSVC_Clang,
	Linux_GCC,
	Linux_Clang,
	Linux_Clang_Ninja,
	MacOS_Clang,
	iOS_Clang,
};
uint8  operator | (ECompiler lhs, ECompiler rhs);
uint8  operator | (uint8 lhs, ECompiler rhs);
uint8  operator | (ECompiler lhs, uint8 rhs);

enum class ECopyMode : uint8
{
	FileReplace,
	FileMerge,
	FolderReplace,
	FolderMerge_FileReplace,
	FolderMerge_FileMerge,
	FolderMerge_FileKeep,
};
uint8  operator | (ECopyMode lhs, ECopyMode rhs);
uint8  operator | (uint8 lhs, ECopyMode rhs);
uint8  operator | (ECopyMode lhs, uint8 rhs);

string  FindAndReplace (const string &, const string &, const string &);
bool  StartsWith (const string &, const string &);
bool  StartsWithIC (const string &, const string &);
bool  EndsWith (const string &, const string &);
bool  EndsWithIC (const string &, const string &);
void  LogError (const string & msg);
void  LogInfo (const string & msg);
void  LogDebug (const string & msg);
void  LogFatal (const string & msg);
void  Assert (bool expr);
void  Assert (bool expr, const string & msg);
void  Server_SetFolder (const string &);
void  StartBuild (OS os, CPUArch arch);
void  StartBuild (OS os, CPUArch arch, const string & name);
void  StartTest (OS os, CPUArch arch);
void  StartTest (OS os, CPUArch arch, const string & name);
void  StartTests (OS os, CPUArch arch);
void  StartTests (OS os, CPUArch arch, const string & name);
void  SessionBarrier ();
void  RemoveFolder (const string &);
void  MakeFolder (const string &);
void  CopyFolder (const string & src, const string & dst);
void  CopyFile (const string & src, const string & dst);
void  DeleteFile (const string &);
void  UploadFile (const string & src, const string & dst);
void  UploadFile (const string & src, const string & dst, ECopyMode mode);
void  UploadFolder (const string & src, const string & dst);
void  UploadFolder (const string & src, const string & dst, const string & filter);
void  UploadFolder (const string & src, const string & dst, ECopyMode mode);
void  UploadFolder (const string & src, const string & dst, const string & filter, ECopyMode mode);
void  DownloadFile (const string & src, const string & dst);
void  DownloadFile (const string & src, const string & dst, ECopyMode mode);
void  DownloadFolder (const string & src, const string & dst);
void  DownloadFolder (const string & src, const string & dst, const string & filter);
void  DownloadFolder (const string & src, const string & dst, ECopyMode mode);
void  DownloadFolder (const string & src, const string & dst, const string & filter, ECopyMode mode);
void  GitClone (const string & repository, const string & dstFolder);
void  GitClone (const string & tag, const string & repository, const string & dstFolder);
void  GitClone (const string & repository, const string & dstFolder, bool recurseSubmodules);
void  GitClone (const string & tag, const string & repository, const string & dstFolder, bool recurseSubmodules);
string  Git_GetHash (const string & repository, const string & branch);
string  Git_GetShortHash (const string & repository, const string & branch);
void  GitCommitAndPush (const string & path, const string & branch);
void  GitRebase (const string & path, const string & srcBranch, const string & dstBranch);
void  CMake (ECompiler compiler, uint compilerVersion, const string & config, const string & cmakeOptions, const string & sourcePath, const string & buildPath);
void  CMake (ECompiler compiler, uint compilerVersion, const string & config, const string & cmakeOptions, const string & sourcePath, const string & buildPath, CPUArch arch);
void  CMakeBuild (const string & buildPath, const string & config, const string & target, uint threadCount);
void  AndroidPatchGradle (const string & buildGradlePath, const string & cmakeOptions);
void  AndroidBuild (const string & projectFolder, bool isDebug, const string & target);
void  RunAndroidTest (const string & libName, const string & fnName);
void  RunTest (const string & exe);
void  RunTest (const string & exe, const string & workDir);
bool  Server_HasFile (const string & path);
bool  Server_HasFolder (const string & path);
void  RunScript (const string & path);
void  Unzip (const string & archive);
