NAME=libft.a

CFLAGS=-fPIC -Wall -Wextra -Werror

OBJ=\
	ft_isalpha.o      \
	ft_isdigit.o      \
	ft_isalnum.o      \
	ft_isascii.o      \
	ft_isprint.o      \
	ft_strlen.o       \
	ft_memset.o       \
	ft_bzero.o        \
	ft_memcpy.o       \
	ft_memmove.o      \
	ft_strlcpy.o      \
	ft_toupper.o      \
	ft_tolower.o      \
	ft_strchr.o       \
	ft_strrchr.o      \
	ft_strncmp.o      \
	ft_memchr.o       \
	ft_memcmp.o       \
	ft_strlcat.o      \
	ft_strnstr.o      \
	ft_atoi.o         \
	ft_calloc.o       \
	ft_lstnew.o       \
	ft_lstadd_front.o \
	ft_lstsize.o      \
	ft_lstlast.o      \
	ft_lstadd_back.o  \
	ft_lstdelone.o    \
	ft_lstclear.o     \
	ft_lstiter.o      \
	ft_lstmap.o

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ)
	$(AR) rcs $@ $(OBJ)

.PHONY: clean
clean:
	@$(RM) -v $(OBJ)

.PHONY: fclean
fclean: clean
	@$(RM) -v $(NAME)

.PHONY: re
re: fclean all

.NOTPARALLEL: re fclean
