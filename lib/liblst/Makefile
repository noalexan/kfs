NAME=liblst.a

ROOT_DIR=../..

KERNEL_INCLUDE_DIR = -I$(ROOT_DIR)/include

CFLAGS ?= -ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector -Wall -Wextra -Werror
CFLAGS += $(KERNEL_INCLUDE_DIR)

OBJ=\
	ft_lstadd_back.o \
	ft_lstclear.o \
	ft_lstlast.o \
	ft_lstnew.o \
	ft_lstsize.o \
	ft_lstadd_front.o \
	ft_lstdelone.o \
	ft_lstiter.o \
	ft_lstmap.o \
	ft_lst_pop.o

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
