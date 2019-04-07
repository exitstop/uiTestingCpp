#include "myLib.h"


int _tmain(int argc, _TCHAR * argv[])
{
	Clicker &clicker = Clicker::getInstance();
	clicker.run();

	return 0;
}
