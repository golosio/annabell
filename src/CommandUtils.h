#ifndef SRC_COMMANDUTILS_H_
#define SRC_COMMANDUTILS_H_

#include <string>

using namespace std;

class CommandUtils {
public:
	/**
	 * This function takes care of plural suffix.
	 * ie. replaces plural occurences with "-s"
	 * @param input a string
	 * @return "-s" if the input represents a plural, and the unchanged input otherwise.
	 */
	static string processPlural(string input) {
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
	static string processArticle(string input) {
		if (input == "an") {
			return "a";
		} else {
			return input;
		}
	}

	/**
	 * @returns true if input string starts with provided character, false otherwise.
	 */
	static bool startsWith(string input, char character) {
		return input[0] == character;
	}

	/**
	 * @returns true if input string starts with provided substring, false otherwise.
	 */
	static bool startsWith(string input, string subString) {
		if (input.find(subString) == 0) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * @returns true if input string ends with provided character, false otherwise.
	 */
	static bool endsWith(string input, char character) {
		int lastIndex = input.size() - 1;
		return input[lastIndex] == character;
	}

	/**
	 * @returns the input string, minus its last character
	 */
	static string removeTrailing(string input) {
		return input.substr(0, input.size() - 1);
	}

	/**
	 * @returns the input string, minus its last character, is such last character is equal to the character specified as second argument, or the unchanged specified string otherwise
	 */
	static string removeTrailing(string input, char character) {
		if (CommandUtils::endsWith(input, character)) {
			return input.substr(0, input.size() - 1);
		} else {
			return input;
		}
	}

};

#endif /* SRC_COMMANDUTILS_H_ */
