/*
** Code to implement a d2q9-bgk lattice boltzmann scheme.
** 'd2' inidates a 2-dimensional grid, and
** 'q9' indicates 9 velocities per grid cell.
** 'bgk' refers to the Bhatnagar-Gross-Krook collision step.
**
** The 'speeds' in each cell are numbered as follows:
**
** 6 2 5
**  \|/
** 3-0-1
**  /|\
** 7 4 8
**
** A 2D grid:
**
**           cols
**       --- --- ---
**      | D | E | F |
** rows  --- --- ---
**      | A | B | C |
**       --- --- ---
**
** 'unwrapped' in row major order to give a 1D array:
**
**  --- --- --- --- --- ---
** | A | B | C | D | E | F |
**  --- --- --- --- --- ---
**
** Grid indicies are:
**
**          ny
**          ^       cols(jj)
**          |  ----- ----- -----
**          | | ... | ... | etc |
**          |  ----- ----- -----
** rows(ii) | | 1,0 | 1,1 | 1,2 |
**          |  ----- ----- -----
**          | | 0,0 | 0,1 | 0,2 |
**          |  ----- ----- -----
**          ----------------------> nx
**
** Note the names of the input parameter and obstacle files
** are passed on the command line, e.g.:
**
**   d2q9-bgk.exe input.params obstacles.dat
**
** Be sure to adjust the grid dimensions in the parameter file
** if you choose a different obstacle file.
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<omp.h>

/* struct to hold the parameter values */
typedef struct
{
  int    nx;            /* no. of cells in x-direction */
  int    ny;            /* no. of cells in y-direction */
  int    maxIters;      /* no. of iterations */
  int    reynolds_dim;  /* dimension for Reynolds number */
  float density;       /* density per link */
  float accel;         /* density redistribution */
  float omega;         /* relaxation parameter */
} t_param;

/* struct to hold the 'speed' values */
typedef struct
{
  float speeds[9];
} t_speed;

/*
** function prototypes
*/

/* load params, allocate memory, load obstacles & initialise fluid particle densities */
int initialise(const char* paramfile, const char* obstaclefile,
               t_param* params, t_speed** cells_ptr, t_speed** tmp_cells_ptr,
               int** obstacles_ptr, float** av_vels_ptr);

/*
** The main calculation methods.
** timestep calls, in order, the functions:
** accelerate_flow(), propagate(), rebound() & collision()
*/
void accelerate_flow(const t_param params, t_speed* cells, int* obstacles);
void collision(const t_param params, t_speed* cells, t_speed* tmp_cells, int* obstacles, float* av_vels, int* tt, const int* tot_cells);
int write_values(const t_param params, t_speed* cells, int* obstacles, float* av_vels);



/* finalise, including freeing up allocated memory */
int finalise(const t_param* params, t_speed** cells_ptr, t_speed** tmp_cells_ptr,
             int** obstacles_ptr, float** av_vels_ptr);

/* Sum all the densities in the grid.
** The total should remain constant from one timestep to the next. */
float total_density(const t_param params, t_speed* cells);

/* calculate Reynolds number */
float calc_reynolds(const t_param params, t_speed* cells, int* obstacles, float* av_vels, int tt);

/* utility functions */
void die(const char* message, const int line, const char* file);
void usage(const char* exe);


