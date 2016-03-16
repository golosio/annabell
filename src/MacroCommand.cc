#include <Command.h>
#include <CommandUtils.h>
#include <MacroCommand.h>
#include <string>
#include <vector>

struct timespec;

bool simplify(Annabell *annabell, Monitor *Mon, display* Display, timespec* clk0, timespec* clk1,
		vector<string> input_token);

MacroCommand::MacroCommand() :
		Command() {
}

int MacroCommand::execute() {
	//TODO this is duplicated with Command::ParseCommand, for now.
	vector<string> input_token;

	stringstream ss(input_line); // Insert the line into a stream
	string buf, buf1; // buffer strings

	buf = "";
	while (ss >> buf1) { // split line in string tokens
		buf1 = CommandUtils::processArticle(buf1);

		buf1 = CommandUtils::processPlural(buf1);

		buf = buf + buf1;

		if (CommandUtils::startsWith(buf, '/') && !CommandUtils::endsWith(buf, '/')) {
			buf = buf + " ";
			continue;
		}

		input_token.push_back(buf);
		buf = "";
	}

	simplify(annabell, Mon, Display, clk0, clk1, input_token);
	return 0;
}
