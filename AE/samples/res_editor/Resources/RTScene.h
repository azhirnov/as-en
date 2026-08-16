// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Resources/IResource.h"
#include "Resources/DataTransferQueue.h"
#include "Resources/ContentVersion.h"

namespace AE::ResEditor
{

	//
	// RayTracing Geometry
	//

	class RTGeometry final : public IResource
	{
		friend class ScriptRTGeometry;

	// types
	public:
		enum class EBuildMode : uint
		{
			Direct,
			Indirect,
			IndirectEmulated,
		};


		struct TriangleMesh : RTGeometryBuild::TrianglesInfo
		{
			RC<Buffer>		vbuffer;
			RC<Buffer>		ibuffer;
			Bytes32u		vertexStride;
			Bytes			vertexDataOffset;
			Bytes			indexDataOffset;

			void  operator = (const RTGeometryBuild::TrianglesInfo &rhs)	{ TrianglesInfo::operator = (rhs); }
		};
		using TriangleMeshes_t	= Array< TriangleMesh >;


		struct MicromapData : RTGeometryBuild::MicromapInfo
		{
			RC<RTMicromap>	micromap;
		//	RC<Buffer>		ibuffer;

			void  operator = (const RTGeometryBuild::MicromapInfo &rhs)		{ MicromapInfo::operator = (rhs); }
		};
		using Micromaps_t		= Array< MicromapData >;

	private:
		using Allocator_t		= LinearAllocator<>;


	// variables
	private:
		StrongAtom<RTGeometryID>		_geomId;
		Strong<BufferID>				_scratchBuffer;			// can be null

		RC<Buffer>						_indirectBuffer;
		Strong<BufferID>				_indirectBufferHostVis;	// ASBuildIndirectCommand [GeometryCount * MaxFrames]
		ASBuildIndirectCommand const*	_indirectBufferMem		= null;

		TriangleMeshes_t				_triangleMeshes;
		Micromaps_t						_micromaps;

		ContentVersion					_version;
		const bool						_isMutable				= false;
		const ERTASOptions				_options				= ERTASOptions::PreferFastBuild;

		const String					_dbgName;


	// methods
	private:
		RTGeometry (TriangleMeshes_t	triangleMeshes,
					Micromaps_t			micromaps,
					RC<Buffer>			indirectBuffer,
					Renderer &			renderer,
					StringView			dbgName,
					Bool				isMutable)									__NE___;

		RTGeometry (Renderer &			renderer,
					StringView			dbgName)									__NE___;

		void  _Init1 ()																__Th___;
		void  _Init2 ()																__Th___;

	public:
		~RTGeometry ()																__NE_OV;

		ND_ static RC<RTGeometry>  Create (TriangleMeshes_t	triangleMeshes,
										   Micromaps_t		micromaps,
										   RC<Buffer>		indirectBuffer,
										   Renderer &		renderer,
										   StringView		dbgName,
										   Bool				isMutable)				__Th___;

		ND_ static RC<RTGeometry>  Create (Renderer &		renderer,
										   StringView		dbgName)				__Th___;

		ND_ RTGeometryID	GetGeometryId (FrameUID)								const	{ return _geomId.Get(); }
		ND_ RTGeometryID	GetGeometryId (uint)									const	{ return _geomId.Get(); }

		ND_ ulong			GetVersion (uint fid)									const	{ return _version.Get( fid ); }
		ND_ ulong			GetVersion (FrameUID fid)								const	{ return _version.Get( fid ); }

		ND_	bool			Build (DirectCtx::ASBuild &, EBuildMode)				__Th___;

			void			Reset (Strong<RTGeometryID> geomId);
			void			CompleteUploading ();

		ND_ bool			IsMutable ()											const	{ return _isMutable; }


	// IResource //
		bool				Resize (TransferCtx_t &)								__Th_OV	{ return true; }
		bool				RequireResize ()										C_Th_OV	{ return false; }
		EUploadStatus		Upload (TransferCtx_t &)								__Th_OV;
		EUploadStatus		Readback (TransferCtx_t &)								__Th_OV	{ return EUploadStatus::Completed; }


	private:
		ND_	bool  _GetTriangles (INOUT RTGeometryBuild &, FrameUID, Allocator_t &)			C_NE___;

		ND_	bool  _BuildIndirectEmulated (DirectCtx::ASBuild &, RTGeometryID, Allocator_t &) const;
	};



	//
	// RayTracing Scene
	//

	class RTScene final : public IResource
	{
		friend class ScriptRTScene;

