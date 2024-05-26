#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include <string.h>

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

//---------------------------------------------------------------------------------------------

int esValido(int x, int y) {
    return (x >= 0 && x < 3 && y >= 0 && y < 3); 
}

//---------------------------------------------------------------------------------------------

void imprimirEstado(const State *estado) 
{
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (estado->square[i][j] == 0)
                printf("x ");
            else
                printf("%d ", estado->square[i][j]);
        }
        printf("\n");
    }
}

//---------------------------------------------------------------------------------------------

int esFinal(const State* estado) 
{
    State finalState = 
    {
            {{0, 1, 2},
             {3, 4, 5},
            {6, 7, 8}},
    
            0,0 //Posición del 0
    };

    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (estado->square[i][j] != finalState.square[i][j])
                    return 0;
        }
    }
    return 1;
}

//---------------------------------------------------------------------------------------------

void copiarState(State original, State* new) 
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            new->square[i][j] = original.square[i][j];

    new->x = original.x;
    new->y = original.y;
    new->nActions = original.nActions;
}

//---------------------------------------------------------------------------------------------

void mover(State* current, int choice) 
{
    int x = current->x;
    int y = current->y;
    int temp;

    switch (choice) {
        case 1: // Mover espacio vacío hacia arriba

            if (esValido(x , y - 1)) 
            {
                temp = current->square[x][y - 1];
                current->square[x][y - 1] = current->square[x][y];
                current->square[x][y] = temp;
                current->y = y - 1;

            }
            break;

        case 2: // Mover espacio vacío hacia abajo

            if (esValido(x , y + 1)) 
            {
                temp = current->square[x][y + 1];
                current->square[x][y + 1] = current->square[x][y];
                current->square[x][y] = temp;
                current->y = y + 1;
            }

            break;
        
        case 3: // Mover espacio vacío hacia la izquierda

            if (esValido(x - 1, y)) 
            {
                temp = current->square[x - 1][y];
                current->square[x - 1][y] = current->square[x][y];
                current->square[x][y] = temp;
                current->x = x - 1;
            }

            break;
        
        case 4: // Mover espacio vacío hacia la derecha
            if (esValido(x + 1, y)) 
            {
                temp = current->square[x + 1][y];
                current->square[x + 1][y] = current->square[x][y];
                current->square[x][y] = temp;
                current->x = x + 1;
                }
                break;
    }
}

//---------------------------------------------------------------------------------------------

State* transicion(State* estado, int accion) {
    State *nuevo = (State*) malloc(sizeof(State));
    copiarState(*estado, nuevo);
    
    mover(nuevo, accion);
    nuevo->nActions++;

    return nuevo;
}

//---------------------------------------------------------------------------------------------

List *nodosAdj(Node* parentNode) {

        List *adjList = list_create();

        int x = parentNode->state.x;
        int y = parentNode->state.y;

        for (int i = 1; i <= 4; i++) {

                State *newState = transicion(&parentNode->state, i);

                if (newState->x != x || newState->y != y) {

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

//---------------------------------------------------------------------------------------------

void busqP(State initialState, int count) {
    Stack* stack = stack_create(stack);

    printf("\nEstado Original:\n");
    imprimirEstado(&initialState);
    printf("\n");

    Node *raiz = (Node *) malloc(sizeof(Node ));

    raiz->parent = NULL;
    raiz->state = initialState;
    stack_push(stack, raiz);

    while (list_size(stack) > 0) 
    {
        Node *current = (Node *) stack_top(stack);
        stack_pop(stack);

        if (current->state.nActions >= 15)
                continue;

        if (esFinal(&current->state)) 
        {
            printf("----------- Resuelto con éxito! -----------\n");
            printf("Iteraciones: %d\n\n", count);

            Node *node = current;
            int pasos = current->state.nActions;

            while (node != NULL) 
            {
                if (pasos == 0) 
                        printf("Estado Inicial:\n");  

                else
                        printf("Paso %d:\n", pasos);                  

                imprimirEstado(&node->state);
                printf("\n");

                node = node->parent;
                pasos--;
            }
            stack_clean(stack);
            free(node);
            return;
        }

        List *adjNodes = nodosAdj(current);
        void *aux = list_first(adjNodes);

        while (aux != NULL) {

                stack_push(stack, aux);
                aux = list_next(adjNodes);

        }

        list_clean(adjNodes);
        count++;

    }

    printf("No se pudo resolver el puzzle.\n");
    stack_clean(stack);
}

//---------------------------------------------------------------------------------------------

void busqA(State initialState, int count) {
    Queue* cola = queue_create(cola);

    printf("Estado Inicial:\n\n");
    imprimirEstado(&initialState);
    printf("\n");

    Node *raiz = (Node *) malloc(sizeof(Node ));

    raiz->parent = NULL;
    raiz->state = initialState;
    queue_insert(cola, raiz);

    while (list_size(cola) > 0) 
    {
        Node *current = (Node *) queue_front(cola);
        queue_remove(cola);
        if (count >= 15000000) 
        {
            printf("Proceso terminado, se alcanzó el límite de iteraciones (15.000.000)\n");
            return;
        }

        if (esFinal(&current->state)) 
        {
            printf("----------- Resuelto con éxito! -----------\n");
            printf("Iteraciones: %d\n\n", count);

            Node *node = current;
            int pasos = current->state.nActions;

            while (node != NULL) 
            {
                if (pasos == 0) 
                        printf("Estado inicial:\n");  

                else
                        printf("Paso %d:\n", pasos);                  

                imprimirEstado(&node->state);
                printf("\n");

                node = node->parent;
                pasos--;
            }
                queue_clean(cola);
                free(node);

                return;
        }
        List *adjNodes = nodosAdj(current);
        void *aux = list_first(adjNodes);

        while (aux != NULL) {

                queue_insert(cola, aux);
                aux = list_next(adjNodes);

        }

        list_clean(adjNodes);
        count++;

    }

    printf("No se pudo resolver el puzzle.\n");
    queue_clean(cola);

}
//---------------------------------------------------------------------------------------------
int calcularHeuristica(const State* estado) {
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
//---------------------------------------------------------------------------------------------
void best_first(State initialState) 
{
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

        if (esFinal(&currentNode->state)) {
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

        List *adjNodes = nodosAdj(currentNode);
        void *aux = list_first(adjNodes);

        while (aux != NULL) {
            Node *adjNode = (Node *) aux;
            int prioridad = calcularHeuristica(&adjNode->state) + adjNode->state.nActions;
            heap_push(heap, adjNode, prioridad);
            heap_size++;
            aux = list_next(adjNodes);
        }

        free(adjNodes);
    }

    printf("La solución no fue encontrada.\n");
    free(heap);
}

//---------------------------------------------------------------------------------------------

int main() {
    int opcion;
    int cont = 0;
    
    State estado_inicial = 
    {
        {{0, 2, 8}, 
         {1, 3, 4}, 
         {6, 5, 7}, 
         },  
        0, 0   // Posición del 0 
    };
    estado_inicial.nActions = 0;

    do {
        mostrarMenuPrincipal();
        printf("Ingrese su opción: ");
        scanf(" %d", &opcion);

        switch (opcion) {
        case 1:
            busqP(estado_inicial, cont);
            break;
            
        case 2:
            busqA(estado_inicial, cont);
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




