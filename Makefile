
program_name   := spdaemon

warnings_flags := -pedantic -Wall -Wextra -Wno-long-long -Wunused-parameter
boost_flags    := -DBOOST_ALL_NO_LIB -DASIO_DISABLE_SERIAL_PORT

source_path :=	../../src/

source_dirs := $(source_path)chat \
		$(source_path)common \
		$(source_path)config \
		$(source_path)file_utils \
		$(source_path)filter \
		$(source_path)frontend \
		$(source_path)http \
		$(source_path)logging \
		$(source_path)statistics \
		$(source_path)misc_utils

override libraries := -lstdc++ \
			$(libraries)

override link_flags := -pthread \
			$(link_flags)

override compile_flags := -ansi -fexceptions \
				$(boost_flags) \
				$(compile_flags) \
				$(warnings_flags)

source_files := $(wildcard $(addsuffix /*.cpp, $(source_dirs)))
object_files := $(notdir $(patsubst %.cpp, %.o, $(source_files)))

VPATH := $(source_dirs)

.PHONY: all
all: $(program_name)

$(program_name): $(object_files)
	$(CXX) $^ -o $@ $(link_flags) $(libraries) -pipe

%.o: %.cpp
	$(CXX) -MD -c $< $(compile_flags) $(addprefix -I, $(source_dirs)) -pipe

include $(wildcard *.d)
