CC 		:= cc
CFLAGS 	:= -Wall -pedantic -pedantic-errors -std=c11
DLIBS 	:= -lm $(shell pkg-config --libs glfw3 opengl glew)
INCLUDE := $(addprefix -I,./include)
OBJDIR 	= objs
SRCDIR  = src
OBJS 	= $(addprefix objs/,main.o shader.o linear.o obj.o)
BIN 	= mverse

all: build

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir ${OBJDIR}

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	${CC} -g -c $< -o $@ ${CFLAGS} ${INCLUDE}

build: $(OBJS)
	${CC} $^ -o ${BIN} ${DLIBS}

run:
	./${BIN}

clean:
	@rm $(OBJS) -v
