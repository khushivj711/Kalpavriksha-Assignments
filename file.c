#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"

typedef struct
{
  int id;
  char name[50];
  int age;
} user;

void addUser();
void displayUsers();
void updateUser();
void deleteUser();

int main()
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
      exit(0);
    default:
      printf("Invalid Choice!\n");
    }
  }

  return 0;
}

void addUser()
{
  FILE *fp = fopen(FILE_NAME, "a");
  if (fp == NULL)
  {
    printf("Error opening file!\n");
    return;
  }

  user u;

  printf("Enter ID: ");
  scanf("%d", &u.id);
  getchar();

  printf("Enter Name: ");
  fgets(u.name, sizeof(u.name), stdin);
  u.name[strcspn(u.name, "\n")] = 0;

  printf("Enter Age: ");
  scanf("%d", &u.age);

  fprintf(fp, "%d %s %d\n", u.id, u.name, u.age);
  int result = fclose(fp);

  if (result == 0)
  {
    fp = NULL;
    printf("User Added Successfully!");
  }
  else
  {
    printf("file cannot be closed!");
    return;
  }
}

void displayUsers()
{
  FILE *fp = fopen(FILE_NAME, "r");
  if (fp == NULL)
  {
    printf("No users found. File not created yet.\n");
    return;
  }

  user u;

  printf("\n--- User Records ---\n");

  while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3)
  {
    printf("ID: %d , Name: %s , Age: %d\n", u.id, u.name, u.age);
  }
  fclose(fp);
}

void updateUser()
{
  FILE *fp = fopen(FILE_NAME, "r");
  if (fp == NULL)
  {
    printf("No records to update.\n");
    return;
  }

  FILE *temp = fopen("temp.txt", "w");
  if (temp == NULL)
  {
    printf("Error creating temp file.\n");
    fclose(fp);
    return;
  }

  int id, found = 0;
  printf("Enter ID to update: ");
  scanf("%d", &id);

  user u;
  while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3)
  {
    if (u.id == id)
    {
      found = 1;
      printf("Enter new Name: ");
      scanf("%s", u.name);
      printf("Enter new Age: ");
      scanf("%d", &u.age);
    }
    fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
  }

  fclose(fp);
  fclose(temp);

  remove(FILE_NAME);
  rename("temp.txt", FILE_NAME);

  if (found)
    printf("User updated successfully!\n");
  else
    printf("User with ID %d not found.\n", id);
}

void deleteUser()
{
  FILE *fp = fopen(FILE_NAME, "r");
  if (fp == NULL)
  {
    printf("No records to delete.\n");
    return;
  }

  FILE *temp = fopen("temp.txt", "w");
  if (temp == NULL)
  {
    printf("Error creating temp file.\n");
    fclose(fp);
    return;
  }

  int id, found = 0;
  printf("Enter ID to delete: ");
  scanf("%d", &id);

  user u;
  while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3)
  {
    if (u.id == id)
    {
      found = 1;
      continue;
    }
    fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
  }

  fclose(fp);
  fclose(temp);

  remove(FILE_NAME);
  rename("temp.txt", FILE_NAME);

  if (found)
    printf("User deleted successfully!\n");
  else
    printf("User with ID %d not found.\n", id);
}