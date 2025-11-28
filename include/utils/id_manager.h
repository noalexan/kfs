#pragma once

#include <bitmap/bitmap.h>
#include <types.h>

typedef struct s_id_manager {
	t_bitmap bitmap;
	ssize_t  next_free;
} t_id_manager;

t_id_manager *id_manager_create(size_t max_id);
void          id_manager_destroy(t_id_manager *mgr);
ssize_t       id_manager_alloc(t_id_manager *mgr);
void          id_manager_free(t_id_manager *mgr, size_t id);
bool          id_manager_reserve_id(t_id_manager *mgr, size_t id);
