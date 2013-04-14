#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pvm3.h"

#include "comm.h"
#include "range.h"

#define SLV_NAME "slave"
#define SLV_NUM 3

void parse_args(const int argc, char *argv[], range_t **glob_rng);
void calculate_ranges(const range_t *glob_rng, const int slv_num, range_t ***slv_rngs);

int main(int argc, char *argv[])
{
    range_t *glob_rng;
    range_t **slv_rngs;
    double rslt, slv_rslt;

    int tid = pvm_mytid();
    // printf("MSTR: tid = %d\n", tid);

    int slv_tids[SLV_NUM];
    int slv_tid, slv_id;
    int actual_slv_num;

    int i;


    actual_slv_num = pvm_spawn(SLV_NAME, NULL, PvmTaskDefault, "", SLV_NUM, slv_tids);
    parse_args(argc, argv, &glob_rng);
    calculate_ranges(glob_rng, actual_slv_num, &slv_rngs);

    for (slv_id = 0; slv_id < actual_slv_num; slv_id++) {
        pvm_initsend(PvmDataDefault);
        pvm_pkint(&tid, 1, 1);
        pvm_pkint(&slv_id, 1, 1);
        pvm_pkdouble(&(slv_rngs[slv_id]->a), 1, 1);
        pvm_pkdouble(&(slv_rngs[slv_id]->b), 1, 1);
        pvm_pkint(&(slv_rngs[slv_id]->n), 1, 1);
        pvm_send(slv_tids[slv_id], MSG_MSTR);
        printf("MSTR wyslal do SLV %d o TID %d\n", slv_id, slv_tids[slv_id]);
    }

    rslt = 0.0;
    for (i = 0; i < actual_slv_num; i++) {
        pvm_recv(-1, MSG_SLV);
        pvm_upkint(&slv_id, 1, 1);
        pvm_upkint(&slv_tid, 1, 1);
        pvm_upkdouble(&slv_rslt, 1, 1);
        printf("MSTR otrzymal od SLV%d(%d) czesciowy wynik %lf\n", slv_id, slv_tid, slv_rslt);
        rslt += slv_rslt;
    }

    printf("*** Calkowity wynik %f ***\n", rslt);

    for (slv_id = 1; slv_id < actual_slv_num; slv_id++) {
        free(slv_rngs[slv_id]);
    }
    free(slv_rngs);
    free(glob_rng);
    pvm_exit();
    return EXIT_SUCCESS;
}

void parse_args(const int argc, char *argv[], range_t **glob_rng)
{
    if (argc != 4) {
        printf("Usage: %s <a> <b> <n>\n", argv[0]);
        pvm_exit();
        exit(EXIT_FAILURE);
    }

    *glob_rng = malloc(sizeof(**glob_rng));
    (*glob_rng)->a = atof(argv[1]);
    (*glob_rng)->b = atof(argv[2]);
    (*glob_rng)->n = atoi(argv[3]);
}

void calculate_ranges(const range_t *glob_rng, const int slv_num, range_t ***slv_rngs)
{
    int i;
    range_t *slv_rng;

    int points_per_job = round((double)glob_rng->n / slv_num);
    double h = (glob_rng->b - glob_rng->a) / glob_rng-> n;

    *slv_rngs = malloc(sizeof(**slv_rngs) * slv_num);
    for (i = 0; i < slv_num; i++) {
        (*slv_rngs)[i] = malloc(sizeof(***slv_rngs));
    }

    for (i = 0; i < slv_num; i++) {
        slv_rng = (*slv_rngs)[i];
        slv_rng->n = points_per_job;
        if (i == slv_num) {
            slv_rng->n = glob_rng->n - points_per_job * (slv_num-1);
        }
    }

    for (i = 0; i < slv_num; i++) {
        slv_rng = (*slv_rngs)[i];
        slv_rng->a = glob_rng->a + h * points_per_job * i;
        slv_rng->b = slv_rng->a + h * slv_rng->n;
    }

    for (i = 0; i < slv_num; i++) {
        slv_rng = (*slv_rngs)[i];
        printf("MSTR przydzielil SLV%d zakres od %lf do %lf z rozdzielczoscia %d\n", i, slv_rng->a, slv_rng->b, slv_rng->n);
    }
}