	// types
	public:
		using EBuildMode		= RTGeometry::EBuildMode;

	private:
		struct Instance
		{
			RC<RTGeometry>		geometry;
			RTMatrixStorage		transform;
			uint				instanceCustomIndex;
			uint				mask;
			uint				instanceSBTOffset;
			ERTInstanceOpt		flags;
		};

		using Instances_t		= Array< Instance >;
		using GeomVerMap_t		= FlatHashMap< RC<RTGeometry>, ulong >;


	// variables
	private:
		Strong<RTSceneID>				_sceneId;
		Strong<BufferID>				_scratchBuffer;
		RC<Buffer>						_instanceBuffer;

		RC<Buffer>						_indirectBuffer;
		Strong<BufferID>				_indirectBufferHostVis;	// ASBuildIndirectCommand [MaxFrames]
		ASBuildIndirectCommand const*	_indirectBufferMem		= null;

		Instances_t						_instances;
		GeomVerMap_t					_uniqueGeometries;

		const ERTASOptions				_options				= ERTASOptions::PreferFastBuild;
		const bool						_isMutable				= false;

		const String					_dbgName;


	// methods
	private:
		RTScene (Instances_t	instances,
				 RC<Buffer>		instanceBuffer,
				 RC<Buffer>		indirectBuffer,
				 Renderer &		renderer,
				 StringView		dbgName,
				 Bool			allowUpdate)									__NE___;

		void  _Init ()															__Th___;

	public:
		~RTScene ()																__NE_OV;

		ND_ static RC<RTScene>  Create (Instances_t		instances,
										RC<Buffer>		instanceBuffer,
										RC<Buffer>		indirectBuffer,
										Renderer &		renderer,
										StringView		dbgName,
										Bool			allowUpdate)			__Th___;

		ND_ RTSceneID	GetSceneId (FrameUID)									const	{ return _sceneId; }
			void		Validate (FrameUID fid)									const;

		ND_	bool		Build (DirectCtx::ASBuild &, EBuildMode)				__Th___;


	// IResource //
		bool			Resize (TransferCtx_t &)								__Th_OV	{ return true; }
		bool			RequireResize ()										C_Th_OV	{ return false; }
		EUploadStatus	Upload (TransferCtx_t &)								__Th_OV;
		EUploadStatus	Readback (TransferCtx_t &)								__Th_OV	{ return EUploadStatus::Completed; }

	private:
		ND_ bool		_Resize ();
		ND_ bool		_UploadInstances (TransferCtx_t &);

		ND_ bool		_BuildIndirectEmulated (DirectCtx::ASBuild &, RTSceneBuild &, RTSceneID) const;
	};



	//
	// RayTracing Micromap
	//

	class RTMicromap final : public IResource
	{
	// types
	public:
		struct BuildData
		{
			using Usage = RTMicromapInfo::Usage;

			EMicromapType			type			= Default;
			EBuildMicromapFlags		buildFlags		= Default;
			Array<Usage>			usage;

			RC<Buffer>				data;
			Bytes					dataOffset;

			RC<Buffer>				triangleArray;
			Bytes					triangleArrayOffset;
		};
		StaticAssert64( sizeof(RTMicromapInfo) == 24 );


	// variables
	private:
		Strong<RTMicromapID>	_micromapId;
		Strong<BufferID>		_scratchBuffer;

		const BuildData			_info;
		const bool				_isMutable			= false;
		const String			_dbgName;


	// methods
	private:
		RTMicromap (BuildData	info,
					Renderer &	renderer,
					StringView	dbgName,
					Bool		allowUpdate)									__NE___;
		void  _Init ()															__Th___;

	public:
		~RTMicromap ()															__NE_OV;

		ND_ static RC<RTMicromap>  Create (BuildData	info,
										   Renderer &	renderer,
										   StringView	dbgName,
										   Bool			allowUpdate)			__Th___;

		ND_	bool		Build (DirectCtx::ASBuild &)							__Th___;

		ND_ RTMicromapID	GetMicromapID (FrameUID)							const	{ return _micromapId.Get(); }
		ND_ RTMicromapID	GetMicromapID (uint)								const	{ return _micromapId.Get(); }

	// IResource //
		bool			Resize (TransferCtx_t &)								__Th_OV	{ return true; }
		bool			RequireResize ()										C_Th_OV	{ return false; }
		EUploadStatus	Upload (TransferCtx_t &)								__Th_OV;
		EUploadStatus	Readback (TransferCtx_t &)								__Th_OV	{ return EUploadStatus::Completed; }
	};


} // AE::ResEditor
