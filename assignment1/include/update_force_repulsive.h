#ifndef UPDATE_FORCE_REPULSIVE_H
#define UPDATE_FORCE_REPULSIVE_H

void add_repulsive_force_idx(float diff_x, float diff_y, float F_repulsive[2], float dist, float eta, float phi_0);

void UpdateForceRepulsive(float F_repulsive[2], int nb_obs, int coor_obs[][4], float x, float x_past, float x_past_past, float y, float y_past, float y_past_past, float repulsion_param, float dist_threshold, float detection_threshold);

#endif