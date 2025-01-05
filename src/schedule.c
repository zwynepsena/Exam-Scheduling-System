#include "schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_START_TIME 8
#define MAX_START_TIME 17
#define MAX_END_TIME 20


// Create a new schedule with 7 days
struct Schedule* CreateSchedule() {
    struct Schedule* schedule = (struct Schedule*)malloc(sizeof(struct Schedule));
    if (!schedule) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    const char* days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    struct Day* firstDay = NULL;
    struct Day* prevDay = NULL;

    for (int i = 0; i < 7; i++) {
        struct Day* newDay = (struct Day*) malloc(sizeof(struct Day));

        strcpy (newDay->dayName, days[i]);
        newDay->examList = NULL;
        newDay->nextDay = NULL;

        if (firstDay == NULL) firstDay = newDay;
        else prevDay->nextDay = newDay;

        prevDay = newDay;
    }

    prevDay->nextDay = firstDay;
    schedule->head = firstDay;

    return schedule;
}

// Add an exam to a day in the schedule

int AddExamToSchedule(struct Schedule* schedule, const char* day, int startTime, int endTime, const char* courseCode) {
    if (endTime - startTime > 3 || startTime < MIN_START_TIME || endTime > MAX_END_TIME) {
        printf("Invalid exam");
        return 3;
    }

    struct Day* currentDay = schedule->head;
    while (currentDay != NULL && strcmp(currentDay->dayName, day) != 0) {
        currentDay = currentDay->nextDay;
    }
    
    struct Day* startDay = currentDay;

    int examDuration = endTime - startTime;
    int conflict = 0;

    do {
        struct Exam* currentExam = currentDay->examList;
        struct Exam* prevExam = NULL;
        
        while (currentExam != NULL && currentExam->startTime <= startTime) {
            prevExam = currentExam;
            currentExam = currentExam->next;
        }

        do {
            if (prevExam == NULL) {
                if (currentExam == NULL || currentExam->startTime >= endTime) {
                    struct Exam* newExam = CreateExam(startTime, endTime, courseCode);

                    newExam->next = currentExam;
                    currentDay->examList = newExam;

                    if (conflict == 0) {
                        printf("%s exam added to %s at time %d to %d without conflict.\n", courseCode, currentDay->dayName, startTime, endTime);
                        return 0;
                    } else {
                        printf("%s exam added to %s at time %d to %d due to conflict.\n", courseCode, currentDay->dayName, startTime, endTime);
                        return 1;
                    }

                } else if (currentExam->endTime <= MAX_START_TIME) {
                    startTime = currentExam->endTime;
                    endTime = startTime + examDuration;
                    conflict = 1;
                } else {
                    currentDay = currentDay->nextDay;
                    startTime = MIN_START_TIME;
                    endTime = startTime + examDuration;
                    conflict = 1;
                }
            } else if (prevExam->endTime <= startTime && (currentExam == NULL || currentExam->startTime >=  endTime)) {
                struct Exam* newExam = CreateExam(startTime, endTime, courseCode);

                prevExam->next = newExam;
                newExam->next = currentExam;

                if (conflict == 0) {
                    printf("%s exam added to %s at time %d to %d without conflict.\n", courseCode, currentDay->dayName, startTime, endTime);
                    return 0;
                } else {
                    printf("%s exam added to %s at time %d to %d due to conflict.\n", courseCode, currentDay->dayName, startTime, endTime);
                    return 1;
                }

            } else {
                startTime = prevExam->endTime;
                endTime = startTime + examDuration;
                conflict = 1;

                if (currentExam) {
                    prevExam = currentExam;
                    currentExam = currentExam->next;
                }
            }

        } while (startTime <= MAX_START_TIME);

        conflict = 1;
        currentDay = currentDay->nextDay;

        startTime = MIN_START_TIME;
        endTime = startTime + examDuration;
    } while (currentDay != startDay);

    printf("Schedule full. Exam cannot be added\n");
    return 2;
}

