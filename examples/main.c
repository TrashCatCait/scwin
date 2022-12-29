#include <scwin.h>

#include <stdio.h>
#include <stdlib.h>

void key_press() {
	printf("Key was pressed\n");
	return;
}

int main() {
	scwin_ptr window = NULL;

	window = scwin_create(NULL);
	
	scwin_map(window);
	
	scwin_set_key_press_fn(window, key_press);

	while(!scwin_should_close(window)) {
		scwin_poll_events(window);
		//Any of your code here
	}

	scwin_destroy(window);

	return 0;
}
