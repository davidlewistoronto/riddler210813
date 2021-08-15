#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/format.hpp>

using namespace boost::multiprecision;
#pragma hdrstop
#pragma argsused

typedef boost::rational<cpp_int> t_die_prob;


#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#define max_level   12
#define n_die_sides 6

bool debug = false;

void solve_level (int level, t_die_prob *expected_score)
{   int n_combs;
	int die_roll [max_level];
	int i;
	int j;
	int iroll;
	int t;
	int ifreeze;
	int freeze_score;
	t_die_prob level_expected_score;
	t_die_prob roll_best_score;
	t_die_prob trial_score;

	n_combs = 1;
	for (i = 0; i < level; i++) {
		n_combs *= n_die_sides;
	}
	level_expected_score = 0;
	for (iroll = 0; iroll < n_combs; iroll++) {
		t = iroll;
		for (i = 0; i < level; i++) {
			die_roll [i] = (t % n_die_sides) + 1;
			if (debug) {
				printf (" %d", die_roll [i]);
			}
			t = t / n_die_sides;
		}

		/* sort the roll in decreasing order so we would freeze the first n values */
		for (i = 0; i < level; i++) {
			for (j = i + 1; j < level; j++) {
				if (die_roll [j] > die_roll [i]) {
					t = die_roll [i];
					die_roll [i] = die_roll [j];
					die_roll [j] = t;
				}
			}
		}

		/* find score freezing 1 to level of the die, highest values first */
		roll_best_score = 0;
		freeze_score = 0;
		for (ifreeze = 1; ifreeze <= level; ifreeze++) {
			freeze_score += die_roll [ifreeze - 1];
			trial_score = freeze_score + expected_score [level - ifreeze];
			if (debug) {
				printf (" %d: ", ifreeze);
				std::cout << trial_score;
			}
			if (trial_score > roll_best_score) {
				roll_best_score = trial_score;
			}

		}
		if (debug) {
			printf (" best ");
			std::cout << roll_best_score;
			printf ("\n");
		}
		level_expected_score += roll_best_score;
	}
	level_expected_score /= n_combs;

	printf ("level %d ", level);
	std::cout << level_expected_score;
	printf ("\n");

	expected_score [level] = level_expected_score;

}

int _tmain(int argc, _TCHAR* argv[]) 
{   int n_dice;
	t_die_prob *expected_score;
	int n_levels;
    int i;

	expected_score = new t_die_prob [max_level + 1];

	for (i = 0; i <= max_level; i++) {
		expected_score [i] = 0;
	}
	sscanf (argv [1], "%d", &n_levels);
	for (i = 1; i <= n_levels; i++) {
		solve_level (i, expected_score);
	}


	return 0;
}