/* int AddExamToSchedule(struct Schedule* schedule, const char* day, int startTime, int endTime, const char* courseCode) {
    if (endTime - startTime > 3 || startTime < MIN_START_TIME || endTime > MAX_END_TIME) {
        printf("Invalid exam");
        return 3;
    }

    struct Day* currentDay = schedule->head;
    while (currentDay != NULL && strcmp(currentDay->dayName, day) != 0) {
        currentDay = currentDay->nextDay;
    }
    struct Day* startDay = currentDay;
    printf("day name: %s\n", currentDay->dayName);

    struct Exam* currentExam = currentDay->examList;
    struct Exam* prevExam = NULL;

    do {
        while (currentExam != NULL && currentExam->startTime <= startTime) {
            printf("current exam %d\n", currentExam->startTime);
            prevExam = currentExam;
            currentExam = currentExam->next;
        }

        if (prevExam == NULL) {
            if (currentExam == NULL || endTime <= currentExam->startTime) {
                struct Exam* newExam = CreateExam(startTime, endTime, courseCode);
                newExam->next = currentDay->examList;
                currentDay->examList = newExam;

                printf("%s exam added to %s at time %d to %d without conflict.\n", courseCode, currentDay->dayName, startTime, endTime);
                return 0;
            }
        } else if (prevExam->endTime <= startTime && (currentExam == NULL || endTime <= currentExam->startTime) && prevExam->endTime <= MAX_START_TIME) {
            struct Exam* newExam = CreateExam(startTime, endTime, courseCode);
            newExam->next = currentExam;
            prevExam->next = newExam;

            printf("%s exam added to %s at time %d to %d without conflict.\n", courseCode, currentDay->dayName, startTime, endTime);
            return 0;
        }
        
        if (prevExam->endTime > startTime || (currentExam != NULL && currentExam->startTime > endTime)) {
            if (currentExam == NULL && prevExam->endTime > MAX_START_TIME) {
                endTime = MIN_START_TIME + (endTime-startTime);
                startTime = MIN_START_TIME;

                currentDay = currentDay->nextDay;
                currentExam = currentDay->examList;
                continue;
            } else if (currentExam == NULL || prevExam->endTime + (endTime-startTime) <= currentExam->startTime) {
                endTime = prevExam->endTime + (endTime-startTime);
                startTime = prevExam->endTime;

                struct Exam* newExam = CreateExam(startTime, endTime, courseCode);

                newExam->next = currentExam;
                prevExam->next = newExam;

                printf("%s exam added to %s at time %d to %d due to conflict.\n", courseCode, currentDay->dayName, startTime, endTime);
                return 1;
            } else {
                int duration = endTime - startTime;

                startTime = currentExam->endTime;
                endTime = startTime + duration;

                prevExam = currentExam;
                currentExam = currentExam->next;
            }
        }
    } while (currentDay != startDay);

    printf("Schedule full. Exam cannot be added\n");
    return 2;
}
*/

// Remove an exam from a specific day in the schedule
int RemoveExamFromSchedule(struct Schedule* schedule, const char* day, int startTime) {
    struct Day* currentDay = schedule->head;

    while (strcmp(currentDay->dayName, day) != 0) currentDay = currentDay->nextDay;

    struct Exam* currentExam = currentDay->examList;
    struct Exam* prevExam = NULL;
    while (currentExam) {
        if (currentExam->startTime == startTime) {
            if (prevExam == NULL) {
                currentDay->examList = currentExam->next; 
            } else {
                prevExam->next = currentExam->next;
            }

            free(currentExam);

            printf("Exam removed successfully.\n");
            return 0;
        }
        prevExam = currentExam;
        currentExam = currentExam->next;
    }

    printf("Exam could not be found.\n");
    return 1;
}

// Update an exam in the schedule
int UpdateExam(struct Schedule* schedule, const char* oldDay, int oldStartTime, const char* newDay, int newStartTime, int newEndTime) {
    if (newEndTime - newStartTime > 3 || newStartTime < MIN_START_TIME || newEndTime > MAX_END_TIME) {
        printf("Invalid exam.\n");
        return 3;
    }

    struct Day* currentDay = schedule->head;
    while (strcmp(currentDay->dayName, oldDay) != 0) currentDay = currentDay->nextDay;
    struct Exam* currentExam = currentDay->examList;
    struct Exam* prevExam = NULL;

    
    while (currentExam != NULL) {
        if (currentExam->startTime == oldStartTime) {
            struct Day* targetDay = schedule->head;
            while (strcmp(targetDay->dayName, newDay) != 0) targetDay = targetDay->nextDay;

            struct Exam* targetExam = targetDay->examList;
            struct Exam* targetPrevExam = NULL;

            while (targetExam != NULL && targetExam->startTime < newStartTime) {
                targetPrevExam = targetExam;
                targetExam = targetExam->next;
            }

            if ((targetPrevExam == NULL || targetPrevExam->endTime <= newStartTime) &&
                (targetExam == NULL || targetExam->startTime >= newEndTime)) {

                struct Exam* newExam = CreateExam(newStartTime, newEndTime, currentExam->courseCode);
                
                if (prevExam == NULL) {
                    currentDay->examList = currentExam->next;
                } else {
                    prevExam->next = currentExam->next;
                }
                free(currentExam);

                if (targetPrevExam == NULL) {
                    newExam->next = targetExam;
                    targetDay->examList = newExam;
                } else {
                    newExam->next = targetPrevExam->next;
                    targetPrevExam->next = newExam;
                }

                printf("Update successful.\n");
                return 0;
                
            } else {
                printf("Update unsuccessful.\n");
                return 1;
            }
        }

        prevExam = currentExam;
        currentExam = currentExam->next;
    }
    
    printf("Exam could not be found.\n");
    return 2;
}


