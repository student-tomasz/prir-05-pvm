#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "pvm3.h"

#include "comm.h"
#include "range.h"

double integrate(double (*f)(double), const double a, const double b, const int n);

int main() {
    range_t *rng = malloc(sizeof(*rng));

    int id, tid, mstr_tid;
    tid = pvm_mytid();
    double rslt;


    pvm_recv(-1, MSG_MSTR);
    pvm_upkint(&mstr_tid, 1, 1);
    pvm_upkint(&id, 1, 1);
    pvm_upkdouble(&(rng->a), 1, 1);
    pvm_upkdouble(&(rng->b), 1, 1);
    pvm_upkint(&(rng->n), 1, 1);
    printf("SLV%d(%d) otrzymal od MSTR(%d) zakres od %lf do %lf z rozdzielczoscia %d\n", id, tid, mstr_tid, rng->a, rng->b, rng->n);

    rslt = integrate(&sin, rng->a, rng->b, rng->n);

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&id, 1, 1);
    pvm_pkint(&tid, 1, 1);
    pvm_pkdouble(&rslt, 1, 1);
    pvm_send(mstr_tid, MSG_SLV);
    printf("SLV%d(%d) wyslal do MSTR(%d) czesciowy wynik %lf\n", id, tid, mstr_tid, rslt);

    pvm_exit();
    return EXIT_SUCCESS;
}

double integrate(double (*f)(double), const double a, const double b, const int n)
{
    double rslt = 0.0;
    double h = (b - a) / n;

    int i;
    for (i = 0; i < n; i++) {
        rslt += f(a + h*i + h/2) * h;
    }

    return rslt;
}
