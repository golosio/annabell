#ifndef SRC_COMMANDUTILS_H_
#define SRC_COMMANDUTILS_H_

#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE_CODE_UNIT_WIDTH 8
#include <pcre.h>


using namespace std;

class CommandUtils {
private:
	static pcre* macroRegex;
	static pcre_extra* optimizedMacroRegex;

	static void compileMacroRegex();

public:

	/**
	 * This is public for testing purporses only.
	 */
	static void compileRegex(const char* regexString);

	static bool isMacroCommand(string input);

	/**
	 * This function takes care of plural suffix.
	 * ie. replaces plural occurences with "-s"
	 * @param input a string
	 * @return "-s" if the input represents a plural, and the unchanged input otherwise.
	 */
	static string processPlural(string input);

	/**
	 * This function takes care of the articles, simplifying "an" occurrences to "a".
	 * @param input a string
	 * @return "a" if the input is "an", and the unchanged input otherwise.
	 */
	static string processArticle(string input);

	/**
	 * @returns true if input string starts with provided character, false otherwise.
	 */
	static bool startsWith(string input, char character);

	/**
	 * @returns true if input string starts with provided substring, false otherwise.
	 */
	static bool startsWith(string input, string subString);

	/**
	 * @returns true if input string ends with provided character, false otherwise.
	 */
	static bool endsWith(string input, char character);

	/**
	 * @returns the input string, minus its last character
	 */
	static string removeTrailing(string input);

	/**
	 * @returns the input string, minus its last character, is such last character is equal to the character specified as second argument, or the unchanged specified string otherwise
	 */
	static string removeTrailing(string input, char character);

};

#endif /* SRC_COMMANDUTILS_H_ */

