
        double sq_xy = sq_x + sq_y;
        double sq_x = u_x * u_x;
        double sq_y = 

        tmp_cells[index].speeds[0] = cells[ii * params.nx + jj].speeds[0]                   
                                                  + params.omega 
                                                  * (local_density * d1 * (16 - (sq_xy) * 864 * d1)
                                                    - cells[ii * params.nx + jj].speeds[0]);
        tmp_cells[index].speeds[1] = cells[ii * params.nx + x_w].speeds[1]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_x * 12 + (sq_x) * 648 * d1- (216 * d1 * (sq_xy)))
                                                    - cells[ii * params.nx + x_w].speeds[1]);
        tmp_cells[index].speeds[2] = cells[y_s * params.nx + jj].speeds[2]
                                                  + params.omega
                                                  * (local_density * d1 * (4 + u_y * 12 + (sq_y) * 648 * d1 - (216 * d1 * (sq_xy)))
                                                    - cells[y_s * params.nx + jj].speeds[2]);
        tmp_cells[index].speeds[3] = cells[ii * params.nx + x_e].speeds[3]
                                                  + params.omega
                                                  * (local_density * d1 * (4 - u_x * 12 + (sq_x) * 648 * d1 - (216 * d1 * (sq_xy)))
                                                    - cells[ii * params.nx + x_e].speeds[3]);
        tmp_cells[index].speeds[4] = cells[y_n * params.nx + jj].speeds[4]
                                                  + params.omega
                                                  * (local_density * d1 * (4 - u_y * 12 + (sq_y) * 648 * d1 - (216 * d1 * (sq_xy)))
                                                    - cells[y_n * params.nx + jj].speeds[4]);
        tmp_cells[index].speeds[5] = cells[y_s * params.nx + x_w].speeds[5]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x + u_y) * 3 + ((sq_xy) - 2 * (u_y * u_x)) * 162 * d1 - (54 * d1 * (sq_xy)))
                                                    - cells[y_s * params.nx + x_w].speeds[5]); 
        tmp_cells[index].speeds[6] = cells[y_s * params.nx + x_e].speeds[6]
                                                  + params.omega
                                                  * (local_density * d1 * (1 - (u_x - u_y) * 3 + ((sq_xy) + 2 * (u_y * u_x)) * 162 * d1 - (54 * d1 * (sq_xy)))
                                                    - cells[y_s * params.nx + x_e].speeds[6]);
        tmp_cells[index].speeds[7] = cells[y_n * params.nx + x_e].speeds[7] 
                                                  + params.omega
                                                  * (local_density * d1 * (1 - (u_x + u_y) * 3 + ((sq_xy) - 2 * (u_y * u_x)) * 162 * d1 - (54 * d1 * (sq_xy)))
                                                    - cells[y_n * params.nx + x_e].speeds[7]);
        tmp_cells[index].speeds[8] = cells[y_n * params.nx + x_w].speeds[8]
                                                  + params.omega
                                                  * (local_density * d1 * (1 + (u_x - u_y) * 3 + ((sq_xy) + 2 * (u_y * u_x)) * 162 * d1 - (54 * d1 * (sq_xy)))
                                                    - cells[y_n * params.nx + x_w].speeds[8]);