/*
** main program:
** initialise, timestep loop, finalise
*/
int main(int argc, char* argv[])
{
  char*    paramfile = NULL;    /* name of the input parameter file */
  char*    obstaclefile = NULL; /* name of a the input obstacle file */
  t_param  params;              /* struct to hold parameter values */
  t_speed* cells     = NULL;    /* grid containing fluid densities */
  t_speed* tmp_cells = NULL;    /* scratch space */
  int*     obstacles = NULL;    /* grid indicating which cells are blocked */
  float* av_vels   = NULL;     /* a record of the av. velocity computed for each timestep */
  struct timeval timstr;        /* structure to hold elapsed time */
  struct rusage ru;             /* structure to hold CPU time--system and user */
  double tic, toc;              /* floating point numbers to calculate elapsed wallclock time */
  double usrtim;                /* floating point number to record elapsed user CPU time */
  double systim;                /* floating point number to record elapsed system CPU time */
  int tot_cells;

  /* parse the command line */
  if (argc != 3)
  {
    usage(argv[0]);
  }
  else
  {
    paramfile = argv[1];
    obstaclefile = argv[2];
  }

  /* initialise our data structures and load values from file */
  tot_cells = initialise(paramfile, obstaclefile, &params, &cells, &tmp_cells, &obstacles, &av_vels);

  /* iterate for maxIters timesteps */
  gettimeofday(&timstr, NULL);        
  tic = timstr.tv_sec + (timstr.tv_usec / 1000000.0);

  // maxIters = 4000
  for (int tt = 0; tt < params.maxIters; tt++)
  {
    accelerate_flow(params, cells, obstacles);
    collision(params, cells, tmp_cells, obstacles, av_vels, &tt, &tot_cells);
    t_speed* temp = cells;
    cells = tmp_cells;
    tmp_cells = temp;
#ifdef DEBUG
    printf("==timestep: %d==\n", tt);
    printf("av velocity: %.12E\n", av_vels[tt]);
    printf("tot density: %.12E\n", total_density(params, cells));
#endif
  }

  gettimeofday(&timstr, NULL);
  toc = timstr.tv_sec + (timstr.tv_usec / 1000000.0);
  getrusage(RUSAGE_SELF, &ru);
  timstr = ru.ru_utime;
  usrtim = timstr.tv_sec + (timstr.tv_usec / 1000000.0);
  timstr = ru.ru_stime;
  systim = timstr.tv_sec + (timstr.tv_usec / 1000000.0);

  /* write final values and free memory */
  printf("==done==\n");
  printf("Elapsed time:\t\t\t%.6lf (s)\n", toc - tic);
  printf("Elapsed user CPU time:\t\t%.6lf (s)\n", usrtim);
  printf("Elapsed system CPU time:\t%.6lf (s)\n", systim);
  write_values(params, cells, obstacles, av_vels);
  finalise(&params, &cells, &tmp_cells, &obstacles, &av_vels);

  return EXIT_SUCCESS;
}

void accelerate_flow(const t_param params, t_speed* cells, int* obstacles)
{
  /* compute weighting factors */

  // float w1 = 0.1 * 0.005 / 9.0;
  // float w2 = 0.1 * 0.005 / 36.0;

  float w2 = 1.0f / 72000.0f;
  float w1 = 4.0f * w2;

  /* modify the 2nd row of the grid */
  int ii = params.ny - 2;
  // array indexing: 16128 -> 16255
  for (int jj = 0; jj < params.nx; jj++)
  {
    int index = ii * params.nx + jj;
    /* if the cell is not occupied and
    ** we don't send a negative density */
    if (!obstacles[index]
        && (cells[index].speeds[3] - w1) > 0.0
        && (cells[index].speeds[6] - w2) > 0.0
        && (cells[index].speeds[7] - w2) > 0.0)
    {
      /* increase 'east-side' densities */
      cells[index].speeds[1] += w1;
      cells[index].speeds[5] += w2;
      cells[index].speeds[8] += w2;
      /* decrease 'west-side' densities */
      cells[index].speeds[3] -= w1;
      cells[index].speeds[6] -= w2;
      cells[index].speeds[7] -= w2;
    }

  }
}

