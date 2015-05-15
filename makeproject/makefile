OBJS := game.o game_socket.o game_info.o

SOURCEDIR := ../source/
TARGETDIR := ../target/
SOBJS := $(addprefix $(SOURCEDIR),$(OBJS))
SRCS := $(SOBJS:.o=.c)
DEP := $(SOBJS:.o=.dep) 

$(TARGETDIR)game: $(DEP) $(SOBJS)
	gcc -o $@ $(SOBJS)

$(SOURCEDIR)%.o:$(SOURCEDIR)%.c
	gcc -o $@ -c $<

$(SOURCEDIR)%.dep: $(SOURCEDIR)%.c
	gcc -MM $<   -MT $(@:.dep=.o) >$@

sinclude $(DEP)

.PHONY clean:
	rm -rf $(TARGETDIR)game $(DEP) $(SOBJS)
