#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef _WIN32
	#include <io.h>
	#include <fcntl.h>
#endif

#define B_SET "\e[1;1m"
#define B_RED "\e[1;31m"
#define SET "\e[0;0m"
#define RED "\e[0;31m"

FILE* fptr;
const char* file = "./TodoList.txt";

int DaysOfMonth(int v) {
	switch(v) {
		case 2: return v & 3 ? 29 : 28;
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
	if (curr) curr->status = ~(curr->status);
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
	printf("%02d/%02d/%04d", date.d, date.m, date.y);
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

	printf("===============================================================================================\n");
	printf(B_RED"| %-3s | %-6s | %-8s | %-10s | %-10s | %-40s \n", "STT", "Status", "Priority", "Start-Date", " Due-Date ", "                 Detail"SET);
	printf("-----------------------------------------------------------------------------------------------\n");

	while (curr) {
		printf("| %-3d | %-3s", index, "");
		#ifdef _WIN32
			_setmode(_fileno(stdout), _O_U16TEXT);
			wprintf((curr->status) ? L"\x2713" : L"_");
			_setmode(_fileno(stdout), _O_TEXT);
		#else
			printf((curr->status) ? "\xE2\x9C\x93" : "_");
		#endif
		printf("%-2s ", "");
		printf("| %4s%-4d | ", "", curr->priority);
		printDate(curr->start);
		printf(" | ");
		printDate(curr->due);
		printf(" | ");
		printf("%s\n", curr->detail);

		curr = curr->next;
		index++;
	}
	printf("===============================================================================================\n");
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
	if (!file) {
		printf("Cannot open file!\n");
		return;
	}
	
	fprintf(fptr, "Task: %s\n", task->detail);
	fprintf(fptr, "Status: %s", (task->status) ? "Done" : "Not_Done");
	fprintf(fptr, "\tPriority: %d\tStart->Due_Date: %02d/%02d/%04d -> %02d/%02d/%04d\n", task->priority, task->start.d, task->start.m, task->start.y, task->due.d, task->due.m, task->due.y);
	fclose(fptr);
}

void resaveToFile(Task* head, const char* file_name) {
	fptr = fopen(file_name, "w");
	
	Task* task = head;
	while (task != NULL) {
		fprintf(fptr, "Task: %s\n", task->detail);
		fprintf(fptr, "Status: %s", (task->status) ? "Done" : "Not_Done");
		fprintf(fptr, "\tPriority: %d\tStart->Due_Date: %02d/%02d/%04d -> %02d/%02d/%04d\n", task->priority, task->start.d, task->start.m, task->start.y, task->due.d, task->due.m, task->due.y);
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
	int tskCount = 0;
	
	while (fscanf(fptr, "Task: %[^\n]%*c", detail) == 1) {
		
		Task* new_task = (Task*)malloc(sizeof(Task));
		strcpy(new_task->detail, detail);
		
		if (fscanf(fptr, "%[^\n]%*c", line) == 1) {
			sscanf(line, "Status: %8s\tPriority: %d\tStart->Due_Date: %d/%d/%d -> %d/%d/%d", statusChar, &(new_task->priority), &(new_task->start.d), &new_task->start.m, &new_task->start.y, &new_task->due.d, &new_task->due.m, &new_task->due.y);
			new_task->status = (strcmp(statusChar, "Done") == 0) ? 1 : 0;
		} else {
			free(new_task);
			continue;
		}
		
		new_task->next = *head;
		*head = new_task;
		tskCount++;
	}
	
	fclose(fptr);
	return tskCount;
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
	
	if(!loadTasksFromFile(&head, file)) printf("There are no tasks yet!\n");
	
	printf("\n********* Main menu *********");
	printf("\n\tPress 1 to add a task");
	printf("\n\tPress 2 to delete a task");
	printf("\n\tPress 3 to change status of a task");
	printf("\n\tPress 4 to display all tasks by priority");
	printf("\n\tPress 5 to display all tasks by due date");
	printf("\n\tPress 6 to display all tasks by status");
	printf("\n\tPress 0 to exit");
	
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
				/*printf("Please enter:\n");
				printf("%3s", "Your task: ");
				scanf("%[^\n]%*c", task);
				printf("%3s", "Task's priority (1->5): ");
				scanf("%d", &priority);
				printf("%3s", "Start day (dd/mm/yyyy): ");
				scanf("%d%*c%d%*c%d", start.d, start.m, start.y);
				printf("%3s", "Due day (dd/mm/yyyy): ");
				scanf("%d%*c%d%*c%d", due.d, due.m, due.y);*/
				//Cach 2
				printf("Please enter your task following this form:\n");
				printf("\tYour Task, Task's_priority(1->5), Start-day(dd/mm/yyyy), Due-day(dd/mm/yyyy)\n");
				scanf("%[^\n]%*c", all);
				if (sscanf(all, "%[^,], %d, %d/%d/%d, %d/%d/%d", task, &priority, &start.d, &start.m, &start.y, &due.d, &due.m, &due.y) == 8) {
					temp = newTask(task, priority, 0, start, due);
					addTask(&head, temp);
					saveTaskToFile(temp, file);
				} else printf("Wrong format, please type again!\n");
				break;
				
			case 2:
				printf("Enter the number of that task: ");
				scanf("%d", &choice);
				deleteTask(&head, choice);
				resaveToFile(head, file);
				break;
				
			case 3:
				printf("Enter STT of the task to change status: ");
				scanf("%d", &choice);
				changeStatus(head, choice);
				resaveToFile(head, file);
				break;
				
			case 4:
				head = mergeList(&head, 1);
				displayTaskss(head, 1);
				break;
				
			case 5:
				head = mergeList(&head, 2);
				displayTaskss(head, 2);
				break;
				
			case 6:
				head = mergeList(&head, 3);
				displayTaskss(head, 3);
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
