#include "stdlib.h"
#include "stdio.h"
#include <time.h>

/* velocity squared */
        double c_sq = (u_x * u_x + u_y * u_y);
        double e1 =  (c_sq * c_sq + c_sq * c_sq);
        double e2 = ((u_x * u_x + u_y * u_y) / (c_sq + c_sq));

       

        cells[index].speeds[0] = tmp_cells[index].speeds[0]
                                                  + params.omega
                                                  * (w0 * local_density * (1.0 - (u_x * u_x + u_y * u_y) / (c_sq + c_sq))
                                                    - tmp_cells[index].speeds[0]);
        cells[index].speeds[1] = tmp_cells[index].speeds[1]
                                                  + params.omega
                                                  * (w1 * local_density * (1.0 + u_x / c_sq + (u_x * u_x) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[1]);
        cells[index].speeds[2] = tmp_cells[index].speeds[2]
                                                  + params.omega
                                                  * (w1 * local_density * (1.0 + u_y / c_sq + (u_y * u_y) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[2]);
        cells[index].speeds[3] = tmp_cells[index].speeds[3]
                                                  + params.omega
                                                  * (w1 * local_density * (1.0 + - u_x / c_sq + (- u_x * - u_x) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[3]);
        cells[index].speeds[4] = tmp_cells[index].speeds[4]
                                                  + params.omega
                                                  * (w1 * local_density * (1.0 + - u_y / c_sq + (- u_y * - u_y) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[4]);
        cells[index].speeds[5] = tmp_cells[index].speeds[5]
                                                  + params.omega
                                                  * (w2 * local_density * (1.0 + (u_x + u_y) / c_sq + ((u_x + u_y) * (u_x + u_y)) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[5]);
        cells[index].speeds[6] = tmp_cells[index].speeds[6]
                                                  + params.omega
                                                  * (w2 * local_density * (1.0 + (- u_x + u_y) / c_sq + ((- u_x + u_y) * (- u_x + u_y)) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[6]);
        cells[index].speeds[7] = tmp_cells[index].speeds[7]
                                                  + params.omega
                                                  * (w2 * local_density * (1.0 + (- u_x - u_y) / c_sq + ((- u_x - u_y) * (- u_x - u_y)) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[7]);
        cells[index].speeds[8] = tmp_cells[index].speeds[8]
                                                  + params.omega
                                                  * (w2 * local_density * (1.0 + (u_x - u_y) / c_sq + ((u_x - u_y) * (u_x - u_y)) / (c_sq * c_sq + c_sq * c_sq) - ((u_x * u_x + u_y * u_y) / (c_sq + c_sq)))
                                                    - tmp_cells[index].speeds[8]);