#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "students.dat"

// 1. 학생 정보 구조체 정의
typedef struct {
    char name[30];
    int kor;
    int eng;
    int math;
    int total;
    float average;
    int rank;
} Student;

// 연결 리스트의 노드 구조체 정의
typedef struct Node {
    Student data;       // 학생 정보 데이터
    struct Node* p_next; // 다음 노드를 가리킬 포인터
} Node;

// --- 함수 프로토타입 선언 ---
void print_menu();
void AddStudent(Node** pp_head, Node** pp_tail, Student new_student);
void read_data_from_file(Node** pp_head, Node** pp_tail);
void input_new_student(Node** pp_head, Node** pp_tail);
void save_data_to_file(Node* p_head);
void calculate_and_display_grades(Node* p_head);
void free_list(Node** pp_head, Node** pp_tail);
void calculate_total_average_and_rank(Node* p_head, int count);

// --- 메인 함수 ---
int main(void) {
    // 노드의 시작과 끝을 기억할 포인터
    Node* p_head = NULL, * p_tail = NULL;
    int choice;

    // 프로그램 시작 시 파일에서 데이터 읽기 시도
    read_data_from_file(&p_head, &p_tail);

    while (1) {
        print_menu();
        if (scanf_s("%d", &choice) != 1) {
            printf("경고: 유효하지 않은 입력입니다. 다시 시도하세요.\n");
            while (getchar() != '\n'); // 입력 버퍼 비우기
            continue;
        }

        switch (choice) {
        case 1: // .dat 파일에서 데이터 읽기
            free_list(&p_head, &p_tail); // 기존 데이터 삭제 후 로드
            read_data_from_file(&p_head, &p_tail);
            break;
        case 2: // 추가 학생 정보 입력
            input_new_student(&p_head, &p_tail);
            break;
        case 3: // .dat 파일 저장
            save_data_to_file(p_head);
            break;
        case 4: // 성적 확인 (평균 계산 등)
            calculate_and_display_grades(p_head);
            break;
        case 5: // 종료
            printf("프로그램을 종료합니다. 메모리를 해제합니다.\n");
            free_list(&p_head, &p_tail); // 종료 전 메모리 정리
            return 0;
        default:
            printf("경고: 1부터 5까지의 메뉴를 선택하세요.\n");
            break;
        }
    }
}

// --- 함수 구현 ---

void print_menu() {
    printf("\n[Menu]\n");
    printf("1. .dat 파일에서 데이터 읽기\n");
    printf("2. 추가 학생 정보 입력\n");
    printf("3. .dat 파일 저장\n");
    printf("4. 성적 확인 (평균 계산 등)\n");
    printf("5. 종료\n");
    printf("-------------------\n");
    printf("선택(1~5): ");
}

/**
 * 새로운 학생 정보를 연결 리스트에 추가합니다. (기존 AddNumber 함수 구조 활용)
 * @param pp_head 헤드 포인터의 주소
 * @param pp_tail 테일 포인터의 주소
 * @param new_student 추가할 학생 데이터
 */
void AddStudent(Node** pp_head, Node** pp_tail, Student new_student) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("오류: 메모리 할당에 실패했습니다.\n");
        return;
    }

    new_node->data = new_student;
    new_node->p_next = NULL;

    if (*pp_head != NULL) {
        // 리스트가 비어있지 않은 경우: tail 다음 노드로 연결
        (*pp_tail)->p_next = new_node;
        *pp_tail = new_node; // tail 포인터를 새 노드로 이동
    }
    else {
        // 리스트가 비어있는 경우: head와 tail 모두 새 노드를 가리킴
        *pp_head = new_node;
        *pp_tail = new_node;
    }
}

/**
 * 1. 이진 파일에서 학생 데이터를 읽어 연결 리스트에 저장합니다.
 */
void read_data_from_file(Node** pp_head, Node** pp_tail) {
    FILE* fp = NULL;
    Student s;

    // 기존 리스트가 있으면 메모리 누수 방지를 위해 먼저 해제
    free_list(pp_head, pp_tail);

    if (fopen_s(&fp, FILENAME, "rb") != 0 || fp == NULL) {
        printf("정보: '%s' 파일이 없거나 열 수 없습니다. 새 데이터를 시작합니다.\n", FILENAME);
        return;
    }

    int count = 0;
    // 파일에서 Student 구조체 단위로 읽어 연결 리스트에 추가
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        AddStudent(pp_head, pp_tail, s);
        count++;
    }

    printf("성공: '%s' 파일에서 학생 정보 %d개를 읽었습니다.\n", FILENAME, count);

    fclose(fp);
}

/**
 * 2. 사용자로부터 추가 학생 정보를 입력받아 연결 리스트에 저장합니다.
 */
