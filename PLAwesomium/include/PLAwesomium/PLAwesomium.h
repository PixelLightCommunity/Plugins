#ifndef __PLAWESOMIUM_H__
#define __PLAWESOMIUM_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/PLCore.h>


//[-------------------------------------------------------]
//[ Import/Export                                         ]
//[-------------------------------------------------------]
#ifdef PLAWESOMIUM_EXPORTS
	// To export classes, methods and variables
	#define PLAWESOMIUM_API			PL_GENERIC_API_EXPORT

	// To export RTTI elements
	#define PLAWESOMIUM_RTTI_EXPORT	PL_GENERIC_RTTI_EXPORT
#else
	// To import classes, methods and variables
	#define PLAWESOMIUM_API			PL_GENERIC_API_IMPORT

	// To import RTTI elements
	#define PLAWESOMIUM_RTTI_EXPORT	PL_GENERIC_RTTI_IMPORT
#endif


#endif // __PLAWESOMIUM_H__