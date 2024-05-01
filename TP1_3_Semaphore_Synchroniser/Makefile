########################################
#~ définitions
########################################

BIN=bin/memoire  bin/init

# liste des fichiers sources
SRCS0=memoire.c
SRCS1=init.c
SRCS3=supprimeSem.c


default: $(BIN)

########################################
#~ regles pour l'executable
########################################

obj/%.o: %.c
	g++ -Wall -Iinclude -c $< -o $@

bin/memoire: $(SRCS2:%.c=obj/%.o) $(SRCS0:%.c=obj/%.o)
	g++ -o $@ $+

bin/init: $(SRCS1:%.c=obj/%.o)
	g++ -o $@ $+

bin/supprimeSem: $(SRCS3:%.c=obj/%.o)
	g++ -o $@ $+




clean:
	rm -f $(BIN) obj/*.o *~