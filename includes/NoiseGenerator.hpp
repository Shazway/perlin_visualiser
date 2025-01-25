/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NoiseGenerator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmoragli <tmoragli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 22:49:04 by tmoragli          #+#    #+#             */
/*   Updated: 2025/01/25 19:53:39 by tmoragli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "perlin.hpp"
#include <cmath>
#include <numeric>
#include <random>
#include <algorithm>

struct NoiseData {
	size_t octaves = 4;
	double lacunarity = 2.0;
	double persistence = 0.4;
	double frequency = 0.5;
	double amplitude = 1.0;
};

class NoiseGenerator {
	private:
		std::vector<int> permutation;

		double grad(int hash, double x, double y) const;
		double fade(double t) const;
		double lerp(double t, double a, double b) const;
		NoiseData _data;

	public:
		NoiseGenerator(unsigned int seed = 0);
		vec2 getBorderWarping(double x, double z);

		// Generate a single layer of noise
		double singleNoise(double x, double y) const;

		// Generate fractal noise with octaves
		double noise(double x, double y) const;

		void setNoiseData(const NoiseData &data);
};

