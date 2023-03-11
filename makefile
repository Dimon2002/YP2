PROG = main

PREF_SRC = ./src/
PREF_OBJ = ./obj/

SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC))

#args = `arg="$(filter-out $@, $(MAKECMDGOALS))" && echo $${arg:-${1}}`

#Program compile (C)
$(PROG): $(OBJ)
	@gcc $(OBJ) -o $(PROG)

$(PREF_OBJ)%.o : $(PREF_SRC)%.c
	@gcc -std=c99 -c -O $< -o $@

clean :
	@rm $(PROGNAME) $(PREF_OBJ)*.o

#start: $(PROG)
#	@./$(PROG) $(call args, defaultstring) 2> er.txt
