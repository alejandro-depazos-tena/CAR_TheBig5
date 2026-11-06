#!/usr/bin/env python3
"""Tablas derivadas de los ejercicios 2 a 6, calculadas SOLO a partir de los
CSV de resultados/ (no se escribe ningún número a mano).

Uso, desde Laboratorio1:
    python3 src/analisis_lab1.py ej2|ej3|ej4|ej5|ej6|resumen   (texto)
    python3 src/analisis_lab1.py markdown > docs/tablas_ej2_ej6.md
    python3 src/analisis_lab1.py refrescar-registro   (recalcula las tablas
        derivadas dentro de resultados/salidas_ejercicios_2_en_adelante.txt
        sobre los mismos CSV, sin volver a medir, y lo anota al final)

'resumen' comprueba todas las verificaciones y termina con código 1 si alguna
falla. Las líneas '#' de los CSV llevan las condiciones de la medida
(N, pasos, repeticiones, criterio, T1...) y se leen como clave=valor.
"""

import csv
import os
import sys
from pathlib import Path

RES = Path(__file__).resolve().parents[1] / "resultados"
HILOS_LOGICOS = None  # se rellena con os.cpu_count() en main
MODO = "txt"          # "txt" o "md"


# ----------------------------------------------------------------- lectura
def leer(ruta):
    with open(ruta, encoding="utf-8-sig") as f:
        lineas = [l for l in f if l.strip() and not l.startswith("#")]
    return list(csv.DictReader(lineas))


def meta(ruta):
    datos = {}
    with open(ruta, encoding="utf-8-sig") as f:
        for linea in f:
            if linea.startswith("#"):
                for trozo in linea[1:].split():
                    if "=" in trozo:
                        k, v = trozo.split("=", 1)
                        datos.setdefault(k, v)
    return datos


def existe(ruta):
    return ruta.exists() and ruta.stat().st_size > 0


# ----------------------------------------------------------------- salida
def titulo(texto, nivel=2):
    if MODO == "md":
        print(f"\n{'#' * (nivel + 1)} {texto}\n")
    else:
        print(f"\n--- {texto} ---")


def parrafo(texto):
    print(texto if MODO == "txt" else texto + "\n")


def tabla(cabecera, filas):
    filas = [[str(c) for c in f] for f in filas]
    if MODO == "md":
        print("| " + " | ".join(cabecera) + " |")
        print("|" + "|".join("---:" for _ in cabecera) + "|")
        for f in filas:
            print("| " + " | ".join(f) + " |")
        print()
        return
    anchos = [max(len(c), *(len(f[i]) for f in filas)) for i, c in enumerate(cabecera)]
    print("  ".join(c.rjust(a) for c, a in zip(cabecera, anchos)))
    print("  ".join("-" * a for a in anchos))
    for f in filas:
        print("  ".join(c.rjust(a) for c, a in zip(f, anchos)))


def pct(x):
    return f"{100 * x:.1f} %"


