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

TEST(CommandUtilsMacroTests, regexTests) {
	CommandUtils::compileRegex("hello");
	EXPECT_TRUE(CommandUtils::isMacroCommand("hello"));
	EXPECT_TRUE(CommandUtils::isMacroCommand("This should hello match"));
	EXPECT_FALSE(CommandUtils::isMacroCommand("goodbye"));

	CommandUtils::compileRegex("(.*)(hello)+");
	EXPECT_TRUE(CommandUtils::isMacroCommand("This should match hello"));
	EXPECT_TRUE(CommandUtils::isMacroCommand("This should match hello! because it hello should"));
	EXPECT_FALSE(CommandUtils::isMacroCommand("This should not match"));


}

int runAllTests(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
