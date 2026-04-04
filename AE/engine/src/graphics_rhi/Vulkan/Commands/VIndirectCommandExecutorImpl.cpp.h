// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	PreprocessGeneratedCommands
=================================================
*/
# ifndef DISABLE_PREPROCESS

	template <typename C>
	void  VCTX_TYPE<C>::PreprocessGeneratedCommands (const PreprocessGeneratedCommandsCmd &cmd) __Th___
	{
		CHECK_THROW( cmd.preprocessStates != null );

		VkCommandBuffer		state_cmdbuf = cmd.preprocessStates->GetCommandBuffer();
		CHECK_THROW( state_cmdbuf != Default );

		auto  [exec_set, cmd_layout] = _GetResourcesOrThrow( cmd.indirectExecutionSet, cmd.indirectCommandsLayout );

		VALIDATE_GCTX( PreprocessGeneratedCommands( cmd, cmd_layout.Usage() ));

		VkGeneratedCommandsInfoEXT	info = {};
		info.sType					= VK_STRUCTURE_TYPE_GENERATED_COMMANDS_INFO_EXT;
		info.indirectExecutionSet	= exec_set.Handle();
		info.indirectCommandsLayout	= cmd_layout.Handle();

		info.preprocessAddress		= BitCast< VkDeviceAddress >( cmd.preprocessAddress );
		info.preprocessSize			= VkDeviceSize( cmd.preprocessSize );

		info.indirectAddress		= BitCast< VkDeviceAddress >( cmd.indirectAddress );
		info.indirectAddressSize	= VkDeviceSize( cmd.indirectAddressSize );

		info.sequenceCountAddress	= BitCast< VkDeviceAddress >( cmd.sequenceCountAddress );

		info.maxSequenceCount		= cmd.maxSequenceCount;
		info.maxDrawCount			= cmd.maxDrawCount;
		info.shaderStages			= VEnumCast( cmd.shaderStages );

		RawCtx::PreprocessGeneratedCommands( info, state_cmdbuf );
	}

	template <typename C>
	void  VCTX_TYPE<C>::PreprocessGeneratedCommands (const PreprocessGeneratedCommands2Cmd &cmd2) __Th___
	{
		auto  [preprocess_buf, ind_buf]	= _GetResourcesOrThrow( cmd2.preprocessBuffer, cmd2.indirectBuffer );
		auto* cnt_buf					= this->_mngr.GetResourceManager().GetResource( cmd2.sequenceCountBuffer, False{}, True{} );

		VALIDATE_GCTX( PreprocessGeneratedCommands( cmd2, preprocess_buf.Description(), ind_buf.Description(), (cnt_buf ? cnt_buf->Description() : BufferDesc{}) ));

		PreprocessGeneratedCommandsCmd	cmd;

		cmd.indirectExecutionSet	= cmd2.indirectExecutionSet;
		cmd.indirectCommandsLayout	= cmd2.indirectCommandsLayout;

		cmd.preprocessAddress		= preprocess_buf.GetDeviceAddress() + cmd2.preprocessBufferOffset;
		cmd.preprocessSize			= Min( cmd2.preprocessSize, SubSat( preprocess_buf.Size(), cmd2.preprocessBufferOffset ));

		cmd.indirectAddress			= ind_buf.GetDeviceAddress() + cmd2.indirectBufferOffset;
		cmd.indirectAddressSize		= Min( cmd2.indirectSize, SubSat( ind_buf.Size(), cmd2.indirectBufferOffset ));

		cmd.sequenceCountAddress	= cnt_buf ? (cnt_buf->GetDeviceAddress() + cmd2.sequenceCountBufferOffset) : Default;

		cmd.maxSequenceCount		= cmd2.maxSequenceCount;
		cmd.maxDrawCount			= cmd2.maxDrawCount;
		cmd.shaderStages			= cmd2.shaderStages;
		cmd.preprocessStates		= cmd2.preprocessStates;

		PreprocessGeneratedCommands( cmd );
	}

# endif // DISABLE_PREPROCESS

