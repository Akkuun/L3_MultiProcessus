########################################
#~ définitions
########################################

BIN=bin/rdv  bin/semInit bin/supprimeSem

# liste des fichiers sources
SRCS0=rdv.c
SRCS1=semInit.c
SRCS3=supprimeSem.c

default: $(BIN)

########################################
#~ regles pour l'executable
########################################

obj/%.o: %.c
	g++ -Wall -Iinclude -c $< -o $@

bin/rdv: $(SRCS2:%.c=obj/%.o) $(SRCS0:%.c=obj/%.o)
	g++ -o $@ $+

bin/semInit: $(SRCS1:%.c=obj/%.o)
	g++ -o $@ $+

bin/supprimeSem: $(SRCS3:%.c=obj/%.o)
	g++ -o $@ $+



clean:
	rm -f $(BIN) obj/*.o *~