# ---------------------------------------------------------- Ejercicio 2
def ej2():
    titulo("Ejercicio 2 · verificación numérica (tolerancia 1e-9, declarada antes de medir)")
    for n in (2048, 256):
        ruta = RES / "ejercicio2" / f"verificacion_{n}.csv"
        if not existe(ruta):
            parrafo(f"(falta {ruta.name})")
            continue
        filas = leer(ruta)
        m = meta(ruta)
        seq = next(f for f in filas if f["tipo"] == "secuencial")
        jac = [f for f in filas if f["tipo"] == "jacobi_omp"]
        ins = [f for f in filas if f["tipo"] == "in_situ"]
        ok = sum(f["correcto"] == "1" for f in jac)
        bit = sum(f["bit_a_bit"] == "1" for f in jac)
        reales = sum(f["hilos_pedidos"] == f["hilos_reales"] for f in jac)
        titulo(f"N={m.get('N')}, {m.get('pasos')} pasos (fichero {ruta.name})", 3)
        parrafo(f"Jacobi OpenMP: {ok}/{len(jac)} recuentos de hilos pasan la verificación; "
                f"{bit}/{len(jac)} salen idénticos bit a bit; hilos reales = pedidos en "
                f"{reales}/{len(jac)}. Mayor diferencia celda a celda: "
                f"{max(float(f['max_diff']) for f in jac):.3e}; mayor diferencia relativa "
                f"del checksum: {max(float(f['dif_checksum_rel']) for f in jac):.3e}; "
                f"residuo idéntico al secuencial en "
                f"{sum(f['residuo'] == seq['residuo'] for f in jac)}/{len(jac)}.")
        rechazadas = sum(f["correcto"] == "0" for f in ins)
        por_p = {}
        for f in ins:
            por_p.setdefault(int(f["hilos_pedidos"]), []).append(f)
        iguales = sum(len({g["checksum"] for g in v}) == 1 for v in por_p.values())
        distintos = len({v[0]["checksum"] for v in por_p.values()})
        parrafo(f"Fallo provocado (in situ): rechazado en {rechazadas}/{len(ins)} ejecuciones; "
                f"{distintos} checksums distintos entre los {len(por_p)} recuentos de hilos; "
                f"las dos ejecuciones con los mismos hilos coinciden en {iguales}/{len(por_p)}.")
        cs_seq = float(seq["checksum"])
        filas_t = []
        anterior = None
        for p in sorted(por_p):
            e = por_p[p]
            cs1 = e[0]["checksum"]
            cs2 = e[1]["checksum"] if len(e) > 1 else "-"
            cambia = "-" if anterior is None else ("sí" if cs1 != anterior else "no")
            filas_t.append([p, cs1, cs2, "sí" if cs1 == cs2 else "no",
                            f"{(float(cs1) - cs_seq) / cs_seq:.4e}", e[0]["max_diff"], cambia])
            anterior = cs1
        tabla(["hilos", "checksum in situ (1)", "checksum in situ (2)", "1 = 2",
               "dif. rel. vs secuencial", "max_diff", "cambia vs p anterior"], filas_t)


# ---------------------------------------------------------- Ejercicio 3
def amdahl_f(puntos):
    """Ajuste por mínimos cuadrados de 1/S = f + (1-f)/p, es decir
    y = 1/S - 1/p = f * (1 - 1/p) = f * x."""
    num = sum((1 - 1 / p) * (1 / s - 1 / p) for p, s in puntos)
    den = sum((1 - 1 / p) ** 2 for p, _ in puntos)
    return num / den if den else float("nan")


def stream_max():
    ruta = RES / "ejercicio6" / "stream.csv"
    if not existe(ruta):
        return None
    filas = leer(ruta)
    mejor = max(filas, key=lambda f: float(f["GB_s_24B"]))
    uno = next(f for f in filas if f["hilos"] == "1")
    return {"hilos": int(mejor["hilos"]), "24": float(mejor["GB_s_24B"]),
            "32": float(mejor["GB_s_32B"]), "uno24": float(uno["GB_s_24B"]),
            "uno32": float(uno["GB_s_32B"])}


