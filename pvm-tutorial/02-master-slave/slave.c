#include <stdio.h>
#include <unistd.h>
#include "pvm3.h"
#include "pvm_comm.h"

int main() {
    int id, tid, mstr_tid;
    char slv_name[SLV_NAME_SIZE];

    tid = pvm_mytid();
    gethostname(slv_name, SLV_NAME_SIZE);
    slv_name[SLV_NAME_SIZE-1] = '\0';

    pvm_recv(-1, MSG_MSTR);
    pvm_upkint(&mstr_tid, 1, 1);
    pvm_upkint(&id, 1, 1);
    printf("SLV %d otrzymal od MSTR(%d)\n", id, mstr_tid);

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&id, 1, 1);
    pvm_pkint(&tid, 1, 1);
    pvm_pkstr(slv_name);
    pvm_send(mstr_tid, MSG_SLV);

    pvm_exit();
    return 0;
}
