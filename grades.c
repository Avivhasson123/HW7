#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grades.h"
#include "linked-list.h"

#define ERROR -1

struct grades {
	struct list *students;
};

struct student {
	char *name;
	int id;
	struct course *courses;
	int num_courses;
};

struct course {
	char *course_name;
	int grade;
};


void clone_course(struct course *course, struct course *target);
struct course* course_init(const char *name, int grade);
struct student* student_init(const char *name, int id);



/* @brief Destroy “student”. always succeed */
void elem_destroy(void *element) {

	struct student *student = (struct student*) element;
	if (student == NULL) { 
		return;
	}
	free(student->name);
	for (int i = 0; i < student->num_courses; i++) {
		free(student->courses[i].course_name);
	}
	free(student->courses);
	free(student);
}

/* @brief Destroy “grades”. always succeed */
void grades_destroy(struct grades *grades) {

	list_destroy(grades->students);
	free(grades);

}

/* @brief clone “element” to “output”. Returns 0 on success */
int elem_clone(void *element, void **output) {

	struct student *original = (struct student*) element;
	if (original == NULL) {
		return ERROR;
	}
	struct student *clone = (struct student*) malloc(sizeof(struct student));
	if (clone == NULL) {
		return ERROR; 
	}
	clone->name = (char*) malloc(strlen(original->name) + 1 * sizeof(char));
	if (clone->name == NULL) {
		free(clone);
		return ERROR;
	}
	strcpy(clone->name, original->name);
	clone->id = original->id;
	clone->num_courses = original->num_courses;
	clone->courses =
			(struct course*) malloc(sizeof(struct course)*clone->num_courses);
	for (int i = 0; i < original->num_courses; i++) {
		clone_course(&original->courses[i], &clone->courses[i]);
	}

	*output = clone;

	return 0;
}

/**
 * @brief Clones existing struct and copies it to a new struct.
 */
void clone_course(struct course *course, struct course *target) {

	struct course *original = course;
	struct course *clone = target;
	if (clone == NULL) {
		return;
	}
	clone->course_name = (char*) malloc(strlen(original->course_name) + 1);
	if (clone->course_name == NULL) {
		return;
	}
	strcpy(clone->course_name, original->course_name);
	clone->grade = original->grade;

}

/**
 * @brief Initializes the "course" data-structure.
 * @returns A pointer to the data-structure, of NULL in case of an error
 */
struct course* course_init(const char *name, int grade) {
	struct course *course = (struct course*) malloc(sizeof(course));
	if (course == NULL) {
		return NULL;
	}
	course->course_name = (char*) malloc(strlen(name) + 1);
	if (course->course_name == NULL) {
		free(course);
		return NULL;
	}
	strcpy(course->course_name, name);
	course->grade = grade;
	return course;
}

/**
 * @brief Initializes the "student" data-structure.
 * @returns A pointer to the data-structure, of NULL in case of an error
 */
struct student* student_init(const char *name, int id) {

	struct student *student = (struct student*) malloc(sizeof(struct student));
	if (student == NULL) {
		return NULL;
	}
	student->name = (char*) malloc(strlen(name) + 1);
	if (student->name == NULL) {
		free(student);
		return NULL;
	}
	struct course *courses = (struct course*) malloc(sizeof(struct course));
	if (courses == NULL) {
		free(student->name);
		free(student);
		return NULL;
	}
	student->courses = courses;
	student->num_courses = 0;
	student->id = id;
	strcpy(student->name, name);

	return student;
}

/**
 * @brief Initializes the "grades" data-structure.
 * @returns A pointer to the data-structure, of NULL in case of an error
 */
struct grades* grades_init() { 

	struct grades *grades;
	grades = (struct grades*) malloc(sizeof(struct grades));
	if (grades == NULL) {
		return NULL;
	}
	grades->students = list_init(elem_clone, elem_destroy);

	return grades;
}

/**
 * @brief Adds a student with "name" and "id" to "grades"
 * @returns 0 on success
 * @note Failes if "grades" is invalid, or a student with 
 * the same "id" already exists in "grades"
 */
int grades_add_student(struct grades *grades, const char *name, int id) {

	if (grades == NULL) {
		return ERROR;
	}
	struct node *it = list_begin(grades->students);
	struct student *temp = (struct student*)list_get(it);
	int size = list_size(grades->students);

	for(int i=0; i<size; i++) { 
			if (temp->id == id) {
				return ERROR;
			}
			it = list_next(it);
			temp = list_get(it);
	}

	struct student *student = student_init(name, id);
	int x = list_push_back(grades->students, student);
	if (x != 0) {
		return ERROR;
	}
	return 0;
}

