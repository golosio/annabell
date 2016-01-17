annabellincludedir = ${includedir}/annabell

annabellinclude_HEADERS = CommandConstants.h AnnabellFlags.h Command.h EmptyCommand.h CommandFactory.h display.h  enum_ssm.h  fssm.h  interface.h  Monitor.h  rnd.h  sizes.h  Annabell.h  ssm.h  ann_exception.h gettime.h

bin_PROGRAMS = annabell

annabell_SOURCES = AnnabellFlags.cc Command.cc EmptyCommand.cc CommandFactory.cc annabell_main.cc Annabell.cc ssm.cc interface.cc Monitor.cc rnd.cc addref.cc fssm.cc display.cc modes.cc goals.cc ssm_omp.cc ssm_file.cc simplify.cc gettime.cc 

annabell_CPPFLAGS = @OPENMP_CXXFLAGS@
annabell_LDFLAGS = @OPENMP_CXXFLAGS@