/*
=================================================
	ExecuteGeneratedCommands
=================================================
*/
	template <typename C>
	void  VCTX_TYPE<C>::ExecuteGeneratedCommands (const ExecuteGeneratedCommandsCmd &cmd) __Th___
	{
		bool	preprocessed = false;
		if ( cmd.preprocessStates != null )
		{
			preprocessed = true;

			if constexpr( IsSame< Validator_t, DrawContextValidation >)
			{
				auto*	gfx_state = cmd.preprocessStates->AsGraphics();
				CHECK_THROW( gfx_state != null );

				gfx_state->CopyStates( *this ); // throw
			}

			if constexpr( IsSame< Validator_t, ComputeContextValidation >)
			{
				auto*	comp_state = cmd.preprocessStates->AsCompute();
				CHECK_THROW( comp_state != null );

				comp_state->CopyStates( *this ); // throw
			}

			if constexpr( IsSame< Validator_t, RayTracingContextValidation >)
			{
				auto*	rt_state = cmd.preprocessStates->AsRayTracing();
				CHECK_THROW( rt_state != null );

				rt_state->CopyStates( *this ); // throw
			}
		}

		auto  [exec_set, cmd_layout] = _GetResourcesOrThrow( cmd.indirectExecutionSet, cmd.indirectCommandsLayout );

		VALIDATE_GCTX( ExecuteGeneratedCommands( cmd, cmd_layout.Usage() ));

		VkGeneratedCommandsInfoEXT	info = {};
		info.sType					= VK_STRUCTURE_TYPE_GENERATED_COMMANDS_INFO_EXT;
		info.indirectExecutionSet	= exec_set.Handle();
		info.indirectCommandsLayout	= cmd_layout.Handle();

		info.preprocessAddress		= BitCast< VkDeviceAddress >( cmd.preprocessAddress );
		info.preprocessSize			= VkDeviceSize( cmd.preprocessSize );

		info.indirectAddress		= BitCast< VkDeviceAddress >( cmd.indirectAddress );
		info.indirectAddressSize	= VkDeviceSize( cmd.indirectAddressSize );

		info.sequenceCountAddress	= BitCast< VkDeviceAddress >( cmd.sequenceCountAddress );

		info.maxSequenceCount		= cmd.maxSequenceCount;
		info.maxDrawCount			= cmd.maxDrawCount;
		info.shaderStages			= VEnumCast( cmd.shaderStages );

		RawCtx::ExecuteGeneratedCommands( info, preprocessed );
	}

	template <typename C>
	void  VCTX_TYPE<C>::ExecuteGeneratedCommands (const ExecuteGeneratedCommands2Cmd &cmd2) __Th___
	{
		auto  [preprocess_buf, ind_buf]	= _GetResourcesOrThrow( cmd2.preprocessBuffer, cmd2.indirectBuffer );
		auto* cnt_buf					= this->_mngr.GetResourceManager().GetResource( cmd2.sequenceCountBuffer, False{}, True{} );

		VALIDATE_GCTX( ExecuteGeneratedCommands( cmd2, preprocess_buf.Description(), ind_buf.Description(), (cnt_buf ? cnt_buf->Description() : BufferDesc{}) ));

		ExecuteGeneratedCommandsCmd	cmd;

		cmd.indirectExecutionSet	= cmd2.indirectExecutionSet;
		cmd.indirectCommandsLayout	= cmd2.indirectCommandsLayout;

		cmd.preprocessAddress		= preprocess_buf.GetDeviceAddress() + cmd2.preprocessBufferOffset;
		cmd.preprocessSize			= Min( cmd2.preprocessSize, SubSat( preprocess_buf.Size(), cmd2.preprocessBufferOffset ));

		cmd.indirectAddress			= ind_buf.GetDeviceAddress() + cmd2.indirectBufferOffset;
		cmd.indirectAddressSize		= Min( cmd2.indirectSize, SubSat( ind_buf.Size(), cmd2.indirectBufferOffset ));

		cmd.sequenceCountAddress	= cnt_buf ? (cnt_buf->GetDeviceAddress() + cmd2.sequenceCountBufferOffset) : Default;

		cmd.maxSequenceCount		= cmd2.maxSequenceCount;
		cmd.maxDrawCount			= cmd2.maxDrawCount;
		cmd.shaderStages			= cmd2.shaderStages;
		cmd.preprocessStates		= cmd2.preprocessStates;

		ExecuteGeneratedCommands( cmd );
	}

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
