/*
 * CommandConstants.h
 *
 *  Created on: Jan 17, 2016
 *      Author: jpp
 */

#ifndef SRC_COMMANDCONSTANTS_H_
#define SRC_COMMANDCONSTANTS_H_

#include <string>

using namespace std;

/** Suggests a phrase to be retrieved by the association process */
const string PHRASE_CMD = ".ph";
const string PHRASE_CMD_LONG = ".phrase";

/** Suggests a word group to be extracted from the working phrase */
const string WORD_GROUP_CMD = ".wg";
const string WORD_GROUP_CMD_LONG = ".word_group";

/** Sends the word group to output and produces a partial reward for the past state-action sequence */
const string PARTIAL_REWARD_CMD = ".po";
const string PARTIAL_REWARD_CMD_LONG = ".partial_reward";
const string PARTIAL_REWARD_CMD2 = ".prw";

/** Sends the word group to output and produces a (conclusive) reward for the past state-action sequence */
const string REWARD_CMD = ".o";
const string REWARD_CMD_LONG = ".reward";
const string REWARD_CMD2 = ".rw";

/**
 * Starts an exploitation phase.
 * At the end of this phase, the system is expected to respond to the input phrase with an appropriate output phrase.
 * Without argument, the exploitation is related to the previous input phrase.
 */
const string EXPLOIT_CMD = ".x";
const string EXPLOIT_CMD_LONG = ".exploit";

/**
 * Loads phrases and/or commands from the specified file path
 */
const string FILE_CMD = ".f";
const string FILE_CMD_LONG = ".file";

/**
 * Clear the goal stack and starts an exploitation phase. At the end of this phase, the system is
 * expected to respond to the input phrase with an appropriate output phrase. Without argument,
 * the exploitation is related to the previous input phrase.
 */
const string CLEAN_EXPLOIT_CMD = ".cx";
const string CLEAN_EXPLOIT_CMD_LONG = ".clean_exploit";

/**
 * Starts an exploitation phase using a random ordering for the WTA rule. At the end of this
 * phase, the system is expected to respond to the input phrase with an appropriate output
 * phrase. Without argument, the exploitation is related to the previous input phrase.
 */
const string EXPLOIT_RANDOM_CMD = ".xr";
const string EXPLOIT_RANDOM_CMD_LONG = ".exploit_random";

/**
 * Starts an exploitation phase.
 * The output phrase is memorized by the system.
 */
const string EXPLOIT_MEMORIZE_CMD = ".xm";
const string EXPLOIT_MEMORIZE_CMD_LONG = ".exploit_memorize";

/**
 * Analogous to .x, but it iterates the exploitation process n_iter times and selects the best
 * output phrase.
 */
const string BEST_EXPLOIT_CMD = ".bx";
const string BEST_EXPLOIT_CMD_LONG = ".best_exploit";

/**
 * Insert the working phrase and the current word group in the top of the goal stack.
 */
const string PUSH_GOAL_CMD = ".pg";
const string PUSH_GOAL_CMD_LONG = ".push_goal";

/**
 * Removes the goal phrase and the goal word group from the top of the goal stack.
 */
const string DROP_GOAL_CMD = ".dg";
const string DROP_GOAL_CMD_LONG = ".drop_goal";

/**
 * Copies the goal phrase from the top of the goal stack to the working phrase.
 */
const string GET_GOAL_PHRASE_CMD = ".ggp";
const string GET_GOAL_PHRASE_CMD_LONG = ".get_goal_phrase";

/**
 * Sends to the output the next part of the working phrase, after the current word, and all
 * subsequent phrases of the same context until the end of the context itself.
 */
const string SENTENCE_OUT_CMD = ".snto";
const string SENTENCE_OUT_CMD_LONG = ".sentence_out";

/**
 * Searches a phrase in the current context of the working phrase.
 */
const string SEARCH_CONTEXT_CMD = ".sctx";
const string SEARCH_CONTEXT_CMD_LONG = ".search_context";

/**
 * TODO: undocumented command.
 */
const string CONTINUE_CONTEXT_CMD = ".cctx";
const string CONTINUE_CONTEXT_CMD_LONG = ".continue_context";

/**
 * TODO: undocumented command.
 */
const string CONTINUE_SEARCH_CONTEXT_CMD = ".csctx";
const string CONTINUE_SEARCH_CONTEXT_CMD_LONG = ".continue_search_context";