def ej3():
    titulo("Ejercicio 3 · escalabilidad fuerte")
    st = stream_max()
    for n in (2048, 256):
        ruta = RES / "ejercicio3" / f"fuerte_{n}.csv"
        if not existe(ruta):
            parrafo(f"(falta {ruta.name})")
            continue
        filas = leer(ruta)
        m = meta(ruta)
        nm = int(m["N"])
        pasos = int(m["pasos"])
        t1 = float(m["T1_secuencial"])
        titulo(f"N={nm}, {pasos} pasos, mínimo de {m['repeticiones']} repeticiones "
               f"(fichero {ruta.name})", 3)
        seq = RES / "ejercicio3" / f"t1_sin_openmp_{n}.csv"
        texto = (f"T1 = {t1:.4f} s (versión secuencial sin pragmas en el mismo proceso, "
                 f"mínimo de las rondas; la peor ronda dio {float(m['T1_peor']):.4f} s).")
        if existe(seq):
            ms = meta(seq)
            t_nofopenmp = float(ms["T1_minimo"])
            texto += (f" Binario compilado SIN -fopenmp (p1_seq): mínimo {t_nofopenmp:.4f} s, "
                      f"mediana {float(ms['T1_mediana']):.4f} s, máximo {float(ms['T1_maximo']):.4f} s "
                      f"(dispersión máx/mín {ms['dispersion_max_min']}); la T1 de la tabla difiere "
                      f"de su mínimo un {100 * (t1 / t_nofopenmp - 1):+.1f} %.")
        parrafo(texto)
        t_1hilo = float(filas[0]["tiempo_s"])
        parrafo(f"Prueba de humo (V.I): T(1 hilo)/T1 = {t_1hilo / t1:.3f}.")
        tab = []
        for f in filas:
            p = int(f["hilos"])
            s = float(f["speedup_abs"])
            techo = float(f["techo_speedup"])
            kf = (1 / s - 1 / p) / (1 - 1 / p) if p > 1 else float("nan")
            paso = float(f["s_por_paso"])
            vacio = float(f["region_vacia_s"])
            tab.append([p, f["hilos_reales"], f"{float(f['tiempo_s']):.4f}",
                        f"{float(f['tiempo_peor_s']) / float(f['tiempo_s']):.2f}", f"{s:.3f}",
                        f"{float(f['speedup_rel']):.3f}", pct(float(f["eficiencia"])),
                        f"{techo:.2f}", pct(s / techo), "-" if p == 1 else f"{kf:.3f}",
                        f"{paso * 1e3:.4f}", f"{vacio * 1e6:.2f}", pct(vacio / paso),
                        "sí" if f["checksum_ok"] == "1" else "NO"])
        tabla(["hilos", "reales", "T (s)", "peor/mejor", "S abs", "S rel", "E abs",
               "techo cálc.", "S/techo", "Karp-Flatt", "ms/paso", "paso vacío (µs)",
               "vacío/paso", "verif."], tab)
        mejor = max(filas, key=lambda f: float(f["speedup_abs"]))
        parrafo(f"Speedup absoluto máximo: {float(mejor['speedup_abs']):.3f} con "
                f"{mejor['hilos']} hilos. Hilos lógicos de la máquina: {HILOS_LOGICOS} "
                f"(por encima hay sobresuscripción).")
        # Solo con menos hilos que CPU lógicas: con todas ocupadas (o más) el
        # tiempo lo domina otro mecanismo (la sincronización con CPU que el
        # sistema también necesita), que Amdahl no modela.
        puntos = [(int(f["hilos"]), float(f["speedup_abs"])) for f in filas
                  if 1 < int(f["hilos"]) < (HILOS_LOGICOS or 12)]
        f_ams = amdahl_f(puntos)
        tab = []
        for f in filas:
            p = int(f["hilos"])
            tab.append([p, f"{float(f['speedup_abs']):.3f}",
                        f"{1 / (f_ams + (1 - f_ams) / p):.3f}"])
        parrafo(f"Amdahl: fracción serie ajustada por mínimos cuadrados con "
                f"{', '.join(str(p) for p, _ in puntos)} hilos (menos que las {HILOS_LOGICOS} "
                f"CPU lógicas): f = {f_ams:.3f} (límite 1/f = {1 / f_ams:.2f}). Compárese con "
                f"la columna Karp-Flatt: si Amdahl describiera la curva, sería constante.")
        tabla(["hilos", "S medido", "S Amdahl con f ajustada"], tab)
        if st and n == 2048:
            bw_seq24 = 24 * (nm - 2) ** 2 / (t1 / pasos) / 1e9
            bw_seq16 = 16 * (nm - 2) ** 2 / (t1 / pasos) / 1e9
            parrafo(f"Techo de ancho de banda: la secuencial mueve {bw_seq16:.2f} GB/s (16 B/celda) "
                    f"o {bw_seq24:.2f} GB/s (24 B/celda, con la lectura previa de la línea escrita). "
                    f"STREAM: 1 hilo {st['uno24']:.2f}/{st['uno32']:.2f} GB/s, máximo "
                    f"{st['24']:.2f}/{st['32']:.2f} GB/s (modelos 24/32 B) con {st['hilos']} hilos. "
                    f"Speedup máximo que permite la memoria si el programa estuviera limitado solo "
                    f"por ella: {st['32']:.2f}/{bw_seq24:.2f} = {st['32'] / bw_seq24:.2f} "
                    f"(modelos con lectura previa) o {st['24'] / bw_seq16:.2f} (modelos sin ella).")
            tab = []
            for f in filas:
                paso = float(f["s_por_paso"])
                bw16 = 16 * (nm - 2) ** 2 / paso / 1e9
                bw24 = 24 * (nm - 2) ** 2 / paso / 1e9
                tab.append([f["hilos"], f"{bw16:.2f}", pct(bw16 / st["24"]), f"{bw24:.2f}",
                            pct(bw24 / st["32"])])
            tabla(["hilos", "GB/s (16 B)", "% STREAM 24 B", "GB/s (24 B)", "% STREAM 32 B"], tab)


