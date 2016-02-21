annabellincludedir = ${includedir}/annabell

annabellinclude_HEADERS = CommandConstants.h CommandUtils.h AnnabellParams.h AnnabellFlags.h Command.h EmptyCommand.h CommentCommand.h FileCommand.h QuitCommand.h CommandFactory.h display.h  enum_ssm.h  fssm.h  interface.h  Monitor.h  rnd.h  sizes.h  Annabell.h  ssm.h  ann_exception.h gettime.h ParseCommandTests.h sensorymotor.h

bin_PROGRAMS = annabell

annabell_SOURCES = AnnabellFlags.cc AnnabellParams.cc CommandUtils.cc Command.cc EmptyCommand.cc CommentCommand.cc FileCommand.cc QuitCommand.cc CommandFactory.cc annabell_main.cc Annabell.cc ssm.cc interface.cc Monitor.cc rnd.cc addref.cc fssm.cc display.cc modes.cc goals.cc ssm_omp.cc ssm_file.cc simplify.cc gettime.cc ParseCommandTests.cc sensorymotor.cc yarpinterface.cc

annabell_CPPFLAGS = @OPENMP_CXXFLAGS@
annabell_LDFLAGS = @OPENMP_CXXFLAGS@ -lgtest -lpthread -lpcre

if ENABLE_YARP
annabell_LDFLAGS += -lYARP_init
annabell_LDFLAGS += -lYARP_OS
annabell_CPPFLAGS += -DUSE_YARP
endif

