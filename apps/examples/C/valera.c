#include "../../include/stdio.h"
#include "../../extlibs/ValeraC/valera.h"

int main() {
	valera_node_t *object = valera_new();
	valera_push_string(object, "name", "David");

	valera_print(object);
	puts(""); // Newline

	return 0;
}
