#include <stdio.h>
#include <conio.h>

#include <string>

int main()
{
	char buff[512] = {0};
	std::string str;
	
	while(fread(buff, 1, 512, stdin) > 0)
	{
		str.append(buff);
	}
	
	printf("%s", str.c_str());
	
	return 0;
}
