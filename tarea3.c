#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/stack.h"
#include "tdas/queue.h"

typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    int nActions;
} State;

typedef struct Node {
    State state;
    struct Node* parent;
} Node;

void limpiarPantalla() {
    // Función de limpieza de pantalla, definida en tdas/extra.h
}

void presioneTeclaParaContinuar() {
    // Función para pausar la ejecución hasta que se presione una tecla, definida en tdas/extra.h
}

void mostrarMenuPrincipal() {
    limpiarPantalla();
    puts("========================================\n");
    puts("     Escoge método de búsqueda\n");
    puts("========================================\n");
    puts("1) Búsqueda en Profundidad");
    puts("2) Buscar en Anchura");
    puts("3) Buscar Mejor Primero");
    puts("4) Salir\n");
}

int isValidMove(int x, int y) {
    return (x >= 0 && x < 3 && y >= 0 && y < 3); 
}

void imprimirEstado(const State *estado) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado->square[i][j] == 0)
                printf("x ");
            else
                printf("%d ", estado->square[i][j]);
        }
        printf("\n");
    }
}

int isFinal(const State* estado) {
    State finalState = {
        {{1, 2, 3},
         {4, 5, 6},
         {7, 8, 0}},
        2, 2
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado->square[i][j] != finalState.square[i][j])
                return 0;
        }
    }
    return 1;
}

void copyState(const State* original, State* newState) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            newState->square[i][j] = original->square[i][j];
    newState->x = original->x;
    newState->y = original->y;
    newState->nActions = original->nActions;
}

void move(State* current, int choice) {
    int x = current->x;
    int y = current->y;
    int temp;

    switch (choice) {
    case 1: // Mover espacio vacío hacia arriba
        if (isValidMove(x - 1, y)) {
            temp = current->square[x - 1][y];
            current->square[x - 1][y] = current->square[x][y];
            current->square[x][y] = temp;
            current->x = x - 1;
        }
        break;
    case 2: // Mover espacio vacío hacia abajo
        if (isValidMove(x + 1, y)) {
            temp = current->square[x + 1][y];
            current->square[x + 1][y] = current->square[x][y];
            current->square[x][y] = temp;
            current->x = x + 1;
        }
        break;
    case 3: // Mover espacio vacío hacia la izquierda
        if (isValidMove(x, y - 1)) {
            temp = current->square[x][y - 1];
            current->square[x][y - 1] = current->square[x][y];
            current->square[x][y] = temp;
            current->y = y - 1;
        }
        break;
    case 4: // Mover espacio vacío hacia la derecha
        if (isValidMove(x, y + 1)) {
            temp = current->square[x][y + 1];
            current->square[x][y + 1] = current->square[x][y];
            current->square[x][y] = temp;
            current->y = y + 1;
        }
        break;
    }
}

State* transition(State* oldState, int action) {
    State *newState = (State*) malloc(sizeof(State));
    copyState(oldState, newState);
    move(newState, action);
    newState->nActions++;
    return newState;
}

List *getAdjNodes(Node* parentNode) {
    List *adjList = list_create();
    for (int i = 1; i <= 4; i++) {
        State *newState = transition(&parentNode->state, i);
        if (newState->x != parentNode->state.x || newState->y != parentNode->state.y) {
            Node *newNode = (Node*) malloc(sizeof(Node));
            newNode->state = *newState;
            newNode->parent = parentNode;
            list_pushBack(adjList, newNode);
        } else {
            free(newState);
        }
    }
    return adjList;
}

void dfs(State initialState, int count) {
    Stack* stack = stack_create();
    int stack_size = 0;

    printf("\nOriginal State:\n\n");
    imprimirEstado(&initialState);
    printf("\n");

    Node *root = (Node *) malloc(sizeof(Node));
    root->parent = NULL;
    root->state = initialState;

    stack_push(stack, root);
    stack_size++;

    while (stack_size > 0) {
        Node *currentNode = (Node *) stack_top(stack);
        stack_pop(stack);
        stack_size--;

        if (currentNode->state.nActions >= 15)
            continue;

        if (isFinal(&currentNode->state)) {
            printf("Puzzle solved!\n");
            printf("Iterations: %d\n\n", count);

            Node *node = currentNode;
            int steps = currentNode->state.nActions;

            while (node != NULL) {
                if (steps == 0) 
                    printf("Initial State:\n\n");  
                else
                    printf("Step %d:\n\n", steps);                  
                imprimirEstado(&node->state);
                printf("\n");
                node = node->parent;
                steps--;
            }

            stack_clean(stack);
            free(node);
            return;
        }

        List *adjNodes = getAdjNodes(currentNode);
        void *aux = list_first(adjNodes);

        while (aux != NULL) {
            stack_push(stack, aux);
            stack_size++;
            aux = list_next(adjNodes);
        }

        list_clean(adjNodes);
        count++;
    }

    printf("The solution was not found within the limited range.\n");
    stack_clean(stack);
}

