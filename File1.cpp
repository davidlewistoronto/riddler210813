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

//typedef double t_die_prob;
typedef boost::rational<cpp_int> t_die_prob;


#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#define max_level_detailed   6
#define max_level   120
#define n_die_sides 6

#define n_combin_table 201


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


t_die_prob combin_table [n_combin_table] [n_combin_table];
t_die_prob fact_table [n_combin_table];

void init_combin_table (void)
{	int i, j;
	combin_table [0] [0] = 1;
	fact_table [0] = 1;
	for (i = 1; i < n_combin_table; i++)
	{	fact_table [i] = fact_table [i - 1] * i;
		combin_table [i] [0] = 1;
		for (j = 1; j <= i; j++)
		{	combin_table [i] [j] = combin_table [i - 1] [j - 1] + combin_table [i - 1] [j];
		}
	}
}


void add_die_roll_parts (int *roll_stack, int level, t_die_prob *expected_score)
{	int i;
	int j;
	int k;
	int die_roll [max_level];
	int ifreeze;
	int freeze_score;
	t_die_prob c;
	t_die_prob roll_best_score;
	t_die_prob trial_score;

	/* flatten the roll stack into a list of sorted rolls */
	k = 0;
	for (i = 0; i < n_die_sides; i++) {
		for (j = 0; j < roll_stack [i]; j++) {
			die_roll [k++] = n_die_sides - i;
		}
	}
	/* find number of combinations of this partition */

	c = fact_table [level];
	for (i = 0; i < n_die_sides; i++) {
		c /= fact_table [roll_stack [i]];
	}
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
	expected_score [level] += roll_best_score * c;

}
void solve_level_recur (int n_dice, int n_bins, int *roll_stack, int depth, int level, t_die_prob *expected_score)
{	int i;

	if (n_bins == 1) {
		roll_stack [depth] = n_dice;
		add_die_roll_parts (roll_stack, level, expected_score);
	} else {
		for (i = 0; i <= n_dice; i++) {
			roll_stack [depth] = i;
			solve_level_recur (n_dice - i, n_bins - 1, roll_stack, depth + 1, level, expected_score);
		}
	}

}

void solve_level_fast (int level, t_die_prob *expected_score)
{   int i;
	int roll_stack [n_die_sides];

	solve_level_recur (level, n_die_sides, roll_stack, 0, level, expected_score);

    /* scale the result by the number of possible rolls */
	for (i = 0; i < level; i++) {
		expected_score [level] /= n_die_sides;
	}
	printf ("fast level %d ", level);
	std::cout << expected_score [level];
	printf ("\n");




}

bool debug_detailed = false;

int _tmain(int argc, _TCHAR* argv[]) 
{   int n_dice;
	t_die_prob *expected_score;
	t_die_prob *expected_score_fast;
	int n_levels;
	int i;

	expected_score = new t_die_prob [max_level + 1];
	expected_score_fast = new t_die_prob [max_level + 1];
	init_combin_table ();

	for (i = 0; i <= max_level; i++) {
		expected_score [i] = 0;
		expected_score_fast [i] = 0;
	}
	sscanf (argv [1], "%d", &n_levels);
	for (i = 1; i <= n_levels; i++) {
		if (debug_detailed && i <= max_level_detailed) {
			solve_level (i, expected_score);
		}
		solve_level_fast (i, expected_score_fast);
	}


	return 0;
}
