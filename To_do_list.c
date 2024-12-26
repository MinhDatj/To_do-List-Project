#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main() {
	
	printf("\n********* Main menu *********");
		printf("\n\tPress 1 to add a task");
		printf("\n\tPress 2 to mark completed task");
		printf("\n\tPress 3 to display all tasks");
		printf("\n\tPress 4 to edit task");
		printf("\n\tPress 1 to add a task");
		printf("\n\tPress 1 to add a task");
		printf("\n\tPress 0 to exit");
	
	while (true) {
		int option;
		printf("\nEnter your choice: ");
		if (scanf("%d", &option) != 1) {
			printf("Please enter again.\n");
			while (getchar() != '\n');
			continue;
		}
		getchar();
		
		switch (option) {
			case 0:
				printf("Exiting.\n");
				
				return 0;
		}
	}
}