void bfs(State initialState, int count) {
    Queue* queue = queue_create();
    int queue_size = 0;

    printf("Initial State:\n\n");
    imprimirEstado(&initialState);
    printf("\n");

    Node *root = (Node *) malloc(sizeof(Node));
    root->parent = NULL;
    root->state = initialState;
    queue_insert(queue, root);
    queue_size++;

    while (queue_size > 0) {
        Node *currentNode = (Node *) queue_front(queue);
        queue_remove(queue);
        queue_size--;

        if (count >= 15000000) {
            printf("Process terminated: Iteration limit reached (15.000.000)\n");
            return;
        }

        if (isFinal(&currentNode->state)) {
            printf("Puzzle solved!\n");
            printf("Iterations: %d\n\n", count);

            Node *node = currentNode;
            int steps = currentNode->state.nActions;

            while (node != NULL) {
                if (steps == 0) 
                    printf("Initial State:\n\n");  
                else
                    printf("Step %d:\n\n", steps);                  
                imprimirEstado(&node->state);
                printf("\n");
                node = node->parent;
                steps--;
            }

            queue_clean(queue);
            free(node);
            return;
        }

        List *adjNodes = getAdjNodes(currentNode);
        void *aux = list_first(adjNodes);

        while (aux != NULL) {
            queue_insert(queue, aux);
            queue_size++;
            aux = list_next(adjNodes);
        }

        list_clean(adjNodes);
        count++;
    }

    printf("La solucion no fue encontrada.\n");
    queue_clean(queue);
}

int calcularHeuristica(const State *estado) {
    int distancia = 0;
    int goal[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int valor = estado->square[i][j];
            if (valor != 0) {
                for (int k = 0; k < 3; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (goal[k][l] == valor) {
                            distancia += abs(i - k) + abs(j - l);
                        }
                    }
                }
            }
        }
    }
    return distancia;
}

void best_first(State initialState) {
    Heap* heap = heap_create();
    int heap_size = 0;

    printf("Initial State:\n\n");
    imprimirEstado(&initialState);
    printf("\n");

    Node *root = (Node *) malloc(sizeof(Node));
    root->parent = NULL;
    root->state = initialState;

    heap_push(heap, root, calcularHeuristica(&initialState) + root->state.nActions);
    heap_size++;

    while (heap_size > 0) {
        Node *currentNode = (Node *) heap_top(heap);
        heap_pop(heap);
        heap_size--;

        if (isFinal(&currentNode->state)) {
            printf("Puzzle solved!\n");

            Node *node = currentNode;
            int steps = currentNode->state.nActions;

            while (node != NULL) {
                if (steps == 0)
                    printf("Initial State:\n\n");
                else
                    printf("Step %d:\n\n", steps);
                imprimirEstado(&node->state);
                printf("\n");
                node = node->parent;
                steps--;
            }

            free(heap);
            free(node);
            return;
        }

        List *adjNodes = getAdjNodes(currentNode);
        void *aux = list_first(adjNodes);

        while (aux != NULL) {
            Node *adjNode = (Node *) aux;
            int prioridad = calcularHeuristica(&adjNode->state) + adjNode->state.nActions;
            heap_push(heap, adjNode, prioridad);
            heap_size++;
            aux = list_next(adjNodes);
        }

        list_clean(adjNodes);
    }

    printf("La solución no fue encontrada.\n");
    free(heap);
}

int main() {
    State estado_inicial = {
        {{2, 8, 3},
         {1, 6, 4},
         {7, 0, 5}},
        2, 1 // Posición del espacio vacío
    };
    estado_inicial.nActions = 0;

    int opcion;
    int cont = 0;

    do {
        mostrarMenuPrincipal();
        printf("Ingrese su opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
        case 1:
            dfs(estado_inicial, cont);
            break;
        case 2:
            bfs(estado_inicial, cont);
            break;
        case 3:
            best_first(estado_inicial);
            break;
        case 4:
            printf("Saliendo del programa...\n");
            break;
        }
        presioneTeclaParaContinuar();
        limpiarPantalla();
    } while (opcion != 4);

    return 0;
}
