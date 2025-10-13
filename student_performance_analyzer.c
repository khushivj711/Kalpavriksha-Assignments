#include <stdio.h>
#include <stdbool.h>

#define MAX_STUDENTS 100
#define NUM_SUBJECTS 3

const int PASS_MARKS = 35;

const int GRADE_A_THRESHOLD = 85;
const int GRADE_B_THRESHOLD = 70;
const int GRADE_C_THRESHOLD = 50;

const int STARS_GRADE_A = 5;
const int STARS_GRADE_B = 4;
const int STARS_GRADE_C = 3;
const int STARS_GRADE_D = 2;
const int STARS_GRADE_F = 0;

typedef enum
{
  GRADE_A = 'A',
  GRADE_B = 'B',
  GRADE_C = 'C',
  GRADE_D = 'D',
  GRADE_F = 'F'
} Grade;

typedef struct
{
  char name[50];
  int rollNo;
  float subjectMarks[NUM_SUBJECTS];
} StudentDetails;

void processStudentPerformance();
float calculateTotalMarks(StudentDetails student);
float calculateAverageMarks(float totalMarks);
Grade assignGrade(float averageMarks);
void displayPerformancePattern(Grade grade);
void printRollNumbersRecursively(StudentDetails students[], int index, int totalStudents);
bool isValidStudentCount(int studentCount);
bool isValidMarks(float marks[]);
void displayStudentPerformance(StudentDetails student);
void sortStudentsByRoll(StudentDetails students[], int noOfStudents);
void displayAllStudentsPerformance(StudentDetails students[], int totalStudents);
bool isDuplicateRoll(StudentDetails students[], int currentIndex);

int main()
{
  processStudentPerformance();
  return 0;
}

void processStudentPerformance()
{
  int noOfStudents;
  printf("Enter Number of Students: ");
  scanf("%d", &noOfStudents);
  getchar();

  if (!isValidStudentCount(noOfStudents))
  {
    printf("Invalid number of students! Please enter between 1 and 100.\n");
    return;
  }

  StudentDetails students[MAX_STUDENTS];

  for (int i = 0; i < noOfStudents; i++)
  {
    scanf("%d %s %f %f %f", &students[i].rollNo, students[i].name, &students[i].subjectMarks[0], &students[i].subjectMarks[1], &students[i].subjectMarks[2]);

    if (isDuplicateRoll(students, i))
    {
      printf("Duplicate roll number %d! Please enter again.\n", students[i].rollNo);
      i--;
      continue;
    }

    if (!isValidMarks(students[i].subjectMarks))
    {
      printf("Invalid marks! Please enter values between 0 and 100.\n");
      continue;
    }
  }

  sortStudentsByRoll(students, noOfStudents);

  printf("\n");

  displayAllStudentsPerformance(students, noOfStudents);

  int startIndex = 0;
  printf("List of Roll Numbers (via recursion): ");
  printRollNumbersRecursively(students, startIndex, noOfStudents);
  printf("\n");
}

void displayAllStudentsPerformance(StudentDetails students[], int totalStudents)
{
  for (int i = 0; i < totalStudents; i++)
  {
    displayStudentPerformance(students[i]);
  }
}

void displayStudentPerformance(StudentDetails student)
{
  float totalMarks = calculateTotalMarks(student);
  float averageMarks = calculateAverageMarks(totalMarks);
  Grade grade = assignGrade(averageMarks);

  printf("Roll: %d\n", student.rollNo);
  printf("Name: %s\n", student.name);
  printf("Total: %.0f\n", totalMarks);
  printf("Average: %.2f\n", averageMarks);
  printf("Grade: %c\n", grade);

  if (averageMarks >= PASS_MARKS)
  {
    printf("Performance: ");
    displayPerformancePattern(grade);
  }

  printf("\n\n");
}

void sortStudentsByRoll(StudentDetails students[], int noOfStudents)
{
  for (int i = 0; i < noOfStudents - 1; i++)
  {
    for (int j = i + 1; j < noOfStudents; j++)
    {
      if (students[i].rollNo > students[j].rollNo)
      {
        StudentDetails temp = students[i];
        students[i] = students[j];
        students[j] = temp;
      }
    }
  }
}

bool isDuplicateRoll(StudentDetails students[], int currentIndex)
{
  for (int i = 0; i < currentIndex; i++)
  {
    if (students[i].rollNo == students[currentIndex].rollNo)
    {
      return true;
    }
  }
  return false;
}

bool isValidStudentCount(int studentCount)
{
  return !(studentCount < 1 || studentCount > 100);
}

bool isValidMarks(float marks[])
{
  for (int i = 0; i < NUM_SUBJECTS; i++)
  {
    if (marks[i] < 0 || marks[i] > 100)
    {
      return false;
    }
  }
  return true;
}

float calculateTotalMarks(StudentDetails student)
{
  float total = 0;
  for (int i = 0; i < NUM_SUBJECTS; i++)
  {
    total += student.subjectMarks[i];
  }
  return total;
}

float calculateAverageMarks(float totalMarks)
{
  return totalMarks / NUM_SUBJECTS;
}

Grade assignGrade(float averageMarks)
{
  if (averageMarks >= GRADE_A_THRESHOLD)
  {
    return GRADE_A;
  }
  else if (averageMarks >= GRADE_B_THRESHOLD)
  {
    return GRADE_B;
  }
  else if (averageMarks >= GRADE_C_THRESHOLD)
  {
    return GRADE_C;
  }
  else if (averageMarks >= PASS_MARKS)
  {
    return GRADE_D;
  }
  else
  {
    return GRADE_F;
  }
}

void displayPerformancePattern(Grade grade)
{
  int stars = 0;
  switch (grade)
  {
  case GRADE_A:
    stars = STARS_GRADE_A;
    break;
  case GRADE_B:
    stars = STARS_GRADE_B;
    break;
  case GRADE_C:
    stars = STARS_GRADE_C;
    break;
  case GRADE_D:
    stars = STARS_GRADE_D;
    break;
  default:
    stars = STARS_GRADE_F;
  }

  for (int i = 0; i < stars; i++)
  {
    printf("*");
  }
}

void printRollNumbersRecursively(StudentDetails students[], int index, int totalStudents)
{
  if (index == totalStudents)
  {
    return;
  }

  printf("%d ", students[index].rollNo);
  printRollNumbersRecursively(students, index + 1, totalStudents);
}
