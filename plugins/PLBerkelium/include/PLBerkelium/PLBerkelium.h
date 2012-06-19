#ifndef __PLBERKELIUM_H__
#define __PLBERKELIUM_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/PLCore.h>


//[-------------------------------------------------------]
//[ Import/Export                                         ]
//[-------------------------------------------------------]
#ifdef PLBERKELIUM_EXPORTS
	// To export classes, methods and variables
	#define PLBERKELIUM_API			PL_GENERIC_API_EXPORT

	// To export RTTI elements
	#define PLBERKELIUM_RTTI_EXPORT	PL_GENERIC_RTTI_EXPORT
#else
	// To import classes, methods and variables
	#define PLBERKELIUM_API			PL_GENERIC_API_IMPORT

	// To import RTTI elements
	#define PLBERKELIUM_RTTI_EXPORT	PL_GENERIC_RTTI_IMPORT
#endif


#endif // __PLBERKELIUM_H__