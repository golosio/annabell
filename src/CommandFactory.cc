/*
 * CommandFactory.cc
 *
 *  Created on: Jan 10, 2016
 *      Author: jpp
 */

#include <CommandConstants.h>
#include <CommandFactory.h>
#include <CommandUtils.h>
#include <CommentCommand.h>
#include <ContinueContextCommand.h>
#include <RetrievePhraseCommand.h>
#include <EmptyCommand.h>
#include <FileCommand.h>
#include <QuitCommand.h>
#include <MacroCommand.h>
#include <InputPhraseCommand.h>
#include <ExtractWordGroupCommand.h>
#include <SearchPhraseCommand.h>
#include <ContinueSearchPhraseCommand.h>
#include <CopyInputPhraseToWorkingPhraseCommand.h>
#include <RewardCommand.h>
#include <PartialRewardCommand.h>
#include <ExploitCommand.h>
#include <CleanExploitCommand.h>
#include <ExploitRandomCommand.h>
#include <ExploitMemorizeCommand.h>

struct timespec;

Annabell* CommandFactory::annabell;
Monitor* CommandFactory::monitor;
display* CommandFactory::aDisplay;
timespec* CommandFactory::clk0;
timespec* CommandFactory::clk1;

void CommandFactory::init(Annabell* annabell, Monitor* monitor, display* aDisplay, timespec* clk0, timespec* clk1) {
	CommandFactory::annabell = annabell;
	CommandFactory::monitor = monitor;
	CommandFactory::aDisplay = aDisplay;
	CommandFactory::clk0 = clk0;
	CommandFactory::clk1 = clk1;
}

void CommandFactory::execute(string input) {
	Command* c = CommandFactory::newCommand(input);
	c->execute();
	delete c;
}

Command* CommandFactory::newCommand(string input) {
	Command* newCommand;

	if (input.empty()) {
		newCommand = new EmptyCommand();

	} else if (CommandUtils::startsWith(input, COMMENT_CMD)) {
		newCommand = new CommentCommand();

	} else if (CommandUtils::startsWith(input, FILE_CMD) || CommandUtils::startsWith(input, FILE_CMD_LONG)) {
		newCommand = new FileCommand();

	} else if (CommandUtils::startsWith(input, QUIT_CMD) || CommandUtils::startsWith(input, QUIT_CMD_LONG)) {
		newCommand = new QuitCommand();

	} else if (CommandUtils::isMacroCommand(input)) {
		newCommand = new MacroCommand();

	} else if (CommandUtils::isInputPhrase(input)) {
		newCommand = new InputPhraseCommand();

	} else if (CommandUtils::startsWith(input, CONTINUE_CONTEXT_CMD)
			|| CommandUtils::startsWith(input, CONTINUE_CONTEXT_CMD_LONG)) {
		newCommand = new ContinueContextCommand();

	} else if (CommandUtils::startsWith(input, PHRASE_CMD_LONG) || CommandUtils::startsWith(input, PHRASE_CMD)) {
		newCommand = new RetrievePhraseCommand();

	} else if (CommandUtils::startsWith(input, WORD_GROUP_CMD_LONG) || CommandUtils::startsWith(input, WORD_GROUP_CMD)) {
		newCommand = new ExtractWordGroupCommand();

	} else if(CommandUtils::startsWith(input, SEARCH_CONTEXT_CMD_LONG) || CommandUtils::startsWith(input, SEARCH_CONTEXT_CMD)) {
		newCommand = new SearchPhraseCommand();

	} else if (CommandUtils::startsWith(input, CONTINUE_SEARCH_CONTEXT_CMD_LONG) || CommandUtils::startsWith(input, CONTINUE_SEARCH_CONTEXT_CMD)) {
		newCommand = new ContinueSearchPhraseCommand();

	} else if (CommandUtils::startsWith(input, RETRIEVE_INPUT_PHRASE_CMD_LONG) || CommandUtils::startsWith(input, RETRIEVE_INPUT_PHRASE_CMD)) {
		newCommand = new CopyInputPhraseToWorkingPhraseCommand();

	} else if (CommandUtils::startsWith(input, REWARD_CMD_LONG) || CommandUtils::startsWith(input, REWARD_CMD2)) {
		newCommand = new RewardCommand();

	} else if (CommandUtils::startsWith(input, PARTIAL_REWARD_CMD_LONG) || CommandUtils::startsWith(input, PARTIAL_REWARD_CMD2)) {
		newCommand = new PartialRewardCommand();

	} else if (CommandUtils::startsWith(input, EXPLOIT_CMD_LONG) || CommandUtils::startsWith(input, EXPLOIT_CMD)) {
		newCommand = new ExploitCommand();

	} else if (CommandUtils::startsWith(input, CLEAN_EXPLOIT_CMD_LONG) || CommandUtils::startsWith(input, CLEAN_EXPLOIT_CMD)) {
		newCommand = new CleanExploitCommand();

	} else if (CommandUtils::startsWith(input, EXPLOIT_RANDOM_CMD_LONG) || CommandUtils::startsWith(input, EXPLOIT_RANDOM_CMD)) {
		newCommand = new ExploitRandomCommand();

	} else if (CommandUtils::startsWith(input, EXPLOIT_MEMORIZE_CMD_LONG) || CommandUtils::startsWith(input, EXPLOIT_MEMORIZE_CMD)) {
		newCommand = new ExploitMemorizeCommand();
	}
	else {
		newCommand = new Command();
	}

	newCommand->init(CommandFactory::annabell, CommandFactory::monitor, CommandFactory::aDisplay, CommandFactory::clk0,
			CommandFactory::clk1, input);

	return newCommand;
}
