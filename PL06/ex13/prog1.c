#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define STUDENTS 300

typedef struct
{
    int number;
    float sprint_grades[3];
    float exam;
    float final_grade;
    pthread_mutex_t mutex;
    pthread_cond_t ready;
    int data_ready; // Flag to indicate data is ready
} student_grade;

typedef struct 
{
    int positives;
    int negatives;
    pthread_mutex_t mutex;
} filtered_num;

typedef struct 
{
    filtered_num globalInfo;
    student_grade students[STUDENTS];
    pthread_mutex_t work_mutex;
    pthread_cond_t t4;
    pthread_cond_t t5;
    int pending_positive;
    int pending_negative;
    int students_processed;
} shared;

void *generate_values(void *arg)
{
    shared *data = (shared*)arg;

    for (int i = 0; i < STUDENTS; i++)
    {
        pthread_mutex_lock(&data->students[i].mutex);
        data->students[i].sprint_grades[0] = ((float)rand() / RAND_MAX) * 20.0f;
        data->students[i].sprint_grades[1] = ((float)rand() / RAND_MAX) * 20.0f;
        data->students[i].sprint_grades[2] = ((float)rand() / RAND_MAX) * 20.0f;
        data->students[i].exam = ((float)rand() / RAND_MAX) * 20.0f;
        data->students[i].data_ready = 1;

        // Signal T2 and T3 through cv
        pthread_cond_broadcast(&data->students[i].ready);
        pthread_mutex_unlock(&data->students[i].mutex);
    }

    printf("T1: Generation complete\n");
    pthread_exit(NULL);
}

void* compute_result(void *arg){
    shared *data = (shared *)arg;
    
    while (1) {
        // Find next unprocessed student
        int student_idx = -1;
        
        for (int i = 0; i < STUDENTS; i++) {
            pthread_mutex_lock(&data->students[i].mutex);
            if (data->students[i].data_ready && data->students[i].final_grade == 0.0f) {
                student_idx = i;
                pthread_mutex_unlock(&data->students[i].mutex);
                break;
            }
            pthread_mutex_unlock(&data->students[i].mutex);
        }
        
        if (student_idx == -1) {
            // Check if all students are processed
            pthread_mutex_lock(&data->work_mutex);
            if (data->students_processed >= STUDENTS) {
                pthread_mutex_unlock(&data->work_mutex);
                break;
            }
            pthread_mutex_unlock(&data->work_mutex);
            
            // Wait a bit and try again
            usleep(1000);
            continue;
        }
        
        // Process this student
        pthread_mutex_lock(&data->students[student_idx].mutex);
        
        // Wait for data to be ready
        while (!data->students[student_idx].data_ready) {
            pthread_cond_wait(&data->students[student_idx].ready, &data->students[student_idx].mutex);
        }
        
        // Check if already processed by another thread
        if (data->students[student_idx].final_grade != 0.0f) {
            pthread_mutex_unlock(&data->students[student_idx].mutex);
            continue;
        }

        // Compute final grade
        float total_grade = 0.4f * ((data->students[student_idx].sprint_grades[0] +
                                   data->students[student_idx].sprint_grades[1] +
                                   data->students[student_idx].sprint_grades[2]) / 3.0f)
                          + 0.6f * data->students[student_idx].exam;
        data->students[student_idx].final_grade = total_grade;
        pthread_mutex_unlock(&data->students[student_idx].mutex);

        // Signal appropriate thread based on grade
        pthread_mutex_lock(&data->work_mutex);
        if (total_grade >= 10.0f) {
            data->pending_positive++;
            pthread_cond_signal(&data->t4);
        } else {
            data->pending_negative++;
            pthread_cond_signal(&data->t5);
        }
        data->students_processed++;
        pthread_mutex_unlock(&data->work_mutex);
    }

    printf("Compute thread exiting\n");
    pthread_exit(NULL);
}