# ---------------------------------------------------------- Ejercicio 4
def ej4():
    titulo("Ejercicio 4 · escalabilidad débil")
    ruta = RES / "ejercicio4" / "debil.csv"
    if not existe(ruta):
        parrafo("(falta debil.csv)")
        return
    filas = leer(ruta)
    m = meta(ruta)
    parrafo(f"N1 = {m['N1']}, {m['pasos']} pasos, mínimo de {m['repeticiones']} repeticiones. "
            f"N(p) = round(N1·√p). Eficiencia débil = T(1 hilo, N1)/T(p hilos, N(p)).")
    st = stream_max()
    pasos = int(m["pasos"])
    tab = []
    ns = []
    for f in filas:
        n = int(f["N"])
        ns.append(float(f["ns_celda_paso_seq"]))
        t = float(f["tiempo_s"])
        # Tiempo mínimo si el paso solo estuviera limitado por la memoria:
        # bytes reales (24 B por celda interior y paso) / máximo de STREAM (32 B).
        t_mem = 24 * (n - 2) ** 2 * pasos / (st["32"] * 1e9) if st else None
        tab.append([f["hilos"], f["hilos_reales"], n, f["celdas_por_hilo"],
                    f"{float(f['MB_dos_buffers']):.1f}", f"{t:.4f}",
                    f"{float(f['tiempo_peor_s']) / t:.2f}",
                    pct(float(f["eficiencia_debil"])),
                    f"{t_mem:.4f}" if t_mem else "-", f"{t / t_mem:.2f}" if t_mem else "-",
                    f"{float(f['tiempo_seq_mismo_N_s']):.4f}",
                    f"{float(f['ns_celda_paso_seq']):.3f}", f"{float(f['speedup_abs_mismo_N']):.3f}",
                    f["max_diff"], "sí" if f["correcto"] == "1" else "NO"])
    tabla(["hilos", "reales", "N", "celdas/hilo", "MB 2 buffers", "T (s)", "peor/mejor",
           "E débil", "T mín. memoria (s)", "T / T mín.", "T sec. mismo N (s)",
           "ns/celda·paso sec.", "S abs mismo N", "max_diff", "verif."], tab)
    if st:
        parrafo(f"T mín. memoria = 24 B × (N-2)² × pasos / {st['32']:.2f} GB/s (máximo de STREAM "
                f"con el mismo criterio de bytes reales): lo que tardaría el paso si solo lo "
                f"limitara el ancho de banda de la máquina.")
    grandes = [x for f, x in zip(filas, ns) if int(f["N"]) >= 2048]
    if grandes:
        parrafo(f"Coste secuencial por celda y paso para N >= 2048 (dos buffers de 64 MB o "
                f"más, fuera de cualquier caché de esta clase de máquina): "
                f"mín {min(grandes):.3f}, máx {max(grandes):.3f} ns "
                f"(variación {100 * (max(grandes) / min(grandes) - 1):.1f} %); "
                f"N = {filas[0]['N']}: {ns[0]:.3f} ns.")
    cpp = [int(f["celdas_por_hilo"]) for f in filas]
    parrafo(f"Celdas por hilo tras redondear N: de {min(cpp)} a {max(cpp)} "
            f"(desviación máxima {100 * (max(cpp) / min(cpp) - 1):.3f} %).")


# ---------------------------------------------------------- Ejercicio 5
RUTAS_EJ5 = (("configuración pedida", "planificaciones.csv"),
             ("complementaria, 8 hilos", "planificaciones_8hilos.csv"))


def ej5():
    titulo("Ejercicio 5 · planificaciones")
    for etiqueta, nombre in RUTAS_EJ5:
        ruta = RES / "ejercicio5" / nombre
        if not existe(ruta):
            parrafo(f"(falta {nombre})")
            continue
        ej5_tabla(ruta, etiqueta)


