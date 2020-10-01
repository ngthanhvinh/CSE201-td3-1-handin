#include <iostream>
#include "td3.hpp"
#include "support.hpp"
#include <stdlib.h>
#include <math.h>       // sin, cos

using namespace std;

using namespace support;

double* extend_array(double* array, int length, int new_size) {
  // IMPLEMENT YOUR FUNCTION HERE
	double* ret = new double[new_size];
	for (int i = 0; i < new_size; ++i) {
		if (i < length) ret[i] = array[i];
		else ret[i] = 0;
	}
	delete array;
	return ret;
}

double* shrink_array(double* array, int length, int new_size) {
	// IMPLEMENT YOUR FUNCTION HERE
	double* ret = new double[new_size];
	for (int i = 0; i < new_size; ++i)
		ret[i] = array[i];
	delete array;
	return ret;
}

double* append_to_array(double element,
                        double* array,
                        int &current_size,
                        int &max_size) {
  // IMPLEMENT YOUR FUNCTION HERE
	if (current_size == max_size) {
		array = extend_array(array, max_size, max_size + 5);
		max_size += 5;
	}
	array[current_size] = element;
	current_size++;
	return array;
}

double* remove_from_array(double* array,
                          int &current_size,
                          int &max_size) {
  // IMPLEMENT YOUR FUNCTION HERE
	if (current_size == 0) return array;
	current_size--;
	while (max_size - current_size >= 5) {
		array = shrink_array(array, max_size - 5, max_size);
		max_size -= 5;
	}
	return array;
}

bool simulate_projectile(const double magnitude, const double angle,
                         const double simulation_interval,
                         double *targets, int &tot_targets,
                         int *obstacles, int tot_obstacles,
                         double* &telemetry,
                         int &telemetry_current_size,
                         int &telemetry_max_size) {
  // YOU CAN MODIFY THIS FUNCTION TO RECORD THE TELEMETRY

  bool hit_target, hit_obstacle;
  double v0_x, v0_y, x, y, t;
  double PI = 3.14159265;
  double g = 9.8;

  v0_x = magnitude * cos(angle * PI / 180);
  v0_y = magnitude * sin(angle * PI / 180);

  t = 0;
  x = 0;
  y = 0;

  hit_target = false;
  hit_obstacle = false;
  while (y >= 0 && (! hit_target) && (! hit_obstacle)) {
		telemetry = append_to_array(t, telemetry, telemetry_current_size, telemetry_max_size);
		telemetry = append_to_array(x, telemetry, telemetry_current_size, telemetry_max_size);
		telemetry = append_to_array(y, telemetry, telemetry_current_size, telemetry_max_size);
    double * target_coordinates = find_collision(x, y, targets, tot_targets);
    if (target_coordinates != NULL) {
      remove_target(targets, tot_targets, target_coordinates);
      hit_target = true;
    } else if (find_collision(x, y, obstacles, tot_obstacles) != NULL) {
      hit_obstacle = true;
    } else {
      t = t + simulation_interval;
      y = v0_y * t  - 0.5 * g * t * t;
      x = v0_x * t;
    }
  }

  return hit_target;
}

void merge_telemetry(double **telemetries,
                     int tot_telemetries,
                     int *telemetries_sizes,
                     double* &telemetry,
                     int &telemetry_current_size,
                     int &telemetry_max_size) {
  // IMPLEMENT YOUR FUNCTION HERE
	int* telemetries_current = new int[tot_telemetries];
	std::fill(telemetries_current, telemetries_current + tot_telemetries, 0);

	while(true) {
		bool remain = false;
		double t = 1e9;
		for (int i = 0; i < tot_telemetries; ++i) {
			if (telemetries_current[i] < telemetries_sizes[i]) {
				remain = true;
				t = std::min(t, telemetries[i][telemetries_current[i]]);
			}
		}
		if (!remain) break;
		for (int i = 0; i < tot_telemetries; ++i) {
			if (telemetries_current[i] < telemetries_sizes[i] && telemetries[i][telemetries_current[i]] == t) {
				telemetry = append_to_array(telemetries[i][telemetries_current[i]], telemetry, telemetry_current_size, telemetry_max_size);
				telemetry = append_to_array(telemetries[i][telemetries_current[i] + 1], telemetry, telemetry_current_size, telemetry_max_size);
				telemetry = append_to_array(telemetries[i][telemetries_current[i] + 2], telemetry, telemetry_current_size, telemetry_max_size);
				telemetries_current[i] += 3;
			}
		}
	}
}
