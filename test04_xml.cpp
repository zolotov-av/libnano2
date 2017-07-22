
/****************************************************************************

Тест №04: тест работы с XML

****************************************************************************/

#include <nanosoft/xmlparser.h>
#include <nanosoft/attagparser.h>
#include <nanosoft/taghelper.h>
#include <nanosoft/easytag.h>

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

void test_easytag()
{
	// создаем пустой тег <foo>
	EasyTag foo("foo");
	
	// создаем тег foo/test/bar
	EasyTag bar = foo["test/bar"];
	
	// тегу foo/test устанавливаем атрибут
	foo["test"].setAttribute("mode", "real");
	
	// создаем тег foo/test/banana и устанавливаем ему атрибут
	foo["test/banana"].setAttribute("lang", "ru");
	
	// в тег foo/test записвыаем текст, вложенные теги отцепляются
	// тег foo/test/banana удаляется т.к. на него больше нет ссылок
	// тег foo/test/bar отцепился, но еще висит в памяти потому что
	// на него есть ссылка в переменной bar
	foo["test"] = "text";
	
	// выведем тег foo: <foo><test mode="real">text</test></foo>
	printf("foo: %s\n", foo.serialize().c_str());
	
	// выведем тег bar: <bar />
	printf("bar: %s\n", bar.serialize().c_str());
	
}

int main(int argc, char** argv)
{
	printf("test parser\n");
	test_parser();
	printf("\n");
	
	printf("test EasyTag\n");
	test_easytag();
	printf("\n");
	
	return 0;
}
