#include <cstdio>
#include <cassert>
#include <chuffed/core/options.h>
#include <chuffed/core/engine.h>
#include <chuffed/core/sat.h>
#include <chuffed/parallel/parallel.h>
#include <chuffed/mip/mip.h>
#include <chuffed/ldsb/ldsb.h>


void Engine::printStats() {
	if (so.thread_no != -1) return;

	auto total_time = std::chrono::duration_cast<duration>(chuffed_clock::now() - start_time);
	duration search_time = total_time - init_time;

	printf("%%%%%%mzn-stat: conflicts=%lld\n", conflicts);
	printf("%%%%%%mzn-stat: ewma_conflicts=%lld\n", ewma_conflicts);
	printf("%%%%%%mzn-stat: decisions=%lld\n",nodes); //number of decisions made
	//printf("%%%%%%mzn-stat: search_iterations=%lld\n",nodepath_len); //number of search iterations
	printf("%%%%%%mzn-stat: ewma_opennodes=%lld\n", ewma_opennodes); //change in num of open nodes
	printf("%%%%%%mzn-stat: vars=%d\n", vars.size() + sat.nVars()); // variables
	printf("%%%%%%mzn-stat: back_jumps=%lld\n", sat.back_jumps);
	printf("%%%%%%mzn-stat: ewma_back_jumps=%lld\n", sat.ewma_back_jumps);
 	printf("%%%%%%mzn-stat: solutions=%lld\n", solutions); // num of solutions found
	printf("%%%%%%mzn-stat: total_time=%.3f\n", to_sec(total_time));
	printf("%%%%%%mzn-stat: search_time=%.3f\n", to_sec(search_time));
	printf("%%%%%%mzn-stat: intVars=%d\n", vars.size()); // int variables
	printf("%%%%%%mzn-stat: propagations=%lld\n", propagations);
   	printf("%%%%%%mzn-stat: ewma_propagations=%lld\n", ewma_propagations);
	printf("%%%%%%mzn-stat: propagators=%d\n", propagators.size());
	printf("%%%%%%mzn-stat: boolVars=%d\n", sat.nVars()-2); //bool variables, Do not count constant True/False
	printf("%%%%%%mzn-stat: learnt=%d\n", sat.learnts.size()); // maybe nogoods
	printf("%%%%%%mzn-stat: bin=%d\n", sat.bin_clauses);        // num of clauses with size 2
   	printf("%%%%%%mzn-stat: tern=%d\n", sat.tern_clauses);    // num of clauses with size 3
	printf("%%%%%%mzn-stat: long=%d\n", sat.long_clauses);    // num of clauses with size > 3
	printf("%%%%%%mzn-stat: peak_depth=%d\n", peak_depth);
	printf("%%%%%%mzn-stat: decision_level_engine=%d\n", decisionLevel());
	printf("%%%%%%mzn-stat: ewma_decision_level_engine=%d\n", ewma_decision_level_engine);
	printf("%%%%%%mzn-stat: decision_level_sat=%d\n", sat.decisionLevel());
	printf("%%%%%%mzn-stat: ewma_decision_level_sat=%d\n", ewma_decision_level_sat);
	// only for non-SAT problems
	if (opt_var) {
		printf("%%%%%%mzn-stat: decision_level_mip=%d\n", mip->decisionLevel());
		printf("%%%%%%mzn-stat: ewma_decision_level_mip=%d\n", ewma_decision_level_mip);
        printf("%%%%%%mzn-stat: best_objective=%d\n", best_sol);
        printf("%%%%%%mzn-stat: ewma_best_objective=%d\n", ewma_best_sol);
    }
	else 
    {
		printf("%%%%%%mzn-stat: decision_level_mip=NaN\n");
		printf("%%%%%%mzn-stat: ewma_decision_level_mip=NaN\n");
        printf("%%%%%%mzn-stat: best_objective=NaN\n");
        printf("%%%%%%mzn-stat: ewma_best_objective=NaN\n");
    }

	printf("%%%%%%mzn-stat-end\n");

}

void Engine::checkMemoryUsage() {
	fprintf(stderr, "%d int vars, %d sat vars, %d propagators\n", vars.size(), sat.nVars(), propagators.size());
	fprintf(stderr, "%.2fMb memory usage\n", memUsed());

	fprintf(stderr, "Size of IntVars: %d %d %d\n", static_cast<int>(sizeof(IntVar)), static_cast<int>(sizeof(IntVarEL)), static_cast<int>(sizeof(IntVarLL)));
	fprintf(stderr, "Size of Propagator: %d\n", static_cast<int>(sizeof(Propagator)));

	long long var_mem = 0;
	for (int i = 0; i < vars.size(); i++) {
		var_mem += sizeof(IntVarLL);
/*
		var_mem += vars[i]->sz;
		if (vars[i]->getType() == INT_VAR_LL) {
			var_mem += 24 * ((IntVarLL*) vars[i])->ld.size();
		}
*/
	}
	fprintf(stderr, "%lld bytes used by vars\n", var_mem);

	long long prop_mem = 0;
	for (int i = 0; i < propagators.size(); i++) {
		prop_mem += sizeof(*propagators[i]);
	}
	fprintf(stderr, "%lld bytes used by propagators\n", prop_mem);
/*
	long long var_range_sum = 0;
	for (int i = 0; i < vars.size(); i++) {
		var_range_sum += vars[i]->max - vars[i]->min;
	}
	fprintf(stderr, "%lld range sum in vars\n", var_range_sum);
*/
	long long clause_mem = 0;
	for (int i = 0; i < sat.clauses.size(); i++) {
		clause_mem += sizeof(Lit) * sat.clauses[i]->size();
	}
	fprintf(stderr, "%lld bytes used by sat clauses\n", clause_mem);
/*
	int constants, hundred, thousand, large;
	constants = hundred = thousand = large = 0;
	for (int i = 0; i < vars.size(); i++) {
		int sz = vars[i]->max - vars[i]->min;
		if (sz == 0) constants++;
		else if (sz <= 100) hundred++;
		else if (sz <= 1000) thousand++;
		else large++;
	}
	fprintf(stderr, "Int sizes: %d %d %d %d\n", constants, hundred, thousand, large);
*/
}
