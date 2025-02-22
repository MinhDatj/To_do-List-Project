#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#define B_BLUE   "\033[1;34m"
#define B_CYAN   "\033[1;36m"
#define B_ORANGE "\033[38;5;214m"
#define GRN		 "\033[92m"
#define RED		 "\033[91m"
#define BR_WHITE "\033[97m"
#define SET      "\033[0m"
#define VER_LINE "\033[1;36m|\033[0m" 

FILE* fptr;
const char* file = "./TodoList100.txt";

int DaysOfMonth(int m, int y) {
	switch(m) {
		case 2: return y & 3 ? 29 : 28;
		case 4: case 6: case 9: case 11: return 30;
		return 31;
	}
}

int limit(int min, int max, int v) {
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

typedef struct Date {
	int d, m, y;
} Date;

typedef struct Task {
	char detail[150];
	int priority;	//1,2,3,4,5
	int status;		//0(not done), 1(done) 
	Date start;		//dd/mm/yyyy
	Date due;
	struct Task* prev;
	struct Task* next;
} Task;

int listLen = 1;
int taskID = 1; 

Task* newTask(const char* detail, int priority, int status, Date start, Date due) {
	Task* new_task = (Task*)malloc(sizeof(Task));
	listLen++;
	strcpy(new_task->detail, detail);
	new_task->priority = priority;
	new_task->status = status;
	new_task->start = start;
	new_task->due = due;
	new_task->prev = new_task->next = NULL;
	return new_task;
}

void addTask(Task** head, Task* new_task) {
	if (!(*head)) {
		*head = new_task;
	} else {
		Task* curr = *head;
		while (curr->next) {
			curr = curr->next;
		}
		curr->next = new_task;
		new_task->prev = curr;
	}
}

void deleteTask(Task** head, int k) {
	if (!(*head)) {
		printf("List is empty!\n");
		return;
	}
	
	if (k < 1 || k > listLen) {
		printf("Type again!\n");
		return;
	}
	
	Task* curr = *head;
	if (k == 1) {
		*head = (*head)->next;
		if (*head) (*head)->prev = NULL;
	} else {
		for (int i = 1; i < k; i++) curr = curr->next;
		
		if (curr->prev) curr->prev->next = curr->next;
		if (curr->next) curr->next->prev = curr->prev;
	}
	
	free(curr);
	listLen--;
}

void editTask(Task* head, int key, int listLength, const char* new_detail, int new_priority, int new_status, Date new_start, Date new_due) {
    if (!head) {
        printf("List is empty!\n");
    }

    if (key < 1 || key > listLength) {
        printf("Number out of range in the list. Please type again!\n");
        return;
    }

    Task* curr = head;
    while (key-- != 1) curr = curr->next;
    if (!curr) return;

    strcpy(curr->detail, new_detail);
    curr->priority = new_priority;
    curr->status = new_status;
    curr->start = new_start;
    curr->due = new_due;
    printf("Edited task successfully!\n");
}

void changeStatus(Task* head, int k) {
	if (!head) {
		printf("List is empty!\n");
		return;
	}
	if (k < 1 || k > listLen) {
		printf("Type again!\n");
		return;
	}

	Task* curr = head;
	while (k-- != 1) {
		curr = curr->next;
	}
	if (curr) curr->status = !(curr->status);
	printf("Changing status succeed!\n");
}

void swapTask(Task** head, Task* a, Task* b) {
	if (!a || !b || a == b) return;
	
	if (a->prev) a->prev->next = b;
	if (b->prev) b->prev->next = a;
	
	if (a->next) a->next->prev = b;
	if (b->next) b->next->prev = a;
	
	Task* temp = a->prev;
	a->prev = b->prev;
	b->prev = temp;
	
	temp = a->next;
	a->next = b->next;
	b->next = temp;
	
	if (*head == a) *head = b;
	else if (*head == b) *head = a;
	
}

Task* splitList(Task* head) {
	if (!head || !(head->next)) return NULL;
	
	Task* slow = head;
	Task* fast = head->next;
	
	while (fast && fast->next) {
		slow = slow->next;
		fast = fast->next->next;
	}
	
	Task* half = slow->next;
	slow->next = NULL;
	if (half) half->prev = NULL;
	
	return half;
}

Task* byPriority(Task* first, Task* second) {
	if (!first) return second;
	if (!second) return first;
	
	if (first->priority <= second->priority) {
		first->next = byPriority(first->next, second);
		if (first->next) first->next->prev = first;
		return first;
	} else {
		second->next = byPriority(first, second->next);
		if (second->next) second->next->prev = second;
		return second;
	}
}

int compareDate(Date d1, Date d2) {
	if (d1.y != d2.y) return d1.y - d2.y;
	if (d1.m != d2.m) return d1.m - d2.m;
	return d1.d - d2.d;
}

Task* byDueDate(Task* first, Task* second) {
	if (!first) return second;
	if (!second) return first;
	
	if (compareDate(first->due, second->due) <= 0) {
		first->next = byDueDate(first->next, second);
		if (first->next) first->next->prev = first;
		return first;
	} else {
		second->next = byDueDate(first, second->next);
		if (second->next) second->next->prev = second;
		return second;
	}
}

Task* byStatus(Task* first, Task* second) {
	if (!first) return second;
	if (!second) return first;
	
	if (first->status) {
		first->next = byStatus(first->next, second);
		if (first->next) first->next->prev = first;
		return first;
	} else {
		second->next = byStatus(first, second->next);
		if (second->next) second->next->prev = second;
		return second;
	}
}

Task* mergeList(Task** head, int choice) {
	if (!head || !(*head) || !(*head)->next) return *head;

	Task* first = *head;
	Task* second = splitList(*head);

	first = mergeList(&first, choice);
	second = mergeList(&second, choice);

	if (choice == 1) return byPriority(first, second);
	else if (choice == 2) return byDueDate(first, second);
	else return byStatus(first, second);
}

void printDate(Date date) {
	printf(B_BLUE"%02d/%02d/%04d", date.d, date.m, date.y);
}

void splitText(const char* text, int maxLen, int index, int status, int priority, Date start, Date due) {
    int len = strlen(text);
    int ptr = 0;
    int first_line = 1;
    
    while (ptr < len) {
        if (len - ptr <= maxLen) {
            printf(B_CYAN "| %3s | %6s | " BR_WHITE "%-39s" SET " " B_CYAN "| %-8s | %-10s |  %-8s  |\n", " ", " ", text + ptr, "", "", "");
            printf(SET);
            break;
        }

        int cutPos = ptr + maxLen;

        while (cutPos > ptr && text[cutPos] != ' ') {
            cutPos--;
        }

        if (cutPos == ptr) {
            cutPos = ptr + maxLen;
        }
		
		if (first_line) {
			first_line = 0;	
			
			printf("\033[1;36m|\033[0m" BR_WHITE" %-3d "SET "\033[1;36m|\033[0m %-2s", index, "");
			printf(status ? "\xE2\x9C\x93" : "\xE2\x9C\x95");
			printf(B_CYAN"%-3s |" "\033[97m  %-39.*s", "", cutPos - ptr, text + ptr);
			printf("\033[1;36m|\033[0m \033[38;5;214m%4s%-4d \033[1;36m|\033[0m ", "", priority);
			printDate(start);
			printf(" \033[1;36m|\033[0m ");
			printDate(due);
			printf(" \033[1;36m|\033[0m\n");
		} 
		else {
        	printf(B_CYAN"| %3s | %6s | ", " ", " ");
        	printf(BR_WHITE "%-38.*s", cutPos - ptr, text + ptr);
        	printf(B_CYAN"| %-8s | %-10s |  %-8s  |\033[0m\n", "", "", "");
		}

        ptr = cutPos;
        while (text[ptr] == ' ') ptr++; 
    }
}

void displayTaskss(Task* head, int choice) {
	if (!head) {
		printf("There are no tasks in the list!\n");
		return;
	}
	
	if (choice == 1) printf("Tasks by priority:\n");
	else if (choice == 2) printf("Tasks by due_date:\n");
	else if (choice == 3) printf("Task by status:\n");

	int index = 1;
	Task* curr = head;

	printf(B_CYAN"===============================================================================================\n");
	printf("| %-3s | %-6s | %16s%-35s | %-8s | %-10s |  %-8s  |\n", 
		BR_WHITE"STT"B_CYAN, GRN"Status"B_CYAN, "", BR_WHITE"Detail" B_CYAN, B_ORANGE"Priority"B_CYAN, B_BLUE"Start-Date"B_CYAN, B_BLUE"Due-Date"B_CYAN);
	printf(B_CYAN"-----------------------------------------------------------------------------------------------\n"SET);
	
	while (curr) {
		if (strlen(curr->detail) < 40) {
			printf("\033[1;36m|\033[0m" "\033[97m %-3d "SET "\033[1;36m|\033[0m %-2s", index, "");
			printf((curr->status) ? "\xE2\x9C\x93" : "\xE2\x9C\x95");
			printf("%-3s "VER_LINE, "");
			if (strlen(curr->detail) == 39) printf(BR_WHITE"  %-38s", curr->detail);
			else printf(BR_WHITE"  %-38s ", curr->detail);
			printf("\033[1;36m|\033[0m %4s\033[38;5;214m%-4d \033[1;36m|\033[0m ", "",curr->priority);
			printDate(curr->start);
			printf(" \033[1;36m|\033[0m ");
			printDate(curr->due);
			printf(" \033[1;36m|\033[0m\n");
		}
		else splitText(curr->detail, 40, index, curr->status, curr->priority, curr->start, curr->due);
		
		if (!curr->next) printf(B_CYAN"===============================================================================================\n"SET);
		else printf(B_CYAN"+-----+--------+-----------------------------------------+----------+------------+------------+\n"SET);
		curr = curr->next;
		index++;
	}
}

void freeTasks(Task* head) {
	while (head) {
		Task* temp = head;
		head = head->next;
		free(temp);
	}
}

void saveTaskToFile(Task* task, const char* file_name) {
	fptr = fopen(file_name, "a");
	if (!fptr) {
		printf("Cannot open file!\n");
		return;
	}
	
	fprintf(fptr, "Task: %s\n", task->detail);
	fprintf(fptr, "\tStatus: %-10s", (task->status) ? "Done" : "Not_Done");
	fprintf(fptr, "Priority: %-5dStart->Due: %02d/%02d/%04d -> %02d/%02d/%04d\n", 
		task->priority, task->start.d, task->start.m, task->start.y, task->due.d, task->due.m, task->due.y);
	task = task->next;	fclose(fptr);
}

void resaveToFile(Task* head, const char* file_name) {
	fptr = fopen(file_name, "w");
	
	Task* task = head;
	while (task != NULL) {
		fprintf(fptr, "Task: %s\n", task->detail);
        fprintf(fptr, "\tStatus: %-10s", (task->status) ? "Done" : "Not_Done");
        fprintf(fptr, "Priority: %-5dStart->Due: %02d/%02d/%04d -> %02d/%02d/%04d\n", 
            task->priority, task->start.d, task->start.m, task->start.y, task->due.d, task->due.m, task->due.y);
		task = task->next;
	}
	fclose(fptr);
}

int loadTasksFromFile(Task** head, const char* file_name) {
	fptr = fopen(file_name, "r");
	if (!fptr) {
		return 0;
	}
	
	char detail[150];
	char line[50];
	char statusChar[9];
	
	while (fscanf(fptr, "Task: %[^\n]%*c", detail) == 1) {
		
		Task* new_task = (Task*)malloc(sizeof(Task));
		strcpy(new_task->detail, detail);
		
		if (fscanf(fptr, "%[^\n]%*c", line) == 1) {
			sscanf(line, " Status: %10s Priority: %d Start->Due: %2d/%2d/%4d -> %2d/%2d/%4d%*c", statusChar, &(new_task->priority), &(new_task->start.d), &new_task->start.m, &new_task->start.y, &new_task->due.d, &new_task->due.m, &new_task->due.y);
			new_task->status = (strcmp(statusChar, "Done") == 0) ? 1 : 0;
		} else {
			free(new_task);
			continue;
		}
		
		new_task->next = *head;
		*head = new_task;
		listLen++;
	}
	
	fclose(fptr);
	return listLen;
}

double get_current_time() {
	return (double)clock() / CLOCKS_PER_SEC;
}

int testing() {
	Task* head = NULL;
	Task* temp;
	char task[150], all[200];
	int priority;
	int status;		
	Date start;		
	Date due;
	int choice;
	double before, after;
	
	// before = get_current_time();
	choice = loadTasksFromFile(&head, file);
	// after = get_current_time();
	printf("running time: %.6f giây\n", after - before);
	if(!choice) printf("There are no tasks yet!\n");
	
	printf("\n********* Main menu *********");
	printf("\n\tPress:");
	printf("\n\t1. Add a task");
	printf("\n\t2. Delete a task");
	printf("\n\t3. Edit a task");
	printf("\n\t4. Change status of a task");
	printf("\n\t5. Display tasks by priority");
	printf("\n\t6. Display tasks by due date");
	printf("\n\t7. Display tasks by status");
	printf("\n\t0. Exit program");
	
	while (1) {
		printf("\nEnter your choice: ");
		if (scanf("%d%*c", &choice) != 1) {
			printf("Please enter again.\n");
			while (getchar() != '\n');
			continue;
		}
		
		switch (choice) {
			case 0:
				printf("Exiting program.\n");
				freeTasks(head);
				return 0;
			
			case 1:
				//Cach 1 
				printf("Please enter:\n");
				printf("\tYour task: ");
				scanf("%[^\n]%*c", task);
				printf("\tPriority(1->5): ");
				scanf("%d", &priority);	if (priority < 1 || priority > 5) printf("\t\tWrong range of number, please remember to edit later!\n");
				printf("\tStart_date(dd/mm/yyyy): ");
				scanf("%d/%d/%d", &start.d, &start.m, &start.y);
				printf("\tDue_date(dd/mm/yyyy): ");
				scanf("%d/%d/%d", &due.d, &due.m, &due.y);
				temp = newTask(task, priority, 0, start, due);
				// before = get_current_time();
				addTask(&head, temp);
				// after = get_current_time();
				printf("running time: %.6f giây\n", after - before);
				saveTaskToFile(temp, file);
				//Cach 2
				/*printf("Please enter your task following this form:\n");
				printf("\tTask1, 1->5(Priority), dd/mm/yyyy(Start_day), dd/mm/yyyy(Due_day)\n");
				scanf("%[^\n]%*c", all);
				if (sscanf(all, "%[^,], %d, %d/%d/%d, %d/%d/%d", task, &priority, &start.d, &start.m, &start.y, &due.d, &due.m, &due.y) == 8) {
					priority = limit(1, 5, priority);
					start.m = limit(1, 12, start.m);
					start.d = limit(1, DaysOfMonth(start.m, start.y), start.d);
					due.m = limit(1, 12, due.m);
					due.d = limit(1, DaysOfMonth(due.m, due.y), due.d);
					temp = newTask(task, priority, 0, start, due);
					addTask(&head, temp);
					saveTaskToFile(temp, file);
				} else printf("Wrong format, please type again!\n");*/
				break;
				
			case 2:
				printf("Please enter the number of that task: ");
				scanf("%d", &choice);
				// before = get_current_time();
				deleteTask(&head, choice);
				// after = get_current_time();
				printf("running time: %.6f giây\n", after - before);
				resaveToFile(head, file);
				break;
				
			case 3:
				printf("Please enter the number of that task: ");
				scanf("%d%*c", &choice);
				printf("Please enter:\n");
				printf("\tYour task: ");
				if (scanf("%[^\n]%*c", task) != 1) { printf("Please do it again!\n"); break; }
				printf("\tPriority(1->5): ");
				if (scanf("%d", &priority) != 1) { printf("Please do it again!\n"); break; }	
				if (priority < 1 || priority > 5) printf("\t\tWrong range of number, please remember to edit later!\n");
				printf("\tStart_date(dd/mm/yyyy): ");
				if (scanf("%d/%d/%d", &start.d, &start.m, &start.y) != 3) { printf("Please do it again!\n"); break; }
				printf("\tDue_date(dd/mm/yyyy): ");
				if (scanf("%d/%d/%d", &due.d, &due.m, &due.y) != 3) { printf("Please do it again!\n"); break; }
				// before = get_current_time();
				editTask(head, choice, listLen, task, priority, 0, start, due);
				// after = get_current_time();
				printf("running time: %.6f giây\n", after - before);
				resaveToFile(head, file);
				break;

			case 4:
				printf("Please enter the number of that task: ");

				scanf("%d", &choice);
				// before = get_current_time();
				changeStatus(head, choice);
				// after = get_current_time();
				printf("running time: %.6f giây\n", after - before);
				resaveToFile(head, file);
				break;
				
			case 5:
				// before = get_current_time();
				head = mergeList(&head, 1);
				// after = get_current_time();
				printf("merging time: %.6f giây\n", after - before);
				// before = get_current_time();
				displayTaskss(head, 1);
				// after = get_current_time();
				printf("running time: %.6f giây\n", after - before);
				break;
				
			case 6:
				// before = get_current_time();
				head = mergeList(&head, 2);
				// after = get_current_time();
				printf("merging time: %.6f giây\n", after - before);
				// before = get_current_time();
				displayTaskss(head, 2);
				// after = get_current_time();
				printf("running time: %.6f giây\n", after - before);
				break;
				
			case 7:
				// before = get_current_time();
				head = mergeList(&head, 3);
				// after = get_current_time();
				printf("merging time: %.6f giây\n", after - before);
				// before = get_current_time();
				displayTaskss(head, 3);
				// after = get_current_time();
				printf("running time: %.6f giây\n", after - before);
				break;
				
			default:
			printf("Please enter again!\n");
			break;
		
		}
	}
	freeTasks(head);
}

int main() {
	testing();
	return 0;
}