void input_new_student(Node** pp_head, Node** pp_tail) {
    Student s;
    char buffer[100]; // 입력 버퍼

    printf("\n--- 새 학생 정보 입력 ---\n");

    // 버퍼 비우기
    while (getchar() != '\n');

    printf("이름: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = 0; // 개행 문자 제거

    printf("국어 점수: ");
    scanf_s("%d", &s.kor);
    printf("영어 점수: ");
    scanf_s("%d", &s.eng);
    printf("수학 점수: ");
    scanf_s("%d", &s.math);

    // 총점, 평균, 등수 초기화 (성적 확인 시 계산됨)
    s.total = 0;
    s.average = 0.0f;
    s.rank = 0;

    AddStudent(pp_head, pp_tail, s);
    printf("정보: %s 학생 정보가 추가되었습니다.\n", s.name);
}

/**
 * 3. 현재 연결 리스트의 데이터를 이진 파일에 저장합니다.
 */
void save_data_to_file(Node* p_head) {
    FILE* fp = NULL;
    Node* p = p_head;
    int count = 0;

    if (p_head == NULL) {
        printf("정보: 저장할 학생 데이터가 없습니다.\n");
        return;
    }

    if (fopen_s(&fp, FILENAME, "wb") != 0 || fp == NULL) {
        printf("오류: '%s' 파일을 쓸 수 없습니다.\n", FILENAME);
        return;
    }

    // 리스트를 순회하며 파일에 쓰기
    while (p != NULL) {
        fwrite(&(p->data), sizeof(Student), 1, fp);
        p = p->p_next;
        count++;
    }

    printf("성공: 학생 정보 %d개를 '%s' 파일에 저장했습니다.\n", count, FILENAME);

    fclose(fp);
}

/**
 * 리스트의 총 학생 수 계산 및 총점, 평균, 등수 계산 로직
 */
void calculate_total_average_and_rank(Node* p_head, int count) {
    if (count == 0) return;

    // 1. 총점 및 평균 계산
    Node* p = p_head;
    while (p != NULL) {
        p->data.total = p->data.kor + p->data.eng + p->data.math;
        p->data.average = (float)p->data.total / 3.0f;
        p->data.rank = 1; // 초기 등수 1로 설정
        p = p->p_next;
    }

    // 2. 등수 계산 (총점을 기준으로 비교)
    Node* current = p_head;
    while (current != NULL) {
        Node* compare = p_head;
        while (compare != NULL) {
            // 자신보다 총점이 높은 학생이 있으면 등수 증가
            if (current->data.total < compare->data.total) {
                current->data.rank++;
            }
            compare = compare->p_next;
        }
        current = current->p_next;
    }
}

/**
 * 4. 성적을 계산하고 총점, 평균, 등수를 출력합니다.
 */
void calculate_and_display_grades(Node* p_head) {
    if (p_head == NULL) {
        printf("정보: 출력할 학생 데이터가 없습니다. 먼저 입력하거나 파일을 읽으세요.\n");
        return;
    }

    // 학생 수 계산
    int count = 0;
    Node* p = p_head;
    while (p != NULL) {
        count++;
        p = p->p_next;
    }

    // 총점, 평균, 등수 계산
    calculate_total_average_and_rank(p_head, count);

    printf("\n=================================================================\n");
    printf("성적 확인 (등수, 총점, 평균 기준)\n");
    printf("=================================================================\n");
    printf("등수 | 이름 | 국어 | 영어 | 수학 | 총점 | 평균\n");
    printf("-----------------------------------------------------------------\n");

    // 등수 순서대로 출력하기 위해 임시로 배열에 복사하여 정렬할 수도 있지만,
    // 여기서는 간단히 리스트를 순회하며 출력합니다.

    // 리스트를 순회하며 출력
    p = p_head;
    while (p != NULL) {
        Student s = p->data;
        printf("%4d | %s | %4d | %4d | %4d | %4d | %.2f\n",
            s.rank, s.name, s.kor, s.eng, s.math, s.total, s.average);
        p = p->p_next;
    }
    printf("=================================================================\n");
}

/**
 * 사용한 모든 노드를 삭제하여 메모리를 해제합니다. (기존 main 함수 구조 활용)
 */
void free_list(Node** pp_head, Node** pp_tail) {
    Node* p_head = *pp_head;
    Node* p;
    int count = 0;

    // 리스트 탐색 및 메모리 해제
    while (p_head != NULL) {
        p = p_head; // 현재 노드를 삭제하기 위해 p 변수에 노드 주소 값을 저장
        p_head = p_head->p_next; // 시작 위치를 다음 노드로 이동
        free(p); // 기억했던 주소를 사용하여 노드를 삭제함
        count++;
    }

    // head와 tail 포인터 정리
    *pp_head = NULL;
    *pp_tail = NULL;

    if (count > 0) {
        printf("정보: 연결 리스트의 노드 %d개를 메모리에서 해제했습니다.\n", count);
    }
}