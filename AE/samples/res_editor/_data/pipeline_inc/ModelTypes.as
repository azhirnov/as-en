// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

const ArraySize	albedoMapsCount	= ArraySize(64);	// same as 'ScriptModelGeometrySrc::_maxTextures'
const uint		maxDirLights	= 8;
const uint		maxConeLights	= 16;
const uint		maxOmniLights	= 8;


void  VertexBuffers ()
{
	// vertex components
	{
		RC<ShaderStructType>	st1 = ShaderStructType( "Posf3" );
		st1.Set( EStructLayout::InternalIO,
				 "packed_float3		Position;" );
		st1.AddUsage( ShaderStructTypeUsage::VertexLayout );

		RC<ShaderStructType>	st2 = ShaderStructType( "Normf3" );
		st2.Set( EStructLayout::InternalIO,
				 "packed_float3		Normal;" );
		st2.AddUsage( ShaderStructTypeUsage::VertexLayout );

		RC<ShaderStructType>	st3 = ShaderStructType( "UVf2" );
		st3.Set( EStructLayout::InternalIO,
				 "packed_float2		Texcoord;" );
		st3.AddUsage( ShaderStructTypeUsage::VertexLayout );
	}

	// vertex buffers
	{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB{Posf3, Normf3, UVf2}" );
		vb.Add( "Position",		"Posf3" );
		vb.Add( "Normals",		"Normf3" );
		vb.Add( "UVs",			"UVf2" );
	}{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB{Posf3, UVf2}" );
		vb.Add( "Position",		"Posf3" );
		vb.Add( "UVs",			"UVf2" );
	}{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB{Posf3, Normf3}" );
		vb.Add( "Position",		"Posf3" );
		vb.Add( "Normals",		"Normf3" );
	}{
		RC<VertexBufferInput>	vb = VertexBufferInput( "VB{Posf3}" );
		vb.Add( "Position",		"Posf3" );
	}
}


void  InitPipelineLayout ()
{
	// uniforms hardcoded in [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/GeomSource/ModelGeomSource.cpp)

	// samplers
	{
		RC<Sampler>		samp = Sampler( "model.Sampler" );
		samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
		samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
		samp.TrySetAnisotropy( 16.0f );
	}

	// descriptor set
	{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "model.mtr.ds" );
		ds.StorageBuffer( EShaderStages::Vertex,	"un_Nodes",		"ModelNode_Array",		EResourceState::ShaderStorage_Read );
		ds.StorageBuffer( EShaderStages::Fragment,	"un_Materials",	"ModelMaterial_Array",	EResourceState::ShaderStorage_Read );
		ds.StorageBuffer( EShaderStages::Fragment,	"un_Lights",	"SceneLights",			EResourceState::ShaderStorage_Read );
		ds.SampledImage(  EShaderStages::Fragment,	"un_AlbedoMaps", albedoMapsCount, EImageType::Float_2D );
		ds.ImtblSampler(  EShaderStages::Fragment,	"un_AlbedoMapSampler", "model.Sampler" );
	}

	// pipeline layout
	{
		RC<PipelineLayout>		pl = PipelineLayout( "model.pl" );
		pl.DSLayout( "pass",	 0, "pass.ds" );
		pl.DSLayout( "material", 1, "model.mtr.ds" );
		pl.Define( "DISABLE_un_PerObject" );
	}
}


void  InitMeshPipelineLayout ()
{
	// uniforms hardcoded in [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/GeomSource/ModelGeomSource.cpp)

	// descriptor set
	{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "ms-model.mtr.ds" );
		ds.StorageBuffer( EShaderStages::Mesh,		"un_Nodes",		"ModelNode_Array",		EResourceState::ShaderStorage_Read );
	//	ds.StorageBuffer( EShaderStages::Mesh,		"un_Meshes",	"ModelMesh_Array",		EResourceState::ShaderStorage_Read );
		ds.StorageBuffer( EShaderStages::Fragment,	"un_Materials",	"ModelMaterial_Array",	EResourceState::ShaderStorage_Read );
		ds.StorageBuffer( EShaderStages::Fragment,	"un_Lights",	"SceneLights",			EResourceState::ShaderStorage_Read );
		ds.SampledImage(  EShaderStages::Fragment,	"un_AlbedoMaps", albedoMapsCount, EImageType::Float_2D );
		ds.ImtblSampler(  EShaderStages::Fragment,	"un_AlbedoMapSampler", "model.Sampler" );
	}

	// pipeline layout
	{
		RC<PipelineLayout>		pl = PipelineLayout( "ms-model.pl" );
		pl.DSLayout( "pass",	 0, "pass.ds" );
		pl.DSLayout( "material", 1, "ms-model.mtr.ds" );
		pl.Define( "DISABLE_un_PerObject" );
	}
}


