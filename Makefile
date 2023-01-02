CC 		:= cc
CFLAGS 	:= -Wall -pedantic -pedantic-errors -std=c99
DLIBS 	:= -lm $(shell pkg-config --libs glfw3 opengl glew)
INCLUDE := $(addprefix -I,./include)
OBJDIR 	= objs
SRCDIR  = src
OBJS 	= $(addprefix objs/,main.o shader.o linear.o obj.o)
BIN 	= mverse

SHADERS_DIR 	= /usr/share/${BIN}
VERTEX 			= shaders/dummy.vsh
FRAGMENT 		= shaders/dummy.fsh

all: build

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir ${OBJDIR}

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	${CC} -c $< -o $@ ${CFLAGS} ${INCLUDE}

build: $(OBJS)
	${CC} $^ -o ${BIN} ${DLIBS}

run:
	./${BIN}

install: build
	install -Dm 644 ${VERTEX} -t ${SHADERS_DIR}
	install -Dm 644 ${FRAGMENT} -t ${SHADERS_DIR}
	install -D ${BIN} -t /usr/bin
	@echo "========================================================"
	@echo "To use the ${BIN} set the environment variables:"
	@echo "  MVERSE_VERTEX=${SHADERS_DIR}/$(notdir ${VERTEX})"
	@echo "  MVERSE_FRAGMENT=${SHADERS_DIR}/$(notdir ${FRAGMENT})"
	@echo "========================================================"

uninstall:
	rm -f /usr/bin/${BIN}
	rm -rvf ${SHADERS_DIR}

clean:
	@rm $(OBJS) -v
