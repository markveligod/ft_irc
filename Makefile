#colors
RED=\033[1;31m
GREEN=\033[1;32m
YELLOW=\033[1;33m
CYAN=\033[1;36m
RESET=\033[0m

CC		= clang++
RM		= rm -rf
DEBUG	= -g
FLAGS	= -Wall -Wextra -Werror -MMD -std=c++98 -I/usr/local/opt/openssl/include
SSL		= -lssl -lcrypto -L/usr/local/opt/openssl/lib
INCLUDES= -I ./includes

ifeq ($(UNAME), Darwin)
	INCLUDES += -I/usr/local/opt/openssl/include
	SSL += -L/usr/local/opt/openssl/lib
endif

NAME	= ircserv

SRC_DIR = ./sources/
OBJ_DIR	= ./objects/

SRCS	= main.cpp \
		  Class.Utils.cpp \
		  Class.IRC.cpp \
		  Class.Socket.cpp \
		  Class.Client.cpp \
		  Class.Command.cpp \
		  Class.User.cpp \
		  Class.Channel.cpp \
		  Class.Server.cpp

SRC		= $(addprefix $(SRC_DIR), $(SRCS))
OBJS	= $(SRC:.cpp=.o) $(CLASS:.cpp=.o)
OBJ		= $(patsubst %.cpp,$(OBJ_DIR)%.o,$(SRCS))
DEP		= $(OBJ:.o=.d)

all: $(NAME)

-include $(DEP)

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	@test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	@$(CC) $(DEBUG) -o $@ -c $< $(FLAGS) $(INCLUDES)
	@echo "[$(GREEN)OK$(RESET)]$(YELLOW) Compiling $<$(RESET)"

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $(DEBUG) $(OBJ) $(FLAGS) $(INCLUDES) $(SSL)
	@echo "[$(GREEN)Success$(RESET)]$(GREEN) Successfully compiled $(NAME) project.$(RESET)"
	@echo ""
	@echo "$(CYAN)      /|/|"
	@echo "     ( @ @)"
	@echo "      ) ^ $(RESET)   ft_irc (project 2020-2021) $(CYAN)"
	@echo "     / |||       $(RESET) by ckakuna, leweathe and pdemocri$(CYAN)"
	@echo "    / )|||_"
	@echo "   (_______)$(RESET)"
	@echo ""
	@echo "$(RED)[For example]:$(RESET)"
	@echo "$(YELLOW)./$(NAME) $(RESET)"
	@echo ""

clean:
	@$(RM) $(OBJ_DIR)
	@echo "[$(RED)Deleting$(RESET)]$(RED) Object files deleted.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "[$(RED)Deleting$(RESET)]$(RED) $(NAME) deleted.$(RESET)"

re: fclean all

server: all
	./$(NAME) 1080 111

client: all
	./$(NAME) localhost:1080:111 1280 123

.PHONY: all clean fclean re server client