
// Moving to south west cell -> diagonally backward left
cell[y_s * params.nx + x_w].speed[0] = cells[index].speeds[0]
cell[y_s * params.nx + x_w].speed[1] = cells[ii * params.nx + x_e].speeds[1]  /* east */
cell[y_s * params.nx + x_w].speed[2] = cells[y_n * params.nx + jj].speeds[2]  /* north */
cell[y_s * params.nx + x_w].speed[3] = cells[ii * params.nx + x_w].speeds[3]  /* west */
cell[y_s * params.nx + x_w].speed[4] = cells[y_s * params.nx + jj].speeds[4]  /* south */
cell[y_s * params.nx + x_w].speed[5] = cells[y_n * params.nx + x_e].speeds[5] /* north-east */
cell[y_s * params.nx + x_w].speed[6] = cells[y_n * params.nx + x_w].speeds[6] /* north-west */
cell[y_s * params.nx + x_w].speed[7] = cells[y_s * params.nx + x_w].speeds[7] /* south-west */
cell[y_s * params.nx + x_w].speed[8] = cells[y_s * params.nx + x_e].speeds[8] /* south-east */


if (fusion = 1){

  for (int ii = 1; ii < params.ny + 1; ii++)
  {
    for (int jj = 1; jj < params.nx + 1; jj++)
    {
      int index = ii * (params.nx + 1) + jj;

// scratch space index
      int x_e = (jj + 1) % params.nx + 1;
      int x_w = (jj == 1) ? (jj + (params.nx + 1) - 1) : (jj - 1);

      int y_s = (ii == 1) ? (ii + (params.ny + 1) - 1) : (ii - 1); // could move up
      int y_n = (ii + 1) % params.ny + 1; // Could move up

      /* compute local density total */
        double local_density = 0.0;

        local_density += cells[ii * params.nx + jj].speeds[0];
        local_density += cells[ii * params.nx + x_e].speeds[1];
        local_density += cells[y_n * params.nx + jj].speeds[2];
        local_density += cells[ii * params.nx + x_w].speeds[3];
        local_density += cells[y_s * params.nx + jj].speeds[4];
        local_density += cells[y_n * params.nx + x_e].speeds[5];
        local_density += cells[y_n * params.nx + x_w].speeds[6];
        local_density += cells[y_s * params.nx + x_w].speeds[7];
        local_density += cells[y_s * params.nx + x_e].speeds[8];


        double local_density_invert = 1 / local_density;
        /* compute x velocity component */
        double u_x = (cells[ii * params.nx + x_e].speeds[1]
                      + cells[y_n * params.nx + x_e].speeds[5]
                      + cells[y_s * params.nx + x_e].speeds[8]
                      - (cells[ii * params.nx + x_w].speeds[3]
                         + cells[y_n * params.nx + x_w].speeds[6]
                         + cells[y_s * params.nx + x_w].speeds[7]))
                     * local_density_invert;
        /* compute y velocity component */
        double u_y = (cells[y_n * params.nx + jj].speeds[2]
                      + cells[y_n * params.nx + x_e].speeds[5]
                      + cells[y_n * params.nx + x_w].speeds[6]
                      - (cells[y_s * params.nx + jj].speeds[4]
                         + cells[y_s * params.nx + x_w].speeds[7]
                         + cells[y_s * params.nx + x_e].speeds[8]))
                     * local_density_invert;

      

// -------------rebound--------------------------------
      /* don't consider occupied cells */
      if (obstacles[index])
      {
        /* called after propagate, so taking values from scratch space
        ** mirroring, and writing into main grid */
        cells[index].speeds[1] = cells[ii * params.nx + x_w].speeds[3];
        cells[index].speeds[2] = cells[y_s * params.nx + jj].speeds[4];
        cells[index].speeds[3] = cells[ii * params.nx + x_e].speeds[1];
        cells[index].speeds[4] = cells[y_n * params.nx + jj].speeds[2];
        cells[index].speeds[5] = cells[y_s * params.nx + x_w].speeds[7];
        cells[index].speeds[6] = cells[y_s * params.nx + x_e].speeds[8];
        cells[index].speeds[7] = cells[y_n * params.nx + x_e].speeds[5];
        cells[index].speeds[8] = cells[y_n * params.nx + x_w].speeds[6];
      } 
// ----------------END--------------------------------------------
      else 
      {

        int current_cell = (ii + (params.ny + 1) - 1) * params.nx + x_w



        cells[current_cell].speeds[0] = cells[ii * params.nx + jj].speeds[0]                   
                                                  + params.omega 
                                                  * (local_density * d1 * (16 - (u_x * u_x + u_y * u_y) * 864 * d1)
                                                    - cells[ii * params.nx + jj]);
        cells[current_cell].speeds[1] = cells[ii * params.nx + x_e].speeds[1]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_x * 12 + (u_x * u_x) * 648 * d1- (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[ii * params.nx + x_e].speeds[1]);
        cells[current_cell].speeds[2] = cells[y_n * params.nx + jj].speeds[2]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_y * 12 + (u_y * u_y) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + jj].speeds[2]);
        cells[current_cell].speeds[3] = cells[ii * params.nx + x_w].speeds[3]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + - u_x * 12 + (- u_x * - u_x) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[ii * params.nx + x_w].speeds[3]);
        cells[current_cell].speeds[4] = cells[y_s * params.nx + jj].speeds[4]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + - u_y * 12 + (- u_y * - u_y) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + jj].speeds[4]);
        cells[current_cell].speeds[5] = cells[y_n * params.nx + x_e].speeds[5]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x + u_y) * 3 + ((u_x + u_y) * (u_x + u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + x_e].speeds[5]);
        cells[current_cell].speeds[6] = cells[y_n * params.nx + x_w].speeds[6]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (- u_x + u_y) * 3 + ((- u_x + u_y) * (- u_x + u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + x_w].speeds[6]);
        cells[current_cell].speeds[7] = cells[y_s * params.nx + x_w].speeds[7]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (- u_x - u_y) * 3 + ((- u_x - u_y) * (- u_x - u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + x_w].speeds[7]);
        cells[current_cell].speeds[8] = cells[y_s * params.nx + x_e].speeds[8]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x - u_y) * 3 + ((u_x - u_y) * (u_x - u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + x_e].speeds[8]);

