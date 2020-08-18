riscvsim: riscv_sim_pipeline_framework.c stages.c utils.c executor.c pipeline.c cache.c 
	cc -O3 -std=gnu11  -c -o riscv_sim_pipeline_framework.o riscv_sim_pipeline_framework.c
	cc -O3 -std=gnu11 -c -o stages.o stages.c
	cc -O3 -std=gnu11 -c -o utils.o utils.c
	cc -O3 -std=gnu11 -c -o executor.o executor.c
	cc -O3 -std=gnu11 -c -o pipeline.o pipeline.c
	cc -O3 -std=gnu11 -c -o cache.o cache.c
	cc -o riscvsim riscv_sim_pipeline_framework.o stages.o utils.o executor.o pipeline.o cache.o

clean: 
	rm -f *.o riscvsim