// Clear all exams from a specific day and relocate them to other days
int ClearDay(struct Schedule* schedule, const char* day) {
    struct Day* currentDay = schedule->head;
    while(strcmp(day, currentDay->dayName) != 0) currentDay = currentDay->nextDay;

    struct Exam* currentExam = currentDay->examList;

    if (currentExam == NULL) {
        printf("%s is already clear.\n", day);
        return 1;
    }

    while (currentExam) {
        struct Day* targetDay = currentDay->nextDay;
        struct Exam* targetCurrExam;
        struct Exam* targetPrevExam;

        int examDuration = (currentExam->endTime) - (currentExam->startTime);
        int isPlaced = 0;

        while (targetDay != currentDay && isPlaced == 0) {
            int targetStart = MIN_START_TIME;
            targetCurrExam = targetDay->examList;
            targetPrevExam = NULL;

            while (isPlaced == 0 && targetStart <= MAX_START_TIME) {
                if ((targetCurrExam == NULL || targetStart + examDuration <= targetCurrExam->startTime) && 
                    (targetPrevExam == NULL || targetPrevExam->endTime <= targetStart)) {
                    
                    struct Exam* newExam = CreateExam(targetStart, targetStart + examDuration, currentExam->courseCode); 

                    if (targetPrevExam == NULL) {
                        newExam->next = targetDay->examList;
                        targetDay->examList = newExam;
                    } else {
                        newExam->next = targetPrevExam->next;
                        targetPrevExam->next = newExam;
                    }
                    isPlaced = 1;
                } else {
                    targetStart = targetCurrExam->endTime;
                    targetPrevExam = targetCurrExam;
                    targetCurrExam = targetCurrExam->next;
                }
            }

            if (isPlaced == 0) {
                targetDay = targetDay->nextDay;
            }
        }

        if (isPlaced == 0) {
            printf("Schedule full. Exams from %s could not be relocated.\n", day);
            return 2;
        } 

        struct Exam* temp = currentExam;
        currentExam = currentExam->next;
        free(temp);

    }

    currentDay->examList = NULL; 
    printf("%s is cleared, exams relocated.\n", day);
    return 0;
}


// Clear all exams and days from the schedule and deallocate memory
void DeleteSchedule(struct Schedule* schedule) {
    if (schedule == NULL) {
        printf("Schedule is already NULL.\n");
        return;  
    }

    struct Day* currentDay = schedule->head;
    do {
        struct Exam* currentExam = currentDay->examList;

        while(currentExam) {
            struct Exam* temp = currentExam;
            currentExam = currentExam->next;
            free(temp);
        }

        struct Day* tempDay = currentDay;
        currentDay = currentDay->nextDay;

        if (tempDay == schedule->head) {
            schedule->head = currentDay; 
        }

        free(tempDay);
    } while (currentDay != NULL && currentDay != schedule->head); 

    free(schedule);
    printf("Schedule is cleared.\n");    
}


// Read schedule from file
int ReadScheduleFromFile(struct Schedule* schedule, const char* filename) {
    FILE* inputFile = fopen(filename, "r");

    if (inputFile == NULL) {
        printf("File could not be opened.\n");
        return 1;
    }

    char currentLine[100];

    while (fgets(currentLine, sizeof(currentLine), inputFile)) {
        int startTime, endTime;
        char courseCode[50];
        char dayName[50];

        currentLine[strcspn(currentLine, "\n")] = 0;

        if (sscanf(currentLine, "%d %d %s", &startTime, &endTime, courseCode) == 3) {
            AddExamToSchedule(schedule, dayName, startTime, endTime, courseCode);
        } else if (sscanf(currentLine, "%s", dayName) == 1) { 
            continue;
        } 
    }

    fclose(inputFile);
    return 0;
}


// Write schedule to file
int WriteScheduleToFile(struct Schedule* schedule, const char* filename) {
    FILE* outputFile = fopen(filename, "w");

    if (outputFile == NULL) {
        printf("File could not be opened.\n");
        return 1;
    }

    if (schedule == NULL || schedule->head == NULL) {
        printf("Schedule is empty or NULL.\n");
        return 1;
    }
    
    struct Day* currentDay = schedule->head;

    do {
        fprintf(outputFile, "%s\n", currentDay->dayName);

        struct Exam* currentExam = currentDay->examList;

        if (!currentExam) {
            fprintf(outputFile, "(No exams scheduled)\n");
        }
        
        while (currentExam != NULL) {
            fprintf(outputFile, "%d %d %s\n", currentExam->startTime, currentExam->endTime, currentExam->courseCode);
            currentExam = currentExam->next;
        }

        currentDay = currentDay->nextDay;
        fprintf(outputFile, "\n");
    } while (currentDay != schedule->head);

    fclose(outputFile);
    return 0;
}