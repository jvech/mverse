CC 		:= cc
CFLAGS 	:= -Wall -Wall -pedantic -std=c11
DLIBS 	:= $(shell pkg-config --libs glfw3 opengl glew)
OBJDIR 	= objs
SRCDIR  = src
OBJS 	= $(addprefix objs/,main.o)
BIN 	= mverse

all: build

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir ${OBJDIR}

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	${CC} -c $< -o $@ ${CFLAGS}

build: $(OBJS)
	${CC} $^ -o ${BIN} ${DLIBS}

clean:
	@rm $(OBJS) -v
