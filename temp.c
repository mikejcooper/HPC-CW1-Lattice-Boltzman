

       

        cells[index].speeds[0] = tmp_cells[index].speeds[0]
                                                  + params.omega
                                                  ((4/9.0) * local_density * (1.0 - u_sq * 1.5)
                                                    - tmp_cells[index].speeds[0]);
        cells[index].speeds[1] = tmp_cells[index].speeds[1]
                                                  + params.omega
                                                  * ((1/9.0) * local_density * (1.0 + u_x * 3.0 + (u_x * u_x) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[1]);
        cells[index].speeds[2] = tmp_cells[index].speeds[2]
                                                  + params.omega
                                                  * ((1/9.0) * local_density * (1.0 + u_y * 3.0 + (u_y * u_y) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[2]);
        cells[index].speeds[3] = tmp_cells[index].speeds[3]
                                                  + params.omega
                                                  * ((1/9.0) * local_density * (1.0 + - u_x * 3.0 + (- u_x * - u_x) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[3]);
        cells[index].speeds[4] = tmp_cells[index].speeds[4]
                                                  + params.omega
                                                  * ((1/9.0) * local_density * (1.0 + - u_y * 3.0 + (- u_y * - u_y) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[4]);
        cells[index].speeds[5] = tmp_cells[index].speeds[5]
                                                  + params.omega
                                                  * ((1/36) * local_density * (1.0 + (u_x + u_y) * 3.0 + ((u_x + u_y) * (u_x + u_y)) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[5]);
        cells[index].speeds[6] = tmp_cells[index].speeds[6]
                                                  + params.omega
                                                  * ((1/36) * local_density * (1.0 + (- u_x + u_y) * 3.0 + ((- u_x + u_y) * (- u_x + u_y)) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[6]);
        cells[index].speeds[7] = tmp_cells[index].speeds[7]
                                                  + params.omega
                                                  * ((1/36) * local_density * (1.0 + (- u_x - u_y) * 3.0 + ((- u_x - u_y) * (- u_x - u_y)) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[7]);
        cells[index].speeds[8] = tmp_cells[index].speeds[8]
                                                  + params.omega
                                                  * ((1/36) * local_density * (1.0 + (u_x - u_y) * 3.0 + ((u_x - u_y) * (u_x - u_y)) * 0.5 * 9.0 - (1.5 * u_sq))
                                                    - tmp_cells[index].speeds[8]);






