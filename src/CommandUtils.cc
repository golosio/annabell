#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE_CODE_UNIT_WIDTH 8
#include <pcre.h>
#include "CommandUtils.h"

using namespace std;

pcre* CommandUtils::macroRegex;
pcre_extra* CommandUtils::optimizedMacroRegex;

void CommandUtils::compileRegex(const char* regexString) {

	const char* pcreErrorStr;
	int pcreErrorOffset;

	CommandUtils::macroRegex = pcre_compile(regexString, 0, &pcreErrorStr, &pcreErrorOffset, NULL);
	if (CommandUtils::macroRegex == NULL) {
		printf("ERROR: Could not compile '%s': %s\n", regexString, pcreErrorStr);
		//TODO: replace with exception thrown
		exit(1);
	}
	// Optimize the regex
	CommandUtils::optimizedMacroRegex = pcre_study(CommandUtils::macroRegex, 0, &pcreErrorStr);
	if (pcreErrorStr != NULL) {
		printf("ERROR: Could not study '%s': %s\n", regexString, pcreErrorStr);
		//TODO: replace with exception thrown
		exit(1);
	}
}

void CommandUtils::compileMacroRegex() {

	//only compile the regex once, for performance reasons
	if (CommandUtils::optimizedMacroRegex == NULL) {
		//TODO: use CommandConstants constants and split the regex into a more human-readable concatenation of such constants.
		compileRegex("\\.(ph|pg|wg|po|o|wg\\*|po\\*|o\\*) \\/([^\\/]+)\\/([^\\/]*)\\/([^\\/]*)\\/($|( & ([^\\/]+)\\/([^\\/]*)\\/([^\\/]*)\\/)?$)");
	}
}

/**
 *  Macro commands take the following form:
 *
 *  .COMMAND /CUE/[PHRASE]/[WORD_GROUP]/[ & CUE2/[PHRASE2]/[WORD_GROUP2]]
 *
 *  Where:
 *
 *  - COMMAND is one of: ph, pg, wg, po, o, wg*, po*, o*
 *  - CUE is a word group that must be extracted from the current working phrase and used as a cue
 *  - PHRASE = is the phrase that must be retrieved from the long-term memory using cue.
 *    This is optional. Is PHRASE is not supplied, the command takes the short form of /CUE//WORD_GROUP/ used for implementing substitutions like "you , I".
 *  - WORD_GROUP optional group of words extracted from phrase
 */
bool CommandUtils::isMacroCommand(string inputString) {
	compileMacroRegex();

	/** the result of trying to match the regex with the inputString */
	int matchesCount;

	/** magic number of maximum possible matches to store. PCRE interface needs this upfront :( ... */
	const int MATCH_INDEXES_ARRAY_SIZE = 30;

	/** an array to store the indexes of the possible matches of the regex in the inputString */
	int subStrVec[MATCH_INDEXES_ARRAY_SIZE];

	matchesCount = pcre_exec(CommandUtils::macroRegex, CommandUtils::optimizedMacroRegex, inputString.c_str(),
			inputString.length(), 0, 0, subStrVec, MATCH_INDEXES_ARRAY_SIZE);

	if (matchesCount == PCRE_ERROR_NOMATCH) {
		// regex did not match. command is not a macro.
		return false;

	} else if (matchesCount >= 0) {
		// there are matches with the regex. command is a macro.
		return true;

	} else {
		// some error occured
		printf("Error in regex %i\n", matchesCount);

		//TODO: replace with exception thrown
		exit(matchesCount);
	}
}

/**
 * This function takes care of plural suffix.
 * ie. replaces plural occurences with "-s"
 * @param input a string
 * @return "-s" if the input represents a plural, and the unchanged input otherwise.
 */
string CommandUtils::processPlural(string input) {
	if (input == "-es") {
		return "-s";
	} else {
		return input;
	}
}

/**
 * This function takes care of the articles, simplifying "an" occurrences to "a".
 * @param input a string
 * @return "a" if the input is "an", and the unchanged input otherwise.
 */
string CommandUtils::processArticle(string input) {
	if (input == "an") {
		return "a";
	} else {
		return input;
	}
}

/**
 * @returns true if input string starts with provided character, false otherwise.
 */
bool CommandUtils::startsWith(string input, char character) {
	return input[0] == character;
}

/**
 * @returns true if input string starts with provided substring, false otherwise.
 */
bool CommandUtils::startsWith(string input, string subString) {
	if (input.find(subString) == 0) {
		return true;
	} else {
		return false;
	}
}

/**
 * @returns true if input string ends with provided character, false otherwise.
 */
bool CommandUtils::endsWith(string input, char character) {
	int lastIndex = input.size() - 1;
	return input[lastIndex] == character;
}

/**
 * @returns the input string, minus its last character
 */
string CommandUtils::removeTrailing(string input) {
	return input.substr(0, input.size() - 1);
}

/**
 * @returns the input string, minus its last character, is such last character is equal to the character specified as second argument, or the unchanged specified string otherwise
 */
string CommandUtils::removeTrailing(string input, char character) {
	if (CommandUtils::endsWith(input, character)) {
		return input.substr(0, input.size() - 1);
	} else {
		return input;
	}
}
