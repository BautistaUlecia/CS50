#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
int is_cycle(int a, int b);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    //en ranks se guarda en forma de array los candidatos
    //en orden, segun subindice de candidates
    //osea guardas 2-0-1 si elegis candidato 2 primero, 0 segundo 1 tercero
    //rank es el rango donde puso el string name
    //busco en el array de candidates un match de nombre, si lo encuentro
    //actualizo ranks[] y retorno true

    for (int i = 0 ; i < candidate_count; i++)
    {
        if (!strcmp(candidates[i], name))
        {
            ranks[rank]=i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for(int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }

    /*print matrix preferences para probar. Funciona bien la matriz no jodas
    printf("Matriz preferences\n");
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%d  ", preferences[i][j]);
        }
        printf("\n");
    }*/


    //Ranks[n] es preferido sobre ranks[n+1]...[n+2]... hasta el final del array
    //Preferences es un Arrray 2D. preferences[i][j] es la cantidad que prefiere
    //Al candidato i sobre el candidato j.
    //Itero sobre ranks dos veces, para i y para j, recorriendo desde el subindice
    //Todos los demas subindices hasta el final del array.
    //Lo que haya en ranks[i] lo prefiere sobre todos los ranks[j]
    //Entonces hago preferences[i][j]++ para VALORES no SUBINDICES de ranks
    return;
}

// Record pairs of candidates where one is preferred over the other
//Me agrega un par duplicado y no toma el par de que alice le gana a bob (es el primero que deberia evaluar, 0 le gana a 1)

//El error es que estoy modificando pairs[i] dos veces (ya que la asignacion ocurre en el loop para j) entonces
//Se sobreescribe el primer pair (0 le gana a 1)
//antes de escribir un pair chequear el array para que no exista y si no existe agregarlo
void add_pairs(void)
{
    int a, k, flag;
    for(int i = 0; i < candidate_count; i++)
    {
        for(int j = 0; j < candidate_count; j++)
        {
            a=preferences[i][j]-preferences[j][i];
            if (a > 0)
            {
                {
                pairs[pair_count].winner=i;
                pairs[pair_count].loser=j;
                pair_count++;
                }
            }
        }
    }
    /*Print de pairs para debugear
    for (int i = 0; i < pair_count; i++)
    {
        printf("Pair number %d\n", i);
        printf("Winner = %d, loser = %d\n", pairs[i].winner, pairs[i].loser);
    }*/


    //Los subindices de preferences para parejas son los valores con los valores intercambiados
    //Es decir, [i][j] y luego [j][i] son los dos valores (prefiere i sobre j, prefiere j sobre i).
    //Recorro el array preferences con un doble loop, evaluando [i][j]-[j][i] y luego [i+1][j+1]-[j+1][i+1]...etc
    //Si el valor es distinto a cero, creo un pairs con winner como el subindice i si el numero es positivo, j si es negativo.
    //Ya que si mas gente prefiere a i sobre j de la que prefiere a j sobre i, la resta [i][j]-[j][i] dara positiva.
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    /*print pairs unsorted para debug
    printf("\nUnsorted Pairs\n");
    for (int i = 0; i < pair_count; i++)
    {
        printf("Pair number %d\n", i);
        printf("Winner score = %d\n Loser score = %d\n", pairs[i].winner, pairs[i].loser);
    }*/

    int i, j, strenght1, strenght2;
    pair aux;
    for (i = 0; i  < pair_count ; i++){
        for (j = 0; j < pair_count - i - 1; j++)
        {
            strenght1=preferences[pairs[j].winner][pairs[j].loser]-preferences[pairs[j].loser][pairs[j].winner];
            strenght2=preferences[pairs[j+1].winner][pairs[j+1].loser]-preferences[pairs[j+1].loser][pairs[j+1].winner];
            if (strenght1<strenght2)
            {
                aux = pairs[j];
                pairs[j] = pairs[j+1];
                pairs[j+1] = aux;
            }
        }
    }
    /*print pairs sorted para debug
    printf("\nSorted Pairs\n");
    for (i = 0; i < pair_count; i++)
    {
        printf("Pair number %d\n", i);
        printf("Winner score = %d\n Loser score = %d\n", pairs[i].winner, pairs[i].loser);
    }*/

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles

//antes de agregar, chequeo que no cree un ciclo.
//un ciclo seria piedra papel tijera, donde
//0 le gana a 1, 1 a 2 y 2 a 0, o similar.
//locked puede saber si esta por crear un ciclo, seria
//un caso donde siguiendo las flechas yo vuelvo al candidato
//donde empece.

void lock_pairs(void)
{
    int i;
    for (i = 0; i < pair_count ; i++)
    {
        if(!is_cycle(pairs[i].winner, pairs[i].loser))
            {
                locked[pairs[i].winner][pairs[i].loser]=true;
            }
    }

    /*print matrix para probar
    printf("\nLocked Matrix\n");
    for (i = 0; i < candidate_count; i++)
    {
        for (j = 0; j < candidate_count; j++)
        {
            printf("%d  ", locked[i][j]);
        }
        printf("\n");
    }*/


    //En pairs tengo guardados cada par de ganador sobre perdedor
    //Ordenados ahora de mayor a menor
    //Por cada ganador a perdedor tengo que actualizar el boolean para esos subindices
    //Con un True
    //Es decir, Winner vale algo y Loser vale algo, esos dos valores son el subindice de
    //Locked que tengo que actualizar
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int i, j, count;
    for (j = 0; j < candidate_count; j++)
    {
    count = 0;
        for (i = 0; i < candidate_count; i++)
        {
            if (locked[i][j]==false)
            count++;
        }
        if (count==candidate_count)
            printf("%s\n", candidates[j]);
    }


    //El ganador es el que no tenga ninguna flecha apuntandole
    //A el, es decir la fuente del grafico. En la matriz,
    //El ganador es quien tenga en su columna, todos falses (creo)
    //O si no, quien tenga en su fila todos true excepto para el mismo. (esto creo que no)
    return;
}

int is_cycle(int a, int b)
{
    //recorrer la fila j hasta encontrar un true. si el true esta en columna valor centinela (a)
    //devuelvo false.
    //si no, recorro la fila j hasta encontrar un true.
    //si recorro la matrix candidate_count cantidad de veces
    //y no devolvi false, devolver true (no hay ciclo)
    int j = b;
    for (int n = candidate_count; n >= 0; n--){
        for (int k = 0; k < candidate_count; k++){
            if (locked[j][k] == true)
            {
                if (k == a)
                {
                    return 1;
                }
                j = k;
            }
        }
    }
    return 0;
}