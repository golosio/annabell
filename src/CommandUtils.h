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
};

#endif /* SRC_COMMANDUTILS_H_ */
