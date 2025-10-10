#include <stdio.h>
#include <stdbool.h>

// Structure to store student details
typedef struct
{
  char name[50];         // Student name
  int rollNo;            // Roll number
  float subjectMarks[3]; // Marks of 3 subjects
  float totalMarks;      // Total of all subjects
  float averageMarks;    // Average of marks
  char grade;            // Grade based on average
} StudentDetails;

// Function declarations
void processStudentPerformance();
float calculateTotalMarks(float marks1, float marks2, float marks3);
float calculateAverageMarks(float totalMarks);
char assignGrade(float averageMarks);
void displayPerformancePattern(char grade);
void printRollNumbersRecursively(StudentDetails students[], int index, int totalStudents);
bool isValidStudentCount(int studentCount);
bool isValidMarks(float marks[]);

int main()
{
  processStudentPerformance(); // Run main logic
  return 0;
}

// Handles entire student performance process
void processStudentPerformance()
{
  int noOfStudents;
  printf("Enter Number of Students: ");
  scanf("%d", &noOfStudents);
  getchar(); // To consume leftover newline

  if (!isValidStudentCount(noOfStudents))
  {
    // Validate number of students
    printf("Invalid number of students! Please enter between 1 and 100.\n");
    return;
  }

  StudentDetails students[noOfStudents]; // Array to store student records

  for (int i = 0; i < noOfStudents; i++)
  {
    // Input student data
    scanf("%d %s %f %f %f", &students[i].rollNo, students[i].name, &students[i].subjectMarks[0], &students[i].subjectMarks[1], &students[i].subjectMarks[2]);

    if (!isValidMarks(students[i].subjectMarks))
    {
      // Validate entered marks
      printf("Invalid marks! Please enter values between 0 and 100.\n");
      return;
    }

    // Calculate total, average, and grade
    students[i].totalMarks = calculateTotalMarks(students[i].subjectMarks[0], students[i].subjectMarks[1], students[i].subjectMarks[2]);
    students[i].averageMarks = calculateAverageMarks(students[i].totalMarks);
    students[i].grade = assignGrade(students[i].averageMarks);
  }

  printf("\n");

  for (int i = 0; i < noOfStudents; i++)
  {
    // Display student details
    printf("Roll: %d\n", students[i].rollNo);
    printf("Name: %s\n", students[i].name);
    printf("Total: %.0f\n", students[i].totalMarks);
    printf("Average: %.2f\n", students[i].averageMarks);
    printf("Grade: %c\n", students[i].grade);

    // Skip performance pattern if failed
    if (students[i].averageMarks < 35)
    {
      printf("\n");
      continue;
    }

    printf("Performance: ");
    displayPerformancePattern(students[i].grade);
    printf("\n\n");
  }

  // Print roll numbers recursively
  printf("List of Roll Numbers (via recursion): ");
  printRollNumbersRecursively(students, 0, noOfStudents);
}

// Validate student count (1 to 100)
bool isValidStudentCount(int studentCount)
{
  return !(studentCount < 1 || studentCount > 100);
}

// Validate each subject mark (0 to 100)
bool isValidMarks(float marks[])
{
  return !(marks[0] < 0 || marks[0] > 100 || marks[1] < 0 || marks[1] > 100 || marks[2] < 0 || marks[2] > 100);
}

// Calculate total marks
float calculateTotalMarks(float marks1, float marks2, float marks3)
{
  return marks1 + marks2 + marks3;
}

// Calculate average marks
float calculateAverageMarks(float totalMarks)
{
  return totalMarks / 3;
}

// Assign grade based on average marks
char assignGrade(float averageMarks)
{
  if (averageMarks >= 85)
  {
    return 'A';
  }
  else if (averageMarks < 85 && averageMarks >= 70)
  {
    return 'B';
  }
  else if (averageMarks < 70 && averageMarks >= 50)
  {
    return 'C';
  }
  else if (averageMarks < 50 && averageMarks >= 35)
  {
    return 'D';
  }
  else
  {
    return 'F';
  }
}

// Display performance stars based on grade
void displayPerformancePattern(char grade)
{
  int stars = 0;

  switch (grade)
  {
  case 'A':
    stars = 5;
    break;
  case 'B':
    stars = 4;
    break;
  case 'C':
    stars = 3;
    break;
  case 'D':
    stars = 2;
    break;
  default:
    stars = 0;
  }

  for (int i = 0; i < stars; i++)
  {
    printf("*");
  }
}

// Recursively print roll numbers
void printRollNumbersRecursively(StudentDetails students[], int index, int totalStudents)
{
  if (index == totalStudents)
  {
    return;
  }

  printf("%d ", students[index].rollNo);
  printRollNumbersRecursively(students, index + 1, totalStudents);
}
