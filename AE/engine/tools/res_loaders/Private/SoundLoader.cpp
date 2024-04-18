// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Public/SoundLoader.h"
#include "res_loaders/Public/SoundSaver.h"
#include "res_loaders/Intermediate/IntermSound.h"

namespace AE::ResLoader
{
/*
=================================================
	FindSound2
=================================================
*/
namespace {
	ND_ static bool  FindSound2 (const Path &path, ArrayView<Path> directories, OUT Path &result)
	{
		// check default directory
		if ( FileSystem::IsFile( path ))
		{
			result = path;
			return true;
		}

		// check directories
		for (auto& dir : directories)
		{
			Path	img_path = dir / path;

			if ( FileSystem::IsFile( img_path ))
			{
				result = RVRef(img_path);
				return true;
			}
		}
		return false;
	}
}

/*
=================================================
	PathToSoundFileFormat
=================================================
*/
	EAudioFormat  PathToSoundFileFormat (const Path &path) __NE___
	{
		const auto	path_ext		= path.extension().string();
		char		ext_data [9]	= {};
		uint		j				= path_ext.size() > 0 and path_ext[0] == '.' ? 1 : 0;

		for (usize i = 0, cnt = Min( CountOf(ext_data)-1, path_ext.size() ); i < cnt; ++i, ++j)
		{
			ext_data[i] = ToUpperCase( char(path_ext[j]) );
		}

		const StringView	ext {ext_data};

		switch_enum( EAudioFormat::Unknown )
		{
			case EAudioFormat::Unknown :
			case EAudioFormat::RAW :	if ( ext == "wav" ) return EAudioFormat::RAW;
			case EAudioFormat::OGG :	if ( ext == "ogg" ) return EAudioFormat::OGG;
		}
		switch_end
		return Default;
	}

/*
=================================================
	SoundFormatToExt
=================================================
*/
	StringView  SoundFormatToExt (EAudioFormat fmt) __NE___
	{
		switch_enum( fmt )
		{
			case EAudioFormat::RAW :		return "wav";
			case EAudioFormat::OGG :		return "ogg";
			case EAudioFormat::Unknown :	break;
		}
		switch_end
		return Default;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	LoadSound
=================================================
*/
	bool  ISoundLoader::LoadSound (INOUT IntermSound&	sound,
									ArrayView<Path>		directories,
									RC<IAllocator>		allocator,
									EAudioFormat		fileFormat) __NE___
	{
		Path	filename;
		CHECK_ERR( _FindSound( sound.GetPath(), directories, OUT filename ));

		FileRStream		file{ filename };
		CHECK_ERR( file.IsOpen() );

		if ( fileFormat == Default )
			fileFormat = PathToSoundFileFormat( filename );

		CHECK_ERR( LoadSound( INOUT sound, file, RVRef(allocator), fileFormat ));
		return true;
	}

/*
=================================================
	_FindSound
=================================================
*/
	bool  ISoundLoader::_FindSound (const Path &path, ArrayView<Path> directories, OUT Path &result)
	{
		if ( FindSound2( path, directories, OUT result ))
			return true;

		if ( FindSound2( path.filename(), directories, OUT result ))
			return true;

		RETURN_ERR( "sound file '"s << ToString(path) << "' is not found!" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SaveSound
=================================================
*/
	bool  ISoundSaver::SaveSound (const Path		&	filename,
								  const IntermSound	&	sound,
								  EAudioFormat			fileFormat) __NE___
	{
		FileWStream		file{ filename };
		CHECK_ERR( file.IsOpen() );

		if ( fileFormat == Default )
			fileFormat = PathToSoundFileFormat( filename );

		CHECK_ERR( SaveSound( file, sound, fileFormat ));
		return true;
	}


} // AE::ResLoader