def ej5_tabla(ruta, etiqueta):
    filas = leer(ruta)
    m = meta(ruta)
    titulo(f"{etiqueta}: N={m['N']}, {m['pasos']} pasos, {m['hilos']} hilos "
           f"(reales {m.get('hilos_reales')}), mínimo de {m['repeticiones']} rondas "
           f"(fichero {ruta.name})", 3)
    parrafo(f"T1 = {float(m['T1_secuencial']):.4f} s (peor ronda {float(m['T1_peor']):.4f} s).")
    t = {(f["schedule"], int(f["chunk"])): float(f["tiempo_s"]) for f in filas}
    base = t[("static", 0)]
    orden = sorted(filas, key=lambda f: float(f["tiempo_s"]))
    puesto = {(f["schedule"], f["chunk"]): i + 1 for i, f in enumerate(orden)}
    tab = []
    for f in filas:
        nombre = f["schedule"] + ("" if f["chunk"] == "0" else f",{f['chunk']}")
        tf = float(f["tiempo_s"])
        tab.append([nombre, f"{tf:.4f}", f"{float(f['tiempo_peor_s']) / tf:.2f}",
                    f"{float(f['speedup_abs']):.3f}", f"{tf / base:.3f}",
                    puesto[(f["schedule"], f["chunk"])], f["max_diff"],
                    "sí" if f["correcto"] == "1" else "NO"])
    tabla(["planificación", "T (s)", "peor/mejor", "S abs", "T / T(static)", "puesto",
           "max_diff", "verif."], tab)
    mejor = orden[0]
    parrafo(f"Más rápida: {mejor['schedule']}"
            f"{'' if mejor['chunk'] == '0' else ',' + mejor['chunk']} ({float(mejor['tiempo_s']):.4f} s, "
            f"{base / float(mejor['tiempo_s']):.3f} veces más rápida que static sin trozo).")
    tab = []
    for c in (1, 4, 16, 64):
        if ("static", c) in t and ("dynamic", c) in t:
            tab.append([c, f"{t[('static', c)] / base:.3f}", f"{t[('dynamic', c)] / t[('static', c)]:.3f}"])
    parrafo("Separación de efectos, mismo trozo: T(static,c)/T(static) mide lo que cuesta "
            "entrelazar trozos de c filas sin gestión dinámica; T(dynamic,c)/T(static,c) mide lo "
            "que añade (o quita) repartir esos mismos trozos bajo demanda.")
    tabla(["trozo c (filas)", "T(static,c)/T(static)", "T(dynamic,c)/T(static,c)"], tab)


# ---------------------------------------------------------- Ejercicio 6
RUTAS_EJ6 = (("sin afinidad", "variantes.csv"),
             ("OMP_PROC_BIND=close OMP_PLACES=cores", "variantes_afinidad_close.csv"),
             ("OMP_PROC_BIND=spread OMP_PLACES=threads", "variantes_afinidad_spread.csv"))
RUTA_EJ6_8 = ("complementaria, 8 hilos, sin afinidad", "variantes_8hilos.csv")


def ej6():
    titulo("Ejercicio 6 · optimización guiada por medición")
    st = stream_max()
    if st:
        parrafo(f"Techo de ancho de banda (STREAM triad): máximo {st['24']:.2f} GB/s (24 B) / "
                f"{st['32']:.2f} GB/s (32 B, con lectura previa) con {st['hilos']} hilos; "
                f"1 hilo: {st['uno24']:.2f} / {st['uno32']:.2f} GB/s.")
    datos = {}
    for etiqueta, nombre in RUTAS_EJ6 + (RUTA_EJ6_8,):
        ruta = RES / "ejercicio6" / nombre
        if not existe(ruta):
            parrafo(f"(falta {nombre})")
            continue
        filas = ej6_tabla(ruta, etiqueta, st)
        if (etiqueta, nombre) in RUTAS_EJ6:
            datos[etiqueta] = filas
    if len(datos) > 1:
        titulo("Afinidad: la misma tabla con OMP_PROC_BIND / OMP_PLACES (tiempos en s)", 3)
        etiquetas = list(datos)
        nombres = [f["variante"] for f in datos[etiquetas[0]]]
        tab = []
        for i, nom in enumerate(nombres):
            ts = [float(datos[e][i]["tiempo_s"]) for e in etiquetas]
            tab.append([nom] + [f"{x:.4f}" for x in ts] +
                       [f"{ts[0] / x:.3f}" for x in ts[1:]])
        tabla(["variante"] + etiquetas + [f"mejora {e.split()[0]}" for e in etiquetas[1:]], tab)


