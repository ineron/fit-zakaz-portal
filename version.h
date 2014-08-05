#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char  _DATE[] = "29";
	static const char  _MONTH[] = "07";
	static const char  _YEAR[] = "2013";
	static const char  _UBUNTU_VERSION_STYLE[] = "13.07";
	
	//Software Status
	static const char  _STATUS[] = "Alpha";
	static const char  _STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long  _MAJOR = 1;
	static const long  _MINOR = 0;
	static const long  _BUILD = 4;
	static const long  _REVISION = 19;
	
	//Miscellaneous Version Types
	static const long  _BUILDS_COUNT = 505;
	#define  _RC_FILEVERSION 1,0,4,19
	#define  _RC_FILEVERSION_STRING "1, 0, 4, 19\0"
	static const char  _FULLVERSION_STRING[] = "1.0.4.19";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long  _BUILD_HISTORY = 3;
	

}
#endif //VERSION_H
