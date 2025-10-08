#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define FILE_NAME "users.txt"

typedef struct
{
  int id;
  char name[50];
  int age;
} User;

void addUser();
void displayUsers();
void updateUser();
void deleteUser();
void displayMenu();

bool canOpenFiles(FILE **file_pointer, FILE **temp, const char *action);
void replaceFiles(FILE *file_pointer, FILE *temp);

int main()
{
  displayMenu();
  return 0;
}

void displayMenu()
{
  int choice;

  while (1)
  {
    printf("\n-- user Details --\n");
    printf("1. Add User\n");
    printf("2. Display Users\n");
    printf("3. Update User\n");
    printf("4. Delete User\n");
    printf("5. Exit\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
      addUser();
      break;
    case 2:
      displayUsers();
      break;
    case 3:
      updateUser();
      break;
    case 4:
      deleteUser();
      break;
    case 5:
      printf("Exiting...");
      return;
    default:
      printf("Invalid Choice!\n");
    }
  }
}

void addUser()
{
  FILE *filePointer = fopen(FILE_NAME, "r");

  User u;
  int lastId = 0;

  if (filePointer != NULL)
  {
    while (fscanf(filePointer, "%d %s %d", &u.id, u.name, &u.age) == 3)
    {
      lastId = u.id;
    }
    fclose(filePointer);
  }

  u.id = lastId + 1;
  getchar();

  printf("Enter Name: ");
  fgets(u.name, sizeof(u.name), stdin);
  u.name[strcspn(u.name, "\n")] = 0;

  printf("Enter Age: ");
  scanf("%d", &u.age);
  getchar();

  filePointer = fopen(FILE_NAME, "a");
  if (filePointer == NULL)
  {
    printf("Error opening file!\n");
    return;
  }

  fprintf(filePointer, "%d %s %d\n", u.id, u.name, u.age);

  if (fclose(filePointer) == 0)
  {
    filePointer = NULL;
    printf("User Added Successfully!");
  }
  else
  {
    printf("file cannot be closed!");
  }
}

void displayUsers()
{
  FILE *filePointer = fopen(FILE_NAME, "r");
  if (filePointer == NULL)
  {
    printf("No users found. File not created yet.\n");
    return;
  }

  User u;

  printf("\n--- User Records ---\n");

  while (fscanf(filePointer, "%d %s %d", &u.id, u.name, &u.age) == 3)
  {
    printf("ID: %d , Name: %s , Age: %d\n", u.id, u.name, u.age);
  }
  fclose(filePointer);
}

bool canOpenFiles(FILE **filePointer, FILE **temp, const char *action)
{
  *filePointer = fopen(FILE_NAME, "r");
  if (*filePointer == NULL)
  {
    printf("No records to %s.\n", action);
    return false;
  }

  *temp = fopen("temp.txt", "w");
  if (*temp == NULL)
  {
    printf("Error creating temp file.\n");
    fclose(*filePointer);
    return false;
  }

  return true;
}

void replaceFiles(FILE *filePointer, FILE *temp)
{
  fclose(filePointer);
  fclose(temp);
  remove(FILE_NAME);
  rename("temp.txt", FILE_NAME);
}

void updateUser()
{
  FILE *filePointer, *temp;

  if (!canOpenFiles(&filePointer, &temp, "update"))
  {
    return;
  }

  int id;
  bool isFound = false;
  printf("Enter ID to update: ");
  scanf("%d", &id);
  getchar();

  User u;
  while (fscanf(filePointer, "%d %s %d", &u.id, u.name, &u.age) == 3)
  {
    if (u.id == id)
    {
      isFound = true;
      printf("Enter new Name: ");
      fgets(u.name, sizeof(u.name), stdin);
      u.name[strcspn(u.name, "\n")] = 0;

      printf("Enter new Age: ");
      scanf("%d", &u.age);
    }
    fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
  }

  replaceFiles(filePointer, temp);

  if (isFound)
  {
    printf("User updated successfully!\n");
  }
  else
  {
    printf("User with ID %d not found.\n", id);
  }
}

void deleteUser()
{
  FILE *filePointer, *temp;
  if (!canOpenFiles(&filePointer, &temp, "delete"))
  {
    return;
  }

  int id;
  bool isFound = false;
  printf("Enter ID to delete: ");
  scanf("%d", &id);
  getchar();

  User u;
  while (fscanf(filePointer, "%d %s %d", &u.id, u.name, &u.age) == 3)
  {
    if (u.id == id)
    {
      isFound = true;
      continue;
    }
    fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
  }

  replaceFiles(filePointer, temp);

  if (isFound)
  {
    printf("User deleted successfully!\n");
  }
  else
  {
    printf("User with ID %d not found.\n", id);
  }
}