// --------------av_velocity-----------------------------------------------

        /* accumulate the norm of x- and y- velocity components */
        tot_u += sqrt((u_x * u_x) + (u_y * u_y));
        /* increase counter of inspected cells */
        ++tot_cells;
      }
    }
  }
  fusion = 0;
}
else{

  for (int ii = 0; ii < params.ny; ii++)
  {
    for (int jj = 0; jj < params.nx; jj++)
    {
      int index = ii * (params.nx ) + jj;

// scratch space index
      int x_e = (jj + 1) % params.nx ;
      int x_w = (jj == 1) ? (jj + (params.nx ) - 1) : (jj - 1);

      int y_s = (ii == 1) ? (ii + (params.ny ) - 1) : (ii - 1); // could move up
      int y_n = (ii + 1) % params.ny ; // Could move up

      /* compute local density total */
        double local_density = 0.0;

        local_density += cells[ii * params.nx + jj].speeds[0];
        local_density += cells[ii * params.nx + x_e].speeds[1];
        local_density += cells[y_n * params.nx + jj].speeds[2];
        local_density += cells[ii * params.nx + x_w].speeds[3];
        local_density += cells[y_s * params.nx + jj].speeds[4];
        local_density += cells[y_n * params.nx + x_e].speeds[5];
        local_density += cells[y_n * params.nx + x_w].speeds[6];
        local_density += cells[y_s * params.nx + x_w].speeds[7];
        local_density += cells[y_s * params.nx + x_e].speeds[8];


        double local_density_invert = 1 / local_density;
        /* compute x velocity component */
        double u_x = (cells[ii * params.nx + x_e].speeds[1]
                      + cells[y_n * params.nx + x_e].speeds[5]
                      + cells[y_s * params.nx + x_e].speeds[8]
                      - (cells[ii * params.nx + x_w].speeds[3]
                         + cells[y_n * params.nx + x_w].speeds[6]
                         + cells[y_s * params.nx + x_w].speeds[7]))
                     * local_density_invert;
        /* compute y velocity component */
        double u_y = (cells[y_n * params.nx + jj].speeds[2]
                      + cells[y_n * params.nx + x_e].speeds[5]
                      + cells[y_n * params.nx + x_w].speeds[6]
                      - (cells[y_s * params.nx + jj].speeds[4]
                         + cells[y_s * params.nx + x_w].speeds[7]
                         + cells[y_s * params.nx + x_e].speeds[8]))
                     * local_density_invert;

      

// -------------rebound--------------------------------
      /* don't consider occupied cells */
      if (obstacles[index])
      {
        /* called after propagate, so taking values from scratch space
        ** mirroring, and writing into main grid */
        cells[index].speeds[1] = cells[ii * params.nx + x_w].speeds[3];
        cells[index].speeds[2] = cells[y_s * params.nx + jj].speeds[4];
        cells[index].speeds[3] = cells[ii * params.nx + x_e].speeds[1];
        cells[index].speeds[4] = cells[y_n * params.nx + jj].speeds[2];
        cells[index].speeds[5] = cells[y_s * params.nx + x_w].speeds[7];
        cells[index].speeds[6] = cells[y_s * params.nx + x_e].speeds[8];
        cells[index].speeds[7] = cells[y_n * params.nx + x_e].speeds[5];
        cells[index].speeds[8] = cells[y_n * params.nx + x_w].speeds[6];
      } 
// ----------------END--------------------------------------------
      else 
      {

        int current_cell = (ii + (params.ny) - 1) * params.nx + x_w



        cells[current_cell].speeds[0] = cells[ii * params.nx + jj].speeds[0]                   
                                                  + params.omega 
                                                  * (local_density * d1 * (16 - (u_x * u_x + u_y * u_y) * 864 * d1)
                                                    - cells[ii * params.nx + jj]);
        cells[current_cell].speeds[1] = cells[ii * params.nx + x_e].speeds[1]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_x * 12 + (u_x * u_x) * 648 * d1- (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[ii * params.nx + x_e].speeds[1]);
        cells[current_cell].speeds[2] = cells[y_n * params.nx + jj].speeds[2]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_y * 12 + (u_y * u_y) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + jj].speeds[2]);
        cells[current_cell].speeds[3] = cells[ii * params.nx + x_w].speeds[3]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + - u_x * 12 + (- u_x * - u_x) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[ii * params.nx + x_w].speeds[3]);
        cells[current_cell].speeds[4] = cells[y_s * params.nx + jj].speeds[4]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + - u_y * 12 + (- u_y * - u_y) * 648 * d1 - (216 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + jj].speeds[4]);
        cells[current_cell].speeds[5] = cells[y_n * params.nx + x_e].speeds[5]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x + u_y) * 3 + ((u_x + u_y) * (u_x + u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + x_e].speeds[5]);
        cells[current_cell].speeds[6] = cells[y_n * params.nx + x_w].speeds[6]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (- u_x + u_y) * 3 + ((- u_x + u_y) * (- u_x + u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_n * params.nx + x_w].speeds[6]);
        cells[current_cell].speeds[7] = cells[y_s * params.nx + x_w].speeds[7]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (- u_x - u_y) * 3 + ((- u_x - u_y) * (- u_x - u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + x_w].speeds[7]);
        cells[current_cell].speeds[8] = cells[y_s * params.nx + x_e].speeds[8]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x - u_y) * 3 + ((u_x - u_y) * (u_x - u_y)) * 162 * d1 - (54 * d1 * (u_x * u_x + u_y * u_y)))
                                                    - cells[y_s * params.nx + x_e].speeds[8]);

// --------------av_velocity-----------------------------------------------

        /* accumulate the norm of x- and y- velocity components */
        tot_u += sqrt((u_x * u_x) + (u_y * u_y));
        /* increase counter of inspected cells */
        ++tot_cells;
      }
    }
  }
  fusion = 1;

}


}



