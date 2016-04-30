annabellincludedir = ${includedir}/annabell

annabellinclude_HEADERS = \
commands/CommandConstants.h commands/CommandUtils.h AnnabellParams.h AnnabellFlags.h \
commands/Command.h commands/EmptyCommand.h commands/CommentCommand.h commands/ContinueContextCommand.h commands/RetrievePhraseCommand.h \
commands/ExtractWordGroupCommand.h commands/MacroCommand.h commands/InputPhraseCommand.h commands/FileCommand.h \
commands/QuitCommand.h commands/SearchPhraseCommand.h commands/ContinueSearchPhraseCommand.h commands/ExploitCommand.h \
commands/CopyInputPhraseToWorkingPhraseCommand.h commands/RewardCommand.h commands/PartialRewardCommand.h \
commands/CleanExploitCommand.h commands/ExploitRandomCommand.h commands/ExploitMemorizeCommand.h commands/BestExploitCommand.h \
commands/PushGoalCommand.h commands/DropGoalCommand.h commands/GetGoalPhraseCommand.h commands/WorkingPhraseOutCommand.h \
commands/CommandFactory.h display.h  enum_ssm.h  fssm.h  interface.h  Monitor.h  rnd.h  \
sizes.h  Annabell.h  ssm.h  ann_exception.h gettime.h commands/ParseCommandTests.h sensorymotor.h

bin_PROGRAMS = annabell

annabell_SOURCES = \
AnnabellFlags.cc AnnabellParams.cc commands/CommandUtils.cc commands/Command.cc commands/EmptyCommand.cc \
commands/CommentCommand.cc commands/ContinueContextCommand.cc commands/RetrievePhraseCommand.cc commands/MacroCommand.cc commands/InputPhraseCommand.cc \
commands/ExtractWordGroupCommand.cc commands/FileCommand.cc commands/QuitCommand.cc commands/SearchPhraseCommand.cc \
commands/ContinueSearchPhraseCommand.cc commands/CopyInputPhraseToWorkingPhraseCommand.cc commands/RewardCommand.cc commands/PartialRewardCommand.cc \
commands/ExploitCommand.cc commands/CleanExploitCommand.cc commands/ExploitRandomCommand.cc commands/ExploitMemorizeCommand.cc commands/BestExploitCommand.cc \
commands/PushGoalCommand.cc commands/DropGoalCommand.cc commands/GetGoalPhraseCommand.cc commands/WorkingPhraseOutCommand.cc \
commands/CommandFactory.cc \
annabell_main.cc Annabell.cc ssm.cc interface.cc Monitor.cc rnd.cc addref.cc fssm.cc display.cc modes.cc \
goals.cc ssm_omp.cc ssm_file.cc simplify.cc gettime.cc commands/ParseCommandTests.cc sensorymotor.cc yarpinterface.cc


annabell_CPPFLAGS = @OPENMP_CXXFLAGS@
annabell_LDFLAGS = @OPENMP_CXXFLAGS@ -lgtest -lpthread -lpcre

if ENABLE_YARP
annabell_LDFLAGS += -lYARP_init
annabell_LDFLAGS += -lYARP_OS
annabell_CPPFLAGS += -DUSE_YARP
endif