void collision(const t_param params, t_speed* cells, t_speed* tmp_cells, int* obstacles, float* av_vels, int* tt, const int* tot_cells)
{
  float tot_u = 0.0f;    /* accumulated magnitudes of velocity for each cell */
  static const float d1 = 1 / 36.0f;

  /* loop over the cells in the grid
  ** NB the collision step is called after
  ** the propagate step and so values of interest
  ** are in the scratch-space grid */
// #pragma omp parallel for schedule(dynamic,1) reduction(+: tot_u, tot_cells)
#pragma omp parallel for simd reduction(+:tot_u) schedule(static) num_threads(16)
for (int ii = 0; ii < params.ny; ii++)
  {
      int y_s = (ii == 0) ? (ii + params.ny - 1) : (ii - 1); // could move up
      int y_n = (ii + 1) % params.ny; // Could move up
      
    for (int jj = 0; jj < params.nx; jj++)
    {
      int index = ii * params.nx + jj;

      int x_e = (jj + 1) % params.nx;
      int x_w = (jj == 0) ? (jj + params.nx - 1) : (jj - 1);

// -------------rebound--------------------------------
      /* don't consider occupied cells */
      if (obstacles[index])
      {
        /* called after propagate, so taking values from scratch space
        ** mirroring, and writing into main grid */
        tmp_cells[index].speeds[1] = cells[ii * params.nx + x_e].speeds[3];
        tmp_cells[index].speeds[2] = cells[y_n * params.nx + jj].speeds[4];
        tmp_cells[index].speeds[3] = cells[ii * params.nx + x_w].speeds[1];
        tmp_cells[index].speeds[4] = cells[y_s * params.nx + jj].speeds[2];
        tmp_cells[index].speeds[5] = cells[y_n * params.nx + x_e].speeds[7];
        tmp_cells[index].speeds[6] = cells[y_n * params.nx + x_w].speeds[8];
        tmp_cells[index].speeds[7] = cells[y_s * params.nx + x_w].speeds[5];
        tmp_cells[index].speeds[8] = cells[y_s * params.nx + x_e].speeds[6];
      } 
// ----------------END--------------------------------------------
      else 
      {

        /* compute local density total */
        float local_density = 0.0f;
        local_density += cells[ii * params.nx + jj].speeds[0];
        local_density += cells[ii * params.nx + x_e].speeds[3];
        local_density += cells[y_n * params.nx + jj].speeds[4];
        local_density += cells[ii * params.nx + x_w].speeds[1];
        local_density += cells[y_s * params.nx + jj].speeds[2];
        local_density += cells[y_n * params.nx + x_e].speeds[7];
        local_density += cells[y_n * params.nx + x_w].speeds[8];
        local_density += cells[y_s * params.nx + x_w].speeds[5];
        local_density += cells[y_s * params.nx + x_e].speeds[6];


        float local_density_invert = 1 / local_density;
        /* compute x velocity component */
        float u_x = (cells[ii * params.nx + x_w].speeds[1]
                      + cells[y_s * params.nx + x_w].speeds[5]
                      + cells[y_n * params.nx + x_w].speeds[8]
                      - (cells[ii * params.nx + x_e].speeds[3]
                         + cells[y_s * params.nx + x_e].speeds[6]
                         + cells[y_n * params.nx + x_e].speeds[7]))
                     * local_density_invert;
        /* compute y velocity component */
        float u_y = (cells[y_s * params.nx + jj].speeds[2]
                      + cells[y_s * params.nx + x_w].speeds[5]
                      + cells[y_s * params.nx + x_e].speeds[6]
                      - (cells[y_n * params.nx + jj].speeds[4]
                         + cells[y_n * params.nx + x_e].speeds[7]
                         + cells[y_n * params.nx + x_w].speeds[8]))
                     * local_density_invert;

 


        tmp_cells[index].speeds[0] = cells[ii * params.nx + jj].speeds[0]                   
                                                  + params.omega 
                                                  * (local_density * d1 * (16 - (u_x * u_x + u_y * u_y) * 864 * d1)
                                                    - cells[ii * params.nx + jj].speeds[0]);
        tmp_cells[index].speeds[1] = cells[ii * params.nx + x_w].speeds[1]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_x * 12 + (u_x * u_x) * 648 * d1- (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[ii * params.nx + x_w].speeds[1]);
        tmp_cells[index].speeds[2] = cells[y_s * params.nx + jj].speeds[2]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_y * 12 + (u_y * u_y) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + jj].speeds[2]);
        tmp_cells[index].speeds[3] = cells[ii * params.nx + x_e].speeds[3]
                                                  + params.omega
                                                  * (local_density * d1 * (4 - u_x * 12 + (u_x * u_x) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[ii * params.nx + x_e].speeds[3]);
        tmp_cells[index].speeds[4] = cells[y_n * params.nx + jj].speeds[4]
                                                  + params.omega
                                                  * (local_density * d1 * (4 - u_y * 12 + (u_y * u_y) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + jj].speeds[4]);
        tmp_cells[index].speeds[5] = cells[y_s * params.nx + x_w].speeds[5]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x + u_y) * 3 + ((u_x + u_y) * (u_x + u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + x_w].speeds[5]); 
        tmp_cells[index].speeds[6] = cells[y_s * params.nx + x_e].speeds[6]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (- u_x + u_y) * 3 + ((- u_x + u_y) * (- u_x + u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + x_e].speeds[6]);
        tmp_cells[index].speeds[7] = cells[y_n * params.nx + x_e].speeds[7] 
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (- u_x - u_y) * 3 + ((- u_x - u_y) * (- u_x - u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + x_e].speeds[7]);
        tmp_cells[index].speeds[8] = cells[y_n * params.nx + x_w].speeds[8]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x - u_y) * 3 + ((u_x - u_y) * (u_x - u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + x_w].speeds[8]);


// --------------av_velocity-----------------------------------------------

        /* accumulate the norm of x- and y- velocity components */
        tot_u += sqrt((u_x * u_x) + (u_y * u_y));
      }
    }
  }
  av_vels[*tt] = tot_u / (float)(*tot_cells);
}

