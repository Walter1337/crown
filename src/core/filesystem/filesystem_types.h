/*
 * Copyright (c) 2012-2017 Daniele Bartolini and individual contributors.
 * License: https://github.com/taylor001/crown/blob/master/LICENSE
 */

#pragma once

/// @defgroup Filesystem Filesystem
/// @ingroup Core
namespace crown
{
class Filesystem;
class File;

/// Enumerates file open modes.
///
/// @ingroup Filesystem
struct FileOpenMode
{
	enum Enum
	{
		READ,
		WRITE
	};
};

} // namespace crown
