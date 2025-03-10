#COLORS
RED					:=	\033[0;31m
GREEN				:=	\033[0;32m
YELLOW				:=	\033[0;33m
RESET_COLOR			:=	\033[0m

NAME = webserv

ifdef DEBUG
FLAGS	=	-Wall -Wextra -Werror -fsanitize=address -g -std=c++20 
else ifdef LEAK
FLAGS	=	-Wall -Wextra -Werror -fsanitize=leak -g -std=c++20
else ifdef STRUCTOR
FLAGS	=	-Wall -Wextra -Werror -g -D STRUCTOR -std=c++20
else ifdef FUNC
FLAGS	=	-Wall -Wextra -Werror -g -D FUNC -std=c++20
else ifdef CGI
FLAGS	=	-Wall -Wextra -Werror -g -D CGI -std=c++20
else
FLAGS	=	-Wall -Wextra -Werror -std=c++20
endif

FILES =	main.cpp \
		Server.cpp \
		ServerManager.cpp \
		Config.cpp \
		Client.cpp \
		Location.cpp \
		HttpUtils.cpp \
		Cgi.cpp \
		print.cpp \
		request/httpParsing.cpp \
		request/HttpRequest.cpp \
		request/multiPart.cpp \
		response/errorResponse.cpp \
		response/HttpResponse.cpp \
		response/Methods.cpp \
		#Socket.cpp


HEADER = includes

OBJ_DIR = obj
SRC_DIR = src

SRC = $(addprefix ${SRC_DIR}/, $(FILES))
OBJ = $(addprefix ${OBJ_DIR}/, ${FILES:%.cpp=%.o})
RM		= rm -rf

all: $(NAME)

$(NAME): $(OBJ)
	@c++ $(FLAGS) -o $(NAME) $(OBJ)
	@echo "\n${GREEN}Done !${RESET_COLOR}"

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	@mkdir -p ${@D}
	@printf "${YELLOW}-> Generate %-38.38s\r${RESET_COLOR}" $@
	@c++ $(FLAGS) -I $(HEADER) -c $< -o $@

clean:
	@${RM} -r ${OBJ_DIR}
	@rm -f $(OBJ)
	@printf "${RED}Obj is deleted\n${RESET_COLOR}"
	
fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re