def ej6_tabla(ruta, etiqueta, st):
    filas = leer(ruta)
    m = meta(ruta)
    titulo(f"{etiqueta}: N={m['N']}, {m['pasos']} pasos, {m['hilos']} hilos "
           f"(reales {m['hilos_reales']}), mínimo de {m['repeticiones']} rondas "
           f"(fichero {ruta.name})", 3)
    parrafo(f"T1 = {float(m['T1_secuencial']):.4f} s (peor ronda {float(m['T1_peor']):.4f} s).")
    with open(ruta, encoding="utf-8") as fh:
        for linea in fh:
            if linea.startswith("# E"):
                parrafo("Evidencia " + linea[2:].strip())
    tab = []
    for f in filas:
        real = float(f["GB_s_real"])
        ref_stream = st["32"] if st else None
        tab.append([f["variante"], f["parte_de"], f"{float(f['tiempo_s']):.4f}",
                    f"{float(f['tiempo_peor_s']) / float(f['tiempo_s']):.2f}",
                    f"{float(f['speedup_abs']):.3f}", f"{float(f['mejora_vs_anterior']):.3f}",
                    f"{float(f['mejora_vs_base']):.3f}", f"{float(f['GB_s_16B']):.2f}",
                    f"{f['B_celda_real']} B: {real:.2f}",
                    pct(real / ref_stream) if ref_stream else "-",
                    f["max_diff"], "sí" if f["correcto"] == "1" else "NO"])
    tabla(["variante", "parte de", "T (s)", "peor/mejor", "S abs", "mejora vs anterior",
           "mejora vs base", "GB/s 16 B", "GB/s real", "% STREAM real", "max_diff",
           "verif."], tab)
    parrafo("GB/s real: bytes que la variante mueve de verdad por celda (24 B con escritura "
            "normal, que lee la línea antes de escribirla; 16 B con escritura no temporal, "
            "que no la lee). % STREAM real: frente al máximo de STREAM contado también con "
            "sus bytes reales (modelo de 32 B, que incluye esa misma lectura previa).")
    return filas


# ------------------------------------------------------------ resumen
def resumen():
    titulo("Resumen de validaciones (calculado de los CSV)")
    fallos = []
    filas_t = []

    def anota(nombre, ok, detalle):
        filas_t.append([nombre, "PASA" if ok else "FALLA", detalle])
        if not ok:
            fallos.append(nombre)

    for n in (2048, 256):
        r = RES / "ejercicio2" / f"verificacion_{n}.csv"
        if existe(r):
            f = leer(r)
            jac = [x for x in f if x["tipo"] == "jacobi_omp"]
            ins = [x for x in f if x["tipo"] == "in_situ"]
            anota(f"Ej2 Jacobi N={n}", len(jac) == 8 and all(x["correcto"] == "1" for x in jac),
                  f"{sum(x['correcto'] == '1' for x in jac)}/8 recuentos correctos")
            anota(f"Ej2 in situ rechazado N={n}",
                  len(ins) > 0 and all(x["correcto"] == "0" for x in ins),
                  f"{sum(x['correcto'] == '0' for x in ins)}/{len(ins)} rechazadas")
            anota(f"Ej2 in situ depende de los hilos N={n}",
                  len({x["checksum"] for x in ins if x["ejecucion"] == "1"}) > 1,
                  f"{len({x['checksum'] for x in ins if x['ejecucion'] == '1'})} checksums distintos")
        else:
            anota(f"Ej2 N={n}", False, "falta el CSV")
    for n in (2048, 256):
        r = RES / "ejercicio3" / f"fuerte_{n}.csv"
        if existe(r):
            f = leer(r)
            anota(f"Ej3 fuerte N={n}", len(f) == 8 and all(
                x["checksum_ok"] == "1" and x["hilos"] == x["hilos_reales"] for x in f),
                "checksum y residuo = secuencial; hilos reales = pedidos")
        else:
            anota(f"Ej3 fuerte N={n}", False, "falta el CSV")
    r = RES / "ejercicio4" / "debil.csv"
    if existe(r):
        f = leer(r)
        anota("Ej4 débil", len(f) == 8 and all(x["correcto"] == "1" for x in f),
              "cada N verificada contra la secuencial con la misma N")
    else:
        anota("Ej4 débil", False, "falta el CSV")
    for _, nombre in RUTAS_EJ5:
        r = RES / "ejercicio5" / nombre
        if existe(r):
            f = leer(r)
            m = meta(r)
            anota(f"Ej5 {nombre}", len(f) == 10 and all(x["correcto"] == "1" for x in f)
                  and m.get("hilos") == m.get("hilos_reales"), "todas dan el campo secuencial")
        else:
            anota(f"Ej5 {nombre}", False, "falta el CSV")
    for etiqueta, nombre in RUTAS_EJ6 + (RUTA_EJ6_8,):
        r = RES / "ejercicio6" / nombre
        if existe(r):
            f = leer(r)
            m = meta(r)
            anota(f"Ej6 {nombre}", all(x["correcto"] == "1" for x in f)
                  and m.get("hilos") == m.get("hilos_reales"),
                  f"{sum(x['correcto'] == '1' for x in f)}/{len(f)} variantes verificadas")
        else:
            anota(f"Ej6 {nombre}", False, "falta el CSV")
    r = RES / "ejercicio6" / "stream.csv"
    if existe(r):
        f = leer(r)
        anota("STREAM", all(abs(float(x["comprobacion"]) - 7) < 1e-12 for x in f),
              "a[i] = 1 + 3·2 = 7 en todas las filas")
    tabla(["comprobación", "estado", "detalle"], filas_t)
    parrafo("TODAS LAS VERIFICACIONES PASAN" if not fallos else
            "FALLAN: " + ", ".join(fallos))
    return 0 if not fallos else 1


