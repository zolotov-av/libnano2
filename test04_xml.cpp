
/****************************************************************************

Тест №04: тест работы с XML

****************************************************************************/

#include <nanosoft/xmlparser.h>
#include <nanosoft/attagparser.h>
#include <nanosoft/taghelper.h>

#include <string>
#include <stdio.h>
#include <stdlib.h>

const char xml[] =
	"<?xml version=\"1.0\" ?>\n"
	"<foo>\n"
	" <bar>test</bar>\n"
	"</foo>\n";

void test_parser()
{
	TagHelper foo = parse_xml_string( std::string(xml)  );
	if ( foo )
	{
		printf("result: %s\n", foo["bar"]->getCharacterData().c_str());
	}
}

int main(int argc, char** argv)
{
	test_parser();
	return 0;
}
