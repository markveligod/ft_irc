#colors
RED=\033[1;31m
GREEN=\033[1;32m
YELLOW=\033[1;33m
CYAN=\033[1;36m
RESET=\033[0m

CC		= clang++ 
RM		= rm -rf
DEBUG	= -g
FLAGS	= -Wall -Wextra -Werror -MMD -std=c++98
INCLUDES= -I ../includes

ifeq ($(UNAME), Darwin)
	INCLUDES += -I/usr/local/opt/openssl/include
	SSL += -L/usr/local/opt/openssl/lib
endif

NAME	= pogoda

SRC_BOT_DIR = ./
OBJ_BOT_DIR	= ./objects/
SRCS_BOT	= main.cpp \
			Class.Bot.cpp \
			Class.Command.cpp
OBJ_BOT		= $(patsubst %.cpp,$(OBJ_BOT_DIR)%.o,$(SRCS_BOT))
# OBJ_BOT		= $(SRCS_BOT:.cpp=.o)
DEP_BOT		= $(OBJ_BOT:.o=.d)

SRC_DIR		= ../sources/
OBJ_DIR		= ../objects/
SRCS		= utils.cpp \
			Class.Socket.cpp
OBJ			= $(patsubst %.cpp,$(OBJ_DIR)%.o,$(SRCS))
DEP			= $(OBJ:.o=.d)

all: $(NAME)

-include $(DEP) $(DEP_BOT)

$(OBJ_BOT_DIR)%.o:$(SRC_BOT_DIR)%.cpp
	@test -d $(OBJ_BOT_DIR) || mkdir $(OBJ_BOT_DIR)
	@$(CC) $(DEBUG) -o $@ -c $< $(FLAGS) $(INCLUDES)
	@echo "[$(GREEN)OK$(RESET)]$(YELLOW) Compiling $<$(RESET)"

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	@test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	@$(CC) $(DEBUG) -o $@ -c $< $(FLAGS) $(INCLUDES)
	@echo "[$(GREEN)OK$(RESET)]$(YELLOW) Compiling $<$(RESET)"

$(NAME): $(OBJ_BOT) $(OBJ)
	@$(CC) -o $(NAME) $(DEBUG) $(OBJ_BOT) $(OBJ) $(FLAGS) $(INCLUDES)
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
	@$(RM) $(OBJ_DIR) $(OBJ_BOT)
	@echo "[$(RED)Deleting$(RESET)]$(RED) Object files deleted.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "[$(RED)Deleting$(RESET)]$(RED) $(NAME) deleted.$(RESET)"

re: fclean all

bot: all
	./$(NAME) 127.0.0.1:1080:123

.PHONY: all clean fclean re server client