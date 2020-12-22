CC				= clang++
STD				= -std=c++98
CFLAGS			= -MMD -g
# CFLAGS			+= $(STD)
# CFLAGS			+= -Wall -Werror -Wextra

OBJ_DIR			= ./objects/

NAME_CLIENT 	= client
SRC_CLIENT 		= client_main.cpp \
				  Client.cpp
OBJ_CLIENT		= $(SRC_CLIENT:.cpp=.o)
OBJ_CLIENT		= $(patsubst %.cpp,$(OBJ_DIR)%.o,$(SRC_CLIENT))

NAME_SERVER		= server
SRC_SERVER 		= server_main.cpp \
				  Server.cpp \
				  Client.cpp
OBJ_SERVER		= $(SRC_SERVER:.cpp=.o)
OBJ_SERVER		= $(patsubst %.cpp,$(OBJ_DIR)%.o,$(SRC_SERVER))

OBJ				= $(OBJ_CLIENT) $(OBJ_SERVER)
DEP				= $(OBJ:.o=.d)

all: $(NAME_CLIENT) $(NAME_SERVER)

$(NAME_SERVER): $(OBJ_SERVER)
	$(CC) -o $@ $(OBJ_SERVER) $(CFLAGS)

$(NAME_CLIENT): $(OBJ_CLIENT)
	$(CC) -o $@ $(OBJ_CLIENT) $(CFLAGS)

-include $(DEP)

$(OBJ_DIR)%.o: %.cpp
	@test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	$(CC) -g -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME_SERVER) $(NAME_CLIENT)

re: fclean all

.PHONY: all clean fclean re