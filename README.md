# db.h

**db.h** is header-only database.

```c

#define DB_IMPLEMENTATION
#include "./db.h"

#include <time.h>
#include <stdio.h>

typedef struct {
    size_t id;
    time_t ts_created;
} User;

typedef struct {
    size_t count;
    User items[1024];
} Users;

int main(void)
{
    Users* users = db_table(Users);

    users->items[users->count++] = (User) {
        .id = users->count,
        .ts_created = time(0)
    };

    for (size_t i = 0; i < users->count; i++) {
        User* it = &users->items[i];
        printf("id: %zu, ts_created: %zu\n", it->id, it->ts_created);
    }

    return 0;
}

```
