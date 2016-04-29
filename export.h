#ifndef _SIM_EXPORT_H
#define _SIM_EXPORT_H

#ifdef _MSC_VER
    #ifdef SIM_LIB
        #define SIMLIB_API __declspec(dllexport)
    #else
        #define SIMLIB_API __declspec(dllimport)
    #endif /*SIMLIB_API*/
#else
    #define SIMLIB_API
#endif /*SIMLIB_API*/

#endif /* end if-not-def _SIM_EXPORT_H */
