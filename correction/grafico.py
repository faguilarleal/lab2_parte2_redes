import matplotlib.pyplot as plt
from collections import defaultdict


#estan apuntados, porque eso de hacer graficos en c, está un poco hardcore a comparacion de python
errores_introducidos = [0, 1, 0, 3, 3, 5, 1, 2, 4]
detectados_y_corregidos = [0, 1, 0, 1, 1, 0, 1, 0, 0]

# agrupar por cantidad de errores
grupo = defaultdict(list)
for e, r in zip(errores_introducidos, detectados_y_corregidos):
    grupo[e].append(r)

# calcular promedio 
errores_unicos = sorted(grupo.keys())
tasa_promedio = [sum(grupo[e]) / len(grupo[e]) * 100 for e in errores_unicos]

plt.figure(figsize=(10, 6))
plt.plot(errores_unicos, tasa_promedio, marker='o', linestyle='-', color='blue', label='Hamming (promedio por error)')
for x, y in zip(errores_unicos, tasa_promedio):
    plt.text(x, y + 2, f"{y:.0f}%", ha='center', fontsize=9)

plt.title("Código Hamming - Tasa de éxito promedio por errores introducidos")
plt.xlabel("Errores introducidos")
plt.ylabel("Tasa de éxito (%)")
plt.grid(True)
plt.legend()
plt.ylim(-5, 105)
plt.tight_layout()
plt.show()
