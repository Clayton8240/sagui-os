/* data - data e hora do relógio da placa-mãe. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    static const char *semana[] = { "domingo", "segunda-feira", "terça-feira", "quarta-feira",
                                    "quinta-feira", "sexta-feira", "sábado" };
    data_t d;
    if (ler_data(&d) < 0) return 1;
    imprimir("%s, %02u/%02u/%u %02u:%02u:%02u\n", semana[d.dia_semana % 7], d.dia, d.mes, d.ano,
             d.hora, d.minuto, d.segundo);
    return 0;
}
