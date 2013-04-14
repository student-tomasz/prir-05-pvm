#include <stdlib.h>
#include <stdio.h>
#include "pvm3.h"
#include "pvm_comm.h"

int main()
{
    int tid;
    int actual_slv_num;

    int slv_tids[SLV_NUM];
    int slv_tid, slv_id;
    char slv_name[SLV_NAME_SIZE];

    int i;

    tid = pvm_mytid();
    printf("MSTR: tid = %d\n", tid);

    actual_slv_num = pvm_spawn(SLV_NAME, NULL, PvmTaskDefault, "", SLV_NUM, slv_tids);
    for (slv_id = 0; slv_id < actual_slv_num; slv_id++) {
        pvm_initsend(PvmDataDefault);
        pvm_pkint(&tid, 1, 1);
        pvm_pkint(&slv_id, 1, 1);
        pvm_send(slv_tids[slv_id], MSG_MSTR);
        printf("MSTR wyslal do SLV %d o TID %d\n", slv_id, slv_tids[slv_id]);
    }

    for (i = 0; i < actual_slv_num; i++) {
        pvm_recv(-1, MSG_SLV);
        pvm_upkint(&slv_id, 1, 1);
        pvm_upkint(&slv_tid, 1, 1);
        pvm_upkstr(slv_name);
        printf("MSTR otrzymal od SLV %d o TID %d i nazwie %s\n", slv_id, slv_tid, slv_name);
    }

    pvm_exit();
    return 0;
}