void* negative(void* arg){
    shared* data = (shared*) arg;
    int local_negatives = 0;

    while (1)
    {
        pthread_mutex_lock(&data->work_mutex);
        
        // Wait for signal or check if we're done
        while (data->pending_negative == 0 && data->students_processed < STUDENTS) {
            pthread_cond_wait(&data->t5, &data->work_mutex);
        }
        
        if (data->pending_negative > 0) {
            data->pending_negative--;
            local_negatives++;
            pthread_mutex_unlock(&data->work_mutex);
        } else if (data->students_processed >= STUDENTS) {
            pthread_mutex_unlock(&data->work_mutex);
            break;
        } else {
            pthread_mutex_unlock(&data->work_mutex);
        }
    }
    
    // Update global counter
    pthread_mutex_lock(&data->globalInfo.mutex);
    data->globalInfo.negatives += local_negatives;
    pthread_mutex_unlock(&data->globalInfo.mutex);
    
    printf("T5: Processed %d negative grades\n", local_negatives);
    pthread_exit(NULL); 
}

void* positive(void* arg){
    shared* data = (shared*) arg;
    int local_positives = 0;

    while (1)
    {
        pthread_mutex_lock(&data->work_mutex);
        
        // Wait for signal or check if we're done
        while (data->pending_positive == 0 && data->students_processed < STUDENTS) {
            pthread_cond_wait(&data->t4, &data->work_mutex);
        }
        
        if (data->pending_positive > 0) {
            data->pending_positive--;
            local_positives++;
            pthread_mutex_unlock(&data->work_mutex);
        } else if (data->students_processed >= STUDENTS) {
            pthread_mutex_unlock(&data->work_mutex);
            break;
        } else {
            pthread_mutex_unlock(&data->work_mutex);
        }
    }
    
    // Update global counter
    pthread_mutex_lock(&data->globalInfo.mutex);
    data->globalInfo.positives += local_positives;
    pthread_mutex_unlock(&data->globalInfo.mutex);

    printf("T4: Processed %d positive grades\n", local_positives);
    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));
    pthread_t threads[5];

    shared *global = malloc(sizeof(shared));

    global->globalInfo.negatives = 0;
    global->globalInfo.positives = 0;
    pthread_mutex_init(&global->globalInfo.mutex, NULL);
    pthread_mutex_init(&global->work_mutex, NULL);
    pthread_cond_init(&global->t4, NULL);
    pthread_cond_init(&global->t5, NULL);
    global->pending_positive = 0;
    global->pending_negative = 0;
    global->students_processed = 0;

    for (int i = 0; i < STUDENTS; i++)
    {
        global->students[i].number = i;
        global->students[i].data_ready = 0;
        global->students[i].final_grade = 0.0f; // Important: initialize to 0
        pthread_mutex_init(&global->students[i].mutex, NULL);
        pthread_cond_init(&global->students[i].ready, NULL);
    }

    pthread_create(&threads[0], NULL, generate_values, (void *)global);
    pthread_create(&threads[1], NULL, compute_result, (void *)global);
    pthread_create(&threads[2], NULL, compute_result, (void *)global);
    pthread_create(&threads[3], NULL, positive, (void *)global);
    pthread_create(&threads[4], NULL, negative, (void *)global);
       
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d completed\n", i);
    }

    printf("Total of positive grades: %d\n", global->globalInfo.positives);
    printf("Total of negative grades: %d\n", global->globalInfo.negatives);
    printf("Total processed: %d\n", global->globalInfo.positives + global->globalInfo.negatives);

    for (int i = 0; i < STUDENTS; i++) {
        pthread_mutex_destroy(&global->students[i].mutex);
        pthread_cond_destroy(&global->students[i].ready);
    }
    pthread_mutex_destroy(&global->globalInfo.mutex);
    pthread_mutex_destroy(&global->work_mutex);
    pthread_cond_destroy(&global->t4);
    pthread_cond_destroy(&global->t5);

    free(global);
    return 0;
}