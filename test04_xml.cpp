
/****************************************************************************

Тест №04: тест работы с XML

****************************************************************************/

#include <nanosoft/xmlparser.h>
#include <nanosoft/attagparser.h>
#include <nanosoft/taghelper.h>
#include <nanosoft/easytag.h>
#include <nanosoft/easynode.h>

#include <string>
#include <stdio.h>
#include <stdlib.h>

int test_count;
int fail_count;

const char *test(bool status)
{
	test_count++;
	if ( ! status ) fail_count++;
	return status ? " ok " : "fail";
}

void test_tag(const char *prefix, const EasyTag &tag, const char *expect)
{
	std::string result = tag.serialize();
	int status = result == expect;
	test_count++;
	if ( ! status ) fail_count++;
	const char *msg = status ? " ok " : "fail";
	printf("[ %s ] %s: %s\n", msg, prefix, result.c_str());
}

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
	test_tag("foo-empty", foo, "<foo />");
	
	// создаем тег foo/test/bar
	EasyTag bar = foo["test/bar"];
	test_tag("foo-test-bar", foo, "<foo><test><bar /></test></foo>");
	
	// тегу foo/test устанавливаем атрибут
	foo["test"].setAttribute("mode", "real");
	test_tag("test-attr", foo, "<foo><test mode=\"real\"><bar /></test></foo>");
	
	// создаем тег foo/test/banana и устанавливаем ему атрибут
	foo["test/banana"].setAttribute("lang", "ru");
	test_tag("banana", foo, "<foo><test mode=\"real\"><bar /><banana lang=\"ru\" /></test></foo>");
	
	// в тег foo/test записвыаем текст, вложенные теги отцепляются
	// тег foo/test/banana удаляется т.к. на него больше нет ссылок
	// тег foo/test/bar отцепился, но еще висит в памяти потому что
	// на него есть ссылка в переменной bar
	foo["test"] = "text";
	test_tag("foo-text", foo, "<foo><test mode=\"real\">text</test></foo>");
	
	// добавим в конец тега foo блок cdata
	foo += "notes";
	test_tag("foo-notes", foo, "<foo><test mode=\"real\">text</test>notes</foo>");
	
	// <bar> у нас осеротел, но остался в памяти, т.к. на него есть ссылка
	test_tag("bar-empty", bar, "<bar />");
	
	// присоединим тег foo к тегу bar
	// тег bar был потоком foo, теперь он свободный, а его прежний родитель
	// теперь станет его потомком
	// bar-foo: <bar><foo><test mode="real">text</test>notes</foo></bar>
	bar += foo;
	test_tag("bar-foo", bar, "<bar><foo><test mode=\"real\">text</test>notes</foo></bar>");
	
	// подрежем bar для теста, станет <bar><foo>text</foo></bar>
	bar["foo"] = "text";
	
	// создадим еще одно дерево
	EasyTag text("text");
	text["body"] = "message";
	text["body"].setAttribute("format", "html");
	test_tag("text", text, "<text><body format=\"html\">message</body></text>");
	
	// добавим <body> к дереву <bar>
	// т.к. body имеет родителя, то он будет скопирован
	// и убедимся что <text> не пострадал и живет отдельной жизню
	bar += text["body"];
	text["body"].setAttribute("format", "plain");
	
	// bar-text: <bar><foo>text</foo><body format="html">message</body></bar>
	test_tag("bar-text", bar, "<bar><foo>text</foo><body format=\"html\">message</body></bar>");
	
	// text: <text><body format="plain">message</body></bar>
	test_tag("text-plain", text, "<text><body format=\"plain\">message</body></text>");
	
	// оператор присваивания (генерируемый компилятором) копирует ссылку
	bar = text["body"];
	test_tag("bar-body", bar, "<body format=\"plain\">message</body>");
	
	// очистим <text>
	text.clear();
	test_tag("text-empty", text, "<text />");
}

int main(int argc, char** argv)
{
	test_count = 0;
	fail_count = 0;
	
	printf("test parser\n");
	test_parser();
	printf("\n");
	
	printf("test EasyTag\n");
	test_easytag();
	int leak = EasyNode::node_created - EasyNode::node_destroyed;
	printf("[ %s ] EasyNode leakage = %d \n", test(leak == 0), leak);
	printf("\n");
	
	printf("[ %s ] result %d of %d \n", test(fail_count==0), (test_count - fail_count), test_count);
	
	return 0;
}
