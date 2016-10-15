  /* loop over _all_ cells */
  for (int ii = 0; ii < params.ny; ii++)
  {
    /* determine indices of axis-direction neighbours
    ** respecting periodic boundary conditions (wrap around) */
    int y_s = (ii == 0) ? (ii + params.ny - 1) : (ii - 1); // could move up
    int y_n = (ii + 1) % params.ny; // Could move up
    
    for (int jj = 0; jj < params.nx; jj++)
    {
      int index = ii * params.nx + jj;
      /* determine indices of axis-direction neighbours
      ** respecting periodic boundary conditions (wrap around) */
      int x_e = (jj + 1) % params.nx;
      int x_w = (jj == 0) ? (jj + params.nx - 1) : (jj - 1);
      /* propagate densities to neighbouring cells, following
      ** appropriate directions of travel and writing into
      ** scratch space grid */
      tmp_cells[ii * params.nx + jj].speeds[0]    = cells[index].speeds[0]; /* central cell, no movement */
      tmp_cells[ii * params.nx + x_e].speeds[1]   = cells[index].speeds[1]; /* east */
      tmp_cells[y_n * params.nx + jj].speeds[2]   = cells[index].speeds[2]; /* north */
      tmp_cells[ii * params.nx + x_w].speeds[3]   = cells[index].speeds[3]; /* west */
      tmp_cells[y_s * params.nx + jj].speeds[4]   = cells[index].speeds[4]; /* south */
      tmp_cells[y_n * params.nx + x_e].speeds[5]  = cells[index].speeds[5]; /* north-east */
      tmp_cells[y_n * params.nx + x_w].speeds[6]  = cells[index].speeds[6]; /* north-west */
      tmp_cells[y_s * params.nx + x_w].speeds[7]  = cells[index].speeds[7]; /* south-west */
      tmp_cells[y_s * params.nx + x_e].speeds[8]  = cells[index].speeds[8]; /* south-east */