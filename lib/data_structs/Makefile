NAME=libds.a

ROOT_DIR=../..

KERNEL_INCLUDE_DIR = -I$(ROOT_DIR)/include

CFLAGS ?= -ffreestanding -fno-builtin -fno-exceptions -fno-stack-protector -Wall -Wextra -Werror
CFLAGS += $(KERNEL_INCLUDE_DIR)

OBJ= $(shell find . -name "*.o")

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