NOTA_REFRESCO = "Nota: los bloques de tablas derivadas"


def refrescar_registro():
    """Recalcula, sobre los MISMOS CSV, los bloques que el guion escribió con
    este programa en resultados/salidas_ejercicios_2_en_adelante.txt (lo que
    hay entre '$ ... src/analisis_lab1.py X' y su '[codigo de salida'), y lo
    deja anotado al final del registro. No repite ninguna medida."""
    import contextlib
    import datetime
    import io
    import re
    ruta = RES / "salidas_ejercicios_2_en_adelante.txt"
    texto = ruta.read_text(encoding="utf-8")
    funciones = {"ej2": ej2, "ej3": ej3, "ej4": ej4, "ej5": ej5, "ej6": ej6,
                 "resumen": resumen}
    cambiados = []
    for sec, fun in funciones.items():
        m = re.search(rf"^\$ \S+ src/analisis_lab1\.py {sec}\n", texto, re.M)
        if not m:
            continue
        fin = texto.find("[codigo de salida", m.end())
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            fun()
        texto = texto[:m.end()] + buf.getvalue() + texto[fin:]
        cambiados.append(sec)
    lineas = [l for l in texto.rstrip("\n").split("\n") if not l.startswith(NOTA_REFRESCO)]
    fecha = datetime.datetime.now().astimezone().strftime("%Y-%m-%d %H:%M:%S %z")
    lineas.append(f"{NOTA_REFRESCO} ({', '.join(cambiados)}) se regeneraron el {fecha} "
                  f"con 'python3 src/analisis_lab1.py refrescar-registro' sobre los mismos CSV; "
                  f"no se repitió ninguna medida.")
    ruta.write_text("\n".join(lineas) + "\n", encoding="utf-8")
    print(f"Registro actualizado: {', '.join(cambiados)}")


def main():
    global HILOS_LOGICOS, MODO
    HILOS_LOGICOS = os.cpu_count()
    que = sys.argv[1] if len(sys.argv) > 1 else "resumen"
    if que == "refrescar-registro":
        refrescar_registro()
        return 0
    secciones = {"ej2": ej2, "ej3": ej3, "ej4": ej4, "ej5": ej5, "ej6": ej6}
    if que == "markdown":
        MODO = "md"
        print("# Laboratorio 1 · Tablas de los ejercicios 2 a 6\n")
        print("Generado automáticamente por `src/analisis_lab1.py markdown` a partir de "
              "los CSV de `resultados/`. No editar a mano: se regenera con "
              "`scripts/ejecutar_ej2_en_adelante.sh`.\n")
        for f in secciones.values():
            f()
        resumen()
        return 0
    if que == "resumen":
        return resumen()
    if que not in secciones:
        sys.exit(f"Uso: {sys.argv[0]} ej2|ej3|ej4|ej5|ej6|resumen|markdown|refrescar-registro")
    secciones[que]()
    return 0


if __name__ == "__main__":
    sys.exit(main())