int initialise(const char* paramfile, const char* obstaclefile,
               t_param* params, t_speed** cells_ptr, t_speed** tmp_cells_ptr,
               int** obstacles_ptr, float** av_vels_ptr)
{
  char   message[1024];  /* message buffer */
  FILE*   fp;            /* file pointer */
  int    xx, yy;         /* generic array indices */
  int    blocked;        /* indicates whether a cell is blocked by an obstacle */
  int    retval;         /* to hold return value for checking */

  /* open the parameter file */
  fp = fopen(paramfile, "r");

  if (fp == NULL)
  {
    sprintf(message, "could not open input parameter file: %s", paramfile);
    die(message, __LINE__, __FILE__);
  }

  /* read in the parameter values */
  retval = fscanf(fp, "%d\n", &(params->nx));

  if (retval != 1) die("could not read param file: nx", __LINE__, __FILE__);

  retval = fscanf(fp, "%d\n", &(params->ny));

  if (retval != 1) die("could not read param file: ny", __LINE__, __FILE__);

  retval = fscanf(fp, "%d\n", &(params->maxIters));

  if (retval != 1) die("could not read param file: maxIters", __LINE__, __FILE__);

  retval = fscanf(fp, "%d\n", &(params->reynolds_dim));

  if (retval != 1) die("could not read param file: reynolds_dim", __LINE__, __FILE__);

  retval = fscanf(fp, "%f\n", &(params->density));

  if (retval != 1) die("could not read param file: density", __LINE__, __FILE__);

  retval = fscanf(fp, "%f\n", &(params->accel));

  if (retval != 1) die("could not read param file: accel", __LINE__, __FILE__);

  retval = fscanf(fp, "%f\n", &(params->omega));

  if (retval != 1) die("could not read param file: omega", __LINE__, __FILE__);

  /* and close up the file */
  fclose(fp);

  /*
  ** Allocate memory.
  **
  ** Remember C is pass-by-value, so we need to
  ** pass pointers into the initialise function.
  **
  ** NB we are allocating a 1D array, so that the
  ** memory will be contiguous.  We still want to
  ** index this memory as if it were a (row major
  ** ordered) 2D array, however.  We will perform
  ** some arithmetic using the row and column
  ** coordinates, inside the square brackets, when
  ** we want to access elements of this array.
  **
  ** Note also that we are using a structure to
  ** hold an array of 'speeds'.  We will allocate
  ** a 1D array of these structs.
  */

  /* main grid */
  *cells_ptr = (t_speed*)malloc(sizeof(t_speed) * (params->ny * params->nx));

  if (*cells_ptr == NULL) die("cannot allocate memory for cells", __LINE__, __FILE__);

  /* 'helper' grid, used as scratch space */
  *tmp_cells_ptr = (t_speed*)malloc(sizeof(t_speed) * (params->ny * params->nx));

  if (*tmp_cells_ptr == NULL) die("cannot allocate memory for tmp_cells", __LINE__, __FILE__);

  /* the map of obstacles */
  *obstacles_ptr = malloc(sizeof(int) * (params->ny * params->nx));

  if (*obstacles_ptr == NULL) die("cannot allocate column memory for obstacles", __LINE__, __FILE__);

  /* initialise densities */
  float w0 = params->density * 4.0 / 9.0;
  float w1 = params->density      / 9.0;
  float w2 = params->density      / 36.0;

// ny, nx = 128
  for (int ii = 0; ii < params->ny; ii++)
  {
    for (int jj = 0; jj < params->nx; jj++)
    {   
      /* centre */
      (*cells_ptr)[ii * params->nx + jj].speeds[0] = w0;
      /* axis directions */
      (*cells_ptr)[ii * params->nx + jj].speeds[1] = w1;
      (*cells_ptr)[ii * params->nx + jj].speeds[2] = w1;
      (*cells_ptr)[ii * params->nx + jj].speeds[3] = w1;
      (*cells_ptr)[ii * params->nx + jj].speeds[4] = w1;
      /* diagonals */
      (*cells_ptr)[ii * params->nx + jj].speeds[5] = w2;
      (*cells_ptr)[ii * params->nx + jj].speeds[6] = w2;
      (*cells_ptr)[ii * params->nx + jj].speeds[7] = w2;
      (*cells_ptr)[ii * params->nx + jj].speeds[8] = w2;

      /* first set all cells in obstacle array to zero */
      (*obstacles_ptr)[ii * params->nx + jj] = 0;

    }
  }

  /* open the obstacle data file */
  fp = fopen(obstaclefile, "r");

  if (fp == NULL)
  {
    sprintf(message, "could not open input obstacles file: %s", obstaclefile);
    die(message, __LINE__, __FILE__);
  }

  int tot_cells_blocked = 0;

  /* read-in the blocked cells list */
  while ((retval = fscanf(fp, "%d %d %d\n", &xx, &yy, &blocked)) != EOF)
  {
    /* some checks */
    if (retval != 3) die("expected 3 values per line in obstacle file", __LINE__, __FILE__);

    if (xx < 0 || xx > params->nx - 1) die("obstacle x-coord out of range", __LINE__, __FILE__);

    if (yy < 0 || yy > params->ny - 1) die("obstacle y-coord out of range", __LINE__, __FILE__);

    if (blocked != 1) die("obstacle blocked value should be 1", __LINE__, __FILE__);

    /* assign to array */
    (*obstacles_ptr)[yy * params->nx + xx] = blocked;
    tot_cells_blocked++;

  }

  /* and close the file */
  fclose(fp);

  /*
  ** allocate space to hold a record of the avarage velocities computed
  ** at each timestep
  */
  *av_vels_ptr = (float*)malloc(sizeof(float) * params->maxIters);

  int tot_cells = params->nx * params->ny - tot_cells_blocked;
  
  return tot_cells;
}

