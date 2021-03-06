#include <stdlib.h>
#include <zm.h>

#define NTASKS 10

int counter = 1;

ZMTASKDEF( mycoroutine )
{
	struct Data {
		int id;
	} *self = zmdata;

	ZMSTART

	zmstate 1:
		self = malloc(sizeof(struct Data));
		self->id = counter++;
		zmData(self);
		printf("task %d: -init-\n", self->id);
		zmyield (rand() % 2) ? 2 : 4;

	zmstate 2:
		printf("task %d: Hello\n", self->id);
		zmyield 3;

	zmstate 3:
		printf("task %d: world!\n", self->id);
		zmyield zmTERM;

	zmstate 4:
		printf("task %d: try again\n", self->id);
		zmyield (rand() % 2) ? 2 : 4;



	zmstate ZM_TERM:
		printf("task %d: -end-\n", ((self) ? (self->id) : -1));
		if (self)
			free(self);

	ZMEND
}


int main() {
	int i;
	zm_VM *vm = zm_newVM("test VM");
	for (i = 0; i < NTASKS; i++) {
		zm_State *s = zm_newTasklet(vm, mycoroutine, NULL);
		zm_resume(vm, s, NULL);
	}

	while(zm_go(vm, 1));

	zm_closeVM(vm);
	zm_go(vm, 1000);
	zm_freeVM(vm);

	return 0;
}

