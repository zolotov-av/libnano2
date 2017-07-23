#ifndef EASYLIB_H
#define EASYLIB_H

#include <list>
#include <string>
#include <nanosoft/easyrow.h>
#include <nanosoft/easylist.h>

#define cstr(s) s.c_str()

#define qstr(s) s

std::string implode(const std::string &sep, const std::list<std::string> &list);

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron(char **envp);

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron();

/**
 * Распарсить команду, представленную в виде строки, в
 * набор аргументов EasyList.
 * 
 * @param args выходной набор аргументов  
 */
void parseCmdString(const std::string cmd, EasyList& args);

/**
 * Вызов exec()
 */
int easyExec(const std::string &filename, EasyList args, char **envp);

/**
 * Вызов exec()
 *
 * переменные окружения наследуются из текущегое процесса, из переменной environ
 */
int easyExec(const std::string &filename, EasyList args);

/**
 * Вызов exec()
 *
 * Переменные окружения задаются в виде списка строк вида "KEY=value"
 */
int easyExec(const std::string &filename, EasyList args, EasyList env);

/**
 * Вызов exec()
 *
 * Переменные окружения задаются в виде ассоциативного массива
 */
int easyExec(const std::string &filename, EasyList args, EasyRow env);

#endif // EASYLIB_H
