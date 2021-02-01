#colors
RED=\033[1;31m
GREEN=\033[1;32m
YELLOW=\033[1;33m
CYAN=\033[1;36m
RESET=\033[0m

CC		= clang++ 
RM		= rm -rf
DEBUG	= -g
FLAGS	= -Wall -Wextra -Werror -MMD -I/usr/local/opt/openssl/include -std=c++98
SSL		= -lssl -lcrypto -L/usr/local/opt/openssl/lib
INCLUDES= -I ./includes

ifeq ($(UNAME), Darwin)
	INCLUDES += -I/usr/local/opt/openssl/include
	SSL += -L/usr/local/opt/openssl/lib
endif

NAME	= ircserv

SRC_DIR		= ./sources/
OBJ_DIR		= ./objects/
COMM_DIR	= ./sources/commands/
OBJ_DIR_C	= ./sources/objects/

SRCS		= main.cpp \
			utils.cpp \
			Class.IRC.cpp \
			Class.Socket.cpp \
			Class.Client.cpp \
			Class.Command.cpp \
			Class.User.cpp \
			Class.Channel.cpp \
			Class.Server.cpp \
			Class.Statistics.cpp \

COMMS		= nick.cpp \
			join.cpp \
			oper.cpp \
			quit.cpp \
			part.cpp \
			user.cpp \
			pass.cpp \
			server.cpp \
			names.cpp \
			squit.cpp \
			who.cpp \
			topic.cpp \
			ping.cpp \
			pong.cpp \
			mode.cpp \
			kick.cpp \
			privmsg.cpp \
			away.cpp \
			notice.cpp \
			stats.cpp \
			time.cpp \
			error.cpp \
			admin.cpp \
			wallops.cpp \
			connect.cpp \
			info.cpp \
			version.cpp \
			links.cpp \
			trace.cpp \
			kill.cpp \
			motd.cpp \
			whois.cpp \
			list.cpp \
			lusers.cpp \
			invite.cpp \
			pogoda.cpp


OBJ			= $(patsubst %.cpp,$(OBJ_DIR)%.o,$(SRCS))
OBJ_C		= $(patsubst %.cpp,$(OBJ_DIR_C)%.o,$(COMMS))
DEP			= $(OBJ:.o=.d)
DEP_C		= $(OBJ_C:.o=.d)

all: $(NAME)

-include $(DEP)

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	@test -d $(OBJ_DIR) || mkdir $(OBJ_DIR)
	@$(CC) $(DEBUG) -o $@ -c $< $(FLAGS) $(INCLUDES)
	@echo "[$(GREEN)OK$(RESET)]$(YELLOW) Compiling $<$(RESET)"

$(OBJ_DIR_C)%.o:$(COMM_DIR)%.cpp
	@test -d $(OBJ_DIR_C) || mkdir $(OBJ_DIR_C)
	@$(CC) $(DEBUG) -o $@ -c $< $(FLAGS) $(INCLUDES)
	@echo "[$(GREEN)OK$(RESET)]$(YELLOW) Compiling $<$(RESET)"

$(NAME): $(OBJ) $(OBJ_C)
	@$(CC) -o $(NAME) $(DEBUG) $(OBJ) $(OBJ_C) $(FLAGS) $(INCLUDES) $(SSL)
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
	@$(RM) $(OBJ_DIR) $(OBJ_DIR_C)
	@echo "[$(RED)Deleting$(RESET)]$(RED) Object files deleted.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "[$(RED)Deleting$(RESET)]$(RED) $(NAME) deleted.$(RESET)"

re: fclean all

server: all
	./$(NAME) 1080 123

client: all
	./$(NAME) localhost:6667:123 1080 123

.PHONY: all clean fclean re server client