void  InitRayTracingPipelineLayout ()
{
	// uniforms hardcoded in [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/GeomSource/ModelGeomSource.cpp)

	// descriptor set
	{
		const uint	stages	= EShaderStages::RayClosestHit | EShaderStages::RayCallable | EShaderStages::RayGen;

		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "rt-model.mtr.ds" );
		ds.StorageBuffer( stages, "un_RTInstances",	"ModelRTInstances",		EResourceState::ShaderStorage_Read );
		ds.StorageBuffer( stages, "un_Materials",	"ModelMaterial_Array",	EResourceState::ShaderStorage_Read );
		ds.StorageBuffer( stages, "un_Lights",		"SceneLights",			EResourceState::ShaderStorage_Read );
		ds.SampledImage(  stages, "un_AlbedoMaps",	albedoMapsCount,		EImageType::Float_2D );
		ds.ImtblSampler(  stages, "un_AlbedoMapSampler", "model.Sampler" );
	}

	// pipeline layout
	{
		RC<PipelineLayout>		pl = PipelineLayout( "rt-model.pl" );
		pl.DSLayout( "pass",	 0, "pass.ds" );
		pl.DSLayout( "material", 1, "rt-model.mtr.ds" );
		pl.Define( "DISABLE_un_PerObject" );
	}
}


void  BufferTypes (bool withFS, bool hasRT)
{
	// Warning: run 'ResourceEditor.ResPack' if this function has been modified.

	// scene node
	{
		RC<ShaderStructType>	st = ShaderStructType( "ModelNode" );
		st.Set( EStructLayout::Compatible_Std430,
				"float4x4			transform;"		+
				"float3x3			normalMat;"		+
				"uint				meshIdx;"		+
				"uint				materialIdx;"	);
		st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
	}{
		RC<ShaderStructType>	st = ShaderStructType( "ModelNode_Array" );
		st.Set( EStructLayout::Compatible_Std430,
				"uint				instanceCount;" +
				"ModelNode			elements [];" );
		st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
	}

	// mesh for ray tracing
	if ( hasRT )
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "ModelRTMesh" );
			if ( withFS ) st.AddFeatureSet( "MinRecursiveRayTracing" );
			st.Set( EStructLayout::Std430,
					"packed_float3 *	positions;"	+	// [index_count]
					"packed_float3 *	normals;"	+	// [index_count]
					"float2 *			texcoords;"	+	// [index_count]
					"uint *				indices;"	);	// [primitive_count * 3]
			st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
		}{
			// only 4 instance types, see [ERTGeometryType](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/GeomSource/IGeomSource.h)
			RC<ShaderStructType>	st = ShaderStructType( "ModelRTInstances" );
			st.Set( EStructLayout::Std430,
					"ModelRTMesh &		meshesPerInstance [4];"		+	// address to ModelRTMesh[]
					"uint *				materialsPerInstance [4];"	+
					"float3x3 *			normalMatPerInstance [4];"	+
					"float4x4 *			modelMatPerInstance [4];"	);
			st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
		}
	}

	// material
	{
		RC<ShaderStructType>	st = ShaderStructType( "ModelMaterial" );
		st.Set( EStructLayout::Compatible_Std430,
				"uint				flags;"			+
				"uint				albedoMap;"		+	// packed mapIdx, samplerIdx	// base color
				"uint				normalMap;"		+	// packed mapIdx, samplerIdx
				"uint				albedoRGBM;"	+
				"uint				emissiveRGBM;"	+
				"uint				specularRGBM;"	);
		st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
	}{
		RC<ShaderStructType>	st = ShaderStructType( "ModelMaterial_Array" );
		st.Set( EStructLayout::Compatible_Std430,
				"ModelMaterial		elements [];" );
	}

	// lights
	{
		RC<ShaderStructType>	st = ShaderStructType( "SceneDirectionalLight" );
		st.Set( EStructLayout::Compatible_Std430,
				"float3				direction;"		+
				"float3				attenuation;"	+
				"uint				colorRGBM;"		);
		st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
	}{
		RC<ShaderStructType>	st = ShaderStructType( "SceneConeLight" );
		st.Set( EStructLayout::Compatible_Std430,
				"float3				position;"		+
				"float3				direction;"		+
				"float3				attenuation;"	+
				"float2				cone;"			+
				"uint				colorRGBM;"		);
		st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
	}{
		RC<ShaderStructType>	st = ShaderStructType( "SceneOmniLight" );
		st.Set( EStructLayout::Compatible_Std430,
				"float3				position;"		+
				"float3				attenuation;"	+
				"uint				colorRGBM;"		);
		st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
	}{
		RC<ShaderStructType>	st = ShaderStructType( "SceneLights" );
		st.Set( EStructLayout::Compatible_Std430,
				"uint					directionalCount;"						+
				"uint					coneCount;"								+
				"uint					omniCount;"								+
				"SceneDirectionalLight	directional [" + maxDirLights + "];"	+
				"SceneConeLight			cone [" + maxConeLights + "];"			+
				"SceneOmniLight			omni [" + maxOmniLights + "];"			);
		st.AddUsage( ShaderStructTypeUsage::BufferLayout );		// enable c++ reflection
	}
}

