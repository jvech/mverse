CC 		:= cc
CFLAGS 	:= -Wall -pedantic -pedantic-errors -std=c11
DLIBS 	:= -lm $(shell pkg-config --libs glfw3 opengl glew)
OBJDIR 	= objs
SRCDIR  = src
OBJS 	= $(addprefix objs/,main.o shader.o linear.o)
BIN 	= mverse

all: build run

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir ${OBJDIR}

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	${CC} -c $< -o $@ ${CFLAGS}

build: $(OBJS)
	${CC} $^ -o ${BIN} ${DLIBS}

run:
	./${BIN}

clean:
	@rm $(OBJS) -v
