annabellincludedir = ${includedir}/annabell
annabellinclude_HEADERS = display.h  enum_ssm.h  fssm.h  interface.h  monitor.h  rnd.h  sizes.h  sllm.h  ssm.h

bin_PROGRAMS = annabell

annabell_SOURCES = annabell_main.cc sllm.cc ssm.cc interface.cc monitor.cc rnd.cc addref.cc fssm.cc display.cc modes.cc goals.cc ssm_omp.cc ssm_file.cc simplify.cc

annabell_CPPFLAGS = @OPENMP_CXXFLAGS@
annabell_LDFLAGS = @OPENMP_CXXFLAGS@

