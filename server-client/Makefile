NAME_SERVER 	= server
NAME_CLIENT 	= client

SRC_SERVER 		= ./server.cpp
SRC_CLIENT 		= ./client.cpp

HEADER_SERVER 	= ./Server.hpp
HEADER_CLIENT	= ./Client.hpp

OBJ_SERVER		= $(SRC_SERVER:.cpp=.o)
OBJ_CLIENT		= $(SRC_CLIENT:.cpp=.o)

server: $(NAME_SERVER)

$(NAME_SERVER): $(OBJ_SERVER) $(HEADER_SERVER)
	clang++ -o $(NAME_SERVER) $(OBJ_SERVER)

client: $(NAME_CLIENT)

$(NAME_CLIENT): $(OBJ_CLIENT) $(HEADER_CLIENT)
	clang++ -o $(NAME_CLIENT) $(OBJ_CLIENT)

clean:
	rm -rf $(OBJ_CLIENT) $(OBJ_SERVER)

fclean: clean
	rm -rf $(NAME_CLIENT) $(NAME_SERVER)

re: fclean all

%.o:%.cpp
	clang++ -g -o $@ -c $<
