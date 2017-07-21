
/****************************************************************************

Тест №05: тест работы с EasyRow и EasyList

****************************************************************************/

#include <nanosoft/easylist.h>
#include <nanosoft/easyrow.h>

#include <string>
#include <stdio.h>
#include <stdlib.h>

void test_easyrow()
{
	EasyRow row;
	row["foo"] = "value1";
	row["bar"] = "value2";
	
	EasyList keys = row.keys();
	EasyList values = row.values();
	
	int count = keys.count();
	for(int i = 0; i < count; i++)
	{
		printf("%s=%s\n", keys[i].c_str(), values[i].c_str());
	}
	
	printf("\n");
	printf("keys: %s\n", implode(", ", keys).c_str());
}

int main(int argc, char** argv)
{
	printf("test EasyRow & EasyList\n");
	test_easyrow();
	return 0;
}