int finalise(const t_param* params, t_speed** cells_ptr, t_speed** tmp_cells_ptr,
             int** obstacles_ptr, float** av_vels_ptr)
{
  /*
  ** free up allocated memory
  */
  free(*cells_ptr);
  *cells_ptr = NULL;

  free(*tmp_cells_ptr);
  *tmp_cells_ptr = NULL;

  free(*obstacles_ptr);
  *obstacles_ptr = NULL;

  free(*av_vels_ptr);
  *av_vels_ptr = NULL;

  return EXIT_SUCCESS;
}

float total_density(const t_param params, t_speed* cells)
{
  float total = 0.0;  /* accumulator */

  for (int ii = 0; ii < params.ny; ii++)
  {
    for (int jj = 0; jj < params.nx; jj++)
    {
      for (int kk = 0; kk < 9; kk++)
      {
        total += cells[ii * params.nx + jj].speeds[kk];
      }
    }
  }

  return total;
}

int write_values(const t_param params, t_speed* cells, int* obstacles, float* av_vels)
{
  FILE* fp;                     /* file pointer */
  const float c_sq = 1.0 / 3.0; /* sq. of speed of sound */
  float local_density;         /* per grid cell sum of densities */
  float pressure;              /* fluid pressure in grid cell */
  float u_x;                   /* x-component of velocity in grid cell */
  float u_y;                   /* y-component of velocity in grid cell */
  float u;                     /* norm--root of summed squares--of u_x and u_y */

  fp = fopen("final_state.dat", "w");

  if (fp == NULL)
  {
    die("could not open file output file", __LINE__, __FILE__);
  }

  for (int ii = 0; ii < params.ny; ii++)
  {
    for (int jj = 0; jj < params.nx; jj++)
    {
      /* an occupied cell */
      if (obstacles[ii * params.nx + jj])
      {
        u_x = u_y = u = 0.0;
        pressure = params.density * c_sq;
      }
      /* no obstacle */
      else
      {
        local_density = 0.0;

        for (int kk = 0; kk < 9; kk++)
        {
          local_density += cells[ii * params.nx + jj].speeds[kk];
        }

        /* compute x velocity component */
        u_x = (cells[ii * params.nx + jj].speeds[1]
               + cells[ii * params.nx + jj].speeds[5]
               + cells[ii * params.nx + jj].speeds[8]
               - (cells[ii * params.nx + jj].speeds[3]
                  + cells[ii * params.nx + jj].speeds[6]
                  + cells[ii * params.nx + jj].speeds[7]))
              / local_density;
        /* compute y velocity component */
        u_y = (cells[ii * params.nx + jj].speeds[2]
               + cells[ii * params.nx + jj].speeds[5]
               + cells[ii * params.nx + jj].speeds[6]
               - (cells[ii * params.nx + jj].speeds[4]
                  + cells[ii * params.nx + jj].speeds[7]
                  + cells[ii * params.nx + jj].speeds[8]))
              / local_density;
        /* compute norm of velocity */
        u = sqrt((u_x * u_x) + (u_y * u_y));
        /* compute pressure */
        pressure = local_density * c_sq;
      }

      /* write to file */
      fprintf(fp, "%d %d %.12E %.12E %.12E %.12E %d\n", jj, ii, u_x, u_y, u, pressure, obstacles[ii * params.nx + jj]);
    }
  }

  fclose(fp);

  fp = fopen("av_vels.dat", "w");

  if (fp == NULL)
  {
    die("could not open file output file", __LINE__, __FILE__);
  }

  for (int ii = 0; ii < params.maxIters; ii++)
  {
    fprintf(fp, "%d:\t%.12E\n", ii, av_vels[ii]);
  }

  fclose(fp);

  return EXIT_SUCCESS;
}

void die(const char* message, const int line, const char* file)
{
  fprintf(stderr, "Error at line %d of file %s:\n", line, file);
  fprintf(stderr, "%s\n", message);
  fflush(stderr);
  exit(EXIT_FAILURE);
}

void usage(const char* exe)
{
  fprintf(stderr, "Usage: %s <paramfile> <obstaclefile>\n", exe);
  exit(EXIT_FAILURE);
}