/**
 * @brief Adds a course with "name" and "grade" to the student with "id"
 * @return 0 on success
 * @note Failes if "grades" is invalid, if a student with "id" does not exist
 * in "grades", if the student already has a course with "name", or if "grade"
 * is not between 0 to 100.
 */
int grades_add_grade(struct grades *grades,
					 const char *name,
					 int id,
					 int grade) {
	int flag = ERROR;
	if (grades == NULL || (grade < 0 || grade > 100)) {
		return ERROR;
	}
	struct node *it = list_begin(grades->students);
	struct student *temp = list_get(it);
	int size = list_size(grades->students);

	for (int i=0; i<size; i++) {
		if (temp->id == id) {
			flag = 0;

			for (int i = 0; i<temp->num_courses; i++) {
				if (strcmp(temp->courses[i].course_name,name)== 0) {
					return ERROR;
				}
				
			}
			struct course *course = course_init(name, grade);
			int bigger = (temp->num_courses + 1) * sizeof(struct course);
			temp->courses = (struct course*) realloc(temp->courses, bigger);
			if (temp->courses == NULL) {
				return ERROR;
			}
			clone_course(course,&temp->courses[temp->num_courses]);
			temp->num_courses++;
			free(course->course_name);
			free(course);

		}
		it = list_next(it);
		temp =list_get(it);
	}
	return flag;
}
/**
 * @brief Calcs the average of the student with "id" in "grades".
 * @param[out] out This method sets the variable pointed by "out" to the
 * student's name. Needs to allocate memory. The user is responsible for
 * freeing the memory.
 * @returns The average, or -1 on error
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note If the student has no courses, the average is 0.
 * @note On error, sets "out" to NULL.
 */
float grades_calc_avg(struct grades *grades, int id, char **out) {
	float avg = 0;
	int flag = ERROR;
	if (grades == NULL) {
		out = NULL;
		return ERROR;
	}
	struct node *it = list_begin(grades->students);
	struct student *temp = list_get(it);
	int size = list_size(grades->students);
	for(int i=0; i<size; i++) {
		if (temp->id == id) {
			if (temp->num_courses == 0) {
				*out = (char*)malloc(strlen(temp->name)+1);
				if(*out == NULL){
					return ERROR; 
				}
				strcpy(*out, temp->name);
				return avg;
			}
			for (int i = 0; i < temp->num_courses; i++) {
				avg = avg + (temp->courses[i].grade);
			}
			*out = (char*)malloc(strlen(temp->name)+1);
			if(*out == NULL){
				return ERROR; 
			}
				strcpy(*out, temp->name);
				avg = avg / (temp->num_courses);
				flag = 0;
		}
		it = list_next(it);
		temp = list_get(it);
	}
	if (flag == ERROR) {
		return ERROR;
	}

	return avg;
}


/**
 * @brief Prints the courses of the student with "id" in the following format:
 * STUDENT-NAME STUDENT-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note The courses should be printed according to the order 
 * in which they were inserted into "grades"
 */
int grades_print_student(struct grades *grades, int id) {

	int flag = ERROR;
	if (grades == NULL) {
		return ERROR;
	}
	struct node *it = list_begin(grades->students);
	struct student *temp = list_get(it);
	while (temp != NULL) {
		if (temp->id == id) {
			if(temp->num_courses == 0){
				printf("%s %d:\n", temp->name, temp->id);
			} else {
				printf("%s %d: ", temp->name, temp->id);
			}
			for (int i = 0; i < temp->num_courses; i++) {
				int grade = temp->courses[i].grade;
				if (i == temp->num_courses - 1) {
					printf("%s %d\n", temp->courses[i].course_name, grade);
				} else {
				printf("%s %d, ", temp->courses[i].course_name, grade);
				}
			}
			flag = 0;
		}
		it = list_next(it);
		temp = list_get(it);
	}
	return flag;
}

/**
 * @brief Prints all students in "grade", in the following format:
 * STUDENT-1-NAME STUDENT-1-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * STUDENT-2-NAME STUDENT-2-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid
 * @note The students should be printed according to the order 
 * in which they were inserted into "grades"
 * @note The courses should be printed according to the order 
 * in which they were inserted into "grades"
 */
int grades_print_all(struct grades *grades) {

	int id = 0;
	int i=0;
	if (grades == NULL) {
		return ERROR;
	}
	struct node *it = list_begin(grades->students);
	struct student *temp = list_get(it);
	while (i < list_size(grades->students)) {
		id = temp->id;
		grades_print_student(grades, id);
		it = list_next(it);
		temp =list_get(it);
		i++;

	}
	return 0;
}
