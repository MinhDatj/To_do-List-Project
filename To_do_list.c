#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int DaysOfMonth(int v) {
	switch(v) {
		case 2: v & 3 ? return 29 : return 28;
		case 4: case 6: case 9: case 11: return 30;
		return 31;
	}
}

int limt(int min, int max, int v) {
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

typedef struct Date {
	int d, m, y;
	//d = limit(1, DaysOfMonth(d), d);
	//m = limit(1, 12, m);
} Date;

typedef struct Task {
	
} Task;

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