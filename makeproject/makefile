OBJS := game.o game_socket.o game_info.o game_compare.o game_lookup_table.o game_action.o

SOURCEDIR := ../source/
TARGETDIR := ../target/
SOBJS := $(addprefix $(SOURCEDIR),$(OBJS))
SRCS := $(SOBJS:.o=.c)
DEP := $(SOBJS:.o=.dep) 

$(TARGETDIR)game: $(DEP) $(SOBJS)
	gcc -O3 -o $@ $(SOBJS) -lm

$(SOURCEDIR)%.o:$(SOURCEDIR)%.c
	gcc -O3 -o $@ -c $<

$(SOURCEDIR)%.dep: $(SOURCEDIR)%.c
	gcc -MM $<   -MT $(@:.dep=.o) >$@

sinclude $(DEP)

.PHONY clean:
	rm -rf $(TARGETDIR)game $(DEP) $(SOBJS)
