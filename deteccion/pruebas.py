import random
import matplotlib.pyplot as plt
import pandas as pd
from crc32 import emitir_crc32, verificar_crc32

# -----------------------------------
# Funciones auxiliares
# -----------------------------------

def generar_mensaje_aleatorio(n_bits: int) -> str:
    """Genera un mensaje binario aleatorio de n bits."""
    return ''.join(random.choice(['0', '1']) for _ in range(n_bits))

def añadir_ruido_por_probabilidad(mensaje: str, prob: float, solo_mensaje=True) -> str:
    """Introduce errores con probabilidad en los bits del mensaje."""
    mensaje = list(mensaje)
    limite = len(mensaje) - 32 if solo_mensaje else len(mensaje)

    for i in range(limite):
        if random.random() < prob:
            mensaje[i] = '1' if mensaje[i] == '0' else '0'
    return ''.join(mensaje)

# -----------------------------------
# Función principal de prueba para CRC
# -----------------------------------

resultados = []

def realizar_prueba_crc(tamaño_bits, prob_error, repeticiones=100):
    exitos = 0
    errores = 0
    overhead = 0

    for _ in range(repeticiones):
        # 1. Crear mensaje binario
        mensaje = generar_mensaje_aleatorio(tamaño_bits)

        # 2. Codificar mensaje con CRC
        mensaje_codificado = emitir_crc32(mensaje)
        overhead = len(mensaje_codificado) - len(mensaje)  # bits extra

        # 3. Añadir ruido al mensaje (no al CRC)
        mensaje_con_ruido = añadir_ruido_por_probabilidad(mensaje_codificado, prob_error, solo_mensaje=True)

        # 4. Verificar
        if verificar_crc32(mensaje_con_ruido):
            exitos += 1
        else:
            errores += 1

    tasa_exito = exitos / repeticiones * 100
    resultados.append({
        'algoritmo': 'CRC32',
        'tamano': tamaño_bits,
        'prob_error': prob_error,
        'overhead': overhead,
        'exitos': exitos,
        'errores': errores,
        'tasa_exito': tasa_exito
    })


for tamaño in [8, 16, 32, 64, 128]:
    for prob_error in [0.0, 0.05, 0.1, 0.2]:
        realizar_prueba_crc(tamaño_bits=tamaño, prob_error=prob_error)


def graficar_resultados_crc(resultados):
    df = pd.DataFrame(resultados)
    for tamaño in df['tamano'].unique():
        subset = df[df['tamano'] == tamaño]
        plt.plot(subset['prob_error'], subset['tasa_exito'], marker='o', label=f'{tamaño} bits')

    plt.title("CRC32 - Tasa de éxito vs Probabilidad de error")
    plt.xlabel("Probabilidad de error")
    plt.ylabel("Tasa de éxito (%)")
    plt.ylim(0, 105)
    plt.grid(True)
    plt.legend(title="Tamaño del mensaje")
    plt.show()

# Mostrar gráfica
graficar_resultados_crc(resultados)