/**
 * TODO: undocumented command.
 */
const string WORKING_PHRASE_OUT_CMD = ".wpo";
const string WORKING_PHRASE_OUT_CMD_LONG = ".working_phrase_out";

/**
 * Copies the input phrase to the working phrase.
 */
const string RETRIEVE_INPUT_PHRASE_CMD = ".rip";
const string RETRIEVE_INPUT_PHRASE_CMD_LONG = ".retrieve_input_phrase";

/**
 * Gets the input phrase provided as argument without building the associations between the
 * word groups and the phrase.
 */
const string GET_INPUT_PHRASE_CMD = ".gi";
const string GET_INPUT_PHRASE_CMD_LONG = ".get_input_phrase";

/**
 * Builds the associations between the word groups and the working phrase.
 */
const string BUILD_ASSOCIATION_CMD = ".ba";
const string BUILD_ASSOCIATION_CMD_LONG = ".build_association";

/**
 * Executes single elaboration action
 */
const string ACTION_CMD = ".a";
const string ACTION_CMD_LONG = ".action";

/**
 * Displays the execution time
 */
const string TIME_CMD = ".t";
const string TIME_CMD_LONG = ".time";

/**
 * Displays some statistical information.
 */
const string STAT_CMD = ".stat";

/**
 * Executes a system update with a NULL action.
 */
const string NULL_CMD = ".n";
const string NULL_CMD_LONG = ".null";

/**
 * Sends a command to the monitor
 */
const string MONITOR_CMD = ".m";
const string MONITOR_CMD_LONG = ".monitor";

/**
 * Saves all variable-weight links to a file.
 */
const string SAVE_CMD = ".save";

/**
 * Loads all variable-weight links from a file.
 */
const string LOAD_CMD = ".load";

/**
 * Writes the output to a log file.
 */
const string LOGFILE_CMD = ".logfile";

/**
 * Closes the log file.
 */
const string LOGFILE_OFF = "off";

/**
 * Displays the speaker names using the CHAT format.
 */
const string SPEAKER_CMD = ".speaker";

/**
 * Stops displaying the speaker names.
 */
const string SPEAKER_OFF = "off";

/**
 * Displays a period at the end of the output sentences.
 */
const string PERIOD_CMD = ".period";
const string PERIOD_ON = "on";
const string PERIOD_OFF = "off";

/**
 * Records the answer time.
 */
const string ANSWER_TIME_CMD = ".at";
const string ANSWER_TIME_CMD_LONG = ".answer_time";

/**
 * Evaluates the answer time.
 */
const string EVALUATE_ANSWER_TIME_CMD = ".eat";
const string EVALUATE_ANSWER_TIME_CMD_LONG = ".evaluate_answer_time";

/**
 * Periodically save variable-link weights in files with progressive numbers.
 */
const string AUTO_SAVE_LINKS_CMD = ".asl";
const string AUTO_SAVE_LINKS_CMD_LONG = ".auto_save_links";

/**
 * exits the program or the current input file
 */
const string QUIT_CMD = ".q";
const string QUIT_CMD_LONG = ".quit";

const char COMMENT_CMD = '#';

/**
 * Starts CUDA version of exploitation mode
 */
const string CUDA_CMD = ".cu";
const string CUDA_CMD_LONG = ".cuda";

/**
 * Takes the input from a YARP port
 */
const string YARP_INPUT_CMD = ".yi";
const string YARP_INPUT_CMD_LONG = ".yarp_input";

/**
 * Sends the output to a YARP port
 */
const string YARP_OUTPUT_CMD = ".yo";
const string YARP_OUTPUT_CMD_LONG = ".yarp_output";

/**
 * Sends a command to a sensorymotor device
 */
const string SENSORYMOTOR_CMD = ".sm";
const string SENSORYMOTOR_CMD_LONG = ".sensorymotor";

/**
 * Sets automatic exploitation after every input phrase
 */
const string AUTO_EXPLOIT_CMD = ".ax";
const string AUTO_EXPLOIT_CMD_LONG = ".auto_exploit";
/**
 * Resets the system activation state
 */
const string RESET_CMD = ".rs";
const string RESET_CMD_LONG = ".reset";

#endif /* SRC_COMMANDCONSTANTS_H_ */
