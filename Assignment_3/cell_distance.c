#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>

//data structure: We need a data structure to store the distances and the number of their occurences
//I would suggest a binary tree for that.
typedef struct node {
  int distance, counter;
  node_t *left, *right;
} node_t;
typedef struct tree {
  node_t *root;
  int nnodes;
} tree_t;
int insert_subtree(node_t *root, int distance) {
  if (root->distance == distance) {
    root->counter++;
    return 0;
  }
  if (root->distance < distance) {
    if (NULL == root->right) {
      root->right = calloc(sizeof(node_t));
      (root->right)->distance = distance;
      (root->right)->counter = 1;
      return 1;
    }
    return insert(root->right, distance);
  }
  if (NULL == root->left) {
    root->left = calloc(sizeof(node_t));
    (root->left)->distance = distance;
    (root->left)->counter = 1;
    return 1;
  }
  return insert(root->left, distance);
}

void tree_free(tree_t *tree)
{
  node_free(tree->root);
  free(tree);
  return;
}

void node_free(node_t *node)
{
  if (node == NULL)
    return;
  node_free(node->left);
  node_free(node->right);
  free(node);
  return;
}

void insert_start(tree_t *tree, int distance)
{
  tree->nnodes += insert_subtree(tree->root, distance);
}
int **distance_array(tree_t *tree)
{
  //TODO this function should return a 2xN array with the distances and their number of occurences.
}

typedef struct coordinates {
  int x, y, z; //these are the coordinates * 1000
} coordinates_t;

coordinates_t *input;

int distance_squared(coordinates_t *a, coordinates_t *b)
{
  int dx, dy, dz;
  dx = a->x-b->x;
  dy = a->y-b->y;
  dz = a->z-b->z;
  return (dx*dx + dy*dy + dz*dz);
}

int read_distance(char *str)
{
  //str has format +03.123
  int v =
     10000 * str[1]
    + 1000 * str[2];
    +  100 * str[4];
    +   10 * str[5];
    +    1 * str[6];
    -11111 * '0';
  return str[0] == '+' ? v : -v;
}

int main(int argc, char *argv[])
{
  if (argc < 2 || !strcmp(argv[1], "-t")) {
    printf("Usage: %s -t<number of OpenMP threads>\n", argv[0]);
    exit(1);
  }
  int nthreads = strtoi(argv[1]+2);

  FILE *fp = fopen("cells", "r");
  if (fp == NULL) {
    printf("Could not open \"cells\"\n");
    exit(1);
  }
  struct stat st;
  fstat(fp, &st); //gets some statistics for the file fp. st.size is the filesize in byte
  size_t ncoordinates = st.st_size / 24; //24 characters/line
  input = malloc(ncoordinates * sizeof(*input));
  char *buffer = malloc(24);
  for (size_t i = 0; i < ncoordinates; i++) {
    if (NULL == fgets(buffer, 24, fp))
      exit(1);
    int x, y, z;
    x = read_distance(buffer);
    y = read_distance(buffer+8);
    z = read_distance(buffer+16);
    
    input[i].x = x;
    input[i].y = y;
    input[i].z = z;
  
  }
  free(buffer);
  fclose(fp);

  //TODO Here the computing and openMP should come into play
  
  free(input);
  return 0;
}
