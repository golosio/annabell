#include "ParseCommandTests.h"

#include <CommandUtils.h>
#include <gtest/gtest.h>
#include <string.h>

TEST(CommandUtilsStringTests, pluralTest) {
	EXPECT_EQ("-s", CommandUtils::processPlural("-es"));
	EXPECT_EQ("-s", CommandUtils::processPlural("-s"));
	EXPECT_EQ("shouldNotChange", CommandUtils::processPlural("shouldNotChange"));
}

TEST(CommandUtilsStringTests, articleTest) {
	EXPECT_EQ("a", CommandUtils::processArticle("an"));
	EXPECT_EQ("a", CommandUtils::processArticle("a"));
	EXPECT_EQ("shouldNotChange", CommandUtils::processArticle("shouldNotChange"));
}

TEST(CommandUtilsStringTests, startsWithCharTest) {
	EXPECT_TRUE(CommandUtils::startsWith("abcdef", 'a'));
	EXPECT_FALSE(CommandUtils::startsWith("abcdef", 'f'));
}

TEST(CommandUtilsStringTests, startsWithStringTest) {
	EXPECT_TRUE(CommandUtils::startsWith("abcdef", "abc"));
	EXPECT_TRUE(CommandUtils::startsWith("abcdef", "abcdef"));
	EXPECT_FALSE(CommandUtils::startsWith("abcdef", "bcd"));
}

TEST(CommandUtilsStringTests, endsWithTest) {
	EXPECT_TRUE(CommandUtils::endsWith("abcdef", 'f'));
	EXPECT_FALSE(CommandUtils::endsWith("abcdef", 'a'));
}

TEST(CommandUtilsStringTests, removeTrailingTest) {
	EXPECT_EQ("abcde", CommandUtils::removeTrailing("abcdef"));
	EXPECT_EQ("abcdef", CommandUtils::removeTrailing("abcdef", 'x'));
	EXPECT_EQ("abcdef", CommandUtils::removeTrailing("abcdef*", '*'));
}

TEST(CommandUtilsMacroTests, macroDetectionTests) {

	//valid macro form
	EXPECT_TRUE(CommandUtils::isMacroCommand(".ph /mammal/the dog is a mammal/dog/"));

	//valid macro form with two expressions separated by an '&' character
	EXPECT_TRUE(CommandUtils::isMacroCommand(".ph /mammal/the dog is a mammal/dog/ & mammal/the dog is a mammal/dog/"));

	//should fail because .ph* is not a valid macro command
	EXPECT_FALSE(CommandUtils::isMacroCommand(".ph* /mammal/the dog is a mammal/dog/"));

	//valid commands ending with asterisk
	EXPECT_TRUE(CommandUtils::isMacroCommand(".wg* /mammal/the dog is a mammal/dog/"));
	EXPECT_TRUE(CommandUtils::isMacroCommand(".po* /mammal/the dog is a mammal/dog/"));
	EXPECT_TRUE(CommandUtils::isMacroCommand(".o* /mammal/the dog is a mammal/dog/"));

	//.wg is nos a valid macro command
	EXPECT_FALSE(CommandUtils::isMacroCommand(".wg the dog is a mammal"));

	//valid forms, omitting optional PHRASE to use as substitution
	EXPECT_TRUE(CommandUtils::isMacroCommand(".po* /mammal//dog/"));
	EXPECT_TRUE(CommandUtils::isMacroCommand(".ph /mammal//dog/ & mammal//dog/"));

	//.f command is similar because it has '/' characters, but is nos a macro
	EXPECT_FALSE(CommandUtils::isMacroCommand(".f /path/to/a/file.txt"));

	//just a phrase, not a command
	EXPECT_FALSE(CommandUtils::isMacroCommand("the dog is a mammal"));

	//should fail because it lacks the ending '/'
	EXPECT_FALSE(CommandUtils::isMacroCommand(".ph /mammal/the dog is a mammal/dog"));
}

int runAllTests(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
