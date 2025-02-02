#pragma once

/**
 * Include this to all files of the project, but as THE LAST include!
 * 
 * This will give more information about emmory leaks if they appear.
 */

#include <cstdlib>
#include <crtdbg.h>
#define DEBUG_NEW new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DEBUG